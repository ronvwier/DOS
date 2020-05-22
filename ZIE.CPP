/* ------------------------------------------------------------------------- */
/* ZIE                                                                       */
/* Taak       : Bestand bekijken                                             */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "toets.h"
#include "cua.h"
#include "scherm.h"
#include "taak.h"
#include "dialoog.h"
#include "kleur.h"
#include "tekst.h"
#include "zielees.h"
#include "diropen.h"
#include "dosdir.h"
#ifdef MSDOS
 #include "doserror.h"
#endif
#include "tijd.h"
#include "geluid.h"

#define FORMFEED      0x0C

#define SCHERM_BLANKO      0x0700 | ' '

#define KLEUR_RECHT        kleur_titel_normaal()
#define KLEUR_TITEL        kleur_titel_accent()
#define KLEUR_NAAM         kleur_titel_normaal()
#define KLEUR_PFK          kleur_pf()
#define KLEUR_TEKST        kleur_body_tekst_normaal()
#define KLEUR_HELP         kleur_body_tekst_normaal()
#define KLEUR_ZOEK_TEKST   kleur_body_tekst_normaal()
#define KLEUR_ZOEK_DATA    kleur_body_veld_normaal()
#define KLEUR_MELDING      kleur_body_tekst_accent()
#define KLEUR_LINIAAL      kleur_body_tekst_accent()

#define MIN_REG            (lin_aan ? 3 : 2)
#define MAX_REG            (scherm_aantal_reg() - 2)
#define MIN_KOL            0
#define MAX_KOL            (scherm_aantal_kol() -1)

#define SCRLEN    (MAX_REG - MIN_REG +1)
#define MAXPOS    (REGLEN - scherm_aantal_kol()) /* maximum waarde CURPOS (=132-80) */

#define MAX_CURREG   (zielees_maxreg() > SCRLEN ? zielees_maxreg() + 1 - SCRLEN : 1)

static
int      CURREG,                 /* huidige regel                   */
         CURPOS;                 /* offset binnen regel             */

static
int      MAXKOL;

static
unsigned       toets;

static int lin_aan = 0;
static int reg_aan = 0;
static int ofs_aan = 0;
static int end_aan = 0;

/* ------------------------------------------------------------------------- */
/* SHOW_CURREG                                                               */
/*                                                                           */
/* Taak       : tonen huidig regelnummer plus positie                        */
/*                                                                           */
/* Begin      : reg gevuld                                                   */
/*                                                                           */
/* Einde      : informatie getoond                                           */
/* ------------------------------------------------------------------------- */
static void show_curreg(int reg)
{
   char        hulp[10];

   sprintf(hulp,"%5d",reg);
   scherm_vervang_tekst(0,44,hulp);
   if (CURPOS > 0)
   {
      sprintf(hulp,"%+5d ",CURPOS);
      scherm_vervang_tekst(0,50,hulp);
   }
   else
      scherm_vervang_tekst(0,50,"     ");
}

/* ---------------------------------------------------------------------- */
/* Plaats hulp scherm                                                     */
/* Taak   : Tonen hulpscherm   (Onbegrijpelijk dat dat nog nodig is)      */
/* ---------------------------------------------------------------------- */

