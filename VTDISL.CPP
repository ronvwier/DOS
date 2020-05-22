/* ------------------------------------------------------------------------- */
/* VTDIS      : Tonen videotex scherm                                        */
/*                                                                           */
/* (c) 1991,  : R.A. van Wier RI, Amsterdam.                                 */
/*     1992     Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scherm.h"
#include "kleur.h"
#include "vt.h"

#define RECHTEN_KORT "(c) 1991-99, RvW"

static int MARGE_LINKS = 20;
static int dis_mode = 0;

#define MARGE_RECHTS   (MARGE_LINKS + 39)

typedef unsigned char BYTE;

    /*  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F  */
static BYTE gtab_8x[16] =
      {' ',' ',' ',223,' ',221,223,223,' ',223,222,223,223,223,223,223};
static BYTE gtab_9x[16] =
      {' ',221,' ',223,221,221,221,219,220,221,222,219,220,219,219,219};
static BYTE gtab_Ax[16] =
      {' ',' ',222,223,220,221,222,219,222,222,222,219,220,219,219,219};
static BYTE gtab_Bx[16] =
      {220,220,220,219,220,221,219,219,220,219,222,219,220,219,219,219};

static BYTE conv[256];

static BYTE pos1[256];
static BYTE pos2[256];

static void conv_init(void);
static void conv_init()
{
   register int i;

   for (i = 0; i < 256; i++)
      conv[i] = (BYTE)i;
   for (i = 0; i < 16; i++)
      conv[i+0x80] = gtab_8x[i];
   for (i = 0; i < 16; i++)
      conv[i+0x90] = gtab_9x[i];
   for (i = 0; i < 16; i++)
      conv[i+0xA0] = gtab_Ax[i];
   for (i = 0; i < 16; i++)
      conv[i+0xB0] = gtab_Bx[i];

   conv[0x5B] = '' ;
   conv[0x5D] = ' ' ;
   conv[0x5E] = '' ;
   conv[0x5F] = '#' ;
   conv[0x7B] = '¬' ;
   conv[0x7C] = 'º' ;
   conv[0x7D] = '«' ;
   conv[0x7E] = 'ö' ;
   conv[0x7F] = 'þ' ;
}

/* ---------------------------------------------------------------------- */
/* TOON_PFK                                                               */
/* Taak   : Toon betekenis funktie toetsen                                */
/* ---------------------------------------------------------------------- */

static void toon_pfk(void);
static void toon_pfk()
{
   scherm_tekst(24,0,40,KLEUR_PFK,"F1=Help F3=Afsluiten F5=Ververs F9=DOS");
}

/* ---------------------------------------------------------------------- */
/* PLAATS_LAYOUT                                                          */
/* Taak   : Plaats scherm layout                                          */
/* ---------------------------------------------------------------------- */

static void plaats_layout1(void);
static void plaats_layout1()
{
   /* 40 kolloms */
   MARGE_LINKS = 0;

   scherm_cls(SCHERM_VG_GRYS);
   toon_pfk();
   scherm_bewaar();
   scherm_tekst( 0,0,20,KLEUR_TITEL,TITEL);
   scherm_tekst( 0,20,20,KLEUR_TITEL,RECHTEN_KORT);
}

static void cls(void);
static void cls()
{
   scherm_herstel();
   scherm_bewaar();
}

static int vtdis1_init(void);
static int vtdis1_init()
{
   dis_mode = 1;

   if (!scherm_monochroom())
      scherm_breedte(40);

   plaats_layout1();

   conv_init();

   return VT_OK;
}

static int vtdis1_toon(struct VIDEOTEX_MUT * mut);
static int vtdis1_toon(struct VIDEOTEX_MUT * mut)
{
   register int regel, lengte, kollom;

   /* Copieer variabelen naar lokaal */
   lengte = mut->lengte;
   if (lengte > 40)
      lengte = 40;
   regel  = mut->regel;
   kollom = mut->kollom;

   if (lengte < 0)
      cls();
   if (lengte > 0)
   {
      register int i;
      BYTE tekst[40];
      BYTE kleur[40];

      videotex_regel(regel,tekst,kleur);
      if (scherm_monochroom())
      {
	 for (i = kollom; i < (kollom + lengte); i++)
	 {
	    if (i < 40)
	    {
	       if ((kleur[i] & 0x0F) != ((kleur[i] & 0xF0) >> 4))
		  scherm_plaats_teken(regel,i+MARGE_LINKS,7,conv[tekst[i]]);
	       else
		  scherm_plaats_teken(regel,i+MARGE_LINKS,7,' ');
	    }
	 }
      }
      else
      {
	 for (i = kollom; i < (kollom + lengte); i++)
	    if (i < 40)
	       scherm_plaats_teken(regel,i+MARGE_LINKS,kleur[i],conv[tekst[i]]);
      }
   }

   if (mut->cursor_regel < 0)
      scherm_cursor(0);
   else
   {
      if (mut->cursor_kollom < 40)
	 scherm_cursor_plaats(mut->cursor_regel,mut->cursor_kollom+MARGE_LINKS);
      scherm_cursor(1);
   }

   return VT_OK;
}

int vtdis_toon(struct VIDEOTEX_MUT * mut)
{
   int retcd = VT_FOUT;
   retcd = vtdis1_toon(mut);
   return retcd;
}

int vtdis_init(int mode)
{
   unsigned char kleuren[16];
   register int i;

   /* Copieer de high-intensity kleuren naar de low-intensity kleuren
      om GEEL en WIT goed te laten functioneren */
   scherm_geef_kleurenset(kleuren);
   for (i = 1; i < 8; i++)
     kleuren[i] = kleuren[i+8];
   scherm_zet_kleurenset(kleuren);

   vtdis1_init();

   return VT_OK;
}

int vtdis_einde()
{
   /* Sluit schermafhandeling af */
   scherm_herstel();

   return VT_OK;
}

int vtdis_print(FILE * print)
{
   register int i,j;
   BYTE tekst[80];
   BYTE kleur[80];

   for (i = 0; i < 24; i++)
   {
      /* Druk 1 regel af */
      videotex_lange_regel(i,tekst,kleur);
      for (j = 0; j < 80; j++)
	fputc(conv[tekst[j]],print);
      fputc('\n',print);
   }

   /* Geef het einde van het scherm aan */
   fprintf(print,"\f\n");

   return VT_OK;
}

int vtdis_wacht_start()
{
   int KLEUR = SCHERM_VG_ZWART | SCHERM_AG_GRYS;

   scherm_bewaar();

   scherm_plaats_tekst(24,3,KLEUR,"* ER WORDT EVEN GEWACHT, MOMENTJE *");

   return VT_OK;
}

int vtdis_wacht_einde()
{
   scherm_herstel();
   return VT_OK;
}


