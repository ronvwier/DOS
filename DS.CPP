/* ------------------------------------------------------------------------- */
/* DS         : Besturen dia interface via COM poort                         */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#define VERSIE      "DS 1.1b"
#define PULSE_POORT 1

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>

#include "toets.h"
#include "cua.h"
#include "scherm.h"
#include "taak.h"
#include "dialoog.h"
#include "kleur.h"
#include "tekst.h"
#include "diropen.h"
#include "dosdir.h"
#include "doserror.h"
#include "dbase.h"
#include "tijd.h"
#include "geluid.h"
#include "wekker.h"

#include "diapulse.h"

#define KLEUR_RECHT        kleur_titel_normaal()
#define KLEUR_TITEL        kleur_titel_accent()
#define KLEUR_NAAM         kleur_titel_normaal()
#define KLEUR_PFK          kleur_pf()
#define KLEUR_TEKST        kleur_body_tekst_normaal()
#define KLEUR_DATA         kleur_body_tekst_accent()
#define KLEUR_HELP         kleur_body_tekst_normaal()
#define KLEUR_MELDING      kleur_body_tekst_accent()

static char bestanden[64] = "*.DBF";
static char dir[64]       = "";

static
unsigned       toets;

GELUID geluid;

#define SIZE_DIA       8
#define SIZE_TITEL    30
#define SIZE_LOKATIE  30

#define MAX_DIA      512

static struct DIA
{
   char dia[SIZE_DIA+1];
   char titel[SIZE_TITEL+1];
   double tijd;
   char slede;
   char lokatie[SIZE_LOKATIE +1];
}  *tabel;

static char * dia_reg[MAX_DIA];

static int aantal_dia = 0;
static int laatste_reg = -1;
static int toon_dia = 0;

static char * logo[] = {
  "ÚÄÄÄÄÄÄÄÄÄÄÄ¿    ÚÄÄÄÄÄÄÄÄÄÄÄÄ¿ ",
  "³ ÚÄÄÄÄÄÄÄ¿ À¿  ÚÙ ÚÄÄÄÄÄÄÄÄ¿ À¿",
  "³ ³       À¿ ³  ³ ÚÙ        ÀÄÄÙ",
  "³ ³        ³ ³  ³ À¿            ",
  "³ ³        ³ ³  À¿ ÀÄÄÄÄÄÄÄÄÄÄ¿ ",
  "³ ³        ³ ³   ÀÄÄÄÄÄÄÄÄÄÄ¿ À¿",
  "³ ³  DIA   ³ ³       STUUR  À¿ ³",
  "³ ³       ÚÙ ³  ÚÄÄ¿        ÚÙ ³",
  "³ ÀÄÄÄÄÄÄÄÙ ÚÙ  À¿ ÀÄÄÄÄÄÄÄÄÙ ÚÙ",
  "ÀÄÄÄÄÄÄÄÄÄÄÄÙ    ÀÄÄÄÄÄÄÄÄÄÄÄÄÙ ",
  "" } ;

/* ---------------------------------------------------------------------- */
/* HELP_ALG                                                               */
/* Taak   : Algemeen help scherm                                          */
/* ---------------------------------------------------------------------- */
static void help_alg()
{
   dialoog_popup_start( 9,20,40,8);

   scherm_plaats_tekst(10,22,KLEUR_HELP,"Diastuur is een programma voor het");
   scherm_plaats_tekst(11,22,KLEUR_HELP,"automatisch sturen van projectoren");

   scherm_plaats_tekst(13,22,KLEUR_HELP,"Invoer voor deze sturing is een   ");
   scherm_plaats_tekst(14,22,KLEUR_HELP,"DBase bestand met per dia informatie");
   scherm_plaats_tekst(15,22,KLEUR_HELP,"en stuurgegevens.");

   toets_wacht();
   dialoog_popup_einde();
}

