#define INCL_DOS
#define INCL_DOSDEVIOCTL
#include <os2.h>

#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <string.h>

#include "wekker.h"
#include "taak.h"

#include "commdos.h"

CommDos::CommDos(short p_PortNr) : CommPort(p_PortNr)
{
}

CommDos::CommDos() : CommPort(0)
{
}

CommDos::~CommDos()
{
   if (State==Active)
      Close();
}

CommDos::Open(short p_PortNr)
{
   PortNr = p_PortNr;
   return Open();
}

CommDos::Open()
{
   static ULONG  ulDataLen;
   static USHORT BaudRate;
   static struct {
          BYTE Databits;
          BYTE Pariteit;
          BYTE Stopbits;
          } Opties;

   if (State==Inactive)
   {
      int rc;
      if (IsOk())
      {
         char hulp[12];
         ULONG status;

         strcpy(hulp,"COM?");
         hulp[3] = (unsigned char)('0' + PortNr);

         rc = DosOpen((PSZ)hulp,&com_handle,&status,0,0,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
              FILE_NORMAL);
         if (rc == 0)
         {
            State = Active;
            if (BPS > 0)
            {
               /* Set Baudrate */
               BaudRate = BPS;
               DosDevIOCtl(com_handle,
                        IOCTL_ASYNC,
                        ASYNC_SETBAUDRATE,
                        (PVOID)&BaudRate,sizeof(BaudRate),&ulDataLen,
                        NULL,0,NULL);
               /* Set N,8,1 */
               Opties.Databits = 8;    /* 8 */
               Opties.Pariteit = 0;    /* None */
               Opties.Stopbits = 0;    /* 1 stopbit */
               DosDevIOCtl(com_handle,
                        IOCTL_ASYNC,
                        ASYNC_SETLINECTRL,
                        (PVOID)&Opties,sizeof(Opties),&ulDataLen,
                        NULL,0,NULL);
            }
         }
      }
   }
   return State==Active;
}

CommDos::Close()
{
   if (State==Active)
      DosClose(com_handle);

   State = Inactive;
   return 1;
}

CommDos::IsOk() const
{
   register int retcd = 0;
   char hulp[12];
   ULONG status;
   HFILE T;

   strcpy(hulp,"COM?");
   hulp[3] = (unsigned char)('0' + PortNr);

   /* Probeer een OPEN */
   if (DosOpen((PSZ)hulp,&T,&status,0,0,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
              FILE_NORMAL) == 0)
   {
      retcd = 1;
      DosClose(T);
   }

   return retcd;
}

CommDos::Send(const char * S)
{
   int rc = 0;
   ULONG written;
   ULONG bytes;

   bytes = strlen(S);

   if (DosWrite(com_handle,(void *)S,bytes,&written) == 0)
      if (written == bytes)
         rc = 1;

   return rc;
}

CommDos::Send(const char C)
{
   int rc = 0;
   ULONG written;

   if (DosWrite(com_handle,(void *)&C,1,&written) == 0)
      if (written == 1)
         rc = 1;
   return rc;
}

CommDos::Receive(char & C)
{
   ULONG Read;

   if (IsData())
   {
      if (DosRead(com_handle,(void *)&C,1,&Read) == 0)
         if (Read == 1)
         {
            if (NrBits == 7)
               C = C & 0x7F;
            return 1;
         }
   }

   return 0;
}

CommDos::IsData() const
{
   static struct {
          USHORT Inq;
          USHORT Size;
          } QueueControl;
   static ULONG   ulDataLen;

   DosDevIOCtl(com_handle,
               IOCTL_ASYNC,
               ASYNC_GETINQUECOUNT,
               NULL,0,NULL,
               (PVOID) &QueueControl,sizeof(QueueControl),&ulDataLen);

   return QueueControl.Inq > 0;
}

CommDos::DTR(short aan)
{
   static struct {
          UCHAR OnMask;
          UCHAR OffMask;
          } ModemControl;
   static struct {
          USHORT ComError;
          } ModemError;

   static ULONG   ulDataLen, ulParmLen;

   if (aan)
   {
      ModemControl.OnMask  = 0x01;
      ModemControl.OffMask = 0xFF;
   }
   else
   {
      ModemControl.OnMask  = 0x00;
      ModemControl.OffMask = 0xFE;
   }

   DosDevIOCtl(com_handle,
               IOCTL_ASYNC,
               ASYNC_SETMODEMCTRL,
               (PVOID) &ModemControl,sizeof(ModemControl),&ulParmLen,
               (PVOID) &ModemError,sizeof(ModemError),&ulDataLen);

   return 1;
}

CommDos::Carrier() const
{
   static UCHAR   ModemControl;
   static ULONG   ulDataLen;

   DosDevIOCtl(com_handle,
               IOCTL_ASYNC,
               ASYNC_GETMODEMINPUT,
               NULL,0,NULL,
               (PVOID) &ModemControl,sizeof(ModemControl),&ulDataLen);

   return (ModemControl & DCD_ON) == DCD_ON;
}


