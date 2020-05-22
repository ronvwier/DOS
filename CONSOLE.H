#ifndef _Console_H_
   #define _Console_H_

#include "ioport.h"

class Console: public IOPort
{
   public:

   virtual Send(const char Teken);
   virtual Receive(char & Teken);
   virtual Receive(char & Teken, double TimeOut);
   virtual IsData(void) const;
} ;
#endif