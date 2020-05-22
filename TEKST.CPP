/* ------------------------------------------------------------------------- */
/* TEKST                                                                     */
/* Taak       : Operaties op teksten (strings)                               */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>

#include "tekst.h"

void tekst_vervang(char veld[],char zoek_tekst[],char vervangende_tekst[])
{
    char hulp[255];
    char * ptr_gevonden;
    char * ptr_zoek;

    ptr_zoek = veld;
    ptr_gevonden = strstr(ptr_zoek,zoek_tekst);

    while (ptr_gevonden != NULL)
    {
       /* Kap veld af op eerste positie */
       ptr_gevonden[0] = '\0';
       /* Bewaar alles na zoek_tekst */
       strcpy(hulp,&ptr_gevonden[strlen(zoek_tekst)]);
       /* Plak vervangende tekst en rest er achter aan */
       strcat(veld,vervangende_tekst);
       ptr_zoek = &veld[strlen(veld)];
       strcat(veld,hulp);

       ptr_gevonden = strstr(ptr_zoek,zoek_tekst);
    }
}

void tekst_trim(char veld[])
{
   register int i,j,l;

   /* Verwijder de achter spaties */
   l = strlen(veld);
   for (i = l -1; veld[i] == ' ' && i >= 0 ; i--)
     veld[i] = '\0';

   /* Verwijder de voor spaties */
   for (j = 0; veld[j] == ' ' ; j++ );
   l = strlen(veld) - j;
   if ( j > 0 )
   {
      for (i = 0; i <= l ; i++ )
         veld[i] = veld[i+j];
   }
}
