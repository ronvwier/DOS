#ifndef _String_
    #define _String_
    #include <iostream.h>

class String
{
    public:

    String(void);
    String(const char * init);
    String(const String &init);
    String(const long Nummer);
    String(const int Nummer);
    String(const double Nummer);

    ~String();

    void operator =(const String &Value);
    void operator =(const char * Value);

    operator const char *() const;
    operator long() const;
    operator short() const;
    operator int() const;
    operator double() const;

    operator ==(const String &Value) const;
    operator ==(const char * Value)  const;

    const String operator +(const String &Value) const;
    const String operator +(const char *Value) const;

    int operator <(const String &Value) const;
    int operator <(const char *Value) const;

    int operator >(const String &Value) const;
    int operator >(const char *Value) const;

    int operator !=(const String &Value) { return !(operator ==(Value)); };
    int operator !=(const char *  Value) { return !(operator ==(Value)); };

    int operator >=(const String &Value) { return !(operator <(Value)); };
    int operator >=(const char *  Value) { return !(operator <(Value)); };

    int operator <=(const String &Value) { return !(operator >(Value)); };
    int operator <=(const char *  Value) { return !(operator >(Value)); };

    void operator +=(const String &AddString);
    void operator +=(const char * AddString);

    private:
    char * Data;
} ;

ostream& operator <<(ostream &s,const String &Value);

#endif

