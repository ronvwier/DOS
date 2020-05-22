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
#include "tijd.h"
#include "vt.h"

#define RECHTEN_KORT "(c) 1991-96, RvW"

static int MARGE_LINKS = 20;
static int toon_telesoftware = 0;
static int dis_mode = 0;
static int replay  = 0;

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
   conv[0x7F] = 'ş' ;
}

/* ---------------------------------------------------------------------- */
/* TOON_PFK                                                               */
/* Taak   : Toon betekenis funktie toetsen                                */
/* ---------------------------------------------------------------------- */

static void toon_pfk(void);
static void toon_pfk()
{
   if (replay)
      scherm_tekst(24,0,scherm_aantal_kol(),KLEUR_PFK," F3=Einde  F7/PgUp=Terug  F8/PgDn=Verder");
   else
   {
      if (toon_telesoftware)
      {
         if (scherm_aantal_kol() >= 80)
            scherm_tekst(24,0,scherm_aantal_kol(),KLEUR_PFK,"  Enter=Laden  Esc=Codes  F1=Help  F3=Afsluiten  F5=Ververs  F9=DOS");
         else
            scherm_tekst(24,0,scherm_aantal_kol(),KLEUR_PFK,"Enter=Laden F1=Help F3=Afsluiten F9=DOS");
      }
      else
      {
         if (scherm_aantal_kol() >= 80)
            scherm_tekst(24,0,scherm_aantal_kol(),KLEUR_PFK,"  Enter=#  Esc=Codes  F1=Help  F3=Afsluiten  F5=Ververs  F9=DOS");
         else
            scherm_tekst(24,0,scherm_aantal_kol(),KLEUR_PFK,"F1=Help F3=Afsluiten F5=Ververs F9=DOS");
      }
   }
}

/* ---------------------------------------------------------------------- */
/* PLAATS_LAYOUT                                                          */
/* Taak   : Plaats scherm layout                                          */
/* ---------------------------------------------------------------------- */

static void plaats_layout0(void);
static void plaats_layout0()
{
   register int i;

   MARGE_LINKS = 20;


   /* 80 kollommen met 40 in gebruik in midden */
   scherm_cls(KLEUR_TEKST);
   scherm_tekst( 0,0,scherm_aantal_kol(),KLEUR_TITEL,TITEL);
   scherm_tekst( 0,MARGE_RECHTS + 3,17,KLEUR_TITEL,RECHTEN_KORT);
   for (i = 0; i < 24; i++)
   {
      scherm_tekst(i,MARGE_LINKS -1,1,SCHERM_VG_ROOD,"İ");
      scherm_tekst(i,MARGE_LINKS,40,7," ");
      scherm_tekst(i,MARGE_RECHTS+1,1,SCHERM_VG_ROOD,"Ş");
   }
   toon_pfk();
   scherm_bewaar();
}

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

static void plaats_layout2(void);
static void plaats_layout2()
{
   /* 40 kolloms verdeeld over 80 posities */
   scherm_cls(SCHERM_VG_GRYS);
   toon_pfk();
   scherm_bewaar();
   scherm_tekst( 0,0,scherm_aantal_kol(),KLEUR_TITEL,TITEL);
   scherm_tekst( 0,62,17,KLEUR_TITEL,RECHTEN_KORT);
}

static void plaats_layout3(void);
static void plaats_layout3()
{
   /* 80 kolloms verdeeld over 80 posities */
   scherm_cls(SCHERM_VG_GRYS);
   toon_pfk();
   scherm_bewaar();
}

static void cls(void);
static void cls()
{
   scherm_herstel();
   scherm_bewaar();
}

