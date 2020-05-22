/* ------------------------------------------------------------------------- */
/* SCHADE3    : Gevolgen voor premie op scherm laten zien                    */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
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
#include "schade3.h"

#define KLEUR_TEKST   kleur_body_tekst_normaal()

static
unsigned       toets;

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void schade3(double bruto,int huidige_trede,double rente)
{
   register int i,j;
   char hulp[80];
   int volg[4];
   double premie[4];
   double jaarprem[4];
   double reken_tot[4];
   double corr_faktor = 1.0;

   /* Initialiseer eerste element */
   for (i = 0; i < 4; i++ )
   {
      premie[i] = 0;
      volg[i] = schade1_volg_trede(huidige_trede,i);
   }

   scherm_bewaar();
   kleur_window_start();

   scherm_plaats_tekst(4,10,KLEUR_TEKST,     "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
   scherm_plaats_tekst(5,10,KLEUR_TEKST,     "³ Jaar / Extra premie bij     1 schade  2 schades 3 schades ³");
      scherm_plaats_tekst(6,10,KLEUR_TEKST,  "³ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ³");

   for (j = 0; j < 4; j++ )
      reken_tot[j] = 0;

   for (i = 1; i < 14 ; i++ )
   {
      /* Bereken de premies */
      for (j = 0; j < 4; j++ )
      {
         jaarprem[j] = (bruto * (1.0 - (schade1_bonus(volg[j]) / 100.0)));
         premie[j]  += jaarprem[j];
      }
      for (j = 1; j < 4; j++ )
         reken_tot[j] += (jaarprem[j] - jaarprem[0]) / corr_faktor;

      /* Laat de premies zien */
      sprintf(hulp,"³ t/m %3de jaar               %9.2f %9.2f %9.2f ³",
         i,premie[1]-premie[0],premie[2]-premie[0],premie[3]-premie[0]);
      scherm_plaats_tekst(6+i,10,KLEUR_TEKST,hulp);

      /* Bepaal de vervolgtredes */
      for (j = 0; j < 4; j++ )
         volg[j] = schade1_volg_trede(volg[j],0);

      corr_faktor = corr_faktor * (1.0 + rente);
   }
   scherm_plaats_tekst(6+i,10,KLEUR_TEKST,   "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´");
   sprintf(hulp,"³ Totaal rente gecorrigeerd   %9.2f %9.2f %9.2f ³",
                                               reken_tot[1],reken_tot[2],reken_tot[3]);
   scherm_plaats_tekst(7+i,10,KLEUR_TEKST,hulp);
   scherm_plaats_tekst(8+i,10,KLEUR_TEKST,   "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");

   /* Wacht op een toetsaanslag */
   toets_wacht();
   kleur_window_einde();
   scherm_herstel();
}
