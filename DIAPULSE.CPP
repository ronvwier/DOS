/* ------------------------------------------------------------------------- */
/* DIAPULSE   : Aansturen van diaprojector via de COMx poort op de DTR pin   */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <conio.h>

#include "wekker.h"
#include "diapulse.h"

static int async_adres = 0x3f8;

#define MCR (async_adres + 4)
#define ASYNC_MC_DTR            1

int diapulse_init(int comm_poort)
{
   switch(comm_poort)
   {
      case 1:      async_adres = 0x3f8;     break;
      case 2:      async_adres = 0x2f8;     break;
      case 3:      async_adres = 0x3e8;     break;
      case 4:      async_adres = 0x2e8;     break;
      default:     return DIAPULSE_FOUT;
   }
   outp(MCR,0);

   return DIAPULSE_OK;
}

int diapulse_vooruit()
{
   outp(MCR,ASYNC_MC_DTR);
   wekker_wacht(0.1);
   outp(MCR,0);

   return DIAPULSE_OK;
}

int diapulse_achteruit()
{
   outp(MCR,ASYNC_MC_DTR);
   wekker_wacht(0.5);
   outp(MCR,0);

   return DIAPULSE_OK;
}

int diapulse_einde()
{
   outp(MCR,0);

   return DIAPULSE_OK;
}
