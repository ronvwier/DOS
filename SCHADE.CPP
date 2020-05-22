/* ------------------------------------------------------------------------- */
/* SCHADE     : Berekening premieverhoging bij autoschade                    */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <memory.h>

#include "toets.h"
#include "cua.h"
#include "scherm.h"
#include "taak.h"
#include "tijd.h"
#include "dialoog.h"
#include "schade1.h"
#include "schade2.h"
#include "schade3.h"

#define KLEUR_RECHT        kleur_titel_normaal()
#define KLEUR_TITEL        kleur_titel_accent()
#define KLEUR_PFK          kleur_pf()
#define KLEUR_TEKST        kleur_body_tekst_normaal()
#define KLEUR_HELP         kleur_body_tekst_normaal()
#define KLEUR_VERZ_TEKST   kleur_body_tekst_normaal()
#define KLEUR_VERZ_DATA    kleur_body_veld_normaal()

static  DIALOOG_MENU aktie_menu =
   { { "Reken", NULL } ,
     { "Bonus/Malus-Tabel",NULL },
     NULL };

static
unsigned       toets;

/* ---------------------------------------------------------------------- */
/* Plaats hulp scherm                                                     */
/* Taak   : Tonen hulpscherm                                              */
/* ---------------------------------------------------------------------- */

static void plaats_hulp_scherm(void)
{
   scherm_bewaar();
   kleur_window_help_start();
   scherm_plaats_tekst( 9,10,KLEUR_HELP," ษอออออออออออออออออออออออออออออออออออออออออออออออออออออป ");
   scherm_plaats_tekst(10,10,KLEUR_HELP," บ  Toets            Betekenis                         บ ");
   scherm_plaats_tekst(11,10,KLEUR_HELP," บ                                                     บ ");
   scherm_plaats_tekst(12,10,KLEUR_HELP," บ  Enter            Start verwerking                  บ ");
   scherm_plaats_tekst(13,10,KLEUR_HELP," บ  Esc              Funktie afbreken                  บ ");
   scherm_plaats_tekst(14,10,KLEUR_HELP," บ  F1               Help (=deze tekst)                บ ");
   scherm_plaats_tekst(15,10,KLEUR_HELP," บ  F3               Einde                             บ ");
   scherm_plaats_tekst(16,10,KLEUR_HELP," บ  F10              Aktiveren andere funktie          บ ");
   scherm_plaats_tekst(17,10,KLEUR_HELP," บ                                                     บ ");
   scherm_plaats_tekst(18,10,KLEUR_HELP," บ  Druk op een toets om verder te gaan                บ ");
   scherm_plaats_tekst(19,10,KLEUR_HELP," ศอออออออออออออออออออออออออออออออออออออออออออออออออออออผ ");
   toets_wacht();
   kleur_window_help_einde();
   scherm_herstel();
}

/* ---------------------------------------------------------------------- */
/* PLAATS_LAYOUT                                                          */
/* Taak   : Plaats scherm layout                                          */
/* ---------------------------------------------------------------------- */

