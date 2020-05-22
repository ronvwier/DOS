/* ------------------------------------------------------------------------- */
/* PALMTRIS   : Leuk spelen met de gebruiker                                 */
/*                                                                           */
/* (c) 1988   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#ifdef __OS2__
   #include <os2.h>
#endif
#include <time.h>
#include <conio.h>

#include "score.h"
#include "scherm.h"
#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "muis.h"
#include "tijd.h"
#include "geluid.h"

static
int     REAKTIE_TYD,
        start_nivo,
        nivo;
static
char    naam[255];

#define KEY_END     CUA_AFBREKEN
#define KEY_RIGHT   CUA_NAAR_RECHTS
#define KEY_LEFT    CUA_NAAR_LINKS
#define KEY_DROP    CUA_NAAR_BENEDEN
#define KEY_ROTATE  CUA_ENTER
#define KEY_RETURN  CUA_ENTER
#define KEY_RANDOM  TOETS_F9
#define KEY_SOUND   TOETS_F10

#define MUIS_STAP   2

#define CHAR_VOL           (unsigned char)219
#define KLEUR_ZWART        SCHERM_VG_ZWART
#define KLEUR_VORM_0       SCHERM_VG_GEEL
#define KLEUR_VORM_5       SCHERM_VG_GRYS
#define KLEUR_VORM_6       (SCHERM_VG_GROEN + SCHERM_VG_FEL)
#define KLEUR_VORM_1       (SCHERM_VG_BLAUW + SCHERM_VG_FEL)
#define KLEUR_VORM_2       (SCHERM_VG_ROOD  + SCHERM_VG_FEL)
#define KLEUR_VORM_3       (SCHERM_VG_PAARS + SCHERM_VG_FEL)
#define KLEUR_VORM_4       (SCHERM_VG_GEEL  + SCHERM_VG_FEL)
#define KLEUR_WIT          (SCHERM_VG_GRYS  + SCHERM_VG_FEL)
#define KLEUR_SCORE        75
#define KLEUR_TOPSCORE     30
#define KLEUR_LOGO         31
#define KLEUR_NEXT         SCHERM_AG_ZWART

#define KLEUR_RAND         SCHERM_VG_BLAUW
#define KLEUR_BODY         SCHERM_VG_GRYS

static int LOG_SIZE_REG;
static int LOG_SIZE_KOL;

static int MIN_REG;
static int MAX_REG;
static int MIN_KOL;
static int MAX_KOL;

#define AANTAL_VORMEN      7
#define VORM_SIZE          3

typedef int     VORM[VORM_SIZE][VORM_SIZE];

static
VORM            vormen[AANTAL_VORMEN + 1];

static
VORM            huidige_vorm;

static
int             vorm_punt[AANTAL_VORMEN + 1],
                volgende_vorm = -1;
static
int            log_scherm[20][10],
	       vlog_scherm[20][10];

static
int            kol;

static
long           punten;
static
unsigned       toets;
static
int            muis_kol = -1,
               beep_on = 1,
               random_on = 0;

/* ---------------------------------------------------------------------- */
/* LEES_MUIS                                                              */
/* Taak   : Kijk of de muis bewogen heeft                                 */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static int lees_muis(void);
static int lees_muis()
{
   int hkol,hreg;
   int toets = 0;
   int hulp;

   if ( muis_aanwezig() )
   {
      hulp = muis_toets();
      if ( hulp != 0)
      {
         /* Verwerk de muis toets */
         if ( hulp & MUIS_TOETS_LINKS)
            toets = KEY_DROP;
         if ( hulp & MUIS_TOETS_RECHTS)
            toets = KEY_ROTATE;
      }
      else
      {
         /* Verwerk de bewegingen van de muis */
         muis_geef_positie(&hreg,&hkol);

         if ( (muis_kol - hkol) >= MUIS_STAP )
         {
            /* Beweging naar links */
            toets = KEY_LEFT;
            muis_kol = muis_kol - MUIS_STAP;
         }
         else
         {
            if ( ( hkol - muis_kol) >= MUIS_STAP )
            {
               /* Beweging naar rechts */
               toets = KEY_RIGHT;
               muis_kol = muis_kol + MUIS_STAP;
            }
         }
      }
   }

   return(toets);
}

