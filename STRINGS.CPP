#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strings.h"


String::String()
   { Data = new char; Data[0] = '\0'; }

String::String(const String &init)
   { Data = NULL; operator =((const char *)init.Data); }

String::String(const char * init)
   { Data = NULL; operator =((const char *)init); }

String::String(const long init)
{
    char Hulp[16];

    Data = NULL;
    ltoa(init,Hulp,10);
    operator =((const char *)Hulp);
}

String::String(const int init)
{
    char Hulp[16];

    Data = NULL;
    itoa(init,Hulp,10);
    operator =((const char *)Hulp);
}

String::String(const double init)
{
    int dec, sign, i;
    char Hulp[32], *antwoord;
    Data = NULL;

    antwoord = ecvt(init,16,&dec,&sign);
    if (sign != 0)
       strcpy(Hulp,"-");
    else
       strcpy(Hulp,"");

    if (dec <= 0)
    {
       strcat(Hulp,"0.");
       for (; dec < 0; dec++)
	  strcat(Hulp,"0");
       strcat(Hulp,antwoord);
    }
    else
    {
       strcat(Hulp,antwoord);
       Hulp[dec]   = '.';
       Hulp[dec+1] = '\0';
       strcat(Hulp,&antwoord[dec]);
    }
    for (i = strlen(Hulp); i> 0 && Hulp[i-1]=='0'; i--)
       Hulp[i-1] = '\0';

    operator =(Hulp);
}


String::~String()
   { delete Data;}

String::operator const char *() const
   { return (const char *)Data; }

String::operator double() const
   { return atof(Data); }

String::operator long() const
   { return atol(Data); }

String::operator int() const
   { return atoi(Data); }

void String::operator =(const char * Value)
{
    if (Data != NULL)
       delete Data;
    Data = new char[strlen(Value)+1];
    strcpy(Data,Value);
}

void String::operator =(const String &Value)
   { operator =((const char *)Value.Data); }

String::operator ==(const String &Value) const
   { return (strcmp(Data,Value.Data) == 0);}

String::operator ==(const char *Value) const
   { return (strcmp(Data,Value) == 0);}

int String::operator >(const String &Value) const
   { return strcmp(Data,Value.Data) > 0; }

int String::operator >(const char *Value) const
   { return strcmp(Data,Value) > 0; }

int String::operator <(const String &Value) const
   { return strcmp(Data,Value.Data) < 0; }

int String::operator <(const char *Value) const
   {  return strcmp(Data,Value) < 0; }

void String::operator +=(const char * AddString)
{
    char * DataNew;

    DataNew = new char[strlen(Data)+strlen(AddString)+1];
    strcpy(DataNew,Data);
    strcat(DataNew,AddString);

    operator =((const char *)DataNew);
    delete DataNew;
}

void String::operator +=(const String &AddString)
   { operator +=((const char *)AddString.Data); }

const String String::operator +(const char *AddString) const
{
     String Hulp;

     Hulp = Data;
     Hulp += AddString;

     return Hulp;
}

const String String::operator +(const String &AddString) const
   { return operator +(AddString.Data); }

ostream& operator << (ostream &s, const String &Value)
   { s << (const char *)Value ; return s; }


