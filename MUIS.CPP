/* ------------------------------------------------------------------------- */
/* MUIS                                                                      */
/* Taak       : Besturing van de muis                                        */
/*                                                                           */
/* (c) 1989   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#ifdef MSDOS
 #include <bios.h>
 #include <dos.h>
#endif
#ifdef __OS2__
   #define INCL_BASE
   #include <os2.h>
   static USHORT MouHandle;
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "muis.h"

static int status = -1;

#ifdef MSDOS
static union REGS  regs;
#endif

#define BUTTON_LEFT    0
#define BUTTON_RIGHT   1

#ifdef MSDOS
/* ------------------------------------------------------------------------- */
/* ENABLE_CURSOR_DISPLAY                                                     */
/*                                                                           */
/* Taak       : Muis cursor tonen                                            */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muis cursor zichtbaar                                        */
/* ------------------------------------------------------------------------- */
static void enable_cursor_display(void);
static void enable_cursor_display()
{
   regs.x.ax = 1;
   int86(0x33,&regs,&regs);
}

/* ------------------------------------------------------------------------- */
/* DISABLE_CURSOR_DISPLAY                                                    */
/*                                                                           */
/* Taak       : Muis cursor niet tonen                                       */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muis cursor onzichtbaar                                      */
/* ------------------------------------------------------------------------- */
static void disable_cursor_display(void);
static void disable_cursor_display()
{
   regs.x.ax = 2;
   int86(0x33,&regs,&regs);
}

/* ------------------------------------------------------------------------- */
/* READ_CURSOR_LOCATION                                                      */
/*                                                                           */
/* Taak       : Opvragen muispositie                                         */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : regel en kolom ingevuld                                      */
/* ------------------------------------------------------------------------- */
static void read_cursor_location(int * ptr_regel,int * ptr_kolom)
{
   regs.x.ax = 3;
   int86(0x33,&regs,&regs);

   *ptr_kolom = regs.x.cx;
   *ptr_regel = regs.x.dx;
}

/* ------------------------------------------------------------------------- */
/* SET_CURSOR_LOCATION                                                       */
/*                                                                           */
/* Taak       : Muispositie instelen                                         */
/*                                                                           */
/* Begin      : regel en kolom gevuld                                        */
/*                                                                           */
/* Einde      : muis op positie gezet                                        */
/* ------------------------------------------------------------------------- */
static void set_cursor_location(int regel,int kolom)
{
   regs.x.ax = 4;
   regs.x.cx = kolom;
   regs.x.dx = regel;

   int86(0x33,&regs,&regs);
}

/* ------------------------------------------------------------------------- */
/* READ_BUTTON_PRESS                                                         */
/*                                                                           */
/* Taak       : Opvragen positie/toets die ingedrukt werd                    */
/*                                                                           */
/* Begin      : button ingevuld met links of rechts                          */
/*                                                                           */
/* Einde      : gegevens gevuld                                              */
/* ------------------------------------------------------------------------- */
static void read_button_press(int button,int * count,int * line,int * col)
{
   regs.x.ax = 5;
   regs.x.bx = button;
   int86(0x33,&regs,&regs);

   *count = regs.x.bx;
   *col   = regs.x.cx;
   *line  = regs.x.dx;
}

/* ------------------------------------------------------------------------- */
/* DEFINE_HORIZONTAL_RANGE                                                   */
/*                                                                           */
/* Taak       : Instellen kolommen waarbinnen muis kan bewegen               */
/*                                                                           */
/* Begin      : posities gevuld                                              */
/*                                                                           */
/* Einde      : window ingesteld                                             */
/* ------------------------------------------------------------------------- */
static void define_horizontal_range(int from,int til)
{
   regs.x.ax = 7;
   regs.x.cx = from;
   regs.x.dx = til;

   int86(0x33,&regs,&regs);
}

