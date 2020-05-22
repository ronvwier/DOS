#include <ctype.h>
#include <string.h>
#include "cmdline.h"

CmdLine::CmdLine(const int p_argc, const char **p_argv)
{
   argc = p_argc;
   argv = (char **)p_argv;
}

int CmdLine::IsOption(const int arg) const
   { return arg <= argc && argv[arg][0] == '/' && argv[arg][1]!='\0'; }

char CmdLine::OptionID(const int arg) const
   { return toupper(argv[arg][1]); }

const String CmdLine::OptionValue(const int arg) const
{
   static String Hulp;

   Hulp = &argv[arg][2];
   return Hulp;
}

const String CmdLine::Value(const int arg) const
   { return argv[arg]; }

int CmdLine::Words() const
   { return argc;}
