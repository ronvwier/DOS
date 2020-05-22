/* ------------------------------------------------------------------------- */
/* PATROON    : Tekstpatronen testen                                         */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stddef.h"
#include "malloc.h"
#include "string.h"
#include "patroon.h"

#define MAX_PATROON 16

#define MATCH(_teken_,_patroon_) (_teken_ == _patroon_ || _patroon_ == '?')

static int init = 0;
static int gelijk[MAX_PATROON];
static int lengte[MAX_PATROON];
static char * tekst[MAX_PATROON];

PATROON_NR patroon_tekst(char te_vinden_tekst[])
{
   register int i;
   PATROON_NR handle;

   if (!init)
   {
      /* Initieer de globale variabelen */
      for (i = 0; i < MAX_PATROON; i++) tekst[i] = NULL;
      init = 1;
   }

   /* Zoek een vrije tekst */
   for (i = 0; i < MAX_PATROON && tekst[i] != NULL; i++);
   if (i >= MAX_PATROON)
      handle = -1;
   else
   {
      /* Sla de zoek tekst op in dynamisch geheugen */
      handle = i;
      tekst[i] = (char *)malloc(strlen(te_vinden_tekst) + 1);
      if (tekst[i] == NULL)
         handle = -1;
      else
      {
         strcpy(tekst[i],te_vinden_tekst);
         lengte[i] = strlen(te_vinden_tekst);
         gelijk[i] = 0;
      }
   }

   return handle;
}

int patroon_teken(char teken)
{
   register int i,aantal_gelijk = 0;
   int naar_links,y;
   char * ptr_tekst;

   for (i = 0; i < MAX_PATROON; i++)
   {
      /* Kijk of het teken in de tekst voorkomt op de volgende
         nog onbekeken positie */
      ptr_tekst = tekst[i];
      if (ptr_tekst != NULL)
      {
         /* Gooi een volledig gevonden tekst weg */
         if (gelijk[i] == lengte[i])
            gelijk[i] = 0;
         /* Is dit teken ongelijk schuif het gelijke patroon naar links */
         while (gelijk[i] > 0 && !(MATCH(teken,ptr_tekst[gelijk[i]])) )
         {
            /* Schuif het gevonden patroon naar links tot het nogmaals voorkomt */
            /* bijvoorbeeld zoek ABCAT in ABCABCAT */
            /*                   A--------A        */
            /*                    B--------B       */
            /*                     C--------C      */
            /*                      A--------A     */
            /*                       T<------>B    */
            /*                   AB          AB    */
            /*                     C           C   */
            /*                      A           A  */
            /*                       T           T */
            naar_links = 0;
            do
            {
               /* Schuif 1 positie naar links */
               naar_links++;
               /* Tel aantal gelijke posities */
               for (y = 0; y < (gelijk[i] - naar_links) && ptr_tekst[y] == ptr_tekst[y+naar_links]; y++);
            } while ((gelijk[i] - naar_links) != y) ;
            gelijk[i] = y;
         }
         /* Kijk of dit teken gelijk is met de huidige positie */
         if (MATCH(teken,ptr_tekst[gelijk[i]]))
            gelijk[i] += 1;
         /* Tel het aantal volledig gelijke teksten */
         if (gelijk[i] == lengte[i])
            aantal_gelijk++;
      }
   }

   return aantal_gelijk;
}

int patroon_gevonden(PATROON_NR nummer)
{
   return (gelijk[nummer] == lengte[nummer]);
}

void patroon_einde(PATROON_NR nummer)
{
   free(tekst[nummer]);
   tekst[nummer] = NULL;
}
