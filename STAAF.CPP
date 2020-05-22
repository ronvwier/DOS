/* ------------------------------------------------------------------------- */
/* STAAF      : Staafjes spel spelen                                         */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MSDOS__
   #include <bios.h>
   #include <dos.h>
#endif
#ifdef __OS2__
   #include <os2.h>
#endif
#include <time.h>

#include "score.h"
#include "scherm.h"
#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "muis.h"

#define AANT_KLEUR 6

#define CHAR_VOL           (unsigned char)219

static int code_kleur[8] = { 0,1,10,3,4,14,15,0 };
static unsigned char code_char[16] = { ' ',CHAR_VOL,'2',176,177,
       '5','6','7','8','9',
       177,'B','C','D',178,CHAR_VOL};
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
#define KEY_ROTATE  CUA_NAAR_BOVEN
#define KEY_RETURN  CUA_ENTER
#define KEY_RANDOM  TOETS_F9
#define KEY_SOUND   TOETS_F10

#define MUIS_STAP   2

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
#define KLEUR_NEXT         16

#define KLEUR_RAND         SCHERM_VG_BLAUW
#define KLEUR_BODY         SCHERM_VG_GRYS

#define LOG_SIZE_REG       20
#define LOG_SIZE_KOL       10

#define MIN_REG            2
#define MAX_REG            (LOG_SIZE_REG + MIN_REG)
#define MIN_KOL            28
#define MAX_KOL            (MIN_KOL + (LOG_SIZE_KOL * 2))

#define AANTAL_VORMEN      7
#define VORM_SIZE          3

typedef int     VORM[VORM_SIZE];

static
VORM            huidige_vorm;

static
int            log_scherm[LOG_SIZE_REG][LOG_SIZE_KOL],
	       vlog_scherm[LOG_SIZE_REG][LOG_SIZE_KOL];

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
         if ( hulp & MUIS_TOETS_LINKS)
            toets = KEY_DROP;
         if ( hulp & MUIS_TOETS_RECHTS)
            toets = KEY_ROTATE;
      }
      else
      {
         muis_geef_positie(&hreg,&hkol);

         if ( (muis_kol - hkol) >= MUIS_STAP )
         {
            toets = KEY_LEFT;
            muis_kol = muis_kol - MUIS_STAP;
         }
         else
         {
            if ( ( hkol - muis_kol) >= MUIS_STAP )
            {
               toets = KEY_RIGHT;
               muis_kol = muis_kol + MUIS_STAP;
            }
         }
      }
   }

   return(toets);
}

static int lees_toets(void);
static int lees_toets()
{
   int toets;

   toets = toets_lees();
   if ( toets == 0 )
      toets = lees_muis();

   return(toets);
}