/* ---------------------------------------------------------------------- */
/* LEES_TOETS                                                             */
/* Taak   : Lees een toets indien aanwezig of geef 0 terug                */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static int lees_toets(void);
static int lees_toets()
{
   int toets;

   toets = toets_lees();
   if ( toets == 0 )
      toets = lees_muis();

   return(toets);
}

/* ---------------------------------------------------------------------- */
/* LEES_NIETS                                                             */
/* Taak   : Leeg de toetsbuffer                                           */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void lees_niets(void);
static void lees_niets()
{
    while ( lees_toets() );
}

/* ---------------------------------------------------------------------- */
/* LEES_WACHT_TOETS                                                       */
/* Taak   : Wacht op een toetsaanslag en lees deze                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static int lees_wacht_toets(void);
static int lees_wacht_toets()
{
    int toets;

    toets = 0;
    while ( toets == 0 ) toets = lees_toets();

    return(toets);
}

/* ---------------------------------------------------------------------- */
/* SYSTIME                                                                */
/* Taak   : Keert systeemtijd in ticks terug                              */
/*                                                                        */
/* ---------------------------------------------------------------------- */
long systime(void);
long systime()
{
   long heden;

#ifdef __MSDOS__
   struct dostime_t tijd;

   _dos_gettime(&tijd);
   heden = ((tijd.hour*3600L) + (tijd.minute*60L) + tijd.second) * 18L
    + ( tijd.hsecond / 5);
#endif
#ifdef __OS2__
   DATETIME tijd;

   DosGetDateTime(&tijd);
   heden = ((tijd.hours*3600L) + (tijd.minutes*60L) + tijd.seconds) * 18L
    + ( tijd.hundredths / 5);
#endif

   return(heden);
}
/* ---------------------------------------------------------------------- */
/* AKTIE                                                                  */
/* Taak   : Check of reaktietijd voorbij                                  */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static int aktie(void);
static int aktie()
{
   register
   int         retcode;
   static
   long        vorig = 0,
               heden;

   /* Vraag huidige tijd op en controleer 24:00 overgang */

   heden = systime();

   if (  ( ( heden - vorig ) >= REAKTIE_TYD )
      || ( heden < vorig ) )
   {
      retcode = 1;
      vorig   = heden;
   }
   else
      retcode = 0;

   return(retcode);
}

/* ---------------------------------------------------------------------- */
/* BEEP                                                                   */
/* Taak   : Geef een toontje indien gewenst                               */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void beep(void);
static void beep()
{
   GELUID geluid;

   if (beep_on)
      geluid.beep();
}

/* ---------------------------------------------------------------------- */
/* INIT_SCHERM                                                            */
/* Taak   : Het fysieke scherm vullen met de layout                       */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void init_scherm(void);
static void init_scherm()
{
   register
   int         i;
   char        hulp[100];
   long        heden;

   /* Random funktie initieeren */

   heden = systime();
   srand((int)(heden % 32113L));

   /* Plaats de rand op het scherm */

   scherm_cls(KLEUR_BODY);
   scherm_cursor_kleur(KLEUR_RAND);

   for ( i =  MIN_REG     ; i <= MAX_REG; i++ )
   {
       scherm_cursor_plaats(i,(MIN_KOL-1));
       scherm_schrijf_teken(CHAR_VOL);
       scherm_cursor_plaats(i,MAX_KOL);
       scherm_schrijf_teken(CHAR_VOL);
   }

   scherm_cursor_plaats(MAX_REG,(MIN_KOL -1));
   for ( i = (MIN_KOL -1) ; i <= MAX_KOL; i++ )
       scherm_schrijf_teken(CHAR_VOL);

   if (scherm_aantal_kol() >= 80)
   {
      sprintf(hulp,"  PALMTRIS 1.1                  %-20.20s      auteur: Ron van Wier  ",naam);
      scherm_plaats_tekst(0, 0,KLEUR_LOGO,hulp);

      if ( muis_aanwezig() )
         scherm_plaats_tekst(scherm_aantal_reg() -1,0,KLEUR_LOGO,"  Muis  : \x1B \x1A, Muis-L/\x19=drop, Muis-R/RET=roteer, ESC=eind, F9=random, F10=beep  ");
      else
         scherm_plaats_tekst(scherm_aantal_reg() -1,0,KLEUR_LOGO,"  Toets : cursor \x1b \x1a, \x19=drop, RETURN=roteer,  ESC=einde,  F9=random,  F10=beep  ");
   }
   else
   {
      sprintf(hulp," PALMTRIS %-15.15s       (c) RvW ",naam);
      scherm_plaats_tekst(0, 0,KLEUR_LOGO,hulp);
      scherm_plaats_tekst(scherm_aantal_reg() -1,0,KLEUR_LOGO," \x1b\x19\x1a Esc Enter=rot. F9=random F10=beep  ");
   }

}

