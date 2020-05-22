/* ------------------------------------------------------------------------- */
/* DMGROEP    : Toon/wijzig groep gegevens                                   */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "dmgroep.h"
#include "tekst.h"
#include "scherm.h"
#include "string.h"
#include "stdio.h"
#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "dm.h"

static char groep_titel[32] = "";

int    dmgroep_vraag(void)
{
   int toets;
   int retcode = DMGROEP_MISLUKT;
   int BASIS_REG;
   int BASIS_KOL;

   BASIS_REG = (scherm_aantal_reg() / 2) - 4;
   BASIS_KOL = (scherm_aantal_kol() / 2) - 15;

   dialoog_popup_start(BASIS_REG,BASIS_KOL,24,3);

   /* Laat de vaste tekst zien */
   scherm_plaats_tekst(BASIS_REG+1,BASIS_KOL+2,KLEUR_TEKST,"Titel : ............");

   /* Vraag de titel totdat op enter of escape wordt gedrukt */
   do
   {
      toets = dialoog_lees_veld((BASIS_REG+1),(BASIS_KOL+10),12,KLEUR_DATA,groep_titel);
      if (toets == CUA_HELP)
         dialoog_help("De titel van deze groep verschijnt\nin de horizontale menu balk\n\nOnder deze groep komen keuze's");
   }
   while (toets != CUA_AFBREKEN && toets != CUA_ENTER);

   tekst_trim(groep_titel);

   dialoog_popup_einde();

   if (toets == CUA_ENTER && strlen(groep_titel) > 0)
      retcode = DMGROEP_GELUKT;

   return retcode;
}

char * dmgroep_titel(void)
{
   return groep_titel;
}

void   dmgroep_zet_titel(char titel[])
{
   strcpy(groep_titel,titel);
}

void   dmgroep_leeg(void)
{
   dmgroep_zet_titel("");
}