static void lees_niets(void);
static void lees_niets()
{
    while ( lees_toets() );
}

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
/* Taak   : Even een geluidje geven                                       */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void beep(void);
static void beep()
{
   if (beep_on)
      putch('\a');
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
   int         kleur;
   unsigned char teken;

   /* Random funktie initieeren */
   heden = systime();
   srand((unsigned int)(heden % 32113));

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

   sprintf(hulp,"  S T A A F   1.2a              %-20.20s      auteur: Ron van Wier  ",naam);
   scherm_plaats_tekst(0, 0,KLEUR_LOGO,hulp);

   if ( muis_aanwezig() )
      scherm_plaats_tekst(24,0,KLEUR_LOGO,"  Muis  : \x1B \x1A  Muis-L/\x19=Vallen  Muis-R/\x18=Roteer  ESC=einde  F10=Geluid          ");
   else
      scherm_plaats_tekst(24,0,KLEUR_LOGO,"  Toets : cursor \x1b \x1a  \x19=Vallen  \x18=Roteer  ESC=einde  F10=Geluid                 ");

   scherm_plaats_tekst( 7,55,KLEUR_LOGO,"   Plaats de staafjes   ");

   kleur = code_kleur[1];
   teken = code_char[kleur];
   scherm_plaats_teken( 9,57,kleur,teken);
   scherm_plaats_teken( 9,58,kleur,teken);
   scherm_plaats_teken(10,59,kleur,teken);
   scherm_plaats_teken(10,60,kleur,teken);
   scherm_plaats_teken(11,61,kleur,teken);
   scherm_plaats_teken(11,62,kleur,teken);

   kleur = code_kleur[2];
   teken = code_char[kleur];
   scherm_plaats_teken(13,62,kleur,teken);
   scherm_plaats_teken(13,61,kleur,teken);
   scherm_plaats_teken(14,60,kleur,teken);
   scherm_plaats_teken(14,59,kleur,teken);
   scherm_plaats_teken(15,58,kleur,teken);
   scherm_plaats_teken(15,57,kleur,teken);

   kleur = code_kleur[3];
   teken = code_char[kleur];
   scherm_plaats_teken( 9,66,kleur,teken);
   scherm_plaats_teken( 9,67,kleur,teken);
   scherm_plaats_teken(10,66,kleur,teken);
   scherm_plaats_teken(10,67,kleur,teken);
   scherm_plaats_teken(11,66,kleur,teken);
   scherm_plaats_teken(11,67,kleur,teken);

   kleur = code_kleur[4];
   teken = code_char[kleur];
   scherm_plaats_teken(14,64,kleur,teken);
   scherm_plaats_teken(14,65,kleur,teken);
   scherm_plaats_teken(14,66,kleur,teken);
   scherm_plaats_teken(14,67,kleur,teken);
   scherm_plaats_teken(14,68,kleur,teken);
   scherm_plaats_teken(14,69,kleur,teken);

   kleur = code_kleur[5];
   teken = code_char[kleur];
   scherm_plaats_teken( 9,77,kleur,teken);
   scherm_plaats_teken( 9,76,kleur,teken);
   scherm_plaats_teken(10,75,kleur,teken);
   scherm_plaats_teken(10,74,kleur,teken);
   scherm_plaats_teken(11,73,kleur,teken);
   scherm_plaats_teken(11,72,kleur,teken);

   kleur = code_kleur[6];
   teken = code_char[kleur];
   scherm_plaats_teken(13,72,kleur,teken);
   scherm_plaats_teken(13,73,kleur,teken);
   scherm_plaats_teken(14,74,kleur,teken);
   scherm_plaats_teken(14,75,kleur,teken);
   scherm_plaats_teken(15,76,kleur,teken);
   scherm_plaats_teken(15,77,kleur,teken);

   scherm_plaats_tekst(17,55,KLEUR_LOGO,"  in 1 van deze vormen  ");
}