static void plaats_hulp_scherm(void);
static void plaats_hulp_scherm()
{
   if (scherm_aantal_kol() >= 80)
   {
      /* Toon help info alleen op met minimaal 80 posities */
      scherm_bewaar();
      kleur_window_help_start();
      scherm_plaats_tekst( 2,10,KLEUR_HELP," ษอออออออออออออออออออออออออออออออออออออออออออออออออออออป ");
      scherm_plaats_tekst( 3,10,KLEUR_HELP," บ  Toets:           Betekenis:                        บ ");
      scherm_plaats_tekst( 4,10,KLEUR_HELP," บ  F3/ESC           Einde zie, terug naar DOS         บ ");
      scherm_plaats_tekst( 5,10,KLEUR_HELP," บ  F7/PgUp          Vorige   scherm,23 regels terug   บ ");
      scherm_plaats_tekst( 6,10,KLEUR_HELP," บ  F8/PgDn          Volgende scherm,23 regels verder  บ ");
      scherm_plaats_tekst( 7,10,KLEUR_HELP," บ  F10              Oproepen menu                     บ ");
      scherm_plaats_tekst( 8,10,KLEUR_HELP," บ  Ctrl-PgUp        Vorige   print bladzijde          บ ");
      scherm_plaats_tekst( 9,10,KLEUR_HELP," บ  Ctrl-PgDn        Volgende print bladzijde          บ ");
      scherm_plaats_tekst(10,10,KLEUR_HELP," บ  Home             Ga naar het begin                 บ ");
      scherm_plaats_tekst(11,10,KLEUR_HELP," บ  End              Ga naar het einde                 บ ");
      scherm_plaats_tekst(12,10,KLEUR_HELP," บ  |<-  ->|         Kantlijn maximaal links/rechts    บ ");
      scherm_plaats_tekst(13,10,KLEUR_HELP," บ  ?                Tonen liniaal                     บ ");
      scherm_plaats_tekst(14,10,KLEUR_HELP," บ  A                Afdrukken bestand op printer      บ ");
      scherm_plaats_tekst(15,10,KLEUR_HELP," บ  C                Copie bestand maken               บ ");
      scherm_plaats_tekst(16,10,KLEUR_HELP," บ  E                Einde regel markering tonen       บ ");
      scherm_plaats_tekst(17,10,KLEUR_HELP," บ  H                Hexadecimaal formaat              บ ");
      scherm_plaats_tekst(18,10,KLEUR_HELP," บ  O                Open ander bestand                บ ");
      scherm_plaats_tekst(19,10,KLEUR_HELP," บ  P                Tonen posities (offsets)          บ ");
      scherm_plaats_tekst(20,10,KLEUR_HELP," บ  R                Tonen regelnummers                บ ");
      scherm_plaats_tekst(21,10,KLEUR_HELP," บ  T                Tekst formaat                     บ ");
      scherm_plaats_tekst(22,10,KLEUR_HELP," บ  Z                Zoeken van teksten                บ ");
      scherm_plaats_tekst(23,10,KLEUR_HELP," ศอออออออออออออออออออออออออออออออออออออออออออออออออออออผ ");
      toets_wacht();
      kleur_window_help_einde();
      scherm_herstel();
   }
}

/* ------------------------------------------------------------------------- */
/* LEEG_STATUS_REGEL                                                         */
/*                                                                           */
/* Taak       : lege statusregel tonen                                       */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : statusregel bevat alleen PF aanwijzingen                     */
/* ------------------------------------------------------------------------- */

static void leeg_status_regel(void);
static void leeg_status_regel()
{
   /* Toon statusregel met PF toetsen */
   if (scherm_aantal_kol() > 40)
      scherm_tekst(scherm_aantal_reg()-1,0,scherm_aantal_kol(),KLEUR_PFK,
         "  F1=Help  F3=Einde  F7=Terug  F8=Verder  F10=Menu");
   else
      scherm_tekst(scherm_aantal_reg()-1,0,scherm_aantal_kol(),KLEUR_PFK,
         "  F1=Help  F3=Einde  F10=Menu");
   scherm_cursor_plaats(scherm_aantal_reg()-1,0);
}

/* ------------------------------------------------------------------------- */
/* PLAATS_INFO_REGEL                                                         */
/*                                                                           */
/* Taak       : plaatsen informatie regel                                    */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : regel getoond                                                */
/* ------------------------------------------------------------------------- */
static void plaats_info_regel(void);
static void plaats_info_regel()
{
   scherm_cursor_plaats(0,0);
   scherm_cursor_kleur(KLEUR_TITEL);
   scherm_tekst(0,0,scherm_aantal_kol(),KLEUR_TITEL,"  ZIE 2.7a");
   scherm_plaats_tekst(0,11,KLEUR_NAAM,zielees_naam());
   if (MAXKOL > 40)
   {
      scherm_plaats_tekst(0,44,KLEUR_TITEL,"          ");
      scherm_plaats_tekst(0,MAXKOL -24,KLEUR_RECHT,"(c) 1992 RvW");
   }
}

/* ------------------------------------------------------------------------- */
/* MELDING                                                                   */
/*                                                                           */
/* Taak       : fout melden                                                  */
/*                                                                           */
/* Begin      : tekst gevuld                                                 */
/*                                                                           */
/* Einde      : fout gemeldt                                                 */
/* ------------------------------------------------------------------------- */
static void melding(char tekst[])
{
   /* Toon melding */
   dialoog_popup_start(10,12,56,6);
   scherm_plaats_tekst(12,24,KLEUR_MELDING,tekst);
   scherm_plaats_tekst(14,24,KLEUR_MELDING,"druk op een toets a.u.b.");

   /* Vraag toets en haal melding weer weg */
   toets_wacht();
   dialoog_popup_einde();
}

