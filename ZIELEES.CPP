/* ------------------------------------------------------------------------- */
/* ZIELEES                                                                   */
/* Taak       : Lezen van bestand in tekst/leesbaar/hex formaat              */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <string.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <memory.h>
#include <time.h>

#include "bestand.h"
#include "zielees.h"

static char hex_teken[] = "0123456789ABCDEF";

static
int      F_MAXREG,               /* maximum waarde voor regelnummer */
         F_LAST_READ,            /* laatst gelezen regel            */
         F_EOF;                  /* einde tekstbestand bereikt      */

/* ---------------------------------------------------------------------- */
/* Object : Lees                                                          */
/* ---------------------------------------------------------------------- */

/* export */

/* private data */

static unsigned long tekst_ptr[MAXREG+2];
static int zielees_mode = ZIELEES_MODE_TXT;

/* ------------------------------------------------------------------------- */
/* ZIELEES_OPEN                                                              */
/*                                                                           */
/* Taak       : openen bestand                                               */
/*                                                                           */
/* Begin      : naam gevuld                                                  */
/*                                                                           */
/* Einde      : ZIELEES_GELUKT  + bestand open                               */
/*              ZIELEES_MISLUKT + bestand kon niet ge-opend worden           */
/* ------------------------------------------------------------------------- */
int           zielees_open(char naam[])
{
   F_LAST_READ = 0;
   F_EOF = 0;
   F_MAXREG = MAXREG;         /* The Sky is The limit */

   return bestand_open(naam);
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_SET_STDIN                                                         */
/*                                                                           */
/* Taak       : openen STDIN als bestand                                     */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : ZIELEES_GELUKT  + STDIN open                                 */
/*              ZIELEES_MISLUKT + STDIN is geen bestand                      */
/* ------------------------------------------------------------------------- */
int           zielees_set_stdin()
{
   F_LAST_READ = 0;
   F_EOF = 0;
   F_MAXREG = MAXREG;         /* The Sky is The limit */

   return bestand_set_stdin();
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_NAAM                                                              */
/*                                                                           */
/* Taak       : opvragen naam huidig bestand                                 */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : naam teruggegeven                                            */
/* ------------------------------------------------------------------------- */
char *        zielees_naam()
{
   return bestand_naam();
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_SLUIT                                                             */
/*                                                                           */
/* Taak       : sluiten huidig bestand                                       */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : bestand dicht                                                */
/* ------------------------------------------------------------------------- */
int           zielees_sluit()
{
   return bestand_sluit();
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_MODE_SET                                                          */
/*                                                                           */
/* Taak       : instellen mode (TEKST/LEESBAAR/HEX)                          */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : mode ingesteld                                               */
/* ------------------------------------------------------------------------- */
void zielees_mode_set(int new_mode)
{
   F_LAST_READ = 0;
   F_EOF = 0;
   F_MAXREG = MAXREG;         /* The Sky is The limit */
   zielees_mode = new_mode;
   bestand_set_positie(0L);
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_MODE_GET                                                          */
/*                                                                           */
/* Taak       : opvragen huidige mode                                        */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : mode teruggegeven                                            */
/* ------------------------------------------------------------------------- */
int zielees_mode_get()
{
   return zielees_mode;
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_MAXREG                                                            */
/*                                                                           */
/* Taak       : opvragen maximum regelnummer tot nu toe bekend               */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : maximum regelnummer teruggegeven                             */
/* ------------------------------------------------------------------------- */
int zielees_maxreg(void)
{
   return F_MAXREG;
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_REGEL                                                             */
/*                                                                           */
/* Taak       : lezen regel                                                  */
/*                                                                           */
/* Begin      : curreg bevat te lezen regelnummer                            */
/*                                                                           */
/* Einde      : NULL terugegeven + regelnummer bestaat niet                  */
/*              regel terugegeven + regelnummer bestaat wel                  */
/* ------------------------------------------------------------------------- */
char * zielees_regel(int curreg)
{
   static
   char           *regel;
   register
   int             i,j;
   static
   char           hulpregel[REGLEN + 2];
   static          char   tekstregel[REGLEN + 2];
   char   intern[16];
   int             uit;
   char   tekens[16];
   int             gelezen = 0;
   int             lengte;
   unsigned long   bytes_skipped;

   regel = NULL;

   if (curreg > F_LAST_READ)
   {
      for ( i = F_LAST_READ+1; (i <= curreg) && (F_EOF !=BESTAND_EINDE) ; i++ )
      {
	  if (F_EOF != BESTAND_EINDE)
	  {
	     tekst_ptr[i] = bestand_get_positie();
	     if (zielees_mode == ZIELEES_MODE_TXT)
                F_EOF = bestand_lees_regel(tekstregel);
             if (zielees_mode == ZIELEES_MODE_HEX)
             {
                gelezen = bestand_lees_bytes(tekstregel,16);
                if (gelezen < 1)
                   F_EOF = BESTAND_EINDE;
             }
             if (zielees_mode == ZIELEES_MODE_FILTER)
             {
                F_EOF = bestand_lees_filter(tekstregel,&bytes_skipped);
                tekst_ptr[i] += bytes_skipped;
             }

/*	     F_LAST_READ = i; */

	     if (F_EOF != BESTAND_EINDE)
	     {
		regel = tekstregel;
		F_LAST_READ = i;
	     }
	     else
		F_MAXREG = F_LAST_READ;
          }
      }
   }
   else
   {
      unsigned long bewaar;

      /* Bewaar huidige positie */
      bewaar = bestand_get_positie();

      /* Positioneer op regel */
      bestand_set_positie(tekst_ptr[curreg]);
      if ( zielees_mode == ZIELEES_MODE_TXT )
      {
         if (bestand_lees_regel(tekstregel) != BESTAND_EINDE)
            regel = tekstregel;
      }
      if ( zielees_mode == ZIELEES_MODE_HEX )
      {
         gelezen = bestand_lees_bytes(tekstregel,16);
         if (gelezen != 0)
            regel = tekstregel;
      }
      if ( zielees_mode == ZIELEES_MODE_FILTER)
      {
         if (bestand_lees_filter(tekstregel,&bytes_skipped) != BESTAND_EINDE)
            regel = tekstregel;
      }

      /* Herstel oude positie */
      bestand_set_positie(bewaar);
   }

   if ( zielees_mode == ZIELEES_MODE_TXT && regel != NULL )
   {
      /* Converteer tab tekens */
      strcpy(hulpregel,tekstregel);
      j = 0;
      for (i = 0; hulpregel[i] && j < REGLEN; i++)
      {
         if (hulpregel[i] == '\t')
         {
            do
            {
               tekstregel[j] = ' ';
               j++;
            }
            while ( (j % 8) && (j < REGLEN) );
         }
         else
         {
            tekstregel[j] = hulpregel[i];
            j++;
         }
      }
      tekstregel[j] = '\0';
   }


   if ( zielees_mode == ZIELEES_MODE_HEX && regel != NULL )
   {
      /* hex conversie */
      memcpy(intern,tekstregel,16);
      strcpy(tekstregel,"");

      for ( uit = 0; uit < gelezen; uit++ )
      {
         tekens[0] = hex_teken[intern[uit] / 16];
         tekens[1] = hex_teken[intern[uit] % 16];
         tekens[2] = ' ';
         tekens[3] = 0;
         strcat(tekstregel,tekens);
         if ( uit == 7 ) strcat(tekstregel,"- ");
      }

      for ( ; uit < 16; uit++ )
      {
         strcat(tekstregel,"   ");
         if ( uit == 7 ) strcat(tekstregel," -");
      }

      strcat(tekstregel," *");
      lengte = strlen(tekstregel);
      for ( uit = 0; uit < gelezen; uit++ )
      {
         if ( intern[uit] < ' ' )
            tekstregel[lengte] = '.';
         else
            tekstregel[lengte] = intern[uit];
         lengte++;
      }
      tekstregel[lengte] = '\0';
      strcat(tekstregel,"*");
   }

   return(regel);
}

/* ------------------------------------------------------------------------- */
/* ZIELEES_OFFSET                                                            */
/*                                                                           */
/* Taak       : geef offset terug van de opgegeven regel                     */
/*                                                                           */
/* Begin      : regelnummer gevuld                                           */
/*                                                                           */
/* Einde      : offset tov. bestandbegin teruggegeven                        */
/* ------------------------------------------------------------------------- */
unsigned long zielees_offset(int regelnummer)
{
   return tekst_ptr[regelnummer];
}
