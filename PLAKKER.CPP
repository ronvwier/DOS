/* ------------------------------------------------------------------------- */
/* PLAKKER    : Afdrukken stickers                                           */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <bios.h>
#include <memory.h>

#include "toets.h"
#include "cua.h"
#include "scherm.h"
#include "taak.h"
#include "tijd.h"
#include "dialoog.h"
#include "geluid.h"

#define KLEUR_RECHT        kleur_titel_normaal()
#define KLEUR_TITEL        kleur_titel_accent()
#define KLEUR_PFK          kleur_pf()
#define KLEUR_TEKST        kleur_body_tekst_normaal()
#define KLEUR_DATA         kleur_body_veld_normaal()

GELUID geluid;

static  DIALOOG_MENU aktie_menu =
   { { "Plakker", "1. Enkelvoudige afdruk","2. Meervoudige afdruk...","-","3. Afsluiten",NULL } ,
     NULL };

static
unsigned       toets;

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(void)

{
   int i,j;
   int veld;
   int pos;
   char hulp[4][80];
   char str_aant[8];
   FILE *printer;

   memset(hulp,'\0',sizeof(hulp));

   scherm_init();
   scherm_cls(KLEUR_TEKST);
   scherm_tekst(0,0,scherm_aantal_kol(),KLEUR_TITEL,"  PLAKKER 1.0");
   scherm_plaats_tekst(0,scherm_aantal_kol() - 24,KLEUR_RECHT,"(c) 1992 RvW");
   dialoog_toon_menu(1,0,scherm_aantal_kol()-1,aktie_menu);

   pos = (scherm_aantal_kol() - 42) / 2;

   scherm_plaats_tekst(3,pos,KLEUR_TEKST,"           *** plakkers ***");
   scherm_plaats_tekst(5,pos,KLEUR_TEKST,"[                                        ]");
   scherm_plaats_tekst(6,pos,KLEUR_TEKST,"[                                        ]");
   scherm_plaats_tekst(7,pos,KLEUR_TEKST,"[                                        ]");
   scherm_plaats_tekst(8,pos,KLEUR_TEKST,"[                                        ]");

   scherm_tekst(scherm_aantal_reg() -1,0,scherm_aantal_kol(),KLEUR_PFK,"  Enter  Esc=Annuleer  F1=Help  F3=Einde  F10=Actiebalk");

   printer = fopen("PRN","w");

   tijd_permanent_tonen(0,scherm_aantal_kol() -10);
   veld = 0;

   do
   {
      do
      {
         toets = dialoog_lees_veld(5+veld,pos+1,40,KLEUR_DATA,hulp[veld]);

         switch(toets)
         {
            case CUA_NAAR_BENEDEN:
            case CUA_TABULEREN   :
               veld++;
               break;
            case CUA_NAAR_BOVEN:
               veld--;
               break;
         }
         if (veld < 0)
            veld = 3;
         if ( veld > 3 )
            veld = 0;
      }
      while ( !cua_afsluiten(toets) && toets != CUA_AFBREKEN
           && toets != CUA_ENTER && toets != CUA_AKTIE );

      if (   toets == CUA_AKTIE
          || toets == CUA_ENTER)
      {
         i = 0;
         j = 0;
         if (dialoog_kies_menu(1,0,79,aktie_menu,&i,&j) == CUA_ENTER)
         {
            switch(j)
            {
            case 1: /* enkelvoudige afdruk */
                    fprintf(printer,"\n\n");
                    for ( i = 0; i < 4; i++)
                       fprintf(printer,"%s\n",hulp[i]);
                    break;
            case 2: /* meervoudige afdruk */
                    dialoog_popup_start(10,25,30,4);
                    strcpy(str_aant,"0 ");
                    scherm_plaats_tekst(11,30,KLEUR_TEKST,"Aantal plakkers [__]");
                    toets = dialoog_lees_veld(11,47,2,KLEUR_DATA,str_aant);
                    if (cua_enter(toets) || cua_tabuleren(toets))
                    {
                       for (j = 0; j < atoi(str_aant); j++)
                       {
                          fprintf(printer,"\n\n");
                          for ( i = 0; i < 4; i++)
                             fprintf(printer,"%s\n",hulp[i]);
                       }
                    }
                    dialoog_popup_einde();
                    break;
            case 4: /* Afsluiten */
                    toets = CUA_EINDE;
                    break;
            default:
		    geluid.beep();
            }
            fflush(printer);
         }
      }

   }
   while ( !cua_afsluiten(toets) );

   fclose(printer);
   /* Sluit toetsenbord/schermafhandeling af */
   scherm_einde();

   return 0;
}
