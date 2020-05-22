/* ------------------------------------------------------------------------- */
/* SCORE      : Bijhouden hoogste en gemiddelde score standen                */
/*                                                                           */
/* (c) 1988   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#ifdef __MSDOS__
   #include <dos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"

#define AANTAL_SCORE       20

static
struct {
   short    aantal;
   long     score[AANTAL_SCORE];
   long     gem[AANTAL_SCORE];
   long     gesp[AANTAL_SCORE];
   char     naam[AANTAL_SCORE][22];
       }    score_record;
FILE       *score_bestand;

static status = 0;
static char score_naam[64];

void score_init(char spel[])
{
   /* Lees de oude score's in */

   strcpy(score_naam,spel);
   strcat(score_naam,".top");

   score_bestand = fopen(score_naam,"rb");
   if ( score_bestand == NULL )
      score_record.aantal = 0;
   else
   {
      if ( fread(&score_record,sizeof(score_record),1,score_bestand) < 1)
         score_record.aantal = 0;
      fclose(score_bestand);
   }
   status = 1;
}

void score_einde(void)
{
   status = 0;
}

int    score_aantal(void)
{
   return(score_record.aantal);
}

char * score_plaats_naam(int plaats)
{
    return(score_record.naam[plaats -1]);
}

long   score_plaats_max(int plaats)
{
    return(score_record.score[plaats -1]);
}

long   score_plaats_gem(int plaats)
{
    return(score_record.gem[plaats -1]);
}

void score_bewaar(char naam[],long punten)
{
   register
   int         i,
               j;
   char        hulp[80];
   long        hulp_score,
               hulp_gem,
               hulp_gesp;

   if ( status == 1 )
   {
      /* Kijk of de speler al in de lijst staat */

      naam[21] = '\0';
      strupr(naam);
      j  = 99;
      for ( i = 0; i < score_record.aantal; i++ )
      {
          if ( strcmp(naam,score_record.naam[i]) == 0 )
             j = i;
      }
      if ( j < 99 )
      {
          /* Werk de score's (top/gemiddelde) van de speler bij */
          /* over de laatste 10 spelen                          */

          if ( punten > score_record.score[j] )
             score_record.score[j] = punten;
          score_record.gem[j] = ( ( score_record.gem[j] * score_record.gesp[j] )
                                + punten ) / ( score_record.gesp[j] + 1);
          score_record.gesp[j] = score_record.gesp[j] + 1;
          if (score_record.gesp[j] > 10) score_record.gesp[j] = 10;
      }
      else
      {
         if ( score_record.aantal < AANTAL_SCORE )
         {
            /* Voeg speler aan de lijst toe */

            score_record.score[score_record.aantal] = punten;
            score_record.gem[score_record.aantal]   = punten;
            score_record.gesp[score_record.aantal]  = 1;
            strcpy(score_record.naam[score_record.aantal],naam);
            j = score_record.aantal;
            score_record.aantal++;
         }
         else
         {
            if ( punten > score_record.score[AANTAL_SCORE - 1] )
            {
               /* Vervang laagste score door die van deze speler */

               score_record.score[AANTAL_SCORE - 1] = punten;
               score_record.gem[AANTAL_SCORE - 1]   = punten;
               score_record.gesp[AANTAL_SCORE - 1]  = 1;
               strcpy(score_record.naam[AANTAL_SCORE - 1],naam);
               j = AANTAL_SCORE - 1;
            }
         }
      }

      if ( j < 99 )
      {
         /* Kijk of de speler hoger op de ladder hoort */

         while (   ( j > 0 )
               &&  ( score_record.score[j] > score_record.score[j-1] ) )
         {
            /* Plaats de speler 1 trede hoger */

            strcpy(hulp,score_record.naam[j-1]);
            hulp_score = score_record.score[j-1];
            hulp_gem   = score_record.gem[j-1];
            hulp_gesp  = score_record.gesp[j-1];

            strcpy(score_record.naam[j-1],score_record.naam[j]);
            score_record.score[j-1] = score_record.score[j];
            score_record.gem[j-1]   = score_record.gem[j];
            score_record.gesp[j-1]  = score_record.gesp[j];

            strcpy(score_record.naam[j],hulp);
            score_record.score[j] = hulp_score;
            score_record.gem[j]   = hulp_gem;
            score_record.gesp[j]  = hulp_gesp;

            j--;
         }
      }

      /* Schrijf de score's weg */

      score_bestand = fopen(score_naam,"wb");
      if ( score_bestand != NULL )
      {
         fwrite(&score_record,sizeof(score_record),1,score_bestand);
         fclose(score_bestand);
      }
   }
}
