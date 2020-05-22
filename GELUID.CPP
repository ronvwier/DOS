/* ------------------------------------------------------------------------- */
/* GELUID                                                                    */
/* Taak       : Geven van geluid                                             */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <conio.h>

#ifdef MSDOS
 #include <bios.h>
#else
 #include <os2.h>
#endif

#include "geluid.h"

#ifdef MSDOS
static void wacht(int honderste_sec)
{
   long start, wacht_tik, huidig, hulp;

   wacht_tik = honderste_sec * 18 / 100;
   _bios_timeofday(_TIME_GETCLOCK,&start);
   huidig = start;

   while( (huidig - start) < wacht_tik)
   {
      _bios_timeofday(_TIME_GETCLOCK,&hulp);
      if (hulp >= huidig)
	 huidig = hulp;
      else
      {
	 /* 24 uurs overgang */
	 wacht_tik -= (huidig - start);
	 start  = hulp;
	 huidig = hulp;
      }
   }
}
#endif

GELUID::GELUID()
{
   status = 0;
   frequentie = 1500;
}

GELUID::~GELUID()
{
   uit();
}

int GELUID::beep()
{

#ifdef MSDOS
   aan();
   freq(1500);
   wacht(12);           /* Wacht 0.12 seconden */
   uit();
#else
   DosBeep(frequentie,120);
#endif

   return 1;
}

int GELUID::freq(int hertz)
{
#ifdef MSDOS
   register unsigned divisor = (unsigned)(1193180L/hertz);

   if (status == 1)
   {
      /* Wijzig de huidige toon */
      outp(0x43, 0xB6) ;
      outp(0x42, divisor & 0xFF) ;
      outp(0x42, divisor >> 8) ;
   }
#endif

   frequentie = hertz;

   return 1;
}

int GELUID::aan()
{
#ifdef MSDOS
   register unsigned divisor = (unsigned)(1193180L/frequentie);

   outp(0x43, 0xB6) ;
   outp(0x42, divisor & 0xFF) ;
   outp(0x42, divisor >> 8) ;
   outp(0x61, inp(0x61) | 0x03) ;
#endif

   status = 1;

   return 1;
}

int GELUID::uit()
{
#ifdef MSDOS
   outp(0x61, inp(0x61) & 0xFC) ;
#endif

   status = 0;

   return 1;
}


