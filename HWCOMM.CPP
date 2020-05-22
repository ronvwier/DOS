/* ------------------------------------------------------------------------- */
/* HWCOMM     : Interface voor datacommunicatie via hardware RS232 poort     */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* LAGENMODEL:                                                               */
/*                                                                           */
/*   Hardware I/O:                                                           */
/*                                                                           */
/*     ASYNC_     aansturen van de asynchrone hardware poorten               */
/*     PIC_       aansturen van de programmable interrupt controller         */
/*                                                                           */
/*   Communicatie:                                                           */
/*                                                                           */
/*     HWCOMM_    communicatie via ASYNC poorten met gebruikt van PIC voor   */
/*                interrupt gestuurd ontvangen van tekens (2K buffer)        */
/* ------------------------------------------------------------------------- */

#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <string.h>

#include "wekker.h"
#include "hwcomm.h"

#pragma check_stack(off)

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Interne gegevens                                                          */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#define BUFFER_SIZE 2048

typedef unsigned char BYTE;

static int modem_poort = 1;

static union  REGS  regs;
static struct SREGS sregs;

/* ------------------------------------------------------------------------- */
/* Laag       Hardware I/O                                                   */
/* ASYNC_     aansturen van de asynchrone hardware poorten                   */
/* ------------------------------------------------------------------------- */

#define ASYNC_ID_LINE_STAT      0
#define ASYNC_ID_NONE           1
#define ASYNC_ID_BUFFER_EMPTY   2
#define ASYNC_ID_DATA           4
#define ASYNC_ID_MODEM_STAT     6

#define ASYNC_FC_FIFO_ENABLE    1

#define ASYNC_II_SEND_FIFO      0x40
#define ASYNC_II_RECV_FIFO      0x80

#define ASYNC_INT_TYPE_NONE     0
#define ASYNC_INT_TYPE_DATA     1
#define ASYNC_INT_TYPE_EMPTY    2
#define ASYNC_INT_TYPE_LINE     4
#define ASYNC_INT_TYPE_MODEM    8

#define ASYNC_LC_WL5            0
#define ASYNC_LC_WL6            1
#define ASYNC_LC_WL7            2
#define ASYNC_LC_WL8            3
#define ASYNC_LC_SB1            0
#define ASYNC_LC_SB2            4
#define ASYNC_LC_PBN            0
#define ASYNC_LC_PBO            8
#define ASYNC_LC_PBE           24

#define ASYNC_MC_DTR            1
#define ASYNC_MC_RTS            2
#define ASYNC_MC_OUT1           4
#define ASYNC_MC_OUT2           8
#define ASYNC_MC_LOOP          16

#define ASYNC_LS_DR             1
#define ASYNC_LS_OE             2
#define ASYNC_LS_PE             4
#define ASYNC_LS_FE             8
#define ASYNC_LS_BI            16
#define ASYNC_LS_HRE           32
#define ASYNC_LS_TRE           64

#define ASYNC_MS_DCTS           1
#define ASYNC_MS_DDSR           2
#define ASYNC_MS_TERI           4
#define ASYNC_MS_DDCD           8
#define ASYNC_MS_CTS           16
#define ASYNC_MS_DSR           32
#define ASYNC_MS_RI            64
#define ASYNC_MS_DCD          128

static int async_adres = 0x3f8;

#define IOR (async_adres)
#define IER (async_adres + 1)
#define IIR (async_adres + 2)
#define FCR (async_adres + 2)
#define LCR (async_adres + 3)
#define MCR (async_adres + 4)
#define LSR (async_adres + 5)
#define MSR (async_adres + 6)
#define SCR (async_adres + 7)

static int bewaar_ier;
static int bewaar_lcr;
static int bewaar_mcr;
static int bewaar_baud_ior;
static int bewaar_baud_ier;

static void async_herstel(void);
static void async_herstel()
{
   outp(IER,bewaar_ier);
   outp(MCR,bewaar_mcr);
   outp(LCR,0x80);
   outp(IOR,bewaar_baud_ior);
   outp(IER,bewaar_baud_ier);
   outp(LCR,bewaar_lcr);
}

