#ifndef _LptPort_H_
   #define _LptPort_H_
#include "ioport.h"

class LptPort: public IOPort
{
   public:

	   LptPort(int InPortNr);
   virtual ~LptPort(void);

   virtual Open(void);
   virtual Close(void);

   virtual IsOk(void) const;
   virtual Send(const char);
   virtual Send(const char *);

   protected:
   int PortNr;
   unsigned long PortHandle;
} ;
#endif