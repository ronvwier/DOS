/* ------------------------------------------------------------------------- */
/* DOSERROR                                                                  */
/* Taak       : DOS kritieke foutafhandeling                                 */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#pragma check_stack(off) /* --- Pragma --> Compileroptie instellen --- */

#include <stddef.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include "doserror.h"

#define KLEUR_TEKST  (SCHERM_VG_CYAAN + SCHERM_VG_FEL + SCHERM_AG_ROOD)
#define KLEUR_FOUT   (SCHERM_VG_GRYS  + SCHERM_VG_FEL + SCHERM_AG_ROOD)

static struct Devhdr
   {
      struct Devhdr *next;    /* dword ptr to next device in chain */
      unsigned      attr;     /* type of device driver             */
      unsigned      strat;    /* device strategy entry point       */
      unsigned      intrpt;   /* interrupt entry point             */
      char          dname[8]; /* device name                       */
   };

static int  antw;
static union REGS regs;
static char drive  [ 3],
	    devnaam[9],
	    melding[81];

/* ---------------------------------------------------------------------- */
/* SCHERM                                                               */
/* Taak       Schermafhandeling, copie van SCHERM                         */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define SCHERM_VG_FEL          8
#define SCHERM_VG_KNIPPER    128

#define SCHERM_VG_ZWART        0
#define SCHERM_VG_BLAUW        1
#define SCHERM_VG_GROEN        2
#define SCHERM_VG_CYAAN        3
#define SCHERM_VG_ROOD         4
#define SCHERM_VG_PAARS        5
#define SCHERM_VG_GEEL         6
#define SCHERM_VG_GRYS         7

#define SCHERM_AG_ZWART        0
#define SCHERM_AG_BLAUW       16
#define SCHERM_AG_GROEN       32
#define SCHERM_AG_CYAAN       48
#define SCHERM_AG_ROOD        64
#define SCHERM_AG_PAARS       80
#define SCHERM_AG_GEEL        96
#define SCHERM_AG_GRYS       112
#define scherm_BLANKO      7

#define START_POS      (80 * 10)
#define WINDOW_SIZE    (80 * 5)

static int cursor_pos = 0;
static int cursor_zichtbaar = 1;
static int cursor_kleur = scherm_BLANKO;

static
unsigned far  *ptr_scherm = NULL;

static struct
{
   char           scherm[WINDOW_SIZE*2];
   unsigned int   cursor_pos;
}  bewaar;

static void scherm_bewaar(void)
{
   regs.h.ah = 3;
   regs.h.bh = 0;
   int86(0x10,&regs,&regs);
   bewaar.cursor_pos = regs.x.dx;

   memcpy(bewaar.scherm,ptr_scherm,sizeof(bewaar.scherm));
}

static void scherm_herstel(void)
{
   regs.h.ah = 2;
   regs.h.bh = 0;
   regs.x.dx = bewaar.cursor_pos;
   int86(0x10,&regs,&regs);

   memcpy(ptr_scherm,bewaar.scherm,sizeof(bewaar.scherm));
}

/* ---------------------------------------------------------------------- */
/* PLAATS_CURSOR                                                          */
/* Taak   : Cursor op gegeven schermpositie zetten                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void scherm_cursor_plaats(int regel,int kol)
{
   cursor_pos = regel * 80 + kol;

   regs.h.ah = 2;
   regs.h.bh = 0;
   regs.h.dh = (unsigned char)((cursor_pos + START_POS) / 80);
   regs.h.dl = (unsigned char)((cursor_pos + START_POS) % 80);
   int86(0x10,&regs,&regs);
}

static void scherm_cursor_kleur(int kleur)
{
   cursor_kleur = kleur;
}

static void scherm_cursor_verder(void)
{
   cursor_pos++;

   if ( cursor_pos >= (80*25) )
      cursor_pos = 0;
}

/* ---------------------------------------------------------------------- */
/* SCHERM_INIT                                                            */
/* Taak   : Bepalen van het scherm adres                                  */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void scherm_init()
{
   /* Bepaal scherm adres in geheugen */

   regs.h.ah = 0x0F;
   int86(0x10,&regs,&regs);

   if ( regs.h.al == 7 )
      FP_SEG(ptr_scherm) = 0xB000;
   else
      FP_SEG(ptr_scherm) = 0xB800;

   FP_OFF(ptr_scherm) = START_POS * 2;
}

/* ---------------------------------------------------------------------- */
/* SCHRIJF_TEKEN                                                          */
/* Taak   : Een teken plaatsen op het scherm                              */
/*                                                                        */
/* ---------------------------------------------------------------------- */

static void scherm_schrijf_teken(unsigned char teken)
{
   ptr_scherm[cursor_pos] = (cursor_kleur << 8) | teken;
   scherm_cursor_verder();
}

/* ---------------------------------------------------------------------- */
/* SCHRIJF_TEKST                                                          */
/* Taak   : Een string plaatsen op het scherm op de opgegeven positie     */
/*          in de opgegeven kleur                                         */
/*                                                                        */
/* ---------------------------------------------------------------------- */
static void scherm_schrijf_tekst(unsigned char tekst[])
{
   register
   int         i;

   for ( i = 0; tekst[i]; i++ )
      scherm_schrijf_teken(tekst[i]);
}

