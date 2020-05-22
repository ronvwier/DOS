/* ------------------------------------------------------------------------- */
/* VTDISLD    : Tonen popup voor telesoftware download                       */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "scherm.h"
#include "kleur.h"
#include "vtdisld.h"

static int MARGE_LINKS = 20;

#define KLEUR SCHERM_VG_ZWART | SCHERM_AG_GRYS

void vtdisld_start()
{
   if (scherm_aantal_kol() >= 80)
      MARGE_LINKS = 24;
   else
      MARGE_LINKS = 4;

   scherm_bewaar();

   scherm_plaats_tekst( 6,MARGE_LINKS,KLEUR,"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
   scherm_plaats_tekst( 7,MARGE_LINKS,KLEUR,"³ TELESOFTWARE LADEN          ³");
   scherm_plaats_tekst( 8,MARGE_LINKS,KLEUR,"³                             ³");
   scherm_plaats_tekst( 9,MARGE_LINKS,KLEUR,"³ Bestanden. . :              ³");
   scherm_plaats_tekst(10,MARGE_LINKS,KLEUR,"³                             ³");
   scherm_plaats_tekst(11,MARGE_LINKS,KLEUR,"³ Naam bestand :              ³");
   scherm_plaats_tekst(12,MARGE_LINKS,KLEUR,"³ Beelden. . . :              ³");
   scherm_plaats_tekst(13,MARGE_LINKS,KLEUR,"³                             ³");
   scherm_plaats_tekst(14,MARGE_LINKS,KLEUR,"³ Fouten . . . : geen         ³");
   scherm_plaats_tekst(15,MARGE_LINKS,KLEUR,"ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´");
   scherm_plaats_tekst(16,MARGE_LINKS,KLEUR,"³ Esc=Annuleren van laden     ³");
   scherm_plaats_tekst(17,MARGE_LINKS,KLEUR,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
}

void vtdisld_file(char file[],int file_nr)
{
    char hulp[80];

    sprintf(hulp,"³ Bestanden. . : %-4d         ³",file_nr);
    scherm_plaats_tekst( 9,MARGE_LINKS,KLEUR,hulp);
    sprintf(hulp,"³ Naam bestand : %-12.12s ³",file);
    scherm_plaats_tekst(11,MARGE_LINKS,KLEUR,hulp);
}

void vtdisld_blok(int blok_nr)
{
    char hulp[80];

    sprintf(hulp,"³ Beelden. . . : %-4d         ³",blok_nr);
    scherm_plaats_tekst(12,MARGE_LINKS,KLEUR,hulp);
}

void vtdisld_fout(int fout_nr)
{
    char hulp[80];

    sprintf(hulp,"³ Fouten . . . : %-4d         ³",fout_nr);
    scherm_plaats_tekst(14,MARGE_LINKS,KLEUR,hulp);
}

void vtdisld_einde()
{
   printf("\a");
   scherm_herstel();
}
