class GELUID
{
   public:
   GELUID();
   ~GELUID();
   int beep(void);
   int freq(int hertz);
   int aan(void);
   int uit(void);

   private:
   int frequentie;
   int status;
};