static void async_bewaar(void);
static void async_bewaar()
{
   bewaar_ier = inp(IER);
   bewaar_mcr = inp(MCR);
   bewaar_lcr = inp(LCR);
   outp(LCR,0x80);
   bewaar_baud_ior = inp(IOR);
   bewaar_baud_ier = inp(IER);
   outp(LCR,bewaar_lcr);
}

#define async_get_ier()               (unsigned char)inp(IER)
#define async_get_iir()               (unsigned char)inp(IIR)
#define async_get_ior()               (unsigned char)inp(IOR)
#define async_get_lsr()               (unsigned char)inp(LSR)
#define async_get_mcr()               (unsigned char)inp(MCR)
#define async_get_msr()               (unsigned char)inp(MSR)
#define async_get_scr()               (unsigned char)inp(SCR)

#define async_put_fcr(_contr_reg)     outp(FCR,(int)_contr_reg)
#define async_put_ier(_int_type)      outp(IER,(int)_int_type)
#define async_put_ior(_teken)         outp(IOR,(int)_teken)
#define async_put_lcr(_contr_reg)     outp(LCR,(int)_contr_reg)
#define async_put_mcr(_contr_reg)     outp(MCR,(int)_contr_reg)
#define async_put_scr(_contr_reg)     outp(SCR,(int)_contr_reg)

static void async_set_baud(long snelheid);
static void async_set_baud(long snelheid)
{
   int oud;

   oud = inp(LCR);
   outp(LCR,0x80);

   outp(IOR,(int)(115200L/snelheid));
   outp(IER,0);

   outp(LCR,oud);
}

static long async_get_baud(void);
static long async_get_baud()
{
   int oud, poort_waarde;
   long baud;

   oud = inp(LCR);
   outp(LCR,0x80);
   poort_waarde = inp(IOR);
   if (poort_waarde == 0)
      baud = 0;
   else
      baud = (long)(115200L / (long)poort_waarde);
   outp(LCR,oud);

   return baud;
}

static void async_read_all(void);
static void async_read_all()
{
   register int i;
   for (i = 0; i <= 6; i++)
      inp(async_adres+i);
}

static int async_poort(int poortnummer)
{
   int retcd = 1;
   unsigned int * biosdata_comm;

   if (poortnummer >= 1 && poortnummer <= 4)
   {
      /* Adres van BIOS DATA gebied op 0040:0000 bevat COM poortadressen */
      FP_SEG(biosdata_comm) = 0x40;
      FP_OFF(biosdata_comm) = 0x00;

      if (biosdata_comm[poortnummer -1] != 0)
         async_adres = biosdata_comm[poortnummer -1];
      else
      {
         switch(poortnummer)
         {
            case 1:      async_adres = 0x3f8;     break;
            case 2:      async_adres = 0x2f8;     break;
            case 3:      async_adres = 0x3e8;     break;
            case 4:      async_adres = 0x2e8;     break;
         }
      }
   }
   else retcd = 0;

   return retcd;
}

/* ------------------------------------------------------------------------- */
/* Laag       Hardware I/O                                                   */
/* PIC_       Interface voor de 8259 programmable interrupt controler        */
/*-------------------------------------------------------------------------- */

static BYTE   vorige_status = 0;
static BYTE   masker;
static void * vorige_handler;

#define pic_uit()             _disable()
#define pic_aan()             _enable()
#define pic_terugmelding()    outp(0x20,0x20)
#define pic_bewaar_status()   vorige_status = (BYTE)inp(0x21)
#define pic_herstel_status()  outp(0x21,vorige_status)

static void pic_bewaar_handler(int irq_nummer);
static void pic_bewaar_handler(int irq_nummer)
{
   /* Bewaar oude handler */
   regs.h.ah = 0x35;
   regs.h.al = (BYTE)(irq_nummer + 8);
   int86x(0x21,&regs,&regs,&sregs);
   FP_SEG(vorige_handler) = sregs.es;
   FP_OFF(vorige_handler) = regs.x.bx;
}

