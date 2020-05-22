#ifndef _CommPort_H_
   #define _CommPort_H_

#include "ioport.h"

class CommPort: public IOPort
{
   public:

   CommPort(short PortNr);
   virtual ~CommPort();

   virtual DTR(short Action);
   virtual Baud(void) const;
   virtual Baud(long BPS);
   virtual Bits(short NrBits);
   virtual Carrier(void) const;

   protected:
   short PortNr;
   long BPS;
   short NrBits;
} ;
#endif