/* ---------------------------------------------------------------------- */
/* SHOW_STATUS                                                            */
/* Taak   : Laat de status van RANDOM zien                                */
/* ---------------------------------------------------------------------- */

static void show_status(void);
static void show_status()
{
   if (scherm_aantal_kol() >= 80)
   {
      scherm_plaats_tekst(16,67,KLEUR_NEXT | KLEUR_WIT,"            ");
      if ( random_on )
         scherm_plaats_tekst(17,67,KLEUR_NEXT | KLEUR_WIT,"   RANDOM   ");
      else
         scherm_plaats_tekst(17,67,KLEUR_NEXT | KLEUR_WIT,"            ");

      scherm_plaats_tekst(18,67,KLEUR_NEXT | KLEUR_WIT,"            ");
   }
   else
   {
      if ( random_on )
         scherm_plaats_tekst(10,26,KLEUR_NEXT | KLEUR_WIT,"   RANDOM   ");
      else
         scherm_plaats_tekst(10,26,KLEUR_NEXT | KLEUR_WIT,"            ");
   }
}

/* ---------------------------------------------------------------------- */
/* SHOW_SCORE                                                             */
/* Taak   : Laat de score en het nivo zien op het fysieke scherm          */
/*          en de volgende vorm                                           */
/* ---------------------------------------------------------------------- */

