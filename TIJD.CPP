/* ------------------------------------------------------------------------- */
/* TIJD       : Tijd operaties                                               */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#ifdef MSDOS
 #include <bios.h>
#endif

#include <time.h>
#include <string.h>

#include "taak.h"
#include "scherm.h"

static int disp_regel = 0, disp_kol = 70;

/* ------------------------------------------------------------------------- */
/* SHOW_TIJD                                                                 */
/*                                                                           */
/* Taak       : Huidige tijd tonen op opgegeven schermpositie                */
/*                                                                           */
/* Begin      : disp_regel en disp_kol gevuld met schermpositie              */
/*                                                                           */
/* Einde      : tijd op scherm gezet                                         */
/* ------------------------------------------------------------------------- */

static TAAK showtaak;

static TAAKPROC show_tijd;
static void show_tijd()
{
   static long oud = 0, nieuw;
   static char buffer[9] = "", bufoud[9] = "";

#ifdef MSDOS
   _bios_timeofday(_TIME_GETCLOCK,&nieuw);
#else
   /* OS/2 - geen optimalisatie */
   oud   = 0;
   nieuw = 1;
#endif

   if ( nieuw != oud )
   {
      _strtime(buffer);
      if (strcmp(buffer,bufoud) != 0)
      {
	 scherm_vervang_tekst(disp_regel,disp_kol,buffer);
	 strcpy(bufoud,buffer);
      }
      oud = nieuw;
   }
}

/* ------------------------------------------------------------------------- */
/* TIJD_PERMANENT_TONEN                                                      */
/*                                                                           */
/* Taak       : Installatie van de toonroutine als achtergrond taak          */
/*                                                                           */
/* Begin      : regel en kol gevuld met schermpositie waar tijd moet komen   */
/*                                                                           */
/* Einde      : tijd wordt constant bijgewerkt op scherm                     */
/* ------------------------------------------------------------------------- */
void tijd_permanent_tonen(int regel,int kol)
{
   disp_regel = regel;
   disp_kol   = kol;
   showtaak.start(show_tijd);
}
