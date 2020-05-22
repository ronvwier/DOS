#ifndef _Modem_H_
   #define _Modem_H_
   #include "CommPort.H"

class Modem
{
   public:
   Modem(CommPort &Port);

   IsOk(void);

   DialTone(int YesNo);
   Dial(const char * Phone);
   HangUp(void);
   Connection(void);

   Command(const char * cmd,char * answer,long mstimeout);

   private:
   CommPort * PtrPort;
   int Tone;
} ;
#endif