/* ------------------------------------------------------------------------- */
/* WACHT_START                                                               */
/*                                                                           */
/* Taak       : melden dat er gewacht wordt                                  */
/*                                                                           */
/* Begin      : tekst gevuld                                                 */
/*                                                                           */
/* Einde      : melding op scherm                                            */
/* ------------------------------------------------------------------------- */
static void wacht_start(char tekst[])
{
   dialoog_popup_start(10,12,60,6);
   scherm_plaats_tekst(12,22,KLEUR_MELDING,tekst);
   scherm_plaats_tekst(14,22,KLEUR_MELDING,"momentje a.u.b.");
}

/* ------------------------------------------------------------------------- */
/* WACHT_EINDE                                                               */
/*                                                                           */
/* Taak       : wachtmelding verwijderen                                     */
/*                                                                           */
/* Begin      : melding op scherm                                            */
/*                                                                           */
/* Einde      : melding niet meer op scherm                                  */
/* ------------------------------------------------------------------------- */
static void wacht_einde(void);
static void wacht_einde()
{
   dialoog_popup_einde();
}

/* ------------------------------------------------------------------------- */
/* DISPLAY_SCHERM                                                            */
/*                                                                           */
/* Taak       : vulln scherm met regels tekst                                */
/*                                                                           */
/* Begin      : start_regel gevuld met vanaf regelnr                         */
/*                                                                           */
/* Einde      : regels getoond                                               */
/* ------------------------------------------------------------------------- */
static void display_scherm(int start_regel)
{
   register
   int         i, j;
   char        *regel;
   int         reg_oud, kol_oud;
   int         start_pos;

   /* Bepaal de startpositie van de echte data */
   start_pos = 0;
   if (reg_aan)
      start_pos = 6;
   if (ofs_aan)
      start_pos = 11;

   /* Bewaar de cursorpositie */
   scherm_cursor_pos(&reg_oud,&kol_oud);

   if (lin_aan)
   {
      register int pos;
      char * ptr_teken;
      char * tiental = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

      /* Toon een liniaal */
      pos = CURPOS;
      if (reg_aan || ofs_aan)
         scherm_tekst(MIN_REG-1,0,start_pos,KLEUR_LINIAAL,"          ");
      for (i = start_pos; i < MAXKOL; i++)
      {
         pos++;
         ptr_teken = ".";
	 if ((pos % 5) == 0)  ptr_teken = "+";
	 if ((pos % 10) == 0)
	    ptr_teken = &tiental[pos/10];
	 scherm_tekst(MIN_REG-1,i,1,KLEUR_LINIAAL,ptr_teken);
      }
   }

   /* Toon het maximaal aantal regels */
   for (i = 0 ; i < SCRLEN ; i++ )
   {
      j = start_regel + i;
      regel = zielees_regel(j);  /* zet de tekstregel in het buffer */
      if (ofs_aan)
      {
	 char ofs_tekst[20];

	 /* Toon een offset */
	 if (regel != NULL)
	    sprintf(ofs_tekst,"%8lX > ",zielees_offset(j));
	 else
	    strcpy(ofs_tekst,"           ");
	 scherm_tekst((i+MIN_REG),0,11,KLEUR_LINIAAL,ofs_tekst);
      }
      if (reg_aan)
      {
	 char reg_tekst[20];

	 /* Toon een regelnummer */
	 if (regel != NULL)
	    sprintf(reg_tekst,"%5d ",j);
	 else
/*	    strcpy(reg_tekst,"%5d       "); */
	    sprintf(reg_tekst,"%5d!",j);
	 scherm_tekst((i+MIN_REG),0,6,KLEUR_LINIAAL,reg_tekst);
      }

      /* Toon de regel */
      if (regel != NULL && CURPOS < (int)strlen(regel))
	 scherm_tekst((i+MIN_REG),start_pos,MAXKOL-start_pos,KLEUR_TEKST,&regel[CURPOS]);
      else
	 scherm_tekst((i+MIN_REG),start_pos,MAXKOL-start_pos,KLEUR_TEKST," ");

      if (end_aan && regel != NULL)
      {
	 int end_pos = strlen(regel)+start_pos-CURPOS;

	 /* Toon een punt na het laatste teken in de regel */
	 if (   end_pos >= 0 && end_pos < MAXKOL )
	    scherm_tekst((i+MIN_REG),end_pos,1,KLEUR_LINIAAL,".");
      }
   }

   /* Gooi alle toetsaanslagen weg om verwarring te voorkomen */
   while ( toets_lees() );

   /* Herstel cursor positie */
   scherm_cursor_plaats(reg_oud,kol_oud);
}

