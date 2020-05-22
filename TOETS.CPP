/* ------------------------------------------------------------------------- */
/* TOETS                                                                     */
/* Taak       : Afhandelen toetsenbord invoer via BIOS                       */
/*                                                                           */
/* (c) 1988   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */
#ifdef MSDOS
 #include <bios.h>
 #include <dos.h>
#else
 #include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "toets.h"
#include "taak.h"

#ifdef MSDOS

#define BIOS_DATA_SEG     0x0040
#define KYBD_STAT_OFF     0x0017
#define KYBD_TYPE_OFF     0x0096
#define EXTENDED_KYDB       0x10
#define INS_MODE_TOGGLE     0x80
#define BIOS_KYBD_FUNCTION  0x16
#define KYBD_GET_EXT_STATUS 0x12

static int peekb(int seg,int off)
{
   unsigned char *teken;

   FP_SEG(teken) = seg;
   FP_OFF(teken) = off;

   return *teken;
}

static int iskybdext(void)
{
   if (peekb(BIOS_DATA_SEG, KYBD_TYPE_OFF) & EXTENDED_KYDB)
       return(1);
   else
       return(0);
}
#endif

static int extended = -1;

unsigned int toets_aanwezig()
{
   register
   int         retcode = 0;

#ifdef MSDOS
   if (extended < 0)
   {
      extended = iskybdext();
      if (extended)
      {
	 union REGS regs;
	 /* Verhoog de typematic rate en verlaag delay ofwel snel toetsenbord */
	 regs.h.ah = 0x03;
	 regs.h.al = 0x05;
	 regs.x.bx = 0;
	 int86(0x16,&regs,&regs);
      }
   }

   /* Kijk of er een toets aanwezig is */
#if (_MSC_VER >= 600)
   if (extended)
   {
      if ( _bios_keybrd(_NKEYBRD_READY) != 0)
	 retcode = 1;
   }
   else
#endif
   {
      if ( _bios_keybrd(_KEYBRD_READY) != 0)
	 retcode = 1;
   }
#else
   /* OS/2 - standaard I/O */
   if (kbhit())
      retcode = 1;
#endif

   if (retcode == 0)
   {
      TAAK dummy;
      dummy.run();
   }

   return retcode;
}

unsigned int toets_lees()
{
   register
   short         toets,toets2;

   if ( toets_aanwezig() )
   {
      /* Lees de toets */
#ifdef MSDOS
#if (_MSC_VER >= 600)
      if (extended)
	 toets = _bios_keybrd(_NKEYBRD_READ);
      else
#endif
	 toets = _bios_keybrd(_KEYBRD_READ);

      /* Haal de scankode weg bij een normale toets */
      if ( (toets & 0x00FF) == 0xE0 )
	 toets &= 0xFF00;
      if ( (toets & 0x00FF) != 0 )
	 toets &= 0x00FF;
#else
      /* OS/2 standaard I/O */
      toets = getch();
      if (toets == 0 || toets == 224)
      {
         toets2 = getch();
         toets = toets2 << 8;
      }
#endif
   }
   else
      toets = 0;

   return toets;
}

/* ---------------------------------------------------------------------- */
/* WACHT                                                                  */
/* Taak   : Wacht totdat toets ingedrukt en lees deze                     */
/*                                                                        */
/* ---------------------------------------------------------------------- */
unsigned int toets_wacht()
{
   int toets = 0;

   while( toets == 0 ) toets = toets_lees();

   return toets;
}

static unsigned int
       f_tab[12]     = { TOETS_F1,  TOETS_F2,  TOETS_F3,
                         TOETS_F4,  TOETS_F5,  TOETS_F6,
                         TOETS_F7,  TOETS_F8,  TOETS_F9,
                         TOETS_F10, TOETS_F11, TOETS_F12 } ;
static unsigned int
       alt_f_tab[12] = { TOETS_ALT_F1,  TOETS_ALT_F2,  TOETS_ALT_F3,
                         TOETS_ALT_F4,  TOETS_ALT_F5,  TOETS_ALT_F6,
                         TOETS_ALT_F7,  TOETS_ALT_F8,  TOETS_ALT_F9,
                         TOETS_ALT_F10, TOETS_ALT_F11, TOETS_ALT_F12 } ;