static void pic_vervang_handler(int irq_nummer,void * handler);
static void pic_vervang_handler(int irq_nummer,void * handler)
{
   /* Zet nieuwe handler */
   regs.h.ah = 0x25;
   regs.h.al = (BYTE)(irq_nummer + 8);
   sregs.ds  = FP_SEG(handler);
   regs.x.dx = FP_OFF(handler);
   int86x(0x21,&regs,&regs,&sregs);
}

static void pic_herstel_handler(int irq_nummer);
static void pic_herstel_handler(int irq_nummer)
{
   pic_vervang_handler(irq_nummer,vorige_handler);
}

static void pic_zet_aan(int irq_nummer);
static void pic_zet_aan(int irq_nummer)
{
   /* Zet het maskeer bit voor de interrupt op 0 */
   masker = (BYTE)(0xFF^(0x01 << irq_nummer));
   outp(0x21,(inp(0x21) & masker));
}

static void pic_zet_uit(int irq_nummer);
static void pic_zet_uit(int irq_nummer)
{
   /* Zet het maskeer bit voor de interrupt op 1 */
   masker = (BYTE)(0x01 << irq_nummer);
   outp(0x21,(inp(0x21) | masker));
}

/* ------------------------------------------------------------------------- */
/* Laag       Communicatie                                                   */
/* HWCOMM_    Communicatie via ASYNC met interrupts via PIC                  */
/*-------------------------------------------------------------------------- */

static int  hwcomm_irq = 4;
static BYTE hwcomm_bufin[BUFFER_SIZE];
static int  hwcomm_bufin_head = 0;
static int  hwcomm_bufin_tail = 0;

static int  hwcomm_msr = 0;

static void interrupt hwcomm_handler(void);
static void interrupt hwcomm_handler()
{
   /* Lees teken in */
   hwcomm_bufin[hwcomm_bufin_tail] = async_get_ior();
   hwcomm_bufin_tail++;
   hwcomm_bufin_tail %= BUFFER_SIZE;

   /* Einde interrupt */
   pic_terugmelding();
}

static int hwcomm_flush(void);
static int hwcomm_flush()
{
   /* Leeg alle buffers */
   pic_uit();
   hwcomm_bufin_head  = 0;
   hwcomm_bufin_tail  = 0;
   pic_aan();
   wekker_start(1000);
   while (!(async_get_lsr() & ASYNC_LS_TRE) && !wekker_alarm());
   return (async_get_lsr() & ASYNC_LS_TRE);
}

int hwcomm_carrier()
{
   return async_get_msr() & ASYNC_MS_DCD;
}

int hwcomm_zend(unsigned char teken)
{
   /* Wacht totdat buffer leeg is */
   while (!(async_get_lsr() & ASYNC_LS_HRE) );

   /* Verzend teken */
   async_put_ior(teken);

   return 1;
}

int hwcomm_modem_poort(int poortnummer,int afw_irq)
{
   register int retcd = 1;

   if (async_poort(poortnummer))
   {
      switch(poortnummer)
      {
         case 1:
              hwcomm_irq = 4;
              break;
         case 2:
              hwcomm_irq = 3;
              break;
         case 3:
              hwcomm_irq = 4;
              break;
         case 4:
              hwcomm_irq = 3;
              break;
         default:
              retcd = 0;
      }
      if (afw_irq > 1)
      {
         if (afw_irq < 8)
            hwcomm_irq = afw_irq;
         else
            retcd = 0;
      }
   }
   else
      retcd = 0;

   return retcd;
}

int hwcomm_is_poort(int poortnummer)
{
   register int retcd = 0;

   if (hwcomm_modem_poort(poortnummer,0))
   {
      /* Bit 4,5,6,7 in IER moeten uitstaan */
      if (async_get_ier() & 0xF0)
         retcd = 0;
      else
         /* Bit 5,6,7 in MCR moeten uitstaan */
         if (async_get_mcr() & 0xE0)
            retcd = 0;
         else
            /* Bit 4, in IIR moeten uitstaan */
            if (async_get_iir() & 0x10)
               retcd = 0;
            else
            {
               async_bewaar();
               /* Probeer om 19200 bps te zetten */
               async_set_baud(19200L);
               if (async_get_baud() != 19200L)
                  retcd = 0;
               else
                  retcd = 1;
               async_herstel();
            }
   }
   else
      retcd = 0;

   return retcd;
}

