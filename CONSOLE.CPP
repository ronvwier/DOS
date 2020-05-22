#include <conio.h>
#include <iostream.h>
#include "console.h"


Console::Send(const char C)
{
   cout << C;
   return 1;
}

Console::Receive(char & C)
{
   if (IsData())
   {
      C = getch();
      return 1;
   }
   else return 0;
}

Console::Receive(char & C, double TimeOut)
{
   C = getch();
   return 1;
}

Console::IsData() const
{
   return kbhit();
}