/* ------------------------------------------------------------------------- */
/* VOLGEND_BLAD                                                              */
/*                                                                           */
/* Taak       : zoek eerstvolgende regel na formfeed op                      */
/*                                                                           */
/* Begin      : regel gevuld met startregelnummer                            */
/*                                                                           */
/* Einde      : regelnr na formfeed terugegeven als returnwaarde             */
/* ------------------------------------------------------------------------- */
static int volgend_blad(int regel)
{
   register
   int         i,
               x;
   char        *hulpregel;

   x = MAX_CURREG;
   if (regel > x)      /* niet terugspringen indien reeds voorbij maxreg */
      x=regel;
   else
   {
      /* Zoek naar formfeed die maximaal 80 regels verder ligt */
      for ( i = 1; i < 80; i++ )
      {

         hulpregel = zielees_regel((regel+i));  /* zet de tekstregel in het buffer */
         if ( strchr(hulpregel,FORMFEED) != NULL )
         {
            x = regel+i;
            break;
         }
      }
   }

   return x;
}

/* ------------------------------------------------------------------------- */
/* VORIG_BLAD                                                                */
/*                                                                           */
/* Taak       : zoek eerstvolgende regel na voorgaande formfeed op           */
/*                                                                           */
/* Begin      : regel gevuld met startregelnummer                            */
/*                                                                           */
/* Einde      : regelnr na formfeed terugegeven als returnwaarde             */
/* ------------------------------------------------------------------------- */
static int vorig_blad(int regel)
{
   register
   int         i,
               x;
   char        *hulpregel;

   x = 1;
   for ( i = 1; i < 80; i++ )
   {
      /* Zoek naar formfeed die maximaal 80 regels terug ligt */
      if ((regel-i) > 0)
      {
         hulpregel = zielees_regel((regel-i));  /* zet de tekstregel in het buffer */
         if (strchr(hulpregel,FORMFEED) != NULL )
         {
            x = regel-i;
            break;
         }
      }
      else break;
   }
   return x;
}

/* ------------------------------------------------------------------------- */
/* ZOEK_TEKST                                                                */
/*                                                                           */
/* Taak       : zoeken naar tekst in bestand                                 */
/*                                                                           */
/* Begin      : CURREG gevuld, parm_tekst = NULL of gevuld                   */
/*                                                                           */
/* Einde      : CURREG + CURPOS bijgewerkt                                   */
/* ------------------------------------------------------------------------- */
static void zoek_tekst(char * parm_tekst)
{
   register
   int         i,
               positie;
   char        *regel;
   char        *dummy;
   char        hulpregel[512];
   static
   char        zoektekst[45] = "";
   int         toets;

   if (parm_tekst != NULL)
   {
      /* zoek naar de meegegeven tekst */
      strcpy(zoektekst,parm_tekst);
      toets = CUA_ENTER;
   }
   else
   {
      /* Vraag de zoektekst */
      dialoog_popup_start(10,12,60,5);
      scherm_plaats_tekst(12,15,KLEUR_ZOEK_TEKST,"Zoek : [                                        ]");
      toets = dialoog_lees_veld(12,23,40,KLEUR_ZOEK_DATA,zoektekst);
      dialoog_popup_einde();
   }

   if (toets == CUA_ENTER)
   {
      /* Zoektekst ontdoen van voor/naloop spaties en vertalen naar hoofdletters */
      tekst_trim(zoektekst);
      strupr(zoektekst);

      regel = zielees_regel(CURREG);
      for ( i = CURREG+1; regel != NULL ; i++ )
      {
          regel = zielees_regel(i);
          if (regel != NULL)
          {
              /* Vertaal regel naar hoofdletters */
              strcpy(hulpregel,regel);
              strupr(hulpregel);

              /* Kijk of zoektekst in regel staat */
              dummy = strstr(hulpregel,zoektekst);
              if (dummy != NULL)
              {
                 /* Werk CURREG en CURPOS bij a.d.h.v. gevonden tekst */
                 positie = dummy - hulpregel;
                 CURREG =i;
                 if (positie < CURPOS)
                    CURPOS = positie;
                 if ((positie+strlen(zoektekst)-CURPOS) > MAXKOL)
                    CURPOS = (positie+strlen(zoektekst)-MAXKOL);
                 scherm_cursor_plaats(2,(positie - CURPOS));
                 scherm_cursor(1);
                 break;
              }
          }
      }
      if (regel == NULL)
         melding("Zoekwaarde niet gevonden");
   }
}

