/* ------------------------------------------------------------------------- */
/* SCHADE2    : Muteren bonus/malus tabel met schermafhandeling              */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include "memory.h"
#include "time.h"

#include "toets.h"
#include "cua.h"
#include "scherm.h"
#include "taak.h"
#include "dialoog.h"
#include "schade1.h"

#define KLEUR_TEKST   kleur_body_tekst_normaal()
#define KLEUR_DATA    kleur_body_veld_normaal()

static
unsigned       toets;

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void schade2(void)

{
   register int i,j;
   int trede = 1, kollom = 0;
   float korting[21];
   int volg_trede[22][4];
   char hulp[80];

   scherm_bewaar();
   kleur_window_start();

   scherm_plaats_tekst(2,10,KLEUR_TEKST,     "³ Trede   Korting   0 schade  1 schade  2 schades 3 schades ³");
      scherm_plaats_tekst(3,10,KLEUR_TEKST,  "³ ÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ³");
   for (i = 1; i < 21 ; i++ )
   {
      sprintf(hulp,"³ %3d                                                       ³",i);
      scherm_plaats_tekst(3+i,10,KLEUR_TEKST,hulp);
      korting[i] = 0;
      for ( j = 0; j < 4; j++ )
          volg_trede[i][j] = 0;
   }

   /* Haal de oude waarde's op */
   for (i = 1; i <= schade1_aantal_trede() ; i++)
   {
      korting[i] = schade1_bonus(i);
      for ( j = 0; j < 4; j++ )
          volg_trede[i][j] = schade1_volg_trede(i,j);
      sprintf(hulp,"%5.1f     %-2d        %-2d        %-2d        %-2d",
         korting[i],volg_trede[i][0],volg_trede[i][1],
         volg_trede[i][2],volg_trede[i][3]);
      scherm_plaats_tekst(3+i,20,KLEUR_DATA,hulp);
   }

   trede  = 1;
   kollom = 1;

   do
   {
       /* Verwerk de toetsaanslagen */
       switch(kollom)
       {
          case 1 :
                  sprintf(hulp,"%5.1f",korting[trede]);
                  toets = dialoog_lees_veld(trede+3,20,6,KLEUR_DATA,hulp);
                  korting[trede] = atof(hulp);
               break;
          case 2 :
                  sprintf(hulp,"%-2d",volg_trede[trede][0]);
                  toets = dialoog_lees_veld(trede+3,30,2,KLEUR_DATA,hulp);
                  volg_trede[trede][0] = atoi(hulp);
               break;
          case 3 :
                  sprintf(hulp,"%-2d",volg_trede[trede][1]);
                  toets = dialoog_lees_veld(trede+3,40,2,KLEUR_DATA,hulp);
                  volg_trede[trede][1] = atoi(hulp);
               break;
          case 4 :
                  sprintf(hulp,"%-2d",volg_trede[trede][2]);
                  toets = dialoog_lees_veld(trede+3,50,2,KLEUR_DATA,hulp);
                  volg_trede[trede][2] = atoi(hulp);
               break;
          case 5 :
                  sprintf(hulp,"%-2d",volg_trede[trede][3]);
                  toets = dialoog_lees_veld(trede+3,60,2,KLEUR_DATA,hulp);
                  volg_trede[trede][3] = atoi(hulp);
               break;
       }

       switch(toets)
       {
          case CUA_NAAR_BENEDEN:
             trede++;
             break;
          case CUA_NAAR_BOVEN:
             trede--;
             break;
          case CUA_TABULEREN   :
             kollom++;
             if (kollom > 5)
                trede++;
             break;
          case CUA_TAB_TERUG   :
             kollom--;
             if (kollom < 1)
                trede--;
             break;
       }
       if (trede < 1)
          trede = 20;
       if ( trede > 20)
          trede = 1;
       if (kollom < 1)
          kollom = 5;
       if ( kollom > 5)
          kollom = 1;
   }
   while ( toets != CUA_EINDE && toets != CUA_AFBREKEN && toets != CUA_ENTER );

   if (toets != CUA_AFBREKEN )
   {
      /* Bewaar de nieuwe waarde's */
      for (i = 1; i <= 20 ; i++)
      {
         schade1_zet_bonus(i,korting[i]);
         for ( j = 0; j < 4; j++ )
            schade1_zet_trede(i,j,volg_trede[i][j]);
      }
   }
   kleur_window_einde();
   scherm_herstel();
}
