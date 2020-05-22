#ifndef _IOPort_H_
   #define _IOPort_H_

class IOPort
{
   public:

	   IOPort(void);
   virtual ~IOPort(void);

   virtual Open(void);
   virtual Close(void);

   virtual IsOk(void) const;
   virtual Send(const char);
   virtual Send(const char *);
   virtual Receive(char &);
   virtual Receive(char &C,long msTimeOut);
   virtual IsData(void) const;
   virtual Flush(void);

   protected:
   enum { Active, Inactive } State;
} ;
#endif