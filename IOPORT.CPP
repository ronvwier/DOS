#include "IOPort.h"
#include "wekker.h"
#include "taak.h"

IOPort::IOPort(void)
{  State  = Inactive; }

IOPort::~IOPort() { } ;

IOPort::Open()
{
   State = Active;
   return 1;
}

IOPort::Close()
{
   State = Inactive;
   return 1;
}

IOPort::IsOk() const
   { return 1; }

IOPort::Send(const char)
   { return 0; }

IOPort::Receive(char &)
   { return 0; }

IOPort::Receive(char &C,long msTimeOut)
{
   int OK = 0;

   wekker_start(msTimeOut);

   while ( !OK && !wekker_alarm())
   {
      if (IsData())
	 OK = Receive(C);
      else
      {
	 TAAK dummy;
	 dummy.run();
      }
   }

   return OK;
}

IOPort::IsData() const
   { return 0; }

IOPort::Flush()
{
   char Temp;
   while( IsData() )
      Receive(Temp);
   return 1;
}

IOPort::Send(const char *CS)
{
   int OK=1;
   int i=0;

   while ( CS[i] && OK )
   {
      OK=Send(CS[i]);
      i++;
   }

   return OK;
}


