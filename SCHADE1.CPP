/* ------------------------------------------------------------------------- */
/* SCHADE1    : Afhandelen I/O op bonus/malus bestand                        */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stdlib.h"
#include "stdio.h"
#include "schade1.h"

static struct TREDE
{
   float bonus;
   short volg_trede[4];
}  trede[32];

static int max_trede = 0;
static int update = 0;

void  schade1_start(void)
{
   FILE * tabel;

   max_trede = 0;
   tabel = fopen("schade.tab","rb");

   if (tabel != NULL )
   {
      while ( fread(&trede[max_trede+1],sizeof(struct TREDE),1,tabel) > 0 )
         max_trede++;
      fclose(tabel);
   }
}

void  schade1_einde(void)
{
   int i;
   FILE * tabel;

   if (update)
   {
      tabel = fopen("schade.tab","wb");
      if (tabel != NULL )
      {
         for ( i = 1; i <= max_trede ; i++ )
            fwrite(&trede[i],sizeof(struct TREDE),1,tabel);
         fclose(tabel);
      }
   }
}

int   schade1_volg_trede(int huidige_trede,int aantal_schade)
{
   register int i;

   if (aantal_schade > 3 )
      i = 3;
   else
      i = aantal_schade;

   return trede[huidige_trede].volg_trede[i];
}

int   schade1_aantal_trede(void)
{
    return max_trede;
}

float schade1_bonus(int trede_nr)
{
   return trede[trede_nr].bonus;
}

void  schade1_zet_trede(int trede_nr,int aantal_schade,int volg_trede)
{
   register int i;

   if (aantal_schade > 3 )
      i = 3;
   else
      i = aantal_schade;

   if (trede_nr > max_trede)
      max_trede = trede_nr;

   if (trede[trede_nr].volg_trede[i] != volg_trede)
   {
      update = 1;
      trede[trede_nr].volg_trede[i] = volg_trede;
   }
}

void  schade1_zet_bonus(int trede_nr,float korting)
{
   if (trede_nr > max_trede)
      max_trede = trede_nr;

   if (trede[trede_nr].bonus != korting)
   {
      trede[trede_nr].bonus = korting;
      update = 1;
   }
}