static unsigned int
       shift_f_tab[12] = { TOETS_SHIFT_F1,  TOETS_SHIFT_F2,  TOETS_SHIFT_F3,
                           TOETS_SHIFT_F4,  TOETS_SHIFT_F5,  TOETS_SHIFT_F6,
                           TOETS_SHIFT_F7,  TOETS_SHIFT_F8,  TOETS_SHIFT_F9,
                           TOETS_SHIFT_F10, TOETS_SHIFT_F11, TOETS_SHIFT_F12 } ;
static unsigned int
       ctrl_f_tab[12] =  { TOETS_CTRL_F1,  TOETS_CTRL_F2,  TOETS_CTRL_F3,
                           TOETS_CTRL_F4,  TOETS_CTRL_F5,  TOETS_CTRL_F6,
                           TOETS_CTRL_F7,  TOETS_CTRL_F8,  TOETS_CTRL_F9,
                           TOETS_CTRL_F10, TOETS_CTRL_F11, TOETS_CTRL_F12 } ;

static unsigned int
       alt_l_tab[26] = { TOETS_ALT_A, TOETS_ALT_B, TOETS_ALT_C, TOETS_ALT_D,
                         TOETS_ALT_E, TOETS_ALT_F, TOETS_ALT_G, TOETS_ALT_H,
                         TOETS_ALT_I, TOETS_ALT_J, TOETS_ALT_K, TOETS_ALT_L,
                         TOETS_ALT_M, TOETS_ALT_N, TOETS_ALT_O, TOETS_ALT_P,
                         TOETS_ALT_Q, TOETS_ALT_R, TOETS_ALT_S, TOETS_ALT_T,
                         TOETS_ALT_U, TOETS_ALT_V, TOETS_ALT_W, TOETS_ALT_X,
                         TOETS_ALT_Y, TOETS_ALT_Z } ;

unsigned int toets_tekst_naar_code(char tekst[])
{
   char hulp[20];
   unsigned int code = 0, l, i;

   strcpy(hulp,tekst);
   strupr(hulp);
   l = strlen(tekst);
   if (l < 2)
     code = tekst[0];
   if (hulp[0] == 'F' && l > 1)
   {
      i = atoi(&hulp[1]);
      if ( i > 0 && i <= 12)
         code = f_tab[i-1];
   }
   if (strncmp(hulp,"ALT-F",5) == 0 && l > 5)
   {
      i = atoi(&hulp[5]);
      if ( i > 0 && i <= 12)
         code = alt_f_tab[i-1];
   }
   if (   strncmp(hulp,"ALT-",4) == 0 && l == 5
       && hulp[4] >= 'A' && hulp[4] <= 'Z' )
   {
      /* Alt-letter */
      i = hulp[4] - 'A';
      code = alt_l_tab[i];
   }
   if (strncmp(hulp,"SHIFT-F",7) == 0 && l > 7)
   {
      i = atoi(&hulp[7]);
      if ( i > 0 && i <= 12)
         code = shift_f_tab[i-1];
   }
   if (strncmp(hulp,"CTRL-F",6) == 0 && l > 6)
   {
      i = atoi(&hulp[6]);
      if ( i > 0 && i <= 12)
         code = ctrl_f_tab[i-1];
   }

   return code;
}

void toets_code_naar_tekst(unsigned int code,char tekst[])
{
   int i;

   strcpy(tekst,"");

   if (code >= (unsigned int)' ' && code <= 0xFF)
      tekst[0] = (unsigned char)code, tekst[1] = '\0';
   for (i = 0; i < 12; i++)
      if (code == f_tab[i])
         sprintf(tekst,"F%d",i+1);
   for (i = 0; i < 12; i++)
      if (code == alt_f_tab[i])
         sprintf(tekst,"Alt-F%d",i+1);
   for (i = 0; i < 12; i++)
      if (code == shift_f_tab[i])
         sprintf(tekst,"Shift-F%d",i+1);
   for (i = 0; i < 12; i++)
      if (code == ctrl_f_tab[i])
         sprintf(tekst,"Ctrl-F%d",i+1);
   for (i = 0; i < 26; i++)
      if (code == alt_l_tab[i])
         sprintf(tekst,"Alt-%c",(unsigned char)(i+'A'));

}
