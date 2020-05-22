#ifndef _CommDos_H_
   #define _CommDos_H_
   #include "commport.h"

class CommDos: public CommPort
{
   public:

   CommDos(short PortNr);
   CommDos(void);
   ~CommDos(void);

   Open(void);
   Open(short PortNr);
   virtual Close(void);

   virtual IsOk(void) const;
   virtual DTR(short Action);
   virtual Carrier(void) const;
   virtual Send(const char);
   virtual Send(const char * S);
   virtual Receive(char &);
   virtual IsData(void) const;

   private:
   unsigned long com_handle;
} ;

#endif