/* ------------------------------------------------------------------------- */
/* AFDRUK                                                                    */
/*                                                                           */
/* Taak       : afdrukken bestand op printer                                 */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : bestand afgedrukt                                            */
/* ------------------------------------------------------------------------- */

static void afdruk(void);
static void afdruk()
{
   FILE * uit;
   register int         i;
   char  * regel;

   /* Open een printer zodat bij sluiten op een LAN direct geprint wordt */
   uit = fopen("PRN","w");
   if (uit != NULL)
   {
      /* Geef een statusmelding */
      wacht_start("Bezig met afdrukken bestand");
      i  = 1;
      regel = zielees_regel(i);

      while ( regel != NULL && !ferror(uit))
      {
         /* Druk 1 regel af */
         fprintf(uit,"%s\n",regel);
         i++;
         regel = zielees_regel(i);
      }
      /* Haal melding weg */
      wacht_einde();

      if (!ferror(uit))
         fprintf(uit,"\f\r");
      else
         melding("Afdrukken mislukt");

      /* Sluit printer, op een LAN start nu het printen direct */
      fclose(uit);
   }
   else
      melding("Afdrukken mislukt");
}

/* ------------------------------------------------------------------------- */
/* COPIE                                                                     */
/*                                                                           */
/* Taak       : copie van bestand aanmaken                                   */
/*                                                                           */
/* Begin      : parm_naam = NULL of gevuld                                   */
/*                                                                           */
/* Einde      : bestand gecopieerd indien gewenst                            */
/* ------------------------------------------------------------------------- */
static void copie(char * parm_naam)
{
   FILE * uit;
   register int         i;
   char  * regel;
   int toets;
   static char nieuw[40] = "";

   if (parm_naam != NULL)
   {
      /* Neem de parm_naam over */
      strcpy(nieuw,parm_naam);
      toets = CUA_ENTER;
   }
   else
   {
      /* Vraag de bestandsnaam */
      dialoog_popup_start(10,12,60,5);
      scherm_plaats_tekst(12,14,KLEUR_ZOEK_TEKST,"Copie bestand : [                                   ]");
      toets = dialoog_lees_veld(12,31,35,KLEUR_ZOEK_DATA,nieuw);
      dialoog_popup_einde();
   }

   if (toets == CUA_ENTER)
   {
      /* Open het uitvoer bestand */
      tekst_trim(nieuw);
      uit = fopen(nieuw,"w");

      if (uit != NULL)
      {
         /* Toon een statusmelding */
         wacht_start("Bezig met aanmaken copie");

         i  = 1;
         regel = zielees_regel(i);

         while ( regel != NULL && !ferror(uit) )
         {
            /* Druk 1 regel af */
            fprintf(uit,"%s\n",regel);
            i++;
            regel = zielees_regel(i);
         }
         /* Haal melding weg */
         wacht_einde();

         if (ferror(uit))
            melding("Aanmaken Copie mislukt");
         else
            fclose(uit);
      }
      else
         melding("Aanmaken Copie mislukt");
   }
}

