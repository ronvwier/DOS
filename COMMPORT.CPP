#include "commport.h"

CommPort::CommPort(short p_PortNr)
{  PortNr = p_PortNr;
   BPS    = 0;
   NrBits = 8;
}

CommPort::~CommPort() { } ;

CommPort::DTR(short action)
{
   if (action==0 && State==Active)
      Close();
   return 0;
}

CommPort::Baud() const
   { return BPS; }

CommPort::Baud(long P_BPS)
   { BPS = P_BPS; return BPS ;}

CommPort::Bits(short PNrBits)
  { NrBits = PNrBits; return 1; }

CommPort::Carrier() const
   { return State==Active; }
