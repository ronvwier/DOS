/* ------------------------------------------------------------------------- */
/* DATACOMM   : Interface voor datacommunicatie via een HAYES modem          */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <string.h>

#include "wekker.h"

#include "fossil.h"
#include "hwcomm.h"
#include "bioscomm.h"

#include "commhw.h"


CommHW::CommHW(short PortNr) : CommPort(PortNr)
{
   IrqNr = 0;
   if (fossil_aktief())
       Interface = COMMHW_VIA_FOSSIL;
   else
       Interface = COMMHW_VIA_HARDWARE;
}

CommHW::~CommHW()
{
}

CommHW::Open()
{
   /* Voorzichtig, er kan al een verbinding aanwezig zijn */

   /* Stel modem in */
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
	   if (fossil_init(PortNr-1) == 0)
	      return 0;
	   break;
      case COMMHW_VIA_BIOS:
	   if (bioscomm_init(PortNr) == 0)
	      return 0;
	   break;
      case COMMHW_VIA_HARDWARE:
	   if (hwcomm_init(PortNr,IrqNr) == 0)
	      return 0;
	   break;
   }
   if (BPS > 0)
   {
      switch(Interface)
      {
      case COMMHW_VIA_FOSSIL:
	   fossil_set_baud(BPS,8);
	   break;
      case COMMHW_VIA_BIOS:
	   bioscomm_set_baud(BPS,8);
	   break;
      case COMMHW_VIA_HARDWARE:
	   hwcomm_set_baud(BPS);
	   hwcomm_set_N81();
	   break;
      }
   }

   return 1;
}

CommHW::Close()
{
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
	   fossil_deinit();
	   break;
      case COMMHW_VIA_BIOS:
           bioscomm_einde();
           break;
      case COMMHW_VIA_HARDWARE:
           hwcomm_einde();
           break;
   }
   return 1;
}

CommHW::IsOk() const
{
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
           return 1;
	   break;
      case COMMHW_VIA_BIOS:
	   if (bioscomm_is_poort(PortNr))
	      return 1;
	   break;
      case COMMHW_VIA_HARDWARE:
	   if (hwcomm_is_poort(PortNr))
	      return 1;
           break;
   }
   return 0;
}

CommHW::Type_UART()
{
   int UART_type = COMMHW_UART_ONBEKEND;

   if (IsOk() && Interface == COMMHW_VIA_HARDWARE)
      UART_type = hwcomm_type_uart(PortNr);

   return UART_type;
}

CommHW::DTR(short On)
{
   /* DTR afzetten */
   if (!On)
   {
      switch(Interface)
      {
	 case COMMHW_VIA_FOSSIL:
	      fossil_lower_dtr();
	      break;
	 case COMMHW_VIA_HARDWARE:
	      hwcomm_lower_dtr();
	      break;
      }
   }
   return 1;
}

CommHW::Carrier() const
{
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
           if (fossil_request_status() & FOSSIL_PS_CARRIER)
              return 1;
           break;
      case COMMHW_VIA_BIOS:
           if (bioscomm_carrier())
              return 1;
           break;
      case COMMHW_VIA_HARDWARE:
           if (hwcomm_carrier())
              return 1;
	   break;
   }

   return 0;
}

CommHW::Send(const char *tekst)
{
    return CommPort::Send(tekst);
}

CommHW::Send(const char teken)
{
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
           if (fossil_transmit(teken))
              return 1;
           break;
      case COMMHW_VIA_BIOS:
	   if (bioscomm_zend(teken))
               return 1;
           break;
      case COMMHW_VIA_HARDWARE:
           if (hwcomm_zend(teken))
	      return 1;
           break;
   }

   return 0;
}

CommHW::Receive(char & Teken)
{
   if (IsData())
   {
      switch(Interface)
      {
         case COMMHW_VIA_FOSSIL:
              Teken = fossil_receive();
	      break;
         case COMMHW_VIA_BIOS:
              Teken = bioscomm_lees_teken();
              break;
         case COMMHW_VIA_HARDWARE:
	      Teken = hwcomm_lees_teken();
              break;
      }

      if (NrBits == 7)
         /* Strip het hoogste bitje */
         Teken = (unsigned char)(Teken & 0x7F);

      return 1;
   }

   return 0;
}

CommHW::IsData() const
{
   switch(Interface)
   {
      case COMMHW_VIA_FOSSIL:
           if (fossil_peek() != 0xFFFF)
	      return 1;
           break;
      case COMMHW_VIA_BIOS:
           if (bioscomm_teken_aanwezig())
              return 1;
           break;
      case COMMHW_VIA_HARDWARE:
           if (hwcomm_teken_aanwezig())
	       return 1;
           break;
   }
   return 0;
}

CommHW::Irq(short P_IrqNr)
{
   IrqNr = P_IrqNr;
   return 1;
}

CommHW::SetInterface(short type)
{
   Interface = type;
   return 1;
}
