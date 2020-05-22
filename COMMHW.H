#ifndef _CommHW_H_
   #define _CommHW_H_
   #include "commport.h"

#define COMMHW_UART_ONBEKEND   0
#define COMMHW_UART_8250       1
#define COMMHW_UART_16450      2
#define COMMHW_UART_16550      3
#define COMMHW_UART_16550A     4
#define COMMHW_FOSSIL         11

#define COMMHW_VIA_HARDWARE    1
#define COMMHW_VIA_FOSSIL      2
#define COMMHW_VIA_BIOS        3

class CommHW: public CommPort
{
   public:

   CommHW(short PortNr);
   ~CommHW(void);

   Open(void);
   virtual Close(void);

   virtual SetInterface(short type);
   virtual IsOk(void) const;
   virtual Type_UART(void);
   virtual DTR(short Action);
   virtual Carrier(void) const;
   virtual Send(const char);
   virtual Send(const char *);
   virtual Receive(char &);
   virtual IsData(void) const;
   virtual Irq(short IrqNr);

   private:
   short IrqNr;
   short Interface;
} ;

#endif
