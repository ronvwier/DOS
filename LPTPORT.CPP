#define INCL_DOSPROCESS
#define INCL_DOS
#define INCL_DOSDEVIOCTL

#include <os2.h>

#include <string.h>

#include "LptPort.h"
#include "wekker.h"

LptPort::LptPort(int InPortNr)
{
   PortNr = InPortNr;
   State = Inactive;
}

LptPort::~LptPort()
{
   if (State == Active)
      Close();
} ;

LptPort::Open()
{
   if (State == Inactive)
   {
      char hulp[12];
      ULONG status;

      strcpy(hulp,"LPT?");
      hulp[3] = (unsigned char)('0' + PortNr);

      if (DosOpen((PSZ)hulp,&PortHandle,&status,0,0,
		  OPEN_ACTION_OPEN_IF_EXISTS,
		  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
		  FILE_NORMAL) == 0)
	 State = Active;
   }
   return 1;
}

LptPort::Close()
{
   if (State == Active)
   {
      DosClose(PortHandle);
      State = Inactive;
   }
   return 1;
}

LptPort::IsOk() const
   { return 1; }

LptPort::Send(const char C)
{
   int rc = 0;
   ULONG written;

   if (DosWrite(PortHandle,(void *)&C,1,&written) == 0)
      if (written == 1)
	 rc = 1;
   return rc;
}
LptPort::Send(const char *S)
{
   int rc = 0;
   ULONG written;
   ULONG bytes;

   bytes = strlen(S);

   if (DosWrite(PortHandle,(void *)S,bytes,&written) == 0)
      if (written == bytes)
	 rc = 1;

   return rc;
}