static void help_best()
{
   dialoog_popup_start( 5,20,40,13);

   scherm_plaats_tekst( 6,22,KLEUR_HELP,"Het invoerbestand voor diastuur   ");
   scherm_plaats_tekst( 7,22,KLEUR_HELP,"moet een DBase II, III of IV vorm ");
   scherm_plaats_tekst( 8,22,KLEUR_HELP,"hebben en uit de volgende velden in ");
   scherm_plaats_tekst( 9,22,KLEUR_HELP,"de opgegeven volgorde bestaan:      ");

   scherm_plaats_tekst(11,22,KLEUR_HELP,"1. DIA     het nummer van de dia    ");
   scherm_plaats_tekst(12,22,KLEUR_HELP,"2. TITEL   de titel van de dia      ");
   scherm_plaats_tekst(13,22,KLEUR_HELP,"3. TIJD    de vertoningstijd in sec.");
   scherm_plaats_tekst(14,22,KLEUR_HELP,"4. SLEDE   richting slede A/V/M,    ");
   scherm_plaats_tekst(15,22,KLEUR_HELP,"           M=Magazijnwisseling      ");
   scherm_plaats_tekst(16,22,KLEUR_HELP,"5. LOKATIE waar de dia is genomen   ");

   toets_wacht();
   dialoog_popup_einde();
}

static void help_koppel()
{
   dialoog_popup_start( 5,20,40,13);

   scherm_plaats_tekst( 6,22,KLEUR_HELP,"De projector wordt gestuurd via het ");
   scherm_plaats_tekst( 7,22,KLEUR_HELP,"DTR signaal op de COM1: poort, dit  ");
   scherm_plaats_tekst( 8,22,KLEUR_HELP,"is bij een 25-polige aansluiting pin");
   scherm_plaats_tekst( 9,22,KLEUR_HELP,"20 en 7 (signal ground).            ");
   scherm_plaats_tekst(10,22,KLEUR_HELP,"Bij een 9-polige aansluiting is dit ");
   scherm_plaats_tekst(11,22,KLEUR_HELP,"pin 4 en pin 5 (signal ground).     ");

   scherm_plaats_tekst(13,22,KLEUR_HELP,"Deze twee pinnen worden aangesloten ");
   scherm_plaats_tekst(14,22,KLEUR_HELP,"op een optic-coupler via een 2k7    ");
   scherm_plaats_tekst(15,22,KLEUR_HELP,"weerstand. Het DTR potentiaal gaat  ");
   scherm_plaats_tekst(16,22,KLEUR_HELP,"bij schakelen van + naar - 12v.     ");
   toets_wacht();
   dialoog_popup_einde();
}

/* ---------------------------------------------------------------------- */
/* MELDING                                                                */
/* Taak   : Foutmelding geven en toetsaanslag vragen                      */
/* ---------------------------------------------------------------------- */
static void melding(char tekst[])
{
   int i;

   i = strlen(tekst);
   if (i < 24)
     i = 24;

   dialoog_popup_start( 9,(80 - i -4) /2,i + 4,7);
   scherm_plaats_tekst(11,(80 - i) /2 ,KLEUR_MELDING,tekst);
   scherm_plaats_tekst(13,(80 - 24) /2,KLEUR_TEKST  ,"druk op een toets a.u.b.");

   toets_wacht();

   dialoog_popup_einde();
}

/* ---------------------------------------------------------------------- */
/* WACHT_START                                                            */
/* Taak   : Wachtmelding geven                                            */
/* ---------------------------------------------------------------------- */
static void wacht_start(char tekst[])
{
   int i;

   i = strlen(tekst);
   if (i < 15)
     i = 15;

   dialoog_popup_start( 9,(80 - i -4) /2,i + 4,7);
   scherm_plaats_tekst(11,(80 - i) /2 ,KLEUR_MELDING,tekst);
   scherm_plaats_tekst(13,(80 - 24) /2,KLEUR_TEKST  ,"momentje a.u.b.");
}

static void wacht_einde(void)
{
   dialoog_popup_einde();
}

static void vooruit(void);
static void vooruit()
{
   wacht_start("Slede gaat nu vooruit");
   diapulse_vooruit();
   wacht_einde();
}

static void achteruit(void);
static void achteruit()
{
   wacht_start("Slede gaat nu achteruit");
   diapulse_achteruit();
   wacht_einde();
}

static void magazijn(void);
static void magazijn()
{
   geluid.beep();
   melding("Plaats nu de magazijnen a.u.b.");
   vooruit();
}

