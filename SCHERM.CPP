/* ------------------------------------------------------------------------- */
/* SCHERM                                                                    */
/* Taak       : Schermafhandeling                                            */
/*                                                                           */
/* (c) 1988   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#ifdef MSDOS
 #include <bios.h>
 #include <dos.h>
#endif

#ifdef __OS2__
 #define INCL_VIO
 #define INCL_KBD
 #define INCL_MOU

 #include <os2.h>
 #include <conio.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scherm.h"

#define SCHERM_BLANKO      7

static int cursor_pos = 0;
static int cursor_zichtbaar = 1;
static int cursor_kleur      = SCHERM_BLANKO;
static int cursor_kleur_bits = SCHERM_BLANKO << 8;

static int scherm_kolnr = 80;
static int scherm_regnr = 25;

static int log_kolnr    = 80;
static int log_regnr    = 25;

static int is_monochroom = 0;
static int is_lcd        = 0;

static int is_grys       = 0;

static int is_ega_vga    = 0;     /* 0=Geen EGA/VGA , 1=EGA, 2=VGA */

#ifdef MSDOS
static
unsigned far *ptr_scherm = NULL;
#endif

#ifdef __OS2__
static HVIO hvio = 0;
#endif
		
/* De initieele kleurenset is die van de EGA/VGA tekstmode */
static
char start_kleurenset[16] =
{0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F };
static
char huidige_kleurenset[16] =
{0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F };

static struct bewaar
{
   struct bewaar *ptr_next;
   int            cursor_pos;
   int            cursor_kleur;
   int            mode;
   int            cursor_zichtbaar;
   char           kleurenset[16];
   char           scherm[60*132*2];
   unsigned short lengte;
}  *ptr_first = NULL;

/* ---------------------------------------------------------------------- */
/* PLAATS_CURSOR                                                          */
/*                                                                        */
/* Taak   : Cursor op gegeven schermpositie zetten                        */
/*                                                                        */
/* Begin  : scherm_kolnr en scherm_regnr gevuld                           */
/*                                                                        */
/* Einde  : cursor op opgegeven positie                                   */
/* ---------------------------------------------------------------------- */

static void plaats_cursor(void);
static void plaats_cursor()
{
   /* Pas cursor positie eventueel aan */
   cursor_pos %= scherm_kolnr*scherm_regnr;

   if (cursor_zichtbaar)
   {
#ifdef MSDOS
      union REGS regs;
      regs.h.ah = 2;
      regs.h.bh = 0;
      regs.h.dh = (unsigned char)(cursor_pos / scherm_kolnr);
      regs.h.dl = (unsigned char)(cursor_pos % scherm_kolnr);
      int86(0x10,&regs,&regs);
#endif
#ifdef __OS2__
      VioSetCurPos((cursor_pos / scherm_kolnr),
		   (cursor_pos % scherm_kolnr),
		   hvio);
#endif
   }
}

#ifdef MSDOS
/* ------------------------------------------------------------------------- */
/* LEES_MODE                                                                 */
/*                                                                           */
/* Taak       : opvragen scherm mode                                         */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : returncode = scherm mode                                     */
/* ------------------------------------------------------------------------- */
static int lees_mode(void);
static int lees_mode()
{
   union REGS  regs;

   /* Lees de huidige BIOS mode */
   regs.h.ah = 0x0F;
   int86(0x10,&regs,&regs);

   return regs.h.al & 0x7F;
}
#endif

