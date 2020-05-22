#include <string.h>

#include "modem.h"
#include "wekker.h"

Modem::Modem(CommPort &Port)
{
  Tone = 1;
  PtrPort = &Port;
}

Modem::DialTone(int UseTone)
{ Tone = UseTone; }

Modem::IsOk()
{
   int OK = 0;
   char regel[80];

   /* Verstuur attentie en verwacht OK */
   if (Command("",regel,1000))
       OK = regel[0] == 'O' && regel[1] == 'K';

   return OK;
}

Modem::Dial(const char * Phone)
{
   char Hulp[255];
   char Hulp2[255];

   Command("&C1",Hulp2,5000);

   strcpy(Hulp,"D");
   if (Tone)
      strcat(Hulp,"T");
   else
      strcat(Hulp,"P");

   strcat(Hulp,Phone);
   Command(Hulp,Hulp2,60000);

   wekker_start(15000);

   while ( !PtrPort->Carrier() && !wekker_alarm());

   return PtrPort->Carrier();
}

Modem::HangUp()
{
   char antwoord[32];
   char teken;

   /* Wacht 1 seconde en stuur dan +++ */
   wekker_wacht(1000);
   PtrPort->Flush();
   PtrPort->Send("+++");

   /* Wacht op OK\r */
   while (PtrPort->Receive(teken,5000) && teken != '\r');

   /* Stuur hangup */
   Command("H",antwoord,5000);

   /* Drop het DTR signaal */
   PtrPort->DTR(0);

   return !(PtrPort->Carrier());
}

Modem::Connection()
{  return PtrPort->Carrier(); }

Modem::Command(const char * commando, char * antwoord,long timeout )
{
   int OK = 0;
   char teken;

   /* Leeg de in buffer en het antwoord */
   PtrPort->Flush();
   strcpy(antwoord,"");

   OK = PtrPort->Send("ATE1Q0");

   if (OK)
   {
      OK = PtrPort->Send(commando);
      if (OK)
	 OK = PtrPort->Send('\r');
   }

   /* Lees eerst de orginele vraag (echo) uit t/m CR */
   while (PtrPort->Receive(teken,500) && teken != '\r');

   /* Het eerste teken van het antwoord moet binnen de time-out binnenkomen */
   if (!PtrPort->Receive(teken,timeout))
      OK = 0;
   else
   {
      int eerste_lf = 1;
      int i = 0;
      do
      {
	 if (teken != '\r')
	 {
	    if (teken != '\n' || (teken == '\n' && !eerste_lf) )
	    {
	       antwoord[i] = teken;
	       i++;
	    }
	    if (teken == '\n')
	       eerste_lf = 0;
	 }
	 /* Het volgende teken moet binnen 0.5 sec volgen */
      } while (PtrPort->Receive(teken,500));
      antwoord[i] = '\0';
   }

   PtrPort->Flush();

   return OK;
}