static void inlezen(void);
static void inlezen()
{
   char bestand[64];
   int best_nr, i;
   char hulp[256];
   int fout = 0;
   char naam[12], type;
   int lengte, decimalen;

   if (diropen("Inlezen diastuurbestand",bestanden,dir,bestand) == DIROPEN_OK)
   {
      if (dbase_formaat(bestand) != DBASE_OK)
         melding("Bestand niet in DBASE formaat");
      else
      {
         dbase_open_lezen(bestand,&best_nr);

         /* Check of veld 1=DIA, 2=TITEL, 3=TIJD, 4=SLEDE, 5=LOKATIE */
         dbase_geef_veldinfo(best_nr,1,naam,&type,&lengte,&decimalen);
         if (strcmp(naam,"DIA") != 0)
            melding("Veld 1 heet geen DIA");
         dbase_geef_veldinfo(best_nr,2,naam,&type,&lengte,&decimalen);
         if (strcmp(naam,"TITEL") != 0)
            melding("Veld 2 heet geen TITEL");
         dbase_geef_veldinfo(best_nr,3,naam,&type,&lengte,&decimalen);
         if (strcmp(naam,"TIJD") != 0)
            melding("Veld 3 heet geen TIJD");
         dbase_geef_veldinfo(best_nr,4,naam,&type,&lengte,&decimalen);
         if (strcmp(naam,"SLEDE") != 0)
            melding("Veld 4 heet geen SLEDE");
         dbase_geef_veldinfo(best_nr,5,naam,&type,&lengte,&decimalen);
         if (strcmp(naam,"LOKATIE") != 0)
            melding("Veld 5 heet geen LOKATIE");

         aantal_dia = 0;

         for ( i = 1; dbase_lees_record(best_nr,(long)i) == DBASE_OK; i++)
         {
            /* Converteer record naar tabel */
            dbase_geef_veld(best_nr,1,hulp);
            strncpy(tabel[aantal_dia].dia,hulp,SIZE_DIA);

            dbase_geef_veld(best_nr,2,hulp);
            strncpy(tabel[aantal_dia].titel,hulp,SIZE_TITEL);

            dbase_geef_veld(best_nr,3,hulp);
            tabel[aantal_dia].tijd  = atof(hulp);

            dbase_geef_veld(best_nr,4,hulp);
            tabel[aantal_dia].slede = toupper(hulp[0]);

            dbase_geef_veld(best_nr,5,hulp);
            strncpy(tabel[aantal_dia].lokatie,hulp,SIZE_LOKATIE);

	    dia_reg[aantal_dia] = (char *)malloc(74);
            sprintf(dia_reg[aantal_dia]," %-8.8s %-30.30s %-30.30s ",
              tabel[aantal_dia].dia,
              tabel[aantal_dia].titel,
              tabel[aantal_dia].lokatie);

            aantal_dia++;
         }
         dia_reg[aantal_dia] = NULL;
         dbase_sluit(best_nr);
         toon_dia = 0;
      }
   }

}