/* ---------------------------------------------------------------------- */
/* SHOW_TIJD                                                              */
/* Taak   : Laat de tijd op het scherm zien                               */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void show_tijd(void);
static void show_tijd()
{
   char        hulp[20];

   _strtime(hulp);
   scherm_plaats_tekst(0,21,KLEUR_LOGO,hulp);
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

   scherm_plaats_tekst(10,1,KLEUR_SCORE,"             ");
   sprintf(hulp," Nivo  %1d / %1d ",start_nivo,nivo);
   scherm_plaats_tekst(11,1,KLEUR_SCORE,hulp);
   scherm_plaats_tekst(12,1,KLEUR_SCORE,"             ");
   sprintf(hulp," Score %5ld ",punten);
   scherm_plaats_tekst(13,1,KLEUR_SCORE,hulp);
   scherm_plaats_tekst(14,1,KLEUR_SCORE,"             ");
   scherm_plaats_tekst(15,1,KLEUR_SCORE,"             ");
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

   score_init("staaf");
   score_bewaar(naam,punten);

   /* Laat de score lijst op het scherm zien */

   for (i = 0; i < LOG_SIZE_REG; i++)
   {
       if ( i < score_aantal() )
          sprintf(hulp," Top %5ld, gem. %5ld pnt. van %-18.18s ",
                  score_plaats_max(i+1),
                  score_plaats_gem(i+1),
                  score_plaats_naam(i+1));
       else
          strcpy(hulp,"                                                   ");
       scherm_plaats_tekst(MIN_REG+i,MIN_KOL,KLEUR_TOPSCORE,hulp);
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
               j,x;
   char        hulp[2];

   for ( i = 0; i < LOG_SIZE_REG; i++ )
   {
       for ( j = 0; j < LOG_SIZE_KOL; j++ )
       {
	 /* 1 logische punt wordt 2 blokjes op het scherm */
	 if (log_scherm[i][j] != vlog_scherm[i][j])
	 {
	    x = log_scherm[i][j];
	    hulp[0] = code_char[x];
	    hulp[1] = code_char[x];
	    scherm_tekst((MIN_REG+i),(MIN_KOL+(j*2)),2,x,hulp);
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
       {
           log_scherm[i][j] = KLEUR_ZWART;
       }
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
   int         x,y,
               aktie;

   do
   {
       /* Kijk of er een rij van 3 is */
       aktie = 0;
       for ( i = LOG_SIZE_REG - 1 ; i > 0; i--)
       {
          for ( j = 0; j < LOG_SIZE_KOL; j++)
          {
             if (log_scherm[i][j] != KLEUR_ZWART)
             {
                if (   log_scherm[i][j] != KLEUR_ZWART
                    && j > 0 && j < (LOG_SIZE_KOL -1)
                    && log_scherm[i][j]  == log_scherm[i][j+1]
                    && log_scherm[i][j]  == log_scherm[i][j-1])
                {
                   aktie = 1;
                   log_scherm[i][j]   = KLEUR_ZWART;
                   log_scherm[i][j+1] = KLEUR_ZWART;
                   log_scherm[i][j-1] = KLEUR_ZWART;
                }
                if (   log_scherm[i][j] != KLEUR_ZWART
                    && i > 0 && i < (LOG_SIZE_REG -1)
                    && log_scherm[i][j]  == log_scherm[i+1][j]
                    && log_scherm[i][j]  == log_scherm[i-1][j])
                {
                   aktie = 1;
                   log_scherm[i][j]   = KLEUR_ZWART;
                   log_scherm[i+1][j] = KLEUR_ZWART;
                   log_scherm[i-1][j] = KLEUR_ZWART;
                }
                if (   log_scherm[i][j] != KLEUR_ZWART
                    && j > 0 && j < (LOG_SIZE_KOL -1)
                    && i > 0 && i < (LOG_SIZE_REG -1)
                    && log_scherm[i][j]  == log_scherm[i+1][j+1]
                    && log_scherm[i][j]  == log_scherm[i-1][j-1])
                {
                   aktie = 1;
                   log_scherm[i][j]     = KLEUR_ZWART;
                   log_scherm[i+1][j+1] = KLEUR_ZWART;
                   log_scherm[i-1][j-1] = KLEUR_ZWART;
                }
                if (   log_scherm[i][j] != KLEUR_ZWART
                    && j > 0 && j < (LOG_SIZE_KOL -1)
                    && i > 0 && i < (LOG_SIZE_REG -1)
                    && log_scherm[i][j]  == log_scherm[i-1][j+1]
                    && log_scherm[i][j]  == log_scherm[i+1][j-1])
                {
                   aktie = 1;
                   log_scherm[i][j]     = KLEUR_ZWART;
                   log_scherm[i-1][j+1] = KLEUR_ZWART;
                   log_scherm[i+1][j-1] = KLEUR_ZWART;
                }
             }
          }
       }
       if ( aktie )
       {
          /* Kopieer de bovenliggende kollom naar beneden */

          beep();
          log_display();
          for ( x = 0 ; x < LOG_SIZE_KOL; x++)
          {
             for ( j = LOG_SIZE_REG -1; j > 0; j--)
             {
                int done;

                do
                {
                   done = 0;
                   if (log_scherm[j][x] == KLEUR_ZWART)
                   {
                      for (y = j; y > 0; y--)
                      {
                         /* Copieer kollom 1 regel naar beneden */
                         if (log_scherm[y-1][x] != KLEUR_ZWART)
                            done = 1;
                         log_scherm[y][x] = log_scherm[y-1][x];
                      }
                   }
                   log_scherm[0][x] = KLEUR_ZWART;
                } while (done != 0);
             }
          }
          /* Signaleer en zorg ervoor dat deze rij nogmaals wordt bekeken */

          beep();
          log_display();
       }
   } while (aktie != 0);
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
                                             /* 4 = roteer            */
{
   register
   int         i;
   static
   int         reg,
               kol;
   int
               geslaagd;

   geslaagd = 1;

   if ( aktie != 0 )
   {
      /* Haal de vorm van de oude positie's weg */

      for ( i = 0; i < 3; i++ )
      {
         if ( huidige_vorm[i] != KLEUR_ZWART )
            log_scherm[reg+i][kol]   = KLEUR_ZWART;
      }
   }

   switch(aktie)
   {
      case 0 : /* init plaatsen */
               reg = 0;
               kol = LOG_SIZE_KOL / 2;
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
      case 4 : /* roteer naar boven */
               i = huidige_vorm[0];
               huidige_vorm[0] = huidige_vorm[1];
               huidige_vorm[1] = huidige_vorm[2];
               huidige_vorm[2] = i;
               break;
   }

   /* Kijk of de vorm past op de nieuwe positie's */
   if ( kol >= LOG_SIZE_KOL ) geslaagd = 0;
   if ( kol < 0 ) geslaagd = 0;

   for ( i = 0; i < 3; i++ )
   {
      if  ( log_scherm[reg+i][kol] != KLEUR_ZWART )
          geslaagd = 0;
      if ( ( reg+i ) >= LOG_SIZE_REG ) geslaagd = 0;
      if ( ( reg+i ) < 0 ) geslaagd = 0;
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
      }
   }

   /* Plaats de vorm op de nieuwe positie's */

   for ( i = 0; i < 3; i++ )
      log_scherm[reg+i][kol] = huidige_vorm[i];

   if ( geslaagd )
      log_display();

   return(geslaagd);
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
   int         i;
   static
   int         tel = 0;

   if (punten == 0)
      tel = 0;

   punten = punten + 10 + start_nivo;
   tel++;

   if (  ( ( tel % 25 ) == 0 )
      && ( nivo < 9 ) )
   {
      nivo++;
      beep();
   }

   /* Genereer een random vorm */

   for ( i = 0; i < 3 ; i++)
      huidige_vorm[i] = code_kleur[(rand() % AANT_KLEUR) + 1];

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

   scherm_init();
   muis_init();
   muis_zet_gebied(MIN_REG,MIN_KOL,MAX_REG+1,MAX_KOL+1);

   toets = 0;
   strcpy(naam," ");
   strcpy(nivo_str,"0");

   while ( toets != KEY_END )
   {
      /* Initieer het scherm */

      init_scherm();

      /* Vraag om de naam en nivo van de speler */

      scherm_plaats_tekst(10,11,KLEUR_LOGO,"Wat is uw naam > ");

      dialoog_lees_veld(10,11+17,21,KLEUR_LOGO,naam);
      while ( naam[0] == ' ' )
         dialoog_lees_veld(10,11+17,21,KLEUR_LOGO,naam);

      i = strlen(naam) - 1;
      while ( (naam[i] == ' ') && (i > 0) )  i--;
      i++;
      naam[i] = '\0';

      scherm_plaats_tekst(11,11,KLEUR_LOGO,"Speelnivo 0..9 > ");
      dialoog_lees_veld(11,11+17,1,KLEUR_LOGO,nivo_str);

      while (  ( nivo_str[0] < '0' )
            || ( nivo_str[0] > '9' ) )
         dialoog_lees_veld(11,11+17,1,KLEUR_LOGO,nivo_str);

      nivo = atoi(nivo_str);
      start_nivo = nivo;
      punten = 0;

      /* Bouw het speelscherm op */

      init_scherm();

      scherm_plaats_tekst(MIN_REG,MIN_KOL+3,KLEUR_WIT,"Druk op toets");
      lees_niets();
      toets = lees_wacht_toets();
      log_cls();

      kies_vorm();

      toets = 0;

      while (  ( toets != KEY_END )
            && ( plaats_vorm(0) ) )
      {
          muis_kol = ((MIN_KOL + MAX_KOL) / 2) + 2;
          muis_zet_positie(MIN_REG,muis_kol);

          while (  ( toets != KEY_END )
                && ( plaats_vorm(1) ) )
          {
             /* Verwerk de toetsaanslagen en laat de vorm 1 regel zakken */

             show_tijd();
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
                      case KEY_ROTATE   :
                                          plaats_vorm(4);
                                          break;
                      case KEY_DROP     :
                                          i = 0;
                                          while( plaats_vorm(1)) i++;
                                          punten = punten + start_nivo +
                                             ((i / LOG_SIZE_REG) * 10);
                                          drop = 1;
                                          break;
                      case KEY_SOUND    :
                                          beep_on = !beep_on;
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
