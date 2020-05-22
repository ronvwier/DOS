/* ------------------------------------------------------------------------- */
/* VIDEOTEX   : Videotex communicatie protocol                               */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "memory.h"
#include "scherm.h"
#include "videotex.h"

#define MARGE_RECHTS (aantal_kol -1)
#define MARGE_VOET   23

#define MASKER_AG    0x70
#define MASKER_VG    0x8F

typedef unsigned char BYTE;

static int cursor_reg = 0;
static int cursor_kol = 0;
static int cursor_akt = 0;

static BYTE tekst[24][80];
static BYTE kleur[24][80];
static BYTE soort[24][80];     /* 0=tekst, 1=grafisch, 2=hidden */

static int mut_reg      = 0;
static int mut_kol      = 0;
static int mut_lengte   = 0;
static int dummy        = -1;
static int telesoftware = 0;
static int aantal_kol   = 40;

char * videotex_cmd_cls()           { return("\x0C"); }
char * videotex_cmd_del_char()      { return("\x08"); }
char * videotex_cmd_ins_char()      { return("\x09"); }
char * videotex_cmd_del_line()      { return("\x0B"); }
char * videotex_cmd_ins_line()      { return("\x0A"); }
char * videotex_cmd_annuleer()      { return "*0_"; }
char * videotex_cmd_einde()         { return "*90_"; }
char * videotex_cmd_einde_sub()     { return "*9_"; }
char * videotex_cmd_hoofdmenu()     { return "*6_"; }
char * videotex_cmd_top_scherm()    { return "K"; }
char * videotex_cmd_eind_scherm()   { return "J"; }
char * videotex_cmd_terug()         { return "*_"; }
char * videotex_cmd_verder()        { return "_"; }
char * videotex_cmd_ververs()       { return "*00"; }
char * videotex_cmd_naar_boven()    { return "\x0B" ; }     /* was ** */
char * videotex_cmd_naar_beneden()  { return "\x0A"; }      /* was _  */
char * videotex_cmd_naar_links()    { return "\x08" ; }
char * videotex_cmd_naar_rechts()   { return "\x09" ; }
char * videotex_cmd_tab()           { return "\x09" ; }
char * videotex_cmd_tab_terug()     { return "\x0D\x0A\x0B" ; }
char * videotex_cmd_return()        { return "\x0D\x0A" ; }
char * videotex_cmd_help()          { return "*8_"; }

unsigned
char videotex_vertaal_teken(int toets)
{
   unsigned char uit = (unsigned char)toets;

   if (uit == '#') uit = '_';
   uit = uit & 0x7F;

   return uit;
}


static void hide(void);
static void hide()
{
   register int i;

   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[cursor_reg][i] = 0;
      tekst[cursor_reg][i] = ' ';
   }

   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_bg(void);
static void zet_bg()
{
   register int i;
   register BYTE ag;

   ag = (BYTE)(kleur[cursor_reg][cursor_kol] & 0x07);
   ag = (BYTE)(ag * 16);

   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[cursor_reg][i] = (BYTE)(ag + (kleur[cursor_reg][i] & MASKER_VG));
      tekst[cursor_reg][i] = ' ';
   }

   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_bg_zwart(void);
static void zet_bg_zwart()
{
   register int i;

   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[cursor_reg][i] &= MASKER_VG;
      tekst[cursor_reg][i] = ' ';
   }
   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_flash_aan(void);
static void zet_flash_aan()
{
   register int i,j;

   j = cursor_reg;
   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[j][i] |= 0x80;
      tekst[j][i] = ' ';
   }
   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_flash_uit(void);
static void zet_flash_uit()
{
   register int i,j;

   j = cursor_reg;
   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[j][i] &= 0x7F;
      tekst[j][i] = ' ';
   }
   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_txt_vg(int vg);
static void zet_txt_vg(int vg)
{
   register int i,j;

   j = cursor_reg;
   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[j][i] = (BYTE)((kleur[j][i] & MASKER_AG) | vg);
      soort[j][i] = 0;      /* Tekst */
      tekst[j][i] = ' ';
   }
   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void zet_gra_vg(int vg);
static void zet_gra_vg(int vg)
{
   register int i,j;

   j = cursor_reg;
   for (i = cursor_kol; i <= MARGE_RECHTS; i++)
   {
      kleur[j][i] = (BYTE)((kleur[j][i] & MASKER_AG) | vg);
      soort[j][i] = 1;      /* Grafisch */
      tekst[j][i] = ' ';
   }
   mut_reg = cursor_reg;
   mut_kol = cursor_kol;
   mut_lengte = MARGE_RECHTS - cursor_kol + 1;
}