static void vertonen(void);
static void vertonen()
{
   char hulp[20];

   if (aantal_dia < 1)
      inlezen();
   if (aantal_dia < 1)
      melding("Lees eerst een bestand in a.u.b.");
   else
   {
      toets = dialoog_vertikaal_menu(3,4,18,dia_reg,&toon_dia);

      if (toets == CUA_ENTER)
      {
         dialoog_popup_start( 6,12,56,13);
         scherm_plaats_tekst( 7,14,KLEUR_TEKST,"Vertonen DIA's");
         scherm_plaats_tekst( 9,14,KLEUR_TEKST,"Dia ..............");
         scherm_plaats_tekst(10,14,KLEUR_TEKST,"Titel ............");
         scherm_plaats_tekst(11,14,KLEUR_TEKST,"Lokatie ..........");
         scherm_plaats_tekst(13,14,KLEUR_TEKST,"Vertoningstijd ...      seconde");
         scherm_plaats_tekst(15,14,KLEUR_TEKST,"Richting slede ...");
         scherm_tekst(17,13,54,KLEUR_PFK," Esc=Afbreken  Spatie=Pauze");

         /* Vraag om eerste magazijn(en) */
         if (toon_dia == 0)
             magazijn();
         else
             vooruit();

         toets = 0;

         for (; toon_dia < aantal_dia && toets != CUA_AFBREKEN; toon_dia++)
         {
            /* Toon de DIA de opgegeven tijd */
            scherm_tekst( 9,33,SIZE_DIA,    KLEUR_DATA,tabel[toon_dia].dia);
            scherm_tekst(10,33,SIZE_TITEL,  KLEUR_DATA,tabel[toon_dia].titel);
            scherm_tekst(11,33,SIZE_LOKATIE,KLEUR_DATA,tabel[toon_dia].lokatie);
            sprintf(hulp,"%2.1f",tabel[toon_dia].tijd);
            scherm_tekst(13,33,4,           KLEUR_DATA,hulp);
            scherm_tekst(15,33,1,           KLEUR_DATA,&tabel[toon_dia].slede);
            wekker_start(tabel[toon_dia].tijd);
            toets = 0;
            while (!wekker_alarm() && toets != CUA_AFBREKEN)
            {
               toets = toets_lees();
               if (toets == ' ')
                  melding("Projectie is gepauseerd");
            }
            if (toets != CUA_AFBREKEN)
            {
               /* Beweeg de slede(s) */
               switch(tabel[toon_dia].slede)
               {
                  case 'A' : achteruit();
                             break;
                  case 'M' : magazijn();
                             break;
                  default:   vooruit();
                             break;
               }
            }
         }
         if (toon_dia >= aantal_dia)
            toon_dia = 0;
         dialoog_popup_einde();
      }
   }
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(void);
int main()

{
   int i,j,x;
   static  DIALOOG_MENU aktie_menu = {
        { "Diabestand","1. Inlezen","2. Tonen","3. Einde", NULL },
        { "Opties" , "1. Vooruit", "2. Achteruit" ,"3. Magazijnwissel", NULL } ,
        { "Help" , "1. Algemeen", "2. Bestandsinformatie" ,"3. Koppeling met projector", NULL } ,
        NULL };
   int einde;

   scherm_init();

   doserror_handler();

   scherm_cls(KLEUR_TEKST);
   scherm_tekst(0,0,scherm_aantal_kol(),KLEUR_TITEL,"");
   scherm_plaats_tekst(0,2,KLEUR_TITEL,VERSIE);
   scherm_plaats_tekst(0,scherm_aantal_kol() -24,KLEUR_RECHT,"(c) 1991 RvW");
   scherm_tekst(scherm_aantal_reg() -1,0,scherm_aantal_kol(),KLEUR_PFK,"  Esc=Annuleer  F1=Help  F3=Einde");

   x = (scherm_aantal_reg() - 10) / 2;
   j = (scherm_aantal_kol() - 32) / 2;
   for (i = 0; i < 10; i++)
      scherm_plaats_tekst((i+x),j,KLEUR_TEKST,logo[i]);

   diapulse_init(PULSE_POORT);

   tijd_permanent_tonen(0,scherm_aantal_kol() -10);

   dialoog_toon_menu(1,0,scherm_aantal_kol() -1,aktie_menu);

   tabel = (struct DIA *)malloc(MAX_DIA * sizeof(struct DIA));
   if (tabel == NULL)
   {
      melding("Onvoldoende geheugen beschikbaar");
      einde = 1;
   }
   else
      einde = 0;

   do
   {
      toets = dialoog_kies_menu(1,0,scherm_aantal_kol() -1,aktie_menu,&i,&j);
      switch(toets)
      {
         case CUA_EINDE:
              einde = 1;
              break;
         case CUA_HELP:
              i = 2 , j = 1;
              break;
         case CUA_ENTER:
              switch(i)
              {
                 case 0:                 /* Diabestand */
                      switch(j)
                      {
                         case 1:         /* Inlezen */
                              inlezen();
                              break;
                         case 2:         /* Vertonen */
                              vertonen();
                              break;
                         case 3:         /* Einde */
                              einde = 1;
                              break;
                      }
                      break;
                 case 1:                 /* Opties */
                      switch(j)
                      {
                         case 1:         /* Vooruit */
                              vooruit();
                              break;
                         case 2:         /* Achteruit */
                              achteruit();
                              break;
                         case 3:         /* Magazijn wisseling */
                              magazijn();
                              break;
                      }
                      break;
                 case 2:                 /* Help */
                      switch(j)
                      {
                         case 1:         /* Algemeen */
                              help_alg();
                              break;
                         case 2:         /* Bestandslayout */
                              help_best();
                              break;
                         case 3:         /* Koppeling met projector */
                              help_koppel();
                              break;
                      }
                      break;
              }
      }

   }  while ( !einde );

   diapulse_einde();

   /* Sluit toetsenbord/schermafhandeling af */
   scherm_einde();

   return 0;
}
