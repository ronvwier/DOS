#ifndef _Ordered_
   #define _Ordered_

   class Ordered
   {
       public:

       virtual operator ==(const Ordered &Value) const = 0;
       virtual operator >(const Ordered &Value)  const {return 0; };
       virtual operator <(const Ordered &Value)  const {return 0; };

   } ;

inline int operator !=(const Ordered &Val1,const Ordered &Val2) { return !(Val1==Val2); };
inline int operator >=(const Ordered &Val1,const Ordered &Val2) { return !(Val1<Val2); };
inline int operator <=(const Ordered &Val1,const Ordered &Val2) { return !(Val1>Val2); };


#endif