static void schrijf_teken(BYTE teken);
static void schrijf_teken(BYTE teken)
{
   if (teken == 9)
      cursor_kol++;
   else
   {
      switch(soort[cursor_reg][cursor_kol])
      {
         case 0: /* Tekst */
		 tekst[cursor_reg][cursor_kol] = teken;
                 break;
         case 1: /* Grafisch */
                 if (teken >= 0x20 && teken <= 0x3F)
                    tekst[cursor_reg][cursor_kol] = (BYTE)(teken + 0x60);
                 else
                 if (teken >= 0x60 && teken <= 0x7F)
                    tekst[cursor_reg][cursor_kol] = (BYTE)(teken + 0x40);
                 else
                    tekst[cursor_reg][cursor_kol] = teken;
                 break;
      }
      mut_reg    = cursor_reg;
      mut_kol    = cursor_kol;
      mut_lengte = 1;
      cursor_kol++;
   }
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           VERWERKING VIDEOTEX ONTVANGEN TEKENS                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int videotex_teken(unsigned char teken,struct VIDEOTEX_MUT * uit)
{
   static unsigned char vteken = 0;
   register int i;

   mut_reg    = 0;
   mut_kol    = 0;
   mut_lengte = 0;

   if (vteken == 27)    /* Escape */
   {
      switch(teken)
      {
         case 'A': zet_txt_vg(SCHERM_VG_ROOD);                 break;
         case 'B': zet_txt_vg(SCHERM_VG_GROEN);                break;
         case 'C': zet_txt_vg(SCHERM_VG_GEEL + SCHERM_VG_FEL); break;
         case 'D': zet_txt_vg(SCHERM_VG_BLAUW);                break;
         case 'E': zet_txt_vg(SCHERM_VG_PAARS);                break;
         case 'F': zet_txt_vg(SCHERM_VG_CYAAN);                break;
         case 'G': zet_txt_vg(SCHERM_VG_GRYS);                 break;
         case 'H': zet_flash_aan();                            break;
	 case 'I': zet_flash_uit();                            break;
	 case 'L': dummy = -1;          /* Normale hoogte */   break;
         case 'M': dummy = cursor_reg + 1; /* dubbel hoog */   break;
	 case 'Q': zet_gra_vg(SCHERM_VG_ROOD);                 break;
         case 'R': zet_gra_vg(SCHERM_VG_GROEN);                break;
         case 'S': zet_gra_vg(SCHERM_VG_GEEL + SCHERM_VG_FEL); break;
         case 'T': zet_gra_vg(SCHERM_VG_BLAUW);                break;
         case 'U': zet_gra_vg(SCHERM_VG_PAARS);                break;
         case 'V': zet_gra_vg(SCHERM_VG_CYAAN);                break;
         case 'W': zet_gra_vg(SCHERM_VG_GRYS);                 break;
         case 'X': hide();                                     break;
         case '>': hide(); telesoftware = 1;                   break;
	 case '6': aantal_kol = 80;                            break;
         case '7': aantal_kol = 40;                            break;
         case '?':                                             break;
         case 0x5C: zet_bg_zwart();                            break;
	 case 0x5D: zet_bg();                                  break;
      }
      tekst[cursor_reg][cursor_kol] = ' ';
      cursor_kol++;
   }
   else
   {
      switch(teken)
      {
         case 0:
            break;
         case 5:
            break;
         case 8:                               /* COL LEFT */
            cursor_kol--;
            break;
	 case 10:                              /* LINE DOWN */
	    cursor_reg++;
            break;
	 case 11:                              /* LINE UP */
            cursor_reg--;
            break;
         case 12:                              /* HOME en CLEAR */
            videotex_cls();
            break;
         case 13:                              /* CR */
         case 24:                              /* CAN ??? */
            cursor_kol = 0;
	    break;
         case 17:                              /* CURSOR ON */
            cursor_akt = 1;
            break;
	 case 20:                              /* CURSOR OFF */
            cursor_akt = 0;
            break;
         case 27:
            break;
         case 30:                              /* HOME */
            cursor_reg = 0;
            cursor_kol = 0;
            break;
         default:
            schrijf_teken(teken);
      }
   }

   vteken = teken;

   if (cursor_kol < 0)
   {
      cursor_reg--;
      cursor_kol = MARGE_RECHTS;
   }
   if (cursor_kol > MARGE_RECHTS)
   {
      cursor_reg++;
      cursor_kol = 0;
   }

   if (cursor_reg < 0) cursor_reg = MARGE_VOET;
   if (cursor_reg > MARGE_VOET) cursor_reg = 0;

   if (cursor_reg == dummy)
   {
      for (i = 0; i < aantal_kol; i++)
      {
	 if (soort[cursor_reg-1][i] == 1)
	    tekst[cursor_reg][i] = tekst[cursor_reg -1][i];
	 else
	    tekst[cursor_reg][i] = ' ';
	 kleur[cursor_reg][i] = kleur[cursor_reg -1][i];
	 soort[cursor_reg][i] = soort[cursor_reg -1][i];
      }
      dummy = -1;
      mut_reg    = cursor_reg;
      mut_kol    = 0;
      mut_lengte = aantal_kol;
   }
   if (cursor_akt)
   {
      uit->cursor_regel  = cursor_reg;
      uit->cursor_kollom = cursor_kol;
   }
   else
   {
      uit->cursor_regel  = -1;
      uit->cursor_kollom = 0;
   }
   uit->regel  = mut_reg;
   uit->kollom = mut_kol;
   uit->lengte = mut_lengte;
   uit->telesoftware = telesoftware;
   uit->aantal_kol   = aantal_kol;

   return VIDEOTEX_OK;
}

int  videotex_is_cls(unsigned char teken)
{
   return teken == 12;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           GEEF OPGEGEVEN REGEL TERUG                                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int videotex_regel(int regel,unsigned char p_tekst[40],unsigned char p_kleur[40])
{
   memcpy(p_tekst,&tekst[regel][0],40);
   memcpy(p_kleur,&kleur[regel][0],40);

   return VIDEOTEX_OK;
}

int videotex_lange_regel(int regel,unsigned char p_tekst[80],unsigned char p_kleur[80])
{
   memcpy(p_tekst,&tekst[regel][0],80);
   memcpy(p_kleur,&kleur[regel][0],80);

   return VIDEOTEX_OK;
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           MAAK HET SCHERM SCHOON                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int videotex_cls()
{
   memset(tekst,' ',sizeof(tekst));
   memset(soort,0,sizeof(soort));
   memset(kleur,7,sizeof(kleur));
   cursor_reg = 0;
   cursor_kol = 0;
   cursor_akt = 0;
   mut_reg    = 0;
   mut_kol    = 0;
   mut_lengte = -1;
   telesoftware = 0;

   return VIDEOTEX_OK;
}
