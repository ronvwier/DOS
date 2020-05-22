/* ------------------------------------------------------------------------- */
/* BIOSCOMM   : Interface voor datacommunicatie via BIOS/INT14               */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <string.h>

#include "wekker.h"
#include "taak.h"
#include "bioscomm.h"

#pragma check_stack(off)

static TAAK Taak;

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Interne gegevens                                                          */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#define BUFFER_SIZE 2048

typedef unsigned char BYTE;

static int modem_poort = 1;

/* ------------------------------------------------------------------------- */
/* Laag       Communicatie                                                   */
/* BIOSCOMM_  Communicatie via BIOS/INT14                                    */
/*-------------------------------------------------------------------------- */

#define BIOSCOMM_LS_DR          0x0100
#define BIOSCOMM_LS_OE          0x0200
#define BIOSCOMM_LS_PE          0x0400
#define BIOSCOMM_LS_FE          0x0800
#define BIOSCOMM_LS_BI          0x1000
#define BIOSCOMM_LS_HRE         0x2000
#define BIOSCOMM_LS_TRE         0x4000
#define BIOSCOMM_LS_TO          0x8000

#define BIOSCOMM_MS_DCTS        0x0001
#define BIOSCOMM_MS_DDSR        0x0002
#define BIOSCOMM_MS_TERI        0x0004
#define BIOSCOMM_MS_DDCD        0x0008
#define BIOSCOMM_MS_CTS         0x0010
#define BIOSCOMM_MS_DSR         0x0020
#define BIOSCOMM_MS_RI          0x0040
#define BIOSCOMM_MS_DCD         0x0080

static BYTE bioscomm_bufin[BUFFER_SIZE];
static int  bioscomm_bufin_head = 0;
static int  bioscomm_bufin_tail = 0;

static BYTE bioscomm_bufuit[BUFFER_SIZE];
static int  bioscomm_bufuit_head = 0;
static int  bioscomm_bufuit_tail = 0;

static unsigned bioscomm_status = 0;

static void bioscomm_check(void);
static void bioscomm_check()
{
   unsigned hulp;
   bioscomm_status = _bios_serialcom(_COM_STATUS,modem_poort-1,0);

   do
   {
      while (bioscomm_status & BIOSCOMM_LS_DR)
      {
         hulp = _bios_serialcom(_COM_RECEIVE,modem_poort-1,0);
         bioscomm_bufin[bioscomm_bufin_tail] = (unsigned char)(hulp & 0x00FF);
	 bioscomm_bufin_tail++;
	 bioscomm_bufin_tail %= BUFFER_SIZE;
         bioscomm_status = _bios_serialcom(_COM_STATUS,modem_poort-1,0);
      }

      if (bioscomm_bufuit_tail != bioscomm_bufuit_head)
      {
         /* Kijk of we iets kunnen versturen */
         if (    bioscomm_status & BIOSCOMM_LS_HRE
              && bioscomm_status & BIOSCOMM_MS_CTS )
         {
            /* Verstuur een teken */
	    bioscomm_status = _bios_serialcom(_COM_SEND,modem_poort-1,bioscomm_bufuit[bioscomm_bufuit_head]);
	    if (!(bioscomm_status & BIOSCOMM_LS_TO))
            {
               /* Versturen gelukt */
               bioscomm_bufuit_head++;
               bioscomm_bufuit_head %= BUFFER_SIZE;
            }
         }
      }
      bioscomm_status = _bios_serialcom(_COM_STATUS,modem_poort-1,0);
   }
   while (bioscomm_status & BIOSCOMM_LS_DR);
}

static int bioscomm_flush(void);
static int bioscomm_flush()
{
   /* Leeg alle buffers */
   bioscomm_bufin_head  = 0;
   bioscomm_bufin_tail  = 0;
   bioscomm_bufuit_head = 0;
   bioscomm_bufuit_tail = 0;

   return 1;
}

int bioscomm_carrier()
{
   bioscomm_check();
   return bioscomm_status & BIOSCOMM_MS_DCD;
}

int bioscomm_zend(unsigned char teken)
{
   bioscomm_check();

   /* Zet teken in buffer */
   bioscomm_bufuit[bioscomm_bufuit_tail] = teken;
   bioscomm_bufuit_tail++;
   bioscomm_bufuit_tail %= BUFFER_SIZE;

   /* Probeer te verzenden */
   bioscomm_check();

   return 1;
}

int bioscomm_is_poort(int poortnummer)
{
   register int retcd = 0;
   unsigned int * biosdata_comm;

   if (poortnummer >= 1 && poortnummer <= 4)
   {
      /* Adres van BIOS DATA gebied op 0040:0000 bevat COM poortadressen */
      FP_SEG(biosdata_comm) = 0x40;
      FP_OFF(biosdata_comm) = 0x00;

      if (biosdata_comm[poortnummer -1] != 0)
         retcd = 1;
   }

   return retcd;
}

int bioscomm_init(int poort)
{
   int retcd;

   modem_poort = poort;
   retcd = bioscomm_is_poort(poort);
   if (retcd)
      Taak.start(bioscomm_check);
   /* Leeg alle buffers */
   bioscomm_flush();

   return retcd;
}

int bioscomm_einde()
{
   Taak.stop();
   bioscomm_flush();

   return 1;
}

int bioscomm_set_baud(long snelheid, int bits)
{
   int retcd = 0;

   if (bits == 7)
   {
      switch(snelheid)
      {
         case 1200 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_1200 |_COM_CHR7 | _COM_EVENPARITY | _COM_STOP1);
                     retcd=1; break;
         case 2400 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_2400 |_COM_CHR7 | _COM_EVENPARITY | _COM_STOP1);
                     retcd=1; break;
         case 4800 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_4800 |_COM_CHR7 | _COM_EVENPARITY | _COM_STOP1);
                     retcd=1; break;
         case 9600 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_9600 |_COM_CHR7 | _COM_EVENPARITY | _COM_STOP1);
                     retcd=1; break;
      }
   }
   else
   {
      switch(snelheid)
      {
         case 1200 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_1200 |_COM_CHR8 | _COM_NOPARITY | _COM_STOP1);
                     retcd=1; break;
         case 2400 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_2400 |_COM_CHR8 | _COM_NOPARITY | _COM_STOP1);
                     retcd=1; break;
         case 4800 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_4800 |_COM_CHR8 | _COM_NOPARITY | _COM_STOP1);
                     retcd=1; break;
         case 9600 : _bios_serialcom(_COM_INIT,modem_poort-1,_COM_9600 |_COM_CHR8 | _COM_NOPARITY | _COM_STOP1);
                     retcd=1; break;
      }
   }

   return retcd;
}

int bioscomm_teken_aanwezig()
{
   int retcd = 0;

   bioscomm_check();
   if (bioscomm_bufin_tail != bioscomm_bufin_head)
      retcd = 1;

   return retcd;
}

unsigned char bioscomm_lees_teken()
{
   register BYTE teken;

   bioscomm_check();

   teken = bioscomm_bufin[bioscomm_bufin_head];
   bioscomm_bufin_head++;
   bioscomm_bufin_head %= BUFFER_SIZE;

   return teken;
}