static int vtdis1_init(int kols);
static int vtdis1_init(int kols)
{
   if (kols < 80)
   {
      dis_mode = 1;
      scherm_breedte(40);
   }
   else
   {
      dis_mode = 0;
      scherm_breedte(80);
   }

   if (scherm_aantal_kol() >= 80)
   {
      tijd_permanent_tonen(24,scherm_aantal_kol() -10);
      plaats_layout0();
   }
   else
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
               if (kleur[i])
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

   if (mut->cursor_regel < 0 || replay)
      scherm_cursor(0);
   else
   {
      if (mut->cursor_kollom < 40)
         scherm_cursor_plaats(mut->cursor_regel,mut->cursor_kollom+MARGE_LINKS);
      scherm_cursor(1);
   }

   return VT_OK;
}

static int vtdis2_init(void);
static int vtdis2_init()
{
   register int i;

   scherm_breedte(80);
   dis_mode = 2;

   conv_init();

   for (i = 0; i < 256; i++)
   {
      pos1[i] = conv[i];
      pos2[i] = ' ';
   }

   /* Invullen grafische tekens */
   pos1[0x7F] = 'ş' ;
   pos2[0x7F] = 'ş' ;

   /* De originele 2x serie */
   pos1[0x80] = ' '; pos2[0x80] = ' ';

   pos1[0x81] = 'ß'; pos2[0x81] = ' ';

   pos1[0x82] = ' '; pos2[0x82] = 'ß';

   pos1[0x83] = 'ß'; pos2[0x83] = 'ß';

   pos1[0x84] = 'Ü'; pos2[0x84] = ' ';

   pos1[0x85] = 'ß'; pos2[0x85] = ' ';

   pos1[0x86] = 'ß'; pos2[0x86] = 'ß';

   pos1[0x87] = 'ß'; pos2[0x87] = 'ß';

   pos1[0x88] = ' '; pos2[0x88] = 'Ü';

   pos1[0x89] = 'ß'; pos2[0x89] = 'ß';

   pos1[0x8A] = ' '; pos2[0x8A] = 'ß';

   pos1[0x8B] = 'ß'; pos2[0x8B] = 'ß';

   pos1[0x8C] = 'Ü'; pos2[0x8C] = 'Ü';

   pos1[0x8D] = 'ß'; pos2[0x8D] = 'ß';

   pos1[0x8E] = 'ß'; pos2[0x8E] = 'ß';

   pos1[0x8F] = 'ß'; pos2[0x8F] = 'ß';

   /* De orginele 3x serie */
   pos1[0x90] = 'Ü'; pos2[0x90] = ' ';

   pos1[0x91] = 'Û'; pos2[0x91] = ' ';

   pos1[0x92] = 'Ü'; pos2[0x92] = 'ß';

   pos1[0x93] = 'Û'; pos2[0x93] = 'ß';

   pos1[0x94] = 'Ü'; pos2[0x94] = ' ';

   pos1[0x95] = 'Û'; pos2[0x95] = ' ';

   pos1[0x96] = 'Ü'; pos2[0x96] = 'ß';

   pos1[0x97] = 'Û'; pos2[0x97] = 'ß';

   pos1[0x98] = 'Ü'; pos2[0x98] = 'Ü';

   pos1[0x99] = 'Û'; pos2[0x99] = 'Ü';

   pos1[0x9A] = 'Ü'; pos2[0x9A] = 'ß';

   pos1[0x9B] = 'Û'; pos2[0x9B] = 'ß';

   pos1[0x9C] = 'Ü'; pos2[0x9C] = 'Ü';

   pos1[0x9D] = 'Û'; pos2[0x9D] = 'Ü';

   pos1[0x9E] = 'Ü'; pos2[0x9E] = 'ß';

   pos1[0x9F] = 'Û'; pos2[0x9F] = 'ß';

   /* de orginele 6x serie */
   pos1[0xA0] = ' '; pos2[0xA0] = 'Ü';

   pos1[0xA1] = 'ß'; pos2[0xA1] = 'Ü';

   pos1[0xA2] = ' '; pos2[0xA2] = 'ß';

   pos1[0xA3] = 'ß'; pos2[0xA3] = 'ß';

   pos1[0xA4] = 'Ü'; pos2[0xA4] = 'Ü';

   pos1[0xA5] = 'ß'; pos2[0xA5] = 'Ü';

   pos1[0xA6] = 'Ü'; pos2[0xA6] = 'ß';

   pos1[0xA7] = 'ß'; pos2[0xA7] = 'ß';

   pos1[0xA8] = ' '; pos2[0xA8] = 'Ü';

   pos1[0xA9] = 'ß'; pos2[0xA9] = 'Ü';

   pos1[0xAA] = ' '; pos2[0xAA] = 'Û';

   pos1[0xAB] = 'ß'; pos2[0xAB] = 'Û';

   pos1[0xAC] = 'Ü'; pos2[0xAC] = 'Ü';

   pos1[0xAD] = 'ß'; pos2[0xAD] = 'Ü';

   pos1[0xAE] = 'ß'; pos2[0xAE] = 'Û';

   pos1[0xAF] = 'ß'; pos2[0xAF] = 'Û';

   /* de orginele 7x serie */
   pos1[0xB0] = 'Ü'; pos2[0xB0] = 'Ü';

   pos1[0xB1] = 'Ü'; pos2[0xB1] = 'Ü';

   pos1[0xB2] = 'Ü'; pos2[0xB2] = 'Ü';

   pos1[0xB3] = 'Ü'; pos2[0xB3] = 'Ü';

   pos1[0xB4] = 'Ü'; pos2[0xB4] = 'Ü';

   pos1[0xB5] = 'Û'; pos2[0xB5] = 'Ü';

   pos1[0xB6] = 'Ü'; pos2[0xB6] = 'Ü';

   pos1[0xB7] = 'Û'; pos2[0xB7] = 'Ü';

   pos1[0xB8] = 'Ü'; pos2[0xB8] = 'Ü';

   pos1[0xB9] = 'Ü'; pos2[0xB9] = 'Ü';

   pos1[0xBA] = 'Ü'; pos2[0xBA] = 'Û';

   pos1[0xBB] = 'Ü'; pos2[0xBB] = 'Û';

   pos1[0xBC] = 'Ü'; pos2[0xBC] = 'Ü';

   pos1[0xBD] = 'Û'; pos2[0xBD] = 'Ü';

   pos1[0xBE] = 'Ü'; pos2[0xBE] = 'Û';

   pos1[0xBF] = 'Û'; pos2[0xBF] = 'Û';

   plaats_layout2();

   return VT_OK;
}

static int vtdis2_toon(struct VIDEOTEX_MUT * mut);
static int vtdis2_toon(struct VIDEOTEX_MUT * mut)
{
   int lengte;

   lengte = mut->lengte;

   if (lengte < 0)
      cls();
   if (lengte > 0)
   {
      register int i,j;
      BYTE tekst[40];
      BYTE kleur[40];

      videotex_regel(mut->regel,tekst,kleur);
      if (scherm_monochroom())
      {
         for (i = mut->kollom; i < (mut->kollom + lengte); i++)
         {
            if (i < 40)
            {
               j = i*2;
               if (kleur[i])
               {
                  scherm_plaats_teken(mut->regel,j  ,7,pos1[tekst[i]]);
                  scherm_plaats_teken(mut->regel,j+1,7,pos2[tekst[i]]);
               }
               else
               {
                  scherm_plaats_teken(mut->regel,j,7,' ');
                  scherm_plaats_teken(mut->regel,j+1,7,' ');
               }
            }
         }
      }
      else
      {
         for (i = mut->kollom; i < (mut->kollom + lengte); i++)
         {
            if (i < 40)
            {
               j = i*2;
               scherm_plaats_teken(mut->regel,j  ,kleur[i],pos1[tekst[i]]);
               scherm_plaats_teken(mut->regel,j+1,kleur[i],pos2[tekst[i]]);
            }
         }
      }
   }
   if (mut->cursor_regel < 0 || replay)
      scherm_cursor(0);
   else
   {
      if (mut->cursor_kollom < 40)
         scherm_cursor_plaats(mut->cursor_regel,mut->cursor_kollom*2);
      scherm_cursor(1);
   }

   return VT_OK;
}