/* ------------------------------------------------------------------------- */
/* LEES_INFO                                                                 */
/*                                                                           */
/* Taak       : opvragen scherm gegevens uit BIOS/RAM en SCHERM environment  */
/*              variabele                                                    */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : ptr_scherm, scherm_kolnr, scherm_regnr, log_regnr, log_kolnr */
/*              cursor_pos, is_lcd, is_monochroom en is_ega_vga gevuld       */
/* ------------------------------------------------------------------------- */
static void lees_info(void);
static void lees_info()
{
#ifdef MSDOS
   union REGS  regs;
#endif
#ifdef __OS2__
   VIOMODEINFO vmi;
#endif
   char * ptr_envvar = NULL;
   char reg[3];
   char kol[3];

   /* Bepaal scherm adres en aantal kollommen */
#ifdef MSDOS
   regs.h.ah = 0x0F;
   int86(0x10,&regs,&regs);
   scherm_kolnr = regs.h.ah;

   if ( lees_mode() == 7 )
      FP_SEG(ptr_scherm) = 0xB000, is_monochroom = 1;
   else
      FP_SEG(ptr_scherm) = 0xB800, is_monochroom = 0;

   FP_OFF(ptr_scherm) = 0x0000;

   /* Bepaal aantal regels op scherm */
   regs.h.ah = 0x11;
   regs.h.al = 0x30;
   regs.h.bh = 0;
   regs.h.dl = (unsigned char)(scherm_regnr - 1);
   int86(0x10,&regs,&regs);
   scherm_regnr = regs.h.dl + 1;
#endif

#ifdef __OS2__
   vmi.cb = sizeof(vmi);
   VioGetMode(&vmi,hvio);
   if (vmi.color > 2)
      is_monochroom = 0;
   else
      is_monochroom = 1;
   scherm_kolnr  = vmi.col;
   scherm_regnr  = vmi.row;
#endif

   /* Vraag de cursor positie op */
#ifdef MSDOS
   regs.h.ah = 3;
   regs.h.bh = 0;
   int86(0x10,&regs,&regs);
   cursor_pos = regs.h.dh * scherm_kolnr + regs.h.dl;
#endif
#ifdef __OS2__
   {
      USHORT x,y;

      VioGetCurPos(&y,&x,hvio);
      cursor_pos = x + (y * scherm_kolnr);
   }
#endif

   log_regnr = scherm_regnr;
   log_kolnr = scherm_kolnr;

   /* Kijk of aantal regels/kolom/type wordt overschreven via DOS SET SCHERM=
      parameter in de vorm SCHERM=t,rr,kk
				  0123456
				  t  = M (mono), L (lcd), C (color)
				  rr = aantal regels
				  kk = aantal kollommen
   */

   ptr_envvar = getenv("SCHERM");
   if (ptr_envvar != NULL)
   {
      if (toupper(ptr_envvar[0]) == 'G')
	 is_grys = 1;
      if (toupper(ptr_envvar[0]) == 'M')
	 is_monochroom = 1;
      if (toupper(ptr_envvar[0]) == 'L')
	 is_lcd        = 1;
      if (toupper(ptr_envvar[0]) == 'C')
	 is_monochroom = 0;

      if (strlen(ptr_envvar) >= 4)
      {
	 reg[0] = ptr_envvar[2];
	 reg[1] = ptr_envvar[3];
	 reg[2] = '\0';
	 log_regnr = atoi(reg);
      }
      if (strlen(ptr_envvar) >= 7)
      {
	 kol[0] = ptr_envvar[5];
	 kol[1] = ptr_envvar[6];
	 kol[2] = '\0';
	 log_kolnr = atoi(kol);
      }
   }

   is_ega_vga = 0;
#ifdef MSDOS
   if (lees_mode() != 7)
   {
      /* Kleurenkaart aanwezig en in kleurenmode */

      /* Test op MCGA/VGA scherm */
      regs.h.ah = 0x1A;
      regs.h.al = 0;
      int86(0x10,&regs,&regs);
      if (regs.h.al == 0x1A)
	 is_ega_vga = 2;
      else
      {
	 /* Test op EGA scherm */
	 regs.h.ah = 0x12;
	 regs.h.bl = 0x10;
	 int86(0x10,&regs,&regs);
	 if (regs.h.bl != 0x10)
	    is_ega_vga = 1;
      }
   }
#endif
#ifdef __OS2__
   {
      VIOCONFIGINFO inf;

      inf.cb = sizeof(inf);
      VioGetConfig(VIO_CONFIG_CURRENT,&inf,hvio);
      if (inf.adapter >= 2)
	 is_ega_vga = 1;
      else
	 is_ega_vga = 0;
   }
#endif
}

/* ------------------------------------------------------------------------- */
/* ZET_MODE                                                                  */
/*                                                                           */
/* Taak       : instellen opgegeven scherm mode met behoud van palette       */
/*                                                                           */
/* Begin      : mode gevuld                                                  */
/*                                                                           */
/* Einde      : schermmode = mode                                            */
/* ------------------------------------------------------------------------- */
#ifdef MSDOS
static void zet_mode(int mode);
static void zet_mode(int mode)
{
   union REGS  regs;

   /* Set video mode */
   regs.h.ah = 0x00;
   regs.h.al = (unsigned char)mode;
   int86(0x10,&regs,&regs);

   /* Herstel de ingestelde kleuren */
   scherm_zet_kleurenset(huidige_kleurenset);

   lees_info();
}
#endif