/* ------------------------------------------------------------------------- */
/* DEFINE_VERTICAL_RANGE                                                     */
/*                                                                           */
/* Taak       : Instellen rijen waarbinnen muis kan bewegen                  */
/*                                                                           */
/* Begin      : posities gevuld                                              */
/*                                                                           */
/* Einde      : window ingesteld                                             */
/* ------------------------------------------------------------------------- */
static void define_vertical_range(int from,int til)
{
   regs.x.ax = 8;
   regs.x.cx = from;
   regs.x.dx = til;

   int86(0x33,&regs,&regs);
}

#endif

/* ---------------------------------------------------------------------- */
/* CHECK_MUIS                                                             */
/*                                                                        */
/* Taak   : Check of er een muis aan het systeem hangt                    */
/*                                                                        */
/* Begin  : -                                                             */
/*                                                                        */
/* Einde  : status bijgewerkt                                             */
/* ---------------------------------------------------------------------- */
static void check_muis(void);
static void check_muis()
{
#ifdef MSDOS
   static
   unsigned
   long far   *int_tabel = 0;

   /* Check of int 33 gevuld en positief reageert */

   if ( int_tabel[0x33] != 0 )
   {
      /* Software reset */
      regs.x.ax = 33;
      int86(0x33,&regs,&regs);

      if ( regs.x.ax == 33)
      {
	 /* Hardware reset */
	 regs.x.ax = 0;
	 int86(0x33,&regs,&regs);

	 if ( regs.x.ax == 0 )
	    status = 0;
	 else
	    status = 1;
      }
      else
	 status = 1;
   }
   else
      status = 0;
#endif
#ifdef __OS2__
   /* OS/2 - muis */
   status = 1;
#endif
}

/* ------------------------------------------------------------------------- */
/* MUIS_IRQ                                                                  */
/*                                                                           */
/* Taak       : Opvragen van IRQ waarop muis werkt                           */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : IRQ nummer teruggegeven                                      */
/* ------------------------------------------------------------------------- */
int  muis_irq()
{
#ifdef MSDOS
   regs.x.ax = 36;
   regs.x.cx = 0;

   int86(0x33,&regs,&regs);

   return regs.h.cl;
#else
   /* OS/2 */
   return 0;
#endif
}

/* ---------------------------------------------------------------------- */
/* MUIS_TOETS                                                             */
/* Taak   : Check of een muis toets is gebruikt                           */
/*                                                                        */
/* Begin  : -                                                             */
/*                                                                        */
/* Einde  : returncode bevat codes ingedrukte toetsen                     */
/* ---------------------------------------------------------------------- */
int muis_toets()
{
   static int reg,kol,tel, retcode;

   retcode = 0;

#ifdef MSDOS
   if ( status > 0)
   {
      read_button_press(BUTTON_LEFT,&tel,&reg,&kol);
      if ( tel > 0)
	 retcode += MUIS_TOETS_LINKS;

      read_button_press(BUTTON_RIGHT,&tel,&reg,&kol);
      if ( tel > 0)
	 retcode += MUIS_TOETS_RECHTS;
   }
#endif
#ifdef __OS2__
   {
      MOUEVENTINFO ME;
      USHORT optie = MOU_NOWAIT;

      if (MouReadEventQue(&ME,&optie,MouHandle) == 0)
      {
         if (ME.fs & MOUSE_BN1_DOWN)
            retcode += MUIS_TOETS_LINKS;
         if (ME.fs & MOUSE_BN2_DOWN)
            retcode += MUIS_TOETS_RECHTS;
      }
   }
#endif
   return(retcode);
}

/* ------------------------------------------------------------------------- */
/* MUIS_GEEF_POSITIE                                                         */
/*                                                                           */
/* Taak       : Opvragen positie van muis                                    */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : regel en kolom gevuld                                        */
/* ------------------------------------------------------------------------- */
void muis_geef_positie(int * ptr_regel,int * ptr_kolom)
{
   static int hulp_reg, hulp_kol;

   if ( status > 0)
   {
#ifdef MSDOS
      read_cursor_location(&hulp_reg,&hulp_kol);
      *ptr_regel = hulp_reg / 8;
      *ptr_kolom = hulp_kol / 8;
#endif
#ifdef __OS2__
      PTRLOC MP;
      MouGetPtrPos(&MP,MouHandle);
      *ptr_regel = MP.row;
      *ptr_kolom = MP.col;
#endif
   }
   else
   {
      *ptr_kolom = 0;
      *ptr_regel = 0;
   }
}

