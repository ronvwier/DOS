/* ------------------------------------------------------------------------- */
/* FOSSIL     : Interface voor datacommunicatie via een FOSSIL interface     */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <conio.h>
#include <bios.h>
#include <dos.h>

#include "fossil.h"

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Interne gegevens                                                          */
/*                                                                           */
/* ------------------------------------------------------------------------- */

typedef unsigned char BYTE;

static union  REGS  regs;
static struct SREGS sregs;

/* ------------------------------------------------------------------------- */
/* FOSSIL_    Communicatie via FOSSIL driver volgens FSC-0015 specs          */
/*-------------------------------------------------------------------------- */

#define FOSSIL_PS_RXCHARS      0x0100          /* Characters in input buffer */
#define FOSSIL_PS_RXOVER       0x0200          /* Receive buffer overrun */
#define FOSSIL_PS_MASK         0x0008          /* Bits always set */
#define FOSSIL_PS_TXCHARS      0x2000          /* Transmit buffer not full */
#define FOSSIL_PS_TXEMPTY      0x4000          /* Transmit buffer is empty */
#define FOSSIL_PS_CARRIER      0x0080          /* Carrier detect is high */

static int fossil_port = 0;

static int fossil_check(void);
static int fossil_check()
{
   int fossil_aktief;

   regs.h.ah = 0x04;
   regs.h.al = 0x00;
   regs.x.bx = 0x0000;
   regs.x.dx = 0x00FF;
   int86(0x14,&regs,&regs);
   if (regs.x.ax == 0x1954)
      fossil_aktief = 1;
   else
      fossil_aktief = 0;

   return fossil_aktief;
}

int fossil_aktief(void);
int fossil_aktief()
{
    static int fossil = -1;

    if (fossil < 0)
       fossil = fossil_check();

    return fossil;
}

int fossil_init(int poort);
int fossil_init(int poort)
{
   regs.h.ah = 0x04;
   regs.h.al = 0x00;
   regs.x.bx = 0x0000;
   regs.x.dx = poort;
   fossil_port = poort;
   int86(0x14,&regs,&regs);

   return (regs.x.ax == 0x1954);
}

int fossil_deinit(void);
int fossil_deinit()
{
   regs.h.ah = 0x05;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);

   return 1;
}

unsigned int fossil_request_status(void);
unsigned int fossil_request_status()
{
   regs.h.ah = 0x03;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);
   return regs.x.ax;
}

int fossil_transmit(unsigned char teken);
int fossil_transmit(unsigned char teken)
{
   regs.h.ah = 0x01;
   regs.h.al = teken;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);

   return 1;
}

unsigned char fossil_receive(void);
unsigned char fossil_receive()
{
   regs.h.ah = 0x02;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);

   return regs.h.al;
}

unsigned int  fossil_peek(void);
unsigned int  fossil_peek()
{
   regs.h.ah = 0x0C;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);
   return regs.x.ax;
}

int fossil_lower_dtr(void);
int fossil_lower_dtr()
{
   regs.h.ah = 0x06;
   regs.h.al = 0x00;
   regs.x.dx = fossil_port;
   int86(0x14,&regs,&regs);

   return 1;
}

int fossil_set_baud(long baud, int bits);
int fossil_set_baud(long baud, int bits)
{
   unsigned char par_bits, baud_bits;

   if (bits == 8)
      par_bits = 0x03;
   else
      par_bits = 0x1A;

   baud_bits = 0xFF;
   if (baud ==   300L) baud_bits = 0x02;
   if (baud ==   600L) baud_bits = 0x03;
   if (baud ==  1200L) baud_bits = 0x04;
   if (baud ==  2400L) baud_bits = 0x05;
   if (baud ==  4800L) baud_bits = 0x06;
   if (baud ==  9600L) baud_bits = 0x07;
   if (baud == 19200L) baud_bits = 0x00;
   if (baud == 38400L) baud_bits = 0x01;
   if (baud_bits != 0xFF)
   {
      regs.h.ah = 0x00;
      regs.h.al = (unsigned char)((baud_bits << 5) | par_bits);
      regs.x.dx = fossil_port;
      int86(0x14,&regs,&regs);
      return 1;
   }
   else
      return 0;
}
