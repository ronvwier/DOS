/* ------------------------------------------------------------------------- */
/* WEKKER     : Wachten op tijdsverloop                                      */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#ifdef MSDOS
   #include <bios.h>
   #include <dos.h>
#endif
#ifdef __OS2__
   #include <os2.h>
#endif


#include "wekker.h"
#include "taak.h"

static long start, wacht_tik, huidig, hulp;

/* void wekker_start(long msec); */
void wekker_start(long msec)
{
#ifdef __MSDOS__
   struct dostime_t tijd;
#endif
#ifdef __OS2__
   DATETIME tijd;
#endif

   long heden;

   wacht_tik = (long)(((msec * 18)/1000L) + 1);

#ifdef __OS2__
   DosGetDateTime(&tijd);
   start = ((tijd.hours*3600L) + (tijd.minutes*60L) + tijd.seconds) * 18L
    + ( tijd.hundredths / 5);
#endif

#ifdef MSDOS
   _bios_timeofday(_TIME_GETCLOCK,&start);
#endif

   huidig = start;
}

int  wekker_alarm(void);
int  wekker_alarm()
{
#ifdef MSDOS
   struct dostime_t tijd;

   if (_bios_timeofday(_TIME_GETCLOCK,&hulp) == 0)
      huidig = hulp;
   else
   {
      /* 24 uurs overgang */
      wacht_tik -= (huidig - start);
      start  = hulp;
      huidig = hulp;
   }
#endif

#ifdef __OS2__
   DATETIME tijd;

   DosGetDateTime(&tijd);
   huidig = ((tijd.hours*3600L) + (tijd.minutes*60L) + tijd.seconds) * 18L
    + ( tijd.hundredths / 5);
#endif

   if ( (huidig - start) < wacht_tik)
      return 0;
   else
      return 1;
}

/* void wekker_wacht(long msec); */
void wekker_wacht(long msec)
{
   TAAK doewat;

   wekker_start(msec);
   while( !wekker_alarm() ) doewat.run();
}