/* ------------------------------------------------------------------------- */
/* NIEUW_BESTAND                                                             */
/*                                                                           */
/* Taak       : open van ander bestand                                       */
/*                                                                           */
/* Begin      : parm_naam = NULL of gevuld met sjabloon                      */
/*                                                                           */
/* Einde      : nieuw bestand open indien mogelijk                           */
/* ------------------------------------------------------------------------- */
void nieuw_bestand(char * parm_naam)
{
   int retcd;
   static char nieuw[40] = "";
   static char bestanden[48] = "*.*";
   static char dir[48] = ".";

   retcd = DIROPEN_NOT_OK;

   if (parm_naam != NULL)
   {
      if (strchr(parm_naam,'*') != NULL || strchr(parm_naam,'?') != NULL)
         /* Copieer sjabloon */
         strcpy(bestanden,parm_naam);
      else
      {
	 /* zoek bestand */
	 DOSDIR dir("");
	 strcpy(nieuw,parm_naam);
	 dir.zoek_bestand(nieuw);
	 retcd = DIROPEN_OK;
      }
   }
   if (retcd != DIROPEN_OK)
      /* Vraag om bestandsnaam */
      retcd = diropen("Open bestand",bestanden,dir,nieuw);

   if (retcd == DIROPEN_OK)
   {
      tekst_trim(nieuw);
      /* Open bestand */
      if ( zielees_open(nieuw) != ZIELEES_GELUKT )
          melding("Bestand niet gevonden");
      else
      {
         /* Positioneer op 1e regel */
         CURREG = 1;
         zielees_mode_set(zielees_mode_get());
	 plaats_info_regel();
      }
   }
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(int argc,char *argv[])

{
   GELUID geluid;
   int i,j;
   static  DIALOOG_MENU aktie_menu = {
	{ "Bestand","1. Open bestand        (o)",
		    "2. Afdrukken bestand   (a)",
		    "3. Copie maken         (c)",
		    "-",
		    "4. Zoeken tekst        (z)",
		    "-",
		    "5. Einde              (f3)",
		    NULL },
	{ "Formaat","1. Tekstueel               (t)",
		    "2. Hexadecimale codes      (h)",
		    "3. Leesbare tekens filter  (l)",
		     NULL } ,
	{ "Tonen",  "1. Liniaal              (?)",
		    "2. Regelnummers         (r)",
		    "3. Posities als offset  (p)",
		    "4. Einde regel          (e)",
		     NULL } ,
	{ NULL } };

#ifdef MSDOS
   /* Speciale fouafhandeling opzetten i.p.v. Abort,Retry,Fail,Ignore */
   doserror_handler();
#endif

   /* Initieel scherm tonen */
   scherm_init();
   scherm_cls(KLEUR_TEKST);
   MAXKOL = scherm_aantal_kol();
   plaats_info_regel();
   leeg_status_regel();
   if (MAXKOL > 40)
      tijd_permanent_tonen(0,MAXKOL -10);
   dialoog_toon_menu(1,0,scherm_aantal_kol() -1,aktie_menu);

   /* Open het al dan niet meegegeven bestand */
   CURREG = -1;
   if (argc < 2)
   {
      /* Kijk of er data in een PIPE is meegegeven */
      if (zielees_set_stdin() == ZIELEES_GELUKT )
	 CURREG = 1;
      else
         nieuw_bestand("*.*");
   }
   else
      nieuw_bestand(argv[1]);

   if (CURREG > 0)
   {
      /* Laat het bestand zien */
      zielees_mode_set(ZIELEES_MODE_TXT);
      plaats_info_regel();

      /* Laat scherm zien en vraag toetsaanslag */
      display_scherm(CURREG);
      show_curreg(CURREG);
      toets = toets_wacht();

      while ( !cua_afsluiten(toets) && !cua_annuleren(toets) )
      {
         /* Verwerk de toetsaanslag */
         scherm_cursor(0);

         if (   toets == CUA_AKTIE
             || toets == TOETS_ALT_B
             || toets == TOETS_ALT_F
             || toets == TOETS_ALT_T
            )
         {
            /* Roep het menu op */
            i = 0;
            if ( toets == TOETS_ALT_B ) i = 0;
            if ( toets == TOETS_ALT_F ) i = 1;
            if ( toets == TOETS_ALT_T ) i = 2;
            if (toets == CUA_AKTIE)
               j = 0;
            else
               j = 1;
            toets = 0;
            if (dialoog_kies_menu(1,0,scherm_aantal_kol() -1,aktie_menu,&i,&j) == CUA_ENTER)
            {
               /* Vertaal de keuze naar een toetsaanslag */
	       toets = aktie_menu[i][j][3];
               if ( i == 0 && j == 7)
		  toets = CUA_EINDE;
               if (i == 2 && j == 1)
                  toets = '?';
            }
         }

         switch(toets)
         {
            case 0 : break;                      /* dummy toets NOP */
            case CUA_NAAR_BENEDEN:               /* volgende regel */
               if (CURREG < zielees_maxreg())
                  CURREG++;
               break;

            case CUA_VERDER_1     :              /* volgende scherm */
            case CUA_VERDER_2     :
               if ((CURREG+SCRLEN) < MAX_CURREG)
                  CURREG += SCRLEN;
               else CURREG = MAX_CURREG;
               break;

            case CUA_NAAR_BOVEN:                 /* vorige regel */
	       if (CURREG > 1)
                  CURREG--;
               break;

            case CUA_TERUG_1     :               /* vorige scherm */
	    case CUA_TERUG_2     :
               if ((CURREG-SCRLEN) >= 1)
                  CURREG = CURREG-SCRLEN;
               else CURREG = 1;
               break;

            case CUA_NAAR_RECHTS:                /* scroll naar rechts */
               if ((CURPOS+10) < MAXPOS)
                  CURPOS = CURPOS+10;
               else CURPOS = MAXPOS;
               break;

            case CUA_TABULEREN   :               /* kantlijn maximaal naar rechts */
               CURPOS = MAXPOS;
               break;

            case CUA_NAAR_LINKS:                 /* scroll naar rechts */
	       if ((CURPOS-10) > 0)
                  CURPOS = CURPOS-10;
               else CURPOS = 0;
               break;

            case CUA_ENTER:                      /* kantlijn maximaal naar rechts */
            case CUA_TAB_TERUG  :
                 CURPOS = 0;
               break;

            case TOETS_HOME     :                /* naar het begin van het bestand */
                CURREG = 1;
		break;

            case TOETS_END      :                /* naar het einde van het bestand */
                while ( CURREG < zielees_maxreg() )
                {
                    /* Toon regelnummer per 100 gelezen regels */
                    CURREG += 100;
                    zielees_regel(CURREG);
                    show_curreg(CURREG);
                }
		CURREG = MAX_CURREG;
                break;

            case TOETS_CPGDN    :                /* naar de volgende bladzijde */
                CURREG = volgend_blad(CURREG);
                break;

            case TOETS_CPGUP    :                /* naar de vorige bladzijde */
                CURREG = vorig_blad(CURREG);
                break;

            case '?'            :                /* Liniaal aan/uit */
                if (lin_aan)
                   lin_aan = 0;
                else
                   lin_aan = 1;
                break;

            case 'r'            :                /* Regelnrs aan/uit */
	    case 'R'            :
                if (reg_aan)
                   reg_aan = 0;
		else
                   reg_aan = 1, ofs_aan = 0;
                break;

            case 'p'            :                /* Offset positie aan/uit */
            case 'P'            :
                if (ofs_aan)
                   ofs_aan = 0;
                else
                   ofs_aan = 1, reg_aan = 0;
                break;

            case 'e'            :                /* Einde regel tonen */
            case 'E'            :
                if (end_aan)
                   end_aan = 0;
                else
                   end_aan = 1;
                break;

            case 'z'            :       /* Zoeken van een opgegeven tekst */
            case 'Z'            :
		zoek_tekst(NULL);
                break;
            case 'n'            :       /* Open ander bestand */
            case 'N'            :
	    case 'o'            :
            case 'O'            :
                nieuw_bestand(NULL);
                break;
            case 'c'            :       /* Copie bestand maken */
            case 'C'            :
                copie(NULL);
                break;
            case 'h'            :       /* Naar HEX display mode */
            case 'H'            :
                CURREG = 1;
                zielees_mode_set(ZIELEES_MODE_HEX);
                reg_aan = 0;
                ofs_aan = 1;
                end_aan = 0;
                break;
            case 'l'            :       /* Naar leesbaar display mode */
            case 'L'            :
		reg_aan = 0;
                ofs_aan = 1;
                CURREG = 1;
                zielees_mode_set(ZIELEES_MODE_FILTER);
                break;

            case 't'            :       /* Naar TXT display mode */
            case 'T'            :
                CURREG = 1;
                ofs_aan = 0;
                zielees_mode_set(ZIELEES_MODE_TXT);
		break;
            case 'a'            :       /* Afdrukken bestand */
            case 'A'            :
                afdruk();
                break;
            case CUA_HELP       :
                plaats_hulp_scherm();
                break;
            case CUA_EINDE:
                break;
            default:
		geluid.beep();
         }

         /* Laat scherm zien en vraag toetsaanslag */
         display_scherm(CURREG);
         show_curreg(CURREG);
         if (toets != CUA_EINDE)
            toets = toets_wacht();
      }
      /* Sluit bestand */
      zielees_sluit();
   }

   /* Sluit toetsenbord/schermafhandeling af */
   scherm_einde();

   return 0;
}