int hwcomm_type_uart(int poortnummer)
{
   register int uart_type;

   uart_type = HWCOMM_UART_ONBEKEND;

   if (hwcomm_is_poort(poortnummer))
   {
      async_put_scr(0xAA);
      if (async_get_scr() != 0xAA)
         uart_type = HWCOMM_UART_8250;
      else
      {
         async_put_fcr(ASYNC_FC_FIFO_ENABLE);
         switch(async_get_iir() & (ASYNC_II_SEND_FIFO | ASYNC_II_RECV_FIFO))
         {
            case (ASYNC_II_SEND_FIFO | ASYNC_II_RECV_FIFO) :
                 uart_type = HWCOMM_UART_16550A; break;
            case 0x00 :
                 uart_type = HWCOMM_UART_16450;  break;
            default   :
                 uart_type = HWCOMM_UART_16550;  break;
         }
         async_put_fcr(0);
      }
   }

   return uart_type;
}


int hwcomm_init(int poort, int afwijkende_irq)
{
   hwcomm_modem_poort(poort,afwijkende_irq);

   /* Geef modem opdracht tot geen interrupts */
   async_bewaar();
   async_put_mcr(ASYNC_MC_DTR | ASYNC_MC_RTS);
   async_put_ier(0);
   async_read_all();

   /* Stel bps op 1200-N-8-1 indien niet ingevuld (koud modem) */
   if (async_get_baud() < 1200)
   {
      async_set_baud(1200L);
      async_put_lcr(ASYNC_LC_WL8 | ASYNC_LC_PBN);
   }

   /* Leeg alle buffers */
   hwcomm_flush();

   /* Zet de interrupt controller uit */
   pic_uit();

   /* Bewaar de huidige status/handler */
   pic_bewaar_handler(hwcomm_irq);
   pic_bewaar_status();

   /* Aktiveer de nieuwe interrupt handler */
   pic_vervang_handler(hwcomm_irq,(void *)hwcomm_handler);
   pic_zet_aan(hwcomm_irq);

   /* Geef modem opdracht tot interrupts bij ontvangst data/lege buffer */
   async_put_ier(ASYNC_INT_TYPE_DATA);
   async_put_mcr(ASYNC_MC_DTR | ASYNC_MC_RTS | ASYNC_MC_OUT2);

   /* Zet de interrupt controller weer aan */
   pic_aan();

   /* Leeg alle buffers */
   hwcomm_flush();

   return 1;
}

int hwcomm_einde()
{
   /* Wacht tot alles leeg */
   hwcomm_flush();

   /* Herstel oude toestand */
   pic_uit();
   pic_herstel_handler(hwcomm_irq);
   pic_herstel_status();
   async_herstel();
   pic_aan();

   return 1;
}

int hwcomm_lower_dtr()
{
   /* Wacht totdat elke buffer leeg is */
   hwcomm_flush();
   async_put_mcr(0);

   return 1;
}

int hwcomm_set_baud(long snelheid)
{
   async_set_baud(snelheid);

   return 1;
}

int hwcomm_set_E71()
{
   async_put_lcr(ASYNC_LC_WL7 | ASYNC_LC_PBE);

   return 1;
}

int hwcomm_set_N81()
{
   async_put_lcr(ASYNC_LC_WL8 | ASYNC_LC_PBN);

   return 1;
}

int hwcomm_teken_aanwezig()
{
   return hwcomm_bufin_tail != hwcomm_bufin_head;
}

unsigned char hwcomm_lees_teken()
{
   register BYTE teken;

   teken = hwcomm_bufin[hwcomm_bufin_head];
   hwcomm_bufin_head++;
   hwcomm_bufin_head %= BUFFER_SIZE;

   return teken;
}