static void show_score(void);
static void show_score()
{
   char        hulp[80];
   register
   int         i,j,x;

   if (scherm_aantal_kol() >= 80)
   {
      scherm_plaats_tekst(10,1,KLEUR_SCORE,"             ");
      sprintf(hulp," Nivo  %1d / %1d ",start_nivo,nivo);
      scherm_plaats_tekst(11,1,KLEUR_SCORE,hulp);
      scherm_plaats_tekst(12,1,KLEUR_SCORE,"             ");
      sprintf(hulp," Score %5ld ",punten);
      scherm_plaats_tekst(13,1,KLEUR_SCORE,hulp);
      scherm_plaats_tekst(14,1,KLEUR_SCORE,"             ");
      scherm_plaats_tekst(15,1,KLEUR_SCORE,"             ");

      scherm_plaats_tekst(10,67,KLEUR_NEXT | SCHERM_VG_GRYS,"  volgende  ");
      scherm_plaats_tekst(11,67,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst(12,67,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst(13,67,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst(14,67,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst(15,67,KLEUR_NEXT             ,"            ");

      /* Plaats de volgende vorm op het scherm */

      for ( i = 0; i < 3 ; i++)
      {
         for ( j = 0; j < 3; j++ )
         {
             x = vormen[volgende_vorm][i][j];
             if ( x != KLEUR_ZWART )
             {
                scherm_cursor_plaats((12+i),(70+j+j));
                scherm_cursor_kleur(KLEUR_NEXT | x);
                scherm_schrijf_teken(CHAR_VOL);
                scherm_schrijf_teken(CHAR_VOL);
             }
         }
      }
   }
   else
   {
      scherm_plaats_tekst(5,1,KLEUR_SCORE,"             ");
      sprintf(hulp," Nivo  %1d / %1d ",start_nivo,nivo);
      scherm_plaats_tekst(6,1,KLEUR_SCORE,hulp);
      scherm_plaats_tekst(7,1,KLEUR_SCORE,"             ");
      sprintf(hulp," Score %5ld ",punten);
      scherm_plaats_tekst(8,1,KLEUR_SCORE,hulp);
      scherm_plaats_tekst(9,1,KLEUR_SCORE,"             ");
      scherm_plaats_tekst(10,1,KLEUR_SCORE,"             ");

      scherm_plaats_tekst( 5,26,KLEUR_NEXT | SCHERM_VG_GRYS,"  volgende  ");
      scherm_plaats_tekst( 6,26,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst( 7,26,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst( 8,26,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst( 9,26,KLEUR_NEXT             ,"            ");
      scherm_plaats_tekst(10,26,KLEUR_NEXT             ,"            ");

      /* Plaats de volgende vorm op het scherm */

      for ( i = 0; i < 3 ; i++)
      {
         for ( j = 0; j < 3; j++ )
         {
             x = vormen[volgende_vorm][i][j];
             if ( x != KLEUR_ZWART )
             {
                scherm_cursor_plaats(( 7+i),(30+j));
                scherm_cursor_kleur(KLEUR_NEXT | x);
                scherm_schrijf_teken(CHAR_VOL);
             }
         }
      }
   }
   show_status();
}
/* ---------------------------------------------------------------------- */
/* SHOW_TOPSCORE                                                          */
/* Taak   : Laat de topscore's zien (1 per speler)                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void show_topscore(void);
static void show_topscore()
{
   register
   int         i;
   char        hulp[80];

   /* Werk de score lijst bij */

   score_init("palmtris");

   score_bewaar(naam,punten);

   /* Laat de score lijst op het scherm zien */

   for (i = 0; i < LOG_SIZE_REG; i++)
   {
       if ( i < score_aantal() )
          sprintf(hulp," Top %5ld, gem. %5ld %-15.15s ",
                  score_plaats_max(i+1),
                  score_plaats_gem(i+1),
                  score_plaats_naam(i+1));
       else
          strcpy(hulp,"                                       ");
       if (scherm_aantal_kol() >= 80)
          scherm_plaats_tekst(MIN_REG+i,MIN_KOL,KLEUR_TOPSCORE,hulp);
       else
          scherm_plaats_tekst(MIN_REG+i,1,KLEUR_TOPSCORE,hulp);
   }
   score_einde();
}

/* ---------------------------------------------------------------------- */
/* LOG_DISPLAY                                                            */
/* Taak   : Het logische scherm (20 x 10) op het fysieke scherm zetten    */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void log_display(void);
static void log_display()
{
   register
   int         i,
               j;
   char hulp[2] = { CHAR_VOL, CHAR_VOL };

   if (scherm_aantal_kol() >= 80)
   {
      for ( i = 0; i < LOG_SIZE_REG; i++ )
      {
	  for ( j = 0; j < LOG_SIZE_KOL; j++ )
	  {
	    /* 1 logische punt wordt 2 blokjes op het scherm */
	    if (log_scherm[i][j] != vlog_scherm[i][j])
	       scherm_tekst((MIN_REG+i),(MIN_KOL+(j*2)),2,log_scherm[i][j],hulp);
	    vlog_scherm[i][j] = log_scherm[i][j];
	  }
      }
   }
   else
   {
      for ( i = 0; i < LOG_SIZE_REG; i++ )
      {
	  for ( j = 0; j < LOG_SIZE_KOL; j++ )
	  {
	    /* 1 logische punt wordt 1 blokje op het scherm */
	    if (log_scherm[i][j] != vlog_scherm[i][j])
	       scherm_tekst((MIN_REG+i),(MIN_KOL+(j*2)),1,log_scherm[i][j],hulp);
	    vlog_scherm[i][j] = log_scherm[i][j];
	  }
      }
   }

}

/* ---------------------------------------------------------------------- */
/* LOG_CLS                                                                */
/* Taak   : Het logische scherm leegmaken                                 */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void log_cls(void);
static void log_cls()
{
   register
   int         i,
               j;

   memset(vlog_scherm,'?',sizeof(vlog_scherm));
   for ( i = 0 ; i < LOG_SIZE_REG; i++)
   {
       for ( j = 0; j < LOG_SIZE_KOL; j++)
	   log_scherm[i][j] = KLEUR_ZWART;
   }
   log_display();
}

/* ---------------------------------------------------------------------- */
/* WEG_VOLLE_RY                                                           */
/* Taak   : De volle rijen van het logische scherm weghalen (opschuiven)  */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void weg_volle_ry(void);
static void weg_volle_ry()
{
   register
   int         i,
               j;
   int         x,
               aktie;

   for ( i = LOG_SIZE_REG - 1 ; i > 0; i--)
   {
       /* Kijk of er een gat in de rij zit */

       aktie = 1;
       for ( j = 0; aktie && (j < LOG_SIZE_KOL); j++)
       {
	   if (log_scherm[i][j] == KLEUR_ZWART) aktie = 0;
       }
       if ( aktie )
       {
          /* Kopieer de bovenliggende rij over de volle rij */

          for ( x = i ; x > 0; x--)
          {
             for ( j = 0; j < LOG_SIZE_KOL; j++)
	     {
                 log_scherm[x][j] = log_scherm[x-1][j];
             }
          }
          /* Maak de allerhoogste rij zwart */

          for ( j = 0; j < LOG_SIZE_KOL; j++)
          {
              log_scherm[0][j] = KLEUR_ZWART;
          }
          /* Signaleer en zorg ervoor dat deze rij nogmaals wordt bekeken */

          beep();
          log_display();
          i++;
       }
   }
}

/* ---------------------------------------------------------------------- */
/* PLAATS_VORM                                                            */
/* Taak   : De huidige vorm op het logische scherm zetten/manipuleren     */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static int plaats_vorm(int aktie);
static int plaats_vorm(int aktie)
                                             /* 0 = init plaatsen     */
                                             /* 1 = omlaag            */
                                             /* 2 = rechts            */
					     /* 3 = links             */
                                             /* 5 = roteer rechts     */
{
   register
   int         i,
               j;
   static
   int         reg,
               kol;
   int         hulp,
               geslaagd;

   geslaagd = 1;

   if ( aktie != 0 )
   {
      /* Haal de vorm van de oude positie's weg */

      for ( i = 0; i < 3; i++ )
      {
	 for ( j = 0; j < 3; j++ )
         {
            if ( huidige_vorm[i][j] != KLEUR_ZWART )
               log_scherm[reg+i][kol+j]   = KLEUR_ZWART;
         }
      }
   }

   switch(aktie)
   {
      case 0 : /* init plaatsen */
               reg = 0;
               kol = (LOG_SIZE_KOL / 2) -1;
               break;
      case 1 : /* regel zakken */
               reg++;
               break;
      case 2 : /* kollom naar rechts */
               kol++;
               break;
      case 3 : /* kollom naar links */
               kol--;
               break;
      case 5 : /* roteer rechtsom */
               hulp = huidige_vorm[0][0];
               huidige_vorm[0][0] = huidige_vorm[2][0];
               huidige_vorm[2][0] = huidige_vorm[2][2];
               huidige_vorm[2][2] = huidige_vorm[0][2];
               huidige_vorm[0][2] = hulp;
	       hulp = huidige_vorm[0][1];
               huidige_vorm[0][1] = huidige_vorm[1][0];
               huidige_vorm[1][0] = huidige_vorm[2][1];
               huidige_vorm[2][1] = huidige_vorm[1][2];
               huidige_vorm[1][2] = hulp;
               break;
   }

   /* Kijk of de vorm past op de nieuwe positie's */

   for ( i = 0; i < 3; i++ )
   {
      for ( j = 0; j < 3; j++ )
      {
         if ( huidige_vorm[i][j] != KLEUR_ZWART )
         {
            if  ( log_scherm[reg+i][kol+j] != KLEUR_ZWART )
                geslaagd = 0;
            if ( ( reg+i ) >= LOG_SIZE_REG ) geslaagd = 0;
            if ( ( reg+i ) < 0 ) geslaagd = 0;
            if ( ( kol+j ) >= LOG_SIZE_KOL ) geslaagd = 0;
            if ( ( kol+j ) < 0 ) geslaagd = 0;
         }
      }
   }

   if ( geslaagd == 0 )
   {
      /* Herstel de oude situatie */
      switch(aktie)
      {
         case 1 : /* herstel regel zakken */
                  reg--;
                  break;
         case 2 : /* herstel kollom naar rechts */
                  kol--;
                  break;
         case 3 : /* herstel kollom naar links */
		  kol++;
                  break;
         case 5 : /* herstel roteer rechtsom */
                  hulp = huidige_vorm[0][0];
                  huidige_vorm[0][0] = huidige_vorm[0][2];
                  huidige_vorm[0][2] = huidige_vorm[2][2];
                  huidige_vorm[2][2] = huidige_vorm[2][0];
                  huidige_vorm[2][0] = hulp;
                  hulp = huidige_vorm[0][1];
                  huidige_vorm[0][1] = huidige_vorm[1][2];
                  huidige_vorm[1][2] = huidige_vorm[2][1];
                  huidige_vorm[2][1] = huidige_vorm[1][0];
                  huidige_vorm[1][0] = hulp;
                  break;
      }
   }

   /* Plaats de vorm op de nieuwe positie's */

   for ( i = 0; i < 3; i++ )
   {
      for ( j = 0; j < 3; j++ )
      {
         if ( huidige_vorm[i][j] != KLEUR_ZWART )
            log_scherm[reg+i][kol+j]   = huidige_vorm[i][j];
      }
   }

   if ( geslaagd )
      log_display();

   return(geslaagd);
}

/* ---------------------------------------------------------------------- */
/* INIT_VORMEN                                                            */
/* Taak   : De vormen in de vorm tabel zetten                             */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void init_vormen(void);
static void init_vormen()
{
   register
   int             kleur;

   vorm_punt[0]    = 10;
   kleur           = KLEUR_VORM_0;
   vormen[0][0][0] = KLEUR_ZWART;     /*   ..#      */
   vormen[0][0][1] = KLEUR_ZWART;     /*   .##      */
   vormen[0][0][2] = kleur;           /*   .#.      */

   vormen[0][1][0] = KLEUR_ZWART;
   vormen[0][1][1] = kleur;
   vormen[0][1][2] = kleur;

   vormen[0][2][0] = KLEUR_ZWART;
   vormen[0][2][1] = kleur;
   vormen[0][2][2] = KLEUR_ZWART;

   vorm_punt[1]    = 10;
   kleur           = KLEUR_VORM_1;
   vormen[1][0][0] = kleur ;     /*   #..      */
   vormen[1][0][1] = KLEUR_ZWART;     /*   ##.      */
   vormen[1][0][2] = KLEUR_ZWART;     /*   .#.      */

   vormen[1][1][0] = kleur ;
   vormen[1][1][1] = kleur ;
   vormen[1][1][2] = KLEUR_ZWART;

   vormen[1][2][0] = KLEUR_ZWART;
   vormen[1][2][1] = kleur ;
   vormen[1][2][2] = KLEUR_ZWART;

   vorm_punt[2]    = 1;
   kleur           = KLEUR_VORM_2;
   vormen[2][0][0] = KLEUR_ZWART;     /*   ...      */
   vormen[2][0][1] = KLEUR_ZWART;     /*   ###      */
   vormen[2][0][2] = KLEUR_ZWART;     /*   ...      */

   vormen[2][1][0] = kleur ;
   vormen[2][1][1] = kleur ;
   vormen[2][1][2] = kleur ;

   vormen[2][2][0] = KLEUR_ZWART;
   vormen[2][2][1] = KLEUR_ZWART;
   vormen[2][2][2] = KLEUR_ZWART;

   vorm_punt[3]    = 7;
   kleur           = KLEUR_VORM_3;
   vormen[3][0][0] = KLEUR_ZWART;     /*   ...      */
   vormen[3][0][1] = KLEUR_ZWART;     /*   ###      */
   vormen[3][0][2] = KLEUR_ZWART;     /*   ..#      */

   vormen[3][1][0] = kleur ;
   vormen[3][1][1] = kleur ;
   vormen[3][1][2] = kleur ;

   vormen[3][2][0] = KLEUR_ZWART;
   vormen[3][2][1] = KLEUR_ZWART;
   vormen[3][2][2] = kleur ;

   vorm_punt[4]    = 7;
   kleur           = KLEUR_VORM_4;
   vormen[4][0][0] = KLEUR_ZWART;     /*   ...      */
   vormen[4][0][1] = KLEUR_ZWART;     /*   ###      */
   vormen[4][0][2] = KLEUR_ZWART;     /*   #..      */

   vormen[4][1][0] = kleur ;
   vormen[4][1][1] = kleur ;
   vormen[4][1][2] = kleur ;

   vormen[4][2][0] = kleur ;
   vormen[4][2][1] = KLEUR_ZWART;
   vormen[4][2][2] = KLEUR_ZWART;

   vorm_punt[5]    = 4;
   kleur           = KLEUR_VORM_5;
   vormen[5][0][0] = KLEUR_ZWART;     /*   ...      */
   vormen[5][0][1] = KLEUR_ZWART;     /*   ##.      */
   vormen[5][0][2] = KLEUR_ZWART;     /*   ##.      */

   vormen[5][1][0] = kleur ;
   vormen[5][1][1] = kleur ;
   vormen[5][1][2] = KLEUR_ZWART;

   vormen[5][2][0] = kleur ;
   vormen[5][2][1] = kleur ;
   vormen[5][2][2] = KLEUR_ZWART;

   vorm_punt[6]    = 3;
   kleur           = KLEUR_VORM_6;
   vormen[6][0][0] = KLEUR_ZWART;     /*   ...      */
   vormen[6][0][1] = KLEUR_ZWART;     /*   .#.      */
   vormen[6][0][2] = KLEUR_ZWART;     /*   ###      */

   vormen[6][1][0] = KLEUR_ZWART;
   vormen[6][1][1] = kleur ;
   vormen[6][1][2] = KLEUR_ZWART;

   vormen[6][2][0] = kleur ;
   vormen[6][2][1] = kleur ;
   vormen[6][2][2] = kleur;

}

/* ---------------------------------------------------------------------- */
/* KIES_VORM                                                              */
/* Taak   : Random een vorm kiezen en puntentelling bijhouden             */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void kies_vorm(void);
static void kies_vorm()
{
   register
   int         i,
               j;
   static
   int         tel = 0;
   static
   long        vpunten = 0;

   if (  ( vpunten > punten )
      || ( volgende_vorm < 0 ) )
   {
      vpunten = 0;
      tel = 0;
      volgende_vorm = rand() % AANTAL_VORMEN;
   }

   /* Plaats de gekozen vorm in huidige_vorm */

   for ( i = 0; i < 3 ; i++)
   {
      for ( j = 0; j < 3; j++ )
      {
          huidige_vorm[i][j] = vormen[volgende_vorm][i][j];
      }
   }
   punten = punten + vorm_punt[volgende_vorm] + start_nivo;

   tel++;

   if (  ( ( tel % 25 ) == 0 )
      && ( nivo < 9 ) )
   {
      nivo++;
      beep();
   }

   vpunten = punten;
   if (  ( random_on )
      && ( rand() % 2 ) )
   {
      /* Genereer een random vorm */

      volgende_vorm = AANTAL_VORMEN;
      for ( i = 0; i < 3 ; i++)
      {
         for ( j = 0; j < 3; j++ )
         {
             if ( rand() % 2 )
                vormen[volgende_vorm][i][j] = KLEUR_WIT;
	     else
                vormen[volgende_vorm][i][j] = KLEUR_ZWART;
         }
      }
      vormen[volgende_vorm][1][1] = KLEUR_WIT;
      vorm_punt[volgende_vorm] = 50;
   }
   else
      volgende_vorm = rand() % AANTAL_VORMEN;

   REAKTIE_TYD = (10 - nivo);
   show_score();
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void main(void);
void main()
{
   register
   int         i,
               drop;
   char        nivo_str[2];
   int         r1 , k1;

   scherm_init();
   tijd_permanent_tonen(0,21);

   if (scherm_aantal_kol() >= 80)
   {
      LOG_SIZE_REG = 20;
      LOG_SIZE_KOL = 10;

      MIN_REG      = 2;
      MAX_REG      = (LOG_SIZE_REG + MIN_REG);
      MIN_KOL      = 28;
      MAX_KOL      = (MIN_KOL + (LOG_SIZE_KOL * 2));
   }
   else
   {
      LOG_SIZE_REG = 14;
      LOG_SIZE_KOL = 10;

      MIN_REG      = 1;
      MAX_REG      = (LOG_SIZE_REG + MIN_REG);
      MIN_KOL      = 15;
      MAX_KOL      = (MIN_KOL + LOG_SIZE_KOL);
   }

   muis_init();
   muis_zet_gebied(MIN_REG,MIN_KOL,MAX_REG,MAX_KOL);

   toets = 0;
   strcpy(naam," ");
   strcpy(nivo_str,"0");

   while ( toets != KEY_END )
   {
      /* Initieer het scherm */

      init_scherm();

      /* Vraag om de naam en nivo van de speler */

      if (scherm_aantal_kol() >= 80)
         r1 = 10, k1 = 11;
      else
         r1 =  6, k1 =  1;

      scherm_plaats_tekst(r1,k1,KLEUR_LOGO,"Wat is uw naam > ");

      toets = dialoog_lees_veld(r1,k1+17,21,KLEUR_LOGO,naam);
      while ( toets != KEY_END && naam[0] == ' ' )
         toets = dialoog_lees_veld(r1,k1+17,21,KLEUR_LOGO,naam);

      i = strlen(naam) - 1;
      while ( (naam[i] == ' ') && (i > 0) )  i--;
      i++;
      naam[i] = '\0';

      if (toets != KEY_END)
      {
         scherm_plaats_tekst(r1+1,k1,KLEUR_LOGO,"Speelnivo 0..9 > ");
	 toets = dialoog_lees_veld(r1+1,k1+17,1,KLEUR_LOGO,nivo_str);

         while (  toets != KEY_END && ( nivo_str[0] < '0' ||  nivo_str[0] > '9' ))
            toets = dialoog_lees_veld(r1+1,k1+17,1,KLEUR_LOGO,nivo_str);
      }

      nivo = atoi(nivo_str);
      start_nivo = nivo;
      punten = 0;

      /* Bouw het speelscherm op */

      init_scherm();
      init_vormen();

      if (toets != KEY_END)
      {
	 scherm_bewaar();
	 scherm_plaats_tekst(MIN_REG,MIN_KOL,KLEUR_WIT,"Druk toets");
	 lees_niets();
         toets = lees_wacht_toets();
         scherm_herstel();
      }

      kies_vorm();

      while (  ( toets != KEY_END )
            && ( plaats_vorm(0) ) )
      {
	  muis_kol = ((MIN_KOL + MAX_KOL) / 2) + 2;
	  muis_zet_positie(MIN_REG,muis_kol);

	  while (  ( toets != KEY_END )
		&& ( plaats_vorm(1) ) )
	  {
	     /* Verwerk de toetsaanslagen en laat de vorm 1 regel zakken */

	     aktie();
	     drop = 0;

	     while (   ( !aktie() )
		   &&  ( !drop )
		   &&  ( toets != KEY_END ) )
	     {
		/* Verwerk de toetsaanslagen van de speler */

		toets = lees_toets();
		if ( toets)
		{
		   /* Verwerk de toetsaanslag */

		   switch(toets)
		   {
		      case KEY_RIGHT    :
					  plaats_vorm(2);
					  break;
		      case KEY_LEFT     :
					  plaats_vorm(3);
					  break;
		      case KEY_DROP     :
					  i = 0;
					  while( plaats_vorm(1)) i++;
					  punten = punten + start_nivo +
					     ((i / LOG_SIZE_REG) * start_nivo);
					  drop = 1;
					  break;
		      case KEY_SOUND    :
					  beep_on = !beep_on;
					  break;
		      case KEY_RANDOM   :
					  random_on = !random_on;
					  show_status();
					  break;
		      case KEY_ROTATE   :
					  plaats_vorm(5);
					  break;
		   }
		}
	     }
	 }
	 /* Haal volle rijen weg, leeg keyboard buffer en kies nieuwe vorm */

	 weg_volle_ry();
	 lees_niets();
	 kies_vorm();
      }

      if ( toets != KEY_END )
      {
         /* Laat score's zien en wacht op een toetsaanslag */

         show_topscore();
         while ( !aktie() );
         while ( !aktie() );
         lees_niets();
         toets = lees_wacht_toets();
      }

      scherm_cls(KLEUR_BODY);
   }

   muis_einde();
   scherm_einde();
}