/* ------------------------------------------------------------------------- */
/* SCHERM_AANTAL_REG                                                         */
/*                                                                           */
/* Taak       : opvragen aantal regels op scherm                             */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : aantal regels terugggeven                                    */
/* ------------------------------------------------------------------------- */
int scherm_aantal_reg()
{
   return log_regnr;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_AANTAL_KOL                                                         */
/*                                                                           */
/* Taak       : opvragen aantal kolommen op scherm                           */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : aantal kolommen teruggegeven                                 */
/* ------------------------------------------------------------------------- */
int scherm_aantal_kol()
{
   return log_kolnr;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_MONOCHROOM                                                         */
/*                                                                           */
/* Taak       : opvragen of het een monochroom scherm betreft                */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : 0 = niet, 1 = wel monochroom                                 */
/* ------------------------------------------------------------------------- */
int scherm_monochroom()
{
   return is_monochroom;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_LCD                                                                */
/*                                                                           */
/* Taak       : opvragen of het een LCD scherm betreft                       */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : 0 = niet, 1 = wel LCD                                        */
/* ------------------------------------------------------------------------- */
int scherm_lcd()
{
   return is_lcd;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_BREEDTE                                                            */
/*                                                                           */
/* Taak       : instellen schermbreedte                                      */
/*                                                                           */
/* Begin      : aantal_kol bevat gewenst aantal kolommen                     */
/*                                                                           */
/* Einde      : 40 (CO40) of 80 (MONO/CO80) ingesteld                        */
/* ------------------------------------------------------------------------- */
void scherm_breedte(int aantal_kol)
{
#ifdef __OS2__
   VIOMODEINFO vmi;

   vmi.cb = 8;
   VioGetMode(&vmi,hvio);
   vmi.col = aantal_kol;
   VioSetMode(&vmi,hvio);

   lees_info();
#endif
#ifdef MSDOS
   switch(aantal_kol)
   {
      case 40:
	       /* MODE CO40 */
	       if (lees_mode() != 1)
		  zet_mode(1);
	       break;
      case 80:
      default:
	       /* MODE MONO of CO80 */
	       if (lees_mode() != 7 && lees_mode() != 3)
		  zet_mode(3);
	       break;
   }
#endif
}

/* ------------------------------------------------------------------------- */
/* SCHERM_BEWAAR                                                             */
/*                                                                           */
/* Taak       : bewaren scherm op stack                                      */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : scherm bewaard                                               */
/* ------------------------------------------------------------------------- */
void scherm_bewaar()
{
   struct bewaar *ptr_hulp;

   ptr_hulp = (struct bewaar *)malloc(sizeof(struct bewaar));

   if (ptr_hulp != NULL)
   {
       ptr_hulp->ptr_next = ptr_first;
       ptr_first = ptr_hulp;
#ifdef MSDOS
       ptr_first->mode         = lees_mode();
       memcpy(ptr_first->scherm,ptr_scherm,scherm_regnr*scherm_kolnr*2);
#endif
#ifdef __OS2__
       ptr_first->lengte = scherm_regnr*scherm_kolnr*2;
       VioReadCellStr((PCH)(ptr_first->scherm),
		      &ptr_first->lengte,
		      0,0,hvio);
#endif
       ptr_first->cursor_pos = cursor_pos;
       ptr_first->cursor_zichtbaar = cursor_zichtbaar;
       ptr_first->cursor_kleur = cursor_kleur;
   }
}

/* ------------------------------------------------------------------------- */
/* SCHERM_HERSTEL                                                            */
/*                                                                           */
/* Taak       : herstellen scherm vanaf stack                                */
/*                                                                           */
/* Begin      : scherm op stack                                              */
/*                                                                           */
/* Einde      : scherm hersteld                                              */
/* ------------------------------------------------------------------------- */
void scherm_herstel()
{
   struct bewaar *ptr_hulp;

   if (ptr_first != NULL)
   {
      ptr_hulp = ptr_first;
#ifdef MSDOS
      if (lees_mode() != ptr_first->mode)
	 zet_mode(ptr_first->mode);
      memcpy(ptr_scherm,ptr_first->scherm,scherm_regnr*scherm_kolnr*2);
#endif
#ifdef __OS2__
      VioWrtCellStr((PCH)(ptr_first->scherm),
		    ptr_first->lengte,
		    0,0,hvio);
#endif
      cursor_pos       = ptr_first->cursor_pos;
      scherm_cursor(ptr_first->cursor_zichtbaar);
      scherm_cursor_kleur(ptr_first->cursor_kleur);
      plaats_cursor();

      ptr_first        = ptr_hulp->ptr_next;
      free(ptr_hulp);
   }

}

/* ------------------------------------------------------------------------- */
/* SCHERM_HERSTEL_NA_EXEC                                                    */
/*                                                                           */
/* Taak       : herstellen scherm vanaf stack nadat dit eventueel is         */
/*              vernield door een ander programma                            */
/*                                                                           */
/* Begin      : scherm op stack                                              */
/*                                                                           */
/* Einde      : scherm hersteld                                              */
/* ------------------------------------------------------------------------- */
void scherm_herstel_na_exec()
{
   /* Herstel scherm vanuit een inconsistente omgeving */
   scherm_herstel();
   scherm_bewaar();

   /* Zet mode nogmaals op om gewijzigde kleuren/cursor te herstellen */
#ifdef MSDOS
   zet_mode(lees_mode());
#endif

   /* Herstel scherm vanuit een consistente omgeving */
   scherm_herstel();
}


/* ------------------------------------------------------------------------- */
/* SCHERM_CURSOR_PLAATS                                                      */
/*                                                                           */
/* Taak       : plaatsen cursor                                              */
/*                                                                           */
/* Begin      : regel en kol gevuld                                          */
/*                                                                           */
/* Einde      : cursor op gewenste positie                                   */
/* ------------------------------------------------------------------------- */
void scherm_cursor_plaats(int regel,int kol)
{
   cursor_pos = regel * scherm_kolnr + kol;
   plaats_cursor();
}

/* ------------------------------------------------------------------------- */
/* SCHERM_CURSOR_POS                                                         */
/*                                                                           */
/* Taak       : opvragen positie cursor                                      */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : regel en kol gevuld                                          */
/* ------------------------------------------------------------------------- */
void scherm_cursor_pos(int * regel,int * kol)
{
   *regel = cursor_pos / scherm_kolnr;
   *kol   = cursor_pos % scherm_kolnr;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_CURSOR_KLEUR                                                       */
/*                                                                           */
/* Taak       : instellen kleur van cursor                                   */
/*                                                                           */
/* Begin      : kleur gevuld                                                 */
/*                                                                           */
/* Einde      : cursor heeft opgegeven kleur                                 */
/* ------------------------------------------------------------------------- */
void scherm_cursor_kleur(int kleur)
{
   cursor_kleur      = kleur;
   cursor_kleur_bits = cursor_kleur << 8;
}

/* ------------------------------------------------------------------------- */
/* SCHERM_CURSOR                                                             */
/*                                                                           */
/* Taak       : instellen type cursor                                        */
/*                                                                           */
/* Begin      : type_cursor gevuld met 0 (geen) 1 (underscore) 2 (blok)      */
/*              of 3 (startup waarde)                                        */
/*                                                                           */
/* Einde      : cursor heeft opgegeven kleur                                 */
/* ------------------------------------------------------------------------- */
static unsigned char cursor_start = 0;
static unsigned char cursor_eind  = 0;

void scherm_cursor(int type_cursor)
{
   if (cursor_zichtbaar != type_cursor )
   {
#ifdef MSDOS
      union REGS  regs;

      switch(type_cursor)
      {
	 case 0:  /* Geen cursor */
		  regs.h.ch = 0x20;
		  regs.h.cl = 0x0;
		  break;
	 case 1:  /* Underscore type */
		  regs.h.ch = (unsigned char)(cursor_eind -1);
		  regs.h.cl = cursor_eind;
		  break;
	 case 2:  /* Blok cursor */
		  regs.h.ch = 0;
		  regs.h.cl = cursor_eind;
		  break;
	 default: /* Startup cursor */
		  regs.h.ch = cursor_start;
		  regs.h.cl = cursor_eind;
		  break;
      }

      regs.h.ah = 1;
      int86(0x10,&regs,&regs);
#endif
#ifdef __OS2__
      VIOCURSORINFO curinf;

      VioGetCurType(&curinf,hvio);

      switch(type_cursor)
      {
	 case 0:  /* Geen cursor */
		  curinf.attr = -1;
		  break;
	 case 1:  /* Underscore type */
		  curinf.yStart = curinf.cEnd -1;
		  curinf.attr   = 0;
		  break;
	 case 2:  /* Blok cursor */
		  curinf.yStart = 0;
		  curinf.attr   = 0;
		  break;
	 default: /* Startup cursor */
		  curinf.yStart = cursor_start;
		  curinf.cEnd   = cursor_eind;
		  curinf.attr   = 0;
		  break;
      }

      VioSetCurType(&curinf,hvio);
#endif
   }
   /* Wijzig de cursor en toon deze op de juiste positie */
   cursor_zichtbaar = type_cursor;
   plaats_cursor();
}

/* ---------------------------------------------------------------------- */
/* SCHRIJF_TEKEN                                                          */
/* Taak   : Een teken plaatsen op het scherm                              */
/*                                                                        */
/* ---------------------------------------------------------------------- */

void scherm_schrijf_teken(char teken)
{
   /* Plaats teken op cursor positie */
#ifdef MSDOS
   ptr_scherm[cursor_pos] = cursor_kleur_bits | teken;
#endif
#ifdef __OS2__
   {
      BYTE bAttr = cursor_kleur;
      int regel, kol;
      regel = cursor_pos / scherm_kolnr;
      kol   = cursor_pos % scherm_kolnr;
      VioWrtCharStrAtt((PCH)&teken,1,regel,kol,&bAttr,hvio);
   }
#endif

   /* Verlaats cursor positie 1 verder */
   cursor_pos++;
   plaats_cursor();
}

/* ---------------------------------------------------------------------- */
/* SCHRIJF_TEKST                                                          */
/* Taak   : Een string plaatsen op het scherm op de cursor positie in     */
/*          de kleur van de cursor                                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */
void scherm_schrijf_tekst(char tekst[])
{
#ifdef MSDOS
   register int i;

   for ( i = 0; tekst[i]; i++ )
   {
      /* Plaats teken op cursor positie */
      ptr_scherm[cursor_pos] = cursor_kleur_bits | tekst[i];
      cursor_pos++;
   }
#endif
#ifdef __OS2__
   {
      BYTE bAttr = cursor_kleur;
      int regel, kol;
      regel = cursor_pos / scherm_kolnr;
      kol   = cursor_pos % scherm_kolnr;
      VioWrtCharStrAtt((PCH)tekst,strlen(tekst),regel,kol,&bAttr,hvio);
      cursor_pos += strlen(tekst);
   }
#endif

   plaats_cursor();
}

int scherm_kleur(int regel,int kol)
{
#ifdef MSDOS
   return (ptr_scherm[regel*scherm_kolnr+kol] >> 8);
#endif
#ifdef __OS2__
   USHORT cell;
   USHORT bytes = sizeof(cell);

   VioReadCellStr ((PCH)&cell, &bytes, regel, kol, hvio );

   return cell >> 8;
#endif
}

void scherm_vervang_kleur(int regel,int kol,int kleur)
{
#ifdef MSDOS
   register int pos;

   pos = regel*scherm_kolnr + kol;
   ptr_scherm[pos] = (ptr_scherm[pos] & 0xff) | (kleur << 8);
#endif
#ifdef __OS2__
   USHORT cell;
   USHORT bytes = sizeof(cell);

   VioReadCellStr ((PCH)&cell, &bytes, regel, kol, hvio );
   cell = (cell & 0xff) | (kleur << 8);
   VioWrtCellStr ((PCH)&cell, bytes, regel, kol, hvio );
#endif

}

void scherm_vervang_teken(int regel,int kol,char teken)
{
#ifdef MSDOS
   register int pos;

   pos = regel*scherm_kolnr + kol;
   ptr_scherm[pos] = (ptr_scherm[pos] & 0xff00) | teken;
#endif
#ifdef __OS2__
   VioWrtCharStr((PCH)&teken,1,regel,kol,hvio);
#endif
}

void scherm_vervang_tekst(int regel,int kol,char tekst[])
{
#ifdef MSDOS
   register
   int         i, pos;

   pos = regel*scherm_kolnr + kol;

   for ( i = 0; tekst[i]; i++ )
   {
      ptr_scherm[pos] = (ptr_scherm[pos] & 0xff00) | tekst[i];
      pos++;
   }
#endif
#ifdef __OS2__
   VioWrtCharStr((PCH)tekst,strlen((const char *)tekst),regel,kol,hvio);
#endif
}

void scherm_plaats_teken(int regel,int kol,int kleur,char teken)
{
   scherm_cursor_plaats(regel,kol);
   scherm_cursor_kleur(kleur);
   scherm_schrijf_teken(teken);
}


void scherm_plaats_tekst(int regel,int kol,int kleur,char tekst[])
{
   scherm_cursor_plaats(regel,kol);
   scherm_cursor_kleur(kleur);
   scherm_schrijf_tekst(tekst);
}

void scherm_tekst(int regel,int kol,int lengte,int kleur,char tekst[])
{
#ifdef MSDOS
   register int i;
#endif

   scherm_cursor_plaats(regel,kol);
   scherm_cursor_kleur(kleur);

#ifdef MSDOS
   /* Toon de tekst */
   for ( i = 0; tekst[i] != '\0' && i < lengte ; i++ )
   {
      ptr_scherm[cursor_pos] = cursor_kleur_bits | tekst[i];
      cursor_pos++;
   }
   /* Vul aan met spaties */
   for (i = strlen(tekst) ; i < lengte; i++)
   {
      ptr_scherm[cursor_pos] = cursor_kleur_bits | ' ';
      cursor_pos++;
   }
#endif
#ifdef __OS2__
   {
      char hulp[255];
      BYTE bAttr = cursor_kleur;
      memset(hulp,' ',lengte);
      memcpy(hulp,tekst,strlen(tekst));
      VioWrtCharStrAtt((PCH)hulp,lengte,regel,kol,&bAttr,hvio);
      cursor_pos += lengte;
   }
#endif

   plaats_cursor();
}


void scherm_cls(int kleur)
{
   register
   int         i;

   /* Maak het scherm leeg */
   scherm_cursor(0);
   for ( i = 0; i < log_regnr; i++ )
      scherm_tekst(i,0,log_kolnr,kleur,"");
   scherm_cursor_plaats(0,0);
}

void scherm_init()
{
#ifdef MSDOS
   union REGS regs;
#endif

   lees_info();

   /* Vraag cursor gegevens op */
#ifdef MSDOS
   regs.h.ah = 3;
   int86(0x10,&regs,&regs);
   cursor_start = regs.h.ch;
   cursor_eind  = regs.h.cl;
#endif

   scherm_geef_kleurenset((char *)start_kleurenset);
   scherm_geef_kleurenset((char *)huidige_kleurenset);

   scherm_bewaar();
   scherm_cursor(0);
}

void scherm_einde()
{
   while (ptr_first != NULL)
      scherm_herstel();
   scherm_zet_kleurenset((char *)start_kleurenset);
   scherm_cursor(-1);
}

void scherm_geef_kleurenset(char kleuren[16])
{
   char totale_set[17];

#ifdef MSDOS
   unsigned char * ptr;
   union REGS regs;
   struct SREGS sregs;
#endif

   memcpy(totale_set,huidige_kleurenset,16);
   if (is_ega_vga == 2)
   {
      /* Vraag het aan het VGA BIOS */
#ifdef MSDOS
      regs.h.ah = 0x10;
      regs.h.al = 0x09;
      ptr       = totale_set;
      sregs.es  = FP_SEG(ptr);
      regs.x.dx = FP_OFF(ptr);
      int86x(0x10,&regs,&regs,&sregs);
#endif
   }
   memcpy(huidige_kleurenset,totale_set,sizeof(huidige_kleurenset));
   memcpy(kleuren           ,totale_set,sizeof(huidige_kleurenset));
}

void scherm_zet_kleurenset(char kleuren[16])
{
   char totale_set[17];
#ifdef MSDOS
   unsigned char * ptr;
   union REGS regs;
   struct SREGS sregs;
#endif

   if (is_ega_vga && !is_grys)
   {
      /* Wijzig de kleurenset van het EGA/VGA bios */
      memcpy(totale_set,kleuren,16);
      totale_set[16] = 0;

#ifdef MSDOS
      regs.h.ah = 0x10;
      regs.h.al = 0x02;
      ptr       = totale_set;
      sregs.es  = FP_SEG(ptr);
      regs.x.dx = FP_OFF(ptr);
      int86x(0x10,&regs,&regs,&sregs);
#endif

      memcpy(huidige_kleurenset,kleuren,16);
   }
}
