/* ------------------------------------------------------------------------- */
/* CLOCK      : Tonen klokje op scherm in rechter bovenhoek                  */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <memory.h>
#include <bios.h>
#include <dos.h>

_getargv() { };
_setenvp() { };

typedef void interrupt IRQ(...);
IRQ far *oldtimer;

static unsigned video_seg;

/* ------------------------------------------------------------------------- */
/* NEWTIMER   : Deze routine wordt zo'n 18x per seconde aangeroepen en zet   */
/*              dan de tijd op het scherm                                    */
/* ------------------------------------------------------------------------- */

void interrupt newtimer()
{
   static int hours, minutes;
   static long seconds;
   static int y, z;
   static int running = 0;
   static int ticker = 0;
   static long ticks;
   static char bf[8];
   static char vdata[16];
   static char *p;

   if (running == 0)
   {
      /* Signaleer dat we al bezig zijn */
      running = 1;

      if (ticker == 0)
      {
         /* Zorg ervoor dat we pas weer over 5/18 sec. weer aktief worden */
         ticker = 5;

         /* Vraag de BIOS tijd in tikken op (1/18.2 sec.) */
	 _bios_timeofday(_TIME_GETCLOCK,&ticks);
         seconds = (ticks * 10L) / 182L;
         hours   = seconds / 3600;
         minutes = (seconds % 3600) / 60;
         seconds = (seconds % 3600) % 60;

	 /* Converteer de tijd naar een string */
         bf[0] = '0' + (unsigned char)hours / 10;
         bf[1] = '0' + (unsigned char)hours % 10;
         bf[2] = ':';
         bf[3] = '0' + (unsigned char)minutes / 10;
         bf[4] = '0' + (unsigned char)minutes % 10;
         bf[5] = ':';
	 bf[6] = '0' + (unsigned char)seconds / 10;
         bf[7] = '0' + (unsigned char)seconds % 10;

         /* We tonen alles in reverse video */
         memset(vdata, 0x70, 16);

	 /* Toon de tijd */
         for (y = 0, z = 0; y <= 8; y++, z += 2)              /* display */
         vdata[z] = bf[y];
         p = vdata;
         movedata(FP_SEG(p),FP_OFF(p), video_seg, 140, 16);
      }
      ticker--;

      /* We zijn klaar voor deze keer */
      running = 0;
   }

   /* En door naar de volgende timer routine */
   _chain_intr(oldtimer);
}


/* ------------------------------------------------------------------------- */
/* RESSTAY    : Maak het programma resident                                  */
/* ------------------------------------------------------------------------- */

static void resstay(void);
static void resstay()
{
   static unsigned int *ptr_int;

   /* Geef de environment ruimte vrij */
   FP_SEG(ptr_int) = _psp;
   FP_OFF(ptr_int) = 0;
   _dos_freemem(ptr_int[0x2c / 2]);

   /* Haal de loadsize op uit het MCB en stay resident */
   FP_SEG(ptr_int) = (_psp - 1);
   FP_OFF(ptr_int) = 3;
   _dos_keep(0, *ptr_int);
}

/* ------------------------------------------------------------------------- */
/* MAIN       : Start de verwerking                                          */
/* ------------------------------------------------------------------------- */

main(void);
main()
{
   static union REGS regs;

   /* Bepaal scherm adres */
   regs.h.ah = 15; int86(0x10, &regs, &regs);          /* get video mode */
   video_seg = (regs.h.al == 7 ? 0xb000 : 0xb800);

   /* Vervang de tijd interrupt door die van NEWTIMER */
   oldtimer = _dos_getvect(0x1c);
   _dos_setvect(0x1c, (IRQ *)&newtimer);

   /* terminate and stay resident */
   resstay();

   return 0;
}
