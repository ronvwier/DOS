#ifndef _CmdLine_H_
    #define _CmdLine_H_
    #include "Strings.h"
class CmdLine
{
   public:
   CmdLine(const int argc, const char **argv);

   int   IsOption(const int arg) const;
   int   Words(void) const;
   char  OptionID(const int arg) const;
   const String OptionValue(const int arg) const;
   const String Value(const int arg) const;

   private:
   int argc;
   char **argv;
} ;

#endif