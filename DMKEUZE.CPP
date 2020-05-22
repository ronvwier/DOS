/* ------------------------------------------------------------------------- */
/* DMKEUZE    : Afhandelen van keuze (opvragen/wijzigen)                     */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include "dmkeuze.h"
#include "tekst.h"
#include "scherm.h"
#include "string.h"
#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "geluid.h"
#include "dm.h"

static int BASIS_REG = 1;
static int BASIS_KOL = 0;

static char * wacht_tekst[] = {"Direkt","Na toets","Geen",NULL} ;
static char * optie_tekst[] = {"Tekst","Vraag",NULL} ;
static char keuze_titel[55] = "";
static char keuze_dir[55] = "";
static char keuze_prog[55] = "";
static int  keuze_optie_type[4] = { 0 , 0 ,0 ,0 };
static char keuze_optie_tekst[4][55] = { "", "","","" };
static int  keuze_wacht = 0;

static void bepaal_pos(void);
static void bepaal_pos()
{
   if (scherm_aantal_kol() < 80)
      BASIS_KOL = 0;
   else
      BASIS_KOL = 7;
   if (scherm_aantal_reg() < 24)
      BASIS_REG = 1;
   else
      BASIS_REG = 5;
}

static void disp_scherm(void)
{
   register int i;

   /* Laat de vaste tekst zien */
   scherm_plaats_tekst(BASIS_REG+1 ,BASIS_KOL+2,KLEUR_TEKST,"Titel....[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+3 ,BASIS_KOL+2,KLEUR_TEKST,"Directory[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+4 ,BASIS_KOL+2,KLEUR_TEKST,"Commando.[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+6 ,BASIS_KOL+2,KLEUR_TEKST,"Terugkeer                                                    ");
   scherm_plaats_tekst(BASIS_REG+8 ,BASIS_KOL+2,KLEUR_TEKST,"Vragen %1[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+9 ,BASIS_KOL+2,KLEUR_TEKST,"       %2[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+10,BASIS_KOL+2,KLEUR_TEKST,"       %3[                                                  ]");
   scherm_plaats_tekst(BASIS_REG+11,BASIS_KOL+2,KLEUR_TEKST,"       %4[                                                  ]");

   /* Laat de gegevens zien */
   scherm_tekst(BASIS_REG+1,BASIS_KOL+12,50,KLEUR_DATA,keuze_titel);
   scherm_tekst(BASIS_REG+3,BASIS_KOL+12,50,KLEUR_DATA,keuze_dir);
   scherm_tekst(BASIS_REG+4,BASIS_KOL+12,50,KLEUR_DATA,keuze_prog);
   dialoog_button_toon(BASIS_REG+6,BASIS_KOL+11,keuze_wacht,wacht_tekst);
   for (i = 0 ; i < 4; i++ )
      scherm_tekst(BASIS_REG+8+i,BASIS_KOL+12,50,KLEUR_DATA,keuze_optie_tekst[i]);
   scherm_plaats_tekst(BASIS_REG+12,BASIS_KOL+1,KLEUR_PF   ," Enter  Esc=Annuleer  F1=Help                                  ");
}

void   dmkeuze_show(void)
{
   bepaal_pos();

   dialoog_popup_start(BASIS_REG,BASIS_KOL,65,14);
   disp_scherm();
   toets_wacht();
   dialoog_popup_einde();
}

int    dmkeuze_vraag(void)
{
   int toets;
   int retcode = DMKEUZE_MISLUKT;
   int veld;
   int i;

   bepaal_pos();

   dialoog_popup_start(BASIS_REG,BASIS_KOL,65,14);
   disp_scherm();

   /* Vraag de velden op totdat op enter of escape wordt gedrukt */
   veld = 1;
   do
   {
      switch(veld)
      {
         case 1 :
           do {
              toets = dialoog_lees_veld((BASIS_REG+1),(BASIS_KOL+12),50,KLEUR_DATA,keuze_titel);
              tekst_trim(keuze_titel);
              if (toets == CUA_HELP)
                 dialoog_help("De titel van deze keuze verschijnt\nin de vertikale menu balk.\nGebruik '-' voor een scheidingslijn.");
           } while ( toets == CUA_HELP );
         break;
         case 2 :
           do {
              toets = dialoog_lees_veld((BASIS_REG+3),(BASIS_KOL+12),50,KLEUR_DATA,keuze_dir);
              tekst_trim(keuze_dir);
              if (toets == CUA_HELP)
                 dialoog_help("Voor het starten van het opgegeven\nprogramma wordt eerst naar\ndeze directory gegaan");
           } while ( toets == CUA_HELP );
         break;
         case 3 :
           do {
              toets = dialoog_lees_veld((BASIS_REG+4),(BASIS_KOL+12),50,KLEUR_DATA,keuze_prog);
              tekst_trim(keuze_prog);
              if (toets == CUA_HELP)
                 dialoog_help("Hier de naam van het programma,\nevt. met directory en parameters.\nVragen in de vorm %1..%4\nWordt DM opgegeven dan kan\neen ander menu worden meegegeven");
           } while ( toets == CUA_HELP );
         break;
         case 4 :
	   do
	   {
	      toets = dialoog_button_vraag(BASIS_REG+6,BASIS_KOL+11,&keuze_wacht,wacht_tekst);
	      if (toets == CUA_HELP)
		 dialoog_help("Hier kan worden opgegeven\nof na uitvoering van het programma\nhet scherm wordt schoongemaakt\nof een toetsaanslag wordt gevraagd\nof er geen terugkeer is");
	   } while ( toets == CUA_HELP );
	   break;
         case 5 :
         case 6 :
         case 7 :
         case 8 :
           do {
              i = veld - 5;
              toets = dialoog_lees_veld((BASIS_REG+8+i),(BASIS_KOL+12),50,KLEUR_DATA,keuze_optie_tekst[i]);
              tekst_trim(keuze_optie_tekst[i]);
              if (toets == CUA_HELP)
                 dialoog_help("Hier de vragen die gesteld worden\nen die %1..%4 zullen vervangen.");
           } while ( toets == CUA_HELP );
         break;
      }
      switch(toets)
      {
         case CUA_TABULEREN :
         case CUA_NAAR_BENEDEN :
            if (veld < 8)
                  veld++;
            else
               veld = 1;
         break;
         case CUA_ENTER :
            veld = 999;
         break;
         case CUA_NAAR_BOVEN :
         case CUA_TAB_TERUG :
            if (veld > 1)
               veld--;
            else
               veld = 8;
         break;
      }
      if (veld > 1 && strlen(keuze_titel) < 1)
      {
	 GELUID geluid;
	 veld = 1;
	 geluid.beep();
      }
   }
   while (toets != CUA_AFBREKEN && veld <= 12);

   dialoog_popup_einde();

   if (toets == CUA_ENTER)
      retcode = DMKEUZE_GELUKT;

   return retcode;
}

void dmkeuze_leeg(void)
{
    dmkeuze_zet_titel("");
    dmkeuze_zet_dir("");
    dmkeuze_zet_prog("");
    dmkeuze_zet_wacht(0);
}

char * dmkeuze_titel(void)
{
   return keuze_titel;
}

void   dmkeuze_zet_titel(char titel[])
{
   strcpy(keuze_titel,titel);
}

char * dmkeuze_dir(void)
{
   return keuze_dir;
}

void   dmkeuze_zet_dir(char dir[])
{
   strcpy(keuze_dir,dir);
}
char * dmkeuze_prog(void)
{
   return keuze_prog;
}

void   dmkeuze_zet_prog(char prog[])
{
   strcpy(keuze_prog,prog);
}

int dmkeuze_wacht(void)
{
   return keuze_wacht;
}

void   dmkeuze_zet_wacht(int wacht)
{
   keuze_wacht = wacht;
}

char * dmkeuze_optie_tekst(int optie)
{
   return keuze_optie_tekst[optie];
}

void   dmkeuze_zet_optie_tekst(int optie,char tekst[])
{
   strcpy(keuze_optie_tekst[optie],tekst);
}
