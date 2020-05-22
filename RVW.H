#ifndef _Included_RvW_
#define _Included_RvW_

#include "stdio.h"
#include "stdlib.h"

class RvW
{
   public:
   void Error(char * ClassName, char * Method, char * Text)
   {
       fprintf(stderr,"Fatal error '%s' in Class '%s' Method '%s'\n",
	       Text,ClassName,Method);
       exit(255);
   };
   char * ClassName(void) { return "RvW" ; };
} ;

#endif