static void scherm_tekst(int regel,int kol,int lengte,int kleur,unsigned char tekst[])
{
   register int i;

   scherm_cursor_plaats(regel,kol);
   scherm_cursor_kleur(kleur);
   scherm_schrijf_tekst(tekst);
   for (i = strlen(tekst) ; i < lengte; i++) scherm_schrijf_teken(' ');
}

static void geef(char melding[])
{
     scherm_tekst(0, 1,77,KLEUR_TEKST,"ษอออออออออออออออออออออออออออออออออ DOS อออออออออออออออออออออออออออออออออออออป");
     scherm_tekst(1, 1,76,KLEUR_TEKST,"บ");
     scherm_tekst(1,77, 1,KLEUR_TEKST,"บ");
     scherm_tekst(2, 1,76,KLEUR_TEKST,"บ");
     scherm_tekst(2,77, 1,KLEUR_TEKST,"บ");
     scherm_tekst(2, 3,73,KLEUR_FOUT,melding);
     scherm_tekst(3, 1,76,KLEUR_TEKST,"บ");
     scherm_tekst(3,77, 1,KLEUR_TEKST,"บ");
     scherm_tekst(4, 1,77,KLEUR_TEKST,"ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ");
     scherm_cursor_plaats(2,2+strlen(melding));
}

static void dos_fout(unsigned deverror, unsigned errcode, unsigned int *X)
{
   struct Devhdr *Devhdr_ptr;

   Devhdr_ptr = (Devhdr *)X;
   strcpy(melding,"");

   if (!(deverror & 0x8000))
   {    /* if bit 15 = 1, disk error */
     drive[0] = (char) ((deverror & 0x00ff) + 'A');  /* bepaal drive letter */
     drive[1] = ':';                                 /* uit low-order byte  */
     drive[2] = '\0';
     strcpy(devnaam,"");
   }
   else
   {
      strcpy(drive, "");
      strncpy(devnaam,Devhdr_ptr->dname,sizeof(devnaam));
      devnaam[sizeof(devnaam) -1] = '\0';
   }

   switch (errcode & 0x00ff)   /* genereer melding adhv waarde  */
   {                           /* low-order byte van errcode    */
      case 0x00 :
         strcpy (melding, "Schrijfbeveiliging op");
         break;
      case 0x01 :
         strcpy (melding, "Onbekende");
         break;
      case 0x02 :
	 strcpy (melding, "NIET gereed melding van");
         break;
      case 0x03 :
         strcpy (melding, "Opdracht/adres fout van");
         break;
      case 0x04 :
         strcpy (melding, "CRC fout in gegevens op");
         break;
      case 0x05 :
         strcpy (melding, "Foutief verzoek op");
         break;
      case 0x06 :
         strcpy (melding, "Zoek fout op");
	 break;
      case 0x07 :
         strcpy (melding, "Onbekend medium type");
         break;
      case 0x08 :
         strcpy (melding, "Sector niet gevonden op");
         break;
      case 0x09 :
         strcpy (melding,"Geen papier in printer");
         break;
      case 0x0A :
         strcpy (melding, "Schrijf fout op");
         break;
      case 0x0B :
         strcpy (melding, "Lees fout op");
         break;
      case 0x0C :
         strcpy (melding,"Niet geformatteerde");
         break;
      default:
         strcpy (melding,"Onbekend probleem met");
         break;
   }

   if (!(deverror & 0x8000))
   {
      strcat (melding, " schijf in poort ");
      strcat (melding, drive);
   }
   else
   {
      strcat (melding, " DOS eenheid ");
      strcat (melding, devnaam);
   }

   while(kbhit())             /* clear keyboard-buffer  */
   {
      antw = getch();
   }

   /* Ga in dialoog met de gebruiker */
   scherm_bewaar();
   if (deverror & 0x1000)     /* Retry allowed */
   {
      strcat(melding, ", (A)fbreken of (H)erhalen : ?");
      do
      {
         geef (melding);
         antw = getch();
      } while (antw != 'a'
           &&  antw != 'A'
           &&  antw != 'h'
	   &&  antw != 'H');
   }
   else
   {  /* retry not allowed */
      strcat(melding, ", Druk op een toets");
      geef (melding);
      getch();
      antw = 'A';
   }
   scherm_herstel();

   switch (antw)
   {
      case 'a':
      case 'A':
         if (deverror & 0x0800)     /* Fail allowed */
            _hardresume(_HARDERR_FAIL);
         else
            _hardresume(_HARDERR_ABORT);
         break;
      case 'h':
      case 'H':
         _hardresume(_HARDERR_RETRY);
         break;
      default :
         _hardresume(_HARDERR_ABORT);
	 break;
   }
}

   /* ---------------------------------------------- */
   /* --- Sluis harware-errors door naar Handler --- */
   /* ---------------------------------------------- */

void doserror_handler(void)
{
   scherm_init();

   _harderr(dos_fout);
}