/* ------------------------------------------------------------------------- */
/* MUIS_ZET_POSITIE                                                          */
/*                                                                           */
/* Taak       : Muiscursor op positie zetten                                 */
/*                                                                           */
/* Begin      : regel en kolom gevuld                                        */
/*                                                                           */
/* Einde      : muiscursor op opgegeven positie                              */
/* ------------------------------------------------------------------------- */
void muis_zet_positie(int regel,int kolom)
{
#ifdef MSDOS
   if ( status > 0)
      set_cursor_location((regel * 8),(kolom * 8));
#endif
}

/* ------------------------------------------------------------------------- */
/* MUIS_ZET_GEBIED                                                           */
/*                                                                           */
/* Taak       : Muiscursorbewegingen beperken tot window                     */
/*                                                                           */
/* Begin      : van/tot regel en kolom gevuld                                */
/*                                                                           */
/* Einde      : muiscursor window ingesteld                                  */
/* ------------------------------------------------------------------------- */
void muis_zet_gebied(int van_reg,int van_kol,int tot_reg, int tot_kol)
{
#ifdef MSDOS
   if ( status > 0)
   {
      define_horizontal_range((van_kol * 8),((tot_kol - 1) * 8));
      define_vertical_range((van_reg * 8),((tot_reg - 1) * 8));
   }
#endif
}

/* ------------------------------------------------------------------------- */
/* MUIS_INIT                                                                 */
/*                                                                           */
/* Taak       : Muissoftware initieeren                                      */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muisstatus bekend                                            */
/* ------------------------------------------------------------------------- */
void muis_init()
{
   check_muis();
#ifdef __OS2__
   MouOpen(NULL,&MouHandle);
#endif

}

/* ------------------------------------------------------------------------- */
/* MUIS_EINDE                                                                */
/*                                                                           */
/* Taak       : Muissoftware afsluiten                                       */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muiscursor niet meer zichtbaar                               */
/* ------------------------------------------------------------------------- */
void muis_einde()
{
#ifdef MSDOS
   if ( status > 0 )
      disable_cursor_display();
#endif
#ifdef __OS2__
   MouClose(MouHandle);
#endif

   status = -1;
}

/* ------------------------------------------------------------------------- */
/* MUIS_AANWEZIG                                                             */
/*                                                                           */
/* Taak       : Opvragen of muis aanwezig                                    */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : returncode = 0 en geen muis aktief                           */
/*              returncode = 1 en muis aktief                                */
/* ------------------------------------------------------------------------- */
int muis_aanwezig()
{
   int retcode = 0;

   if ( status > 0 )
      retcode = 1;

   return retcode;
}

static int muis_zichtbaar = 0;

/* ------------------------------------------------------------------------- */
/* MUIS_AAN                                                                  */
/*                                                                           */
/* Taak       : Muiscursor tonen                                             */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muiscursor zichtbaar indien muis aanwezig                    */
/* ------------------------------------------------------------------------- */
void muis_aan()
{
   if (status > 0)
   {
#ifdef MSDOS
      if (muis_zichtbaar == 0)
	 enable_cursor_display();
#endif
      muis_zichtbaar++;
   }
}

/* ------------------------------------------------------------------------- */
/* MUIS_UIT                                                                  */
/*                                                                           */
/* Taak       : Muiscursor verbergen                                         */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : muiscursor onzichtbaar                                       */
/* ------------------------------------------------------------------------- */
void muis_uit()
{
   if (status > 0)
   {
      muis_zichtbaar--;
#ifdef MSDOS
      if (muis_zichtbaar < 1)
	 disable_cursor_display();
#endif
   }
}