static int vtdis3_init(void);
static int vtdis3_init()
{
   scherm_breedte(80);
   plaats_layout3();

   conv_init();

   return VT_OK;
}

static int vtdis3_toon(struct VIDEOTEX_MUT * mut);
static int vtdis3_toon(struct VIDEOTEX_MUT * mut)
{
   register int regel, lengte, kollom;

   /* Copieer variabelen naar lokaal */
   lengte = mut->lengte;
   regel  = mut->regel;
   kollom = mut->kollom;

   if (lengte < 0)
      cls();
   if (lengte > 0)
   {
      register int i;
      BYTE tekst[80];
      BYTE kleur[80];

      videotex_lange_regel(regel,tekst,kleur);
      if (scherm_monochroom())
      {
         for (i = kollom; i < (kollom + lengte); i++)
         {
            if (kleur[i])
               scherm_plaats_teken(regel,i,7,conv[tekst[i]]);
            else
               scherm_plaats_teken(regel,i,7,' ');
         }
      }
      else
      {
         for (i = kollom; i < (kollom + lengte); i++)
            scherm_plaats_teken(regel,i,kleur[i],conv[tekst[i]]);
      }
   }

   if (mut->cursor_regel < 0 || replay)
      scherm_cursor(0);
   else
   {
      scherm_cursor_plaats(mut->cursor_regel,mut->cursor_kollom);
      scherm_cursor(1);
   }

   return VT_OK;
}

int vtdis_toon(struct VIDEOTEX_MUT * mut)
{
   int retcd = VT_FOUT;
   static int aantal_kol = 40;

   if (mut->aantal_kol != aantal_kol)
   {
      vtdis_einde();
      aantal_kol = mut->aantal_kol;
      if (aantal_kol > 40)
         vtdis3_init();
      else
         vtdis_init(dis_mode);
   }

   if (aantal_kol > 40)
      retcd = vtdis3_toon(mut);
   else
   {
      if (dis_mode != 2)
         retcd = vtdis1_toon(mut);
      else
         retcd = vtdis2_toon(mut);
   }

   if (mut->telesoftware != toon_telesoftware)
   {
      toon_telesoftware = mut->telesoftware;
      toon_pfk();
   }

   return retcd;
}

int vtdis_init(int mode)
{
   char kleuren[16];
   register int i;

   /* Copieer de high-intensity kleuren naar de low-intensity kleuren
      om GEEL en WIT goed te laten functioneren */
   scherm_geef_kleurenset(kleuren);
   for (i = 1; i < 8; i++)
     kleuren[i] = kleuren[i+8];
   scherm_zet_kleurenset(kleuren);

   switch(mode)
   {
      case 0 : vtdis1_init(80);    break;
      case 1 : vtdis1_init(40);    break;
      case 2 : vtdis2_init();      break;
      default: return VT_FOUT;
   }

   return VT_OK;
}

int vtdis_init_replay(int mode)
{
   replay = 1;

   return vtdis_init(mode);
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
   int MARGE_LINKS;

   if (scherm_aantal_kol() >= 80)
      MARGE_LINKS = 24;
   else
      MARGE_LINKS = 4;

   scherm_bewaar();

   scherm_plaats_tekst(10,MARGE_LINKS,KLEUR,"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
   scherm_plaats_tekst(11,MARGE_LINKS,KLEUR,"³ AUTOMATISCHE BESTURING      ³");
   scherm_plaats_tekst(12,MARGE_LINKS,KLEUR,"³                             ³");
   scherm_plaats_tekst(13,MARGE_LINKS,KLEUR,"³ er wordt even gewacht!      ³");
   scherm_plaats_tekst(14,MARGE_LINKS,KLEUR,"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");

   return VT_OK;
}

int vtdis_wacht_einde()
{
   scherm_herstel();
   return VT_OK;
}


