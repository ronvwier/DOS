/* ------------------------------------------------------------------------- */
/* DMCD       : Wijzigen van directory en drive                              */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <string.h>
#include <ctype.h>

#include "direct.h"
#include "dmcd.h"

int dmcd(char nieuw[],char oud_dft_drive[],char oud_nwe_drive[])
{
#ifdef __MSDOS__
   static unsigned drive_nr;
#endif

   /* Vraag huidige dir op de huidige drive op */
   getcwd(oud_dft_drive,64);

   if (nieuw[1] == ':')
      /* Ga naar nieuwe drive */
#ifdef __MSDOS__
      _dos_setdrive((toupper(nieuw[0]) + 1 - 'A'),&drive_nr);
#endif
#ifdef __OS2__
      _chdrive((toupper(nieuw[0]) + 1 - 'A'));
#endif

   getcwd(oud_nwe_drive,64);

   /* Ga naar nieuwe directory */
   chdir(nieuw);

   return DMCD_GELUKT;
}