static void plaats_layout(void)
{
   register
   int         i;

   scherm_cls(KLEUR_VERZ_TEKST);
   scherm_cursor_plaats(0,0);
   scherm_cursor_kleur(KLEUR_TITEL);
   for ( i = 0; i < 80; i++ )
      scherm_schrijf_teken(' ');
   scherm_plaats_tekst(0,2,KLEUR_TITEL,"SCHADE 2.1");
   scherm_plaats_tekst(0,56,KLEUR_RECHT,"(c) 1990 RvW");
   dialoog_toon_menu(1,0,79,aktie_menu);

   scherm_plaats_tekst(3,2,KLEUR_VERZ_TEKST,"Bruto jaarpremie . . . . . . [        ]");
   scherm_plaats_tekst(4,2,KLEUR_VERZ_TEKST,"Huidige bonus/malus trede. . [  ]");
   scherm_plaats_tekst(5,2,KLEUR_VERZ_TEKST,"Reken rente percentage . . . [  ]");

   scherm_plaats_tekst(7,2,KLEUR_VERZ_TEKST,"Kortingspercentage . . . . :       %");
   scherm_plaats_tekst(8,2,KLEUR_VERZ_TEKST,"Netto jaarpremie . . . . . :");

   for ( i = 0; i < 80; i++ )
      scherm_plaats_teken(24,i,KLEUR_PFK,' ');
   scherm_vervang_tekst(24,0,"  Enter=Verwerk  Esc=Annuleer  F1=Help  F3=Einde  F10=Balk");

   tijd_permanent_tonen(0,scherm_aantal_kol() -10);
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(void)

{
   int i,j;
   int veld;
   char hulp[80];
   double bruto = 0.0, netto = 0.0;
   int    huidige_trede = 1;
   int rente_perc_int = 10;

   scherm_init();
   plaats_layout();
   schade1_start();

   veld = 1;

   do
   {
      do
      {
	 sprintf(hulp,"%-8.2f",bruto);
         scherm_plaats_tekst(3,32,KLEUR_VERZ_DATA,hulp);
         sprintf(hulp,"%-2d",huidige_trede);
         scherm_plaats_tekst(4,32,KLEUR_VERZ_DATA,hulp);
         sprintf(hulp,"%-2d",rente_perc_int);
         scherm_plaats_tekst(5,32,KLEUR_VERZ_DATA,hulp);

         switch(veld)
         {
            case 1 :
                  sprintf(hulp,"%-8.2f",bruto);
                  toets = dialoog_lees_veld(3,32,8,KLEUR_VERZ_DATA,hulp);
                  bruto = atof(hulp);
	       break;
            case 2 :
                  sprintf(hulp,"%-2d",huidige_trede);
                  toets = dialoog_lees_veld(4,32,2,KLEUR_VERZ_DATA,hulp);
                  huidige_trede = atoi(hulp);
               break;
            case 3 :
                  sprintf(hulp,"%-2d",rente_perc_int);
                  toets = dialoog_lees_veld(5,32,2,KLEUR_VERZ_DATA,hulp);
                  rente_perc_int = atoi(hulp);
               break;
         }

         sprintf(hulp,"%-6.2f",schade1_bonus(huidige_trede));
         scherm_plaats_tekst(7,32,kleur_body_tekst_accent(),hulp);
         netto = bruto - (schade1_bonus(huidige_trede)*bruto / 100);
         sprintf(hulp,"%-8.2f",netto);
         scherm_plaats_tekst(8,32,kleur_body_tekst_accent(),hulp);

         switch(toets)
         {
            case CUA_NAAR_BENEDEN:
	    case CUA_TABULEREN   :
               veld++;
               break;
            case CUA_NAAR_BOVEN:
               veld--;
               break;
            case CUA_HELP       :
                plaats_hulp_scherm();
                break;
         }
         if (veld < 1)
            veld = 3;
         if ( veld > 3 )
            veld = 1;
      }
      while ( toets != CUA_EINDE && toets != CUA_AFBREKEN
           && toets != CUA_ENTER && toets != CUA_AKTIE );

      if (   toets == CUA_AKTIE
          || toets == CUA_ENTER)
      {
         i = 0;
	 j = 0;
         if (dialoog_kies_menu(1,0,79,aktie_menu,&i,&j) == CUA_ENTER)
         {
            if ( i == 0 )
               /* Voer de berekeningen uit en toon het resultaat */
               schade3(bruto,huidige_trede, (double)(rente_perc_int / 100.0));

            if ( i == 1 )
               /* Laat de tabel wijzigen */
               schade2();
         }
         toets = 0;
      }

   }
   while ( toets != CUA_EINDE && toets != CUA_AFBREKEN );

   /* Sluit toetsenbord/schermafhandeling af */
   schade1_einde();
   scherm_einde();

   return 0;
}
