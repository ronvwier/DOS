/* ------------------------------------------------------------------------- */
/* BESTAND                                                                   */
/* Taak       : Lezen van bestand                                            */
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

#include "taak.h"
#include "bestand.h"

#define MAXREG    16380   /* maximaal aantal regels */
#define REGLEN      255   /* maximale regellengte */

#define BESTAND_BLOCKSIZE 4096
#define BESTAND_MAXBLOCK  1024
#define BESTAND_MAXBUF    128


static char hex_teken[] = "0123456789ABCDEF";


/* ---------------------------------------------------------------------- */
/* BESTAND: operaties op een bestand                                      */
/* ---------------------------------------------------------------------- */

/* private data */

static int invoer = -1;
static unsigned long positie = 0;
static char *block_ptr = NULL;
static char *blockbuf[BESTAND_MAXBLOCK];
static int last_block = -1;
static char bestand__filenaam[128];
static int  kleinste_block;
static int  kleinste_block_size;
static int bestand_maxbuf = BESTAND_MAXBUF;

static int bestand_lees_blok(int bloknr);

#define NR_STDIN 0

static void read_ahead(void);
static void read_ahead()
{
   register int i = 0;

   /* Zoek een nog niet gelezen blok */
   for ( i = 0; i < bestand_maxbuf && blockbuf[i] != NULL; i++);
   if (i < bestand_maxbuf && blockbuf[i] == NULL)
   {
      /* Lees het blok */
      bestand_lees_blok(i);
   }
}

static TAAK lezen;

static void init(void);
static void init()
{
   static int eerste_keer = 1;
   if (eerste_keer)
   {
      lezen.start(read_ahead);
      eerste_keer = 0;
   }
}
/* ------------------------------------------------------------------------- */
/* BESTAND_LEES_BLOK                                                         */
/*                                                                           */
/* Taak       : lezen van een blok in het bestand                            */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : naam terugggeven                                             */
/* ------------------------------------------------------------------------- */
static int bestand_lees_blok(int bloknr)
{
   static int gelezen = 0;
   static char intern_block[BESTAND_BLOCKSIZE];

   if (bloknr >= BESTAND_MAXBLOCK)
      gelezen = 0;
   else
   {
      if ( blockbuf[bloknr] != NULL )
      {
	 if (bloknr == kleinste_block)
	    gelezen = kleinste_block_size;
	 else
	    gelezen = BESTAND_BLOCKSIZE;
	 block_ptr = blockbuf[bloknr];
      }
      else
      {
	 if ( bloknr != last_block )
	 {
	    if (lseek (invoer,(long)((long)bloknr * BESTAND_BLOCKSIZE),SEEK_SET) < 0)
	    {
	       gelezen = 0;
	       if (bloknr < bestand_maxbuf)
		  bestand_maxbuf = bloknr;
	    }
	    else
	    {
	       gelezen = read(invoer,intern_block,BESTAND_BLOCKSIZE);
	       if (gelezen < 1)
	       {
		  if (bloknr < bestand_maxbuf)
		     bestand_maxbuf = bloknr;
	       }
	       else
	       {
		  if (bloknr < bestand_maxbuf)
		  {
		     /* Forceer C tot allocatie van grote blokken
			om fragmentatie tegen te gaan */
		     blockbuf[bloknr] = (char *)malloc(30000);
		     if (blockbuf[bloknr] != NULL)
			blockbuf[bloknr] = (char *)realloc(blockbuf[bloknr],gelezen);
		     else
			bestand_maxbuf = bloknr;
		  }
		  else
		     blockbuf[bloknr] = NULL;

		  if (gelezen < kleinste_block_size)
                  {
		     kleinste_block_size = gelezen;
		     kleinste_block      = bloknr;
		  }

		  if ( blockbuf[bloknr] != NULL )
		     memcpy(blockbuf[bloknr],intern_block,gelezen);
	       }
	    }
	 }
	 block_ptr = intern_block;
	 if (gelezen > 0)
	    last_block = bloknr;
      }
   }

   return(gelezen);
}

/* public */

/* ------------------------------------------------------------------------- */
/* BESTAND_GET_POSITIE                                                       */
/*                                                                           */
/* Taak       : opvragen huidige bestandpositie                              */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : positie teruggegeven                                         */
/* ------------------------------------------------------------------------- */
unsigned long bestand_get_positie()
{
   return(positie);
}

/* ------------------------------------------------------------------------- */
/* BESTAND_SET_POSITIE                                                       */
/*                                                                           */
/* Taak       : bestand positioneren                                         */
/*                                                                           */
/* Begin      : new_pos gevuld                                               */
/*                                                                           */
/* Einde      : positie ingenomen                                            */
/* ------------------------------------------------------------------------- */
void bestand_set_positie(unsigned long new_pos)
{
   positie = new_pos;
}

/* ------------------------------------------------------------------------- */
/* BESTAND_SLUIT                                                             */
/*                                                                           */
/* Taak       : sluiten bestand                                              */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : bestand dicht                                                */
/* ------------------------------------------------------------------------- */
int bestand_sluit()
{
   register int i;

   if (invoer != NR_STDIN)
      close(invoer);

   for ( i = (bestand_maxbuf -1); i >= 0; i-- )
   {
      if (blockbuf[i] != NULL)
         free(blockbuf[i]);
      blockbuf[i] = NULL;
   }

   return(BESTAND_GELUKT);
}

/* ------------------------------------------------------------------------- */
/* BESTAND_SET_STDIN                                                         */
/*                                                                           */
/* Taak       : open STDIN als bestand (pipe)                                */
/*                                                                           */
/* Begin      : bestand dicht                                                */
/*                                                                           */
/* Einde      : return = BESTAND_GELUKT en STDIN is bestand                  */
/*              return = BESTAND_MISLUKT                                     */
/* ------------------------------------------------------------------------- */
int bestand_set_stdin()
{
   register int retcd = BESTAND_MISLUKT;

   if ( isatty(NR_STDIN) )
      retcd = BESTAND_MISLUKT;
   else
   {
      strcpy(bestand__filenaam,"<");
      if ( invoer >= 0)
         bestand_sluit();
      invoer = NR_STDIN;
      setmode(NR_STDIN,O_BINARY);
      retcd = BESTAND_GELUKT;
   }
   if (retcd == BESTAND_GELUKT)
   {
      register int i;

      last_block = -1;
      bestand_set_positie(0L);
      for ( i = 0; i < BESTAND_MAXBLOCK; i++ ) blockbuf[i] = NULL;
      kleinste_block      = 0;
      kleinste_block_size = BESTAND_BLOCKSIZE;
      bestand_maxbuf      = BESTAND_MAXBUF;
      init();
   }

   return(retcd);
}

/* ------------------------------------------------------------------------- */
/* BESTAND_OPEN                                                              */
/*                                                                           */
/* Taak       : openen bestand                                               */
/*                                                                           */
/* Begin      : naam gevuld                                                  */
/*                                                                           */
/* Einde      : BESTAND_GELUKT : bestand open                                */
/*              BESTAND_MISLUKT:                                             */
/* ------------------------------------------------------------------------- */
int bestand_open(char naam[])
{
   register int i, retcd;
   int intern;

   if ( strcmp(naam,"<") == 0)
      retcd = bestand_set_stdin();
   else
   {
      if (_osmajor >= 3)
         intern = sopen(naam,O_BINARY | O_RDONLY,SH_DENYNO);
      else
         intern = open(naam,O_BINARY | O_RDONLY);

      if (intern < 0)
	 retcd = BESTAND_MISLUKT;
      else
      {
         strcpy(bestand__filenaam,naam);
         if (invoer < 0)
            bestand_sluit();
         invoer = intern;
         retcd = BESTAND_GELUKT;
      }
   }
   if (retcd == BESTAND_GELUKT)
   {
      last_block = -1;
      bestand_set_positie(0L);
      for ( i = 0; i < BESTAND_MAXBLOCK; i++ ) blockbuf[i] = NULL;
      kleinste_block      = 0;
      kleinste_block_size = BESTAND_BLOCKSIZE;
      bestand_maxbuf      = BESTAND_MAXBUF;
      init();
   }

   return retcd;
}

/* ------------------------------------------------------------------------- */
/* BESTAND_NAAM                                                              */
/*                                                                           */
/* Taak       : geef naam van huidig bestand                                 */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : naam terugggeven                                             */
/* ------------------------------------------------------------------------- */
char * bestand_naam()
{
   return(bestand__filenaam);
}

/* ------------------------------------------------------------------------- */
/* BESTAND_LEES_BYTES                                                        */
/*                                                                           */
/* Taak       : lezen opgegeven aantal bytes                                 */
/*                                                                           */
/* Begin      : bestand open en lengte gevuld                                */
/*                                                                           */
/* Einde      : aantal gelezen bytes in returnwaarde                         */
/* ------------------------------------------------------------------------- */
int bestand_lees_bytes(char buffer[],int lengte)
{
   register int start_block, tot_block;
   int i,j,jh, gelezen, block_gelezen;

   start_block =  (int)(bestand_get_positie() / (long)BESTAND_BLOCKSIZE);
   tot_block   =  (int)((bestand_get_positie() + lengte) / (long)BESTAND_BLOCKSIZE);

   gelezen = 0;
   i = (int)(bestand_get_positie() % (long)BESTAND_BLOCKSIZE);

   while ( start_block <= tot_block )
   {
      block_gelezen = bestand_lees_blok(start_block);
      jh = block_gelezen - i;
      if ((lengte-gelezen) < jh)
	 jh = lengte - gelezen;
      if (jh > 0)
	 j = jh;
      else
	 j = 0;
      memcpy(&buffer[gelezen],&block_ptr[i],j);
      i = 0;
      gelezen += j;
      start_block++;
   }
   bestand_set_positie(bestand_get_positie() + gelezen);

   return gelezen;
}

/* ------------------------------------------------------------------------- */
/* BESTAND_LEES_REGEL                                                        */
/*                                                                           */
/* Taak       : lezen regel t/m cr/lf                                        */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : BESTAND_GELUKT : regel gevuld                                */
/*              BESTAND_EINDE  : einde bestand bereikt                       */
/* ------------------------------------------------------------------------- */
int bestand_lees_regel(char regel[])
{
   int retcd;
   char intern[REGLEN + 2];
   register int gelezen, uit;
   unsigned long vooraf;

   vooraf = bestand_get_positie();

   gelezen = bestand_lees_bytes(intern,REGLEN + 2);

   if (gelezen < 1)
      retcd = BESTAND_EINDE;
   else
   {
      uit = 0;

      while ( uit < gelezen && intern[uit] != '\r' && intern[uit] != '\n' )
      {
         if ( intern[uit] < 1 )
            regel[uit] = 250;
         else
            regel[uit] = intern[uit];
         uit++;
      }
      retcd = BESTAND_GELUKT;

      if ( intern[uit] == '\r' )
      {
         if ( gelezen > uit && intern[uit+1] == '\n' )
            bestand_set_positie(vooraf + uit + 2);
         else
            bestand_set_positie(vooraf + uit + 1);
      }
      else
         if ( intern[uit] == '\n' )
         {
            if ( gelezen > uit && intern[uit + 1] == '\r' )
               bestand_set_positie(vooraf + uit + 2);
            else
               bestand_set_positie(vooraf + uit + 1);
         }
         else
            bestand_set_positie(vooraf + uit);

      regel[uit] = '\0';
   }

   return(retcd);
}

/* ------------------------------------------------------------------------- */
/* BESTAND_LEES_FILTER                                                       */
/*                                                                           */
/* Taak       : lezen 4 opeenvolgende leesbare tekens                        */
/*                                                                           */
/* Begin      : bestand open                                                 */
/*                                                                           */
/* Einde      : BESTAND_GELUKT : regel gevuld en bytes_skipped gevuld        */
/*              BESTAND_EINDE  : einde bestand bereikt                       */
/* ------------------------------------------------------------------------- */
int bestand_lees_filter(char regel[], unsigned long * ptr_bytes_skipped)
{
   int retcd;
   char test[2];
   unsigned long in = 0L;
   register int gelezen,
       ok = 0;

   gelezen = bestand_lees_bytes(test,1);

   while ( gelezen > 0  && ok < REGLEN )
   {
      /* Kijk of teken "leesbaar" is */
      in++;
      if (test[0] >= ' ' && test[0] <= 'z')
      {
         /* Tel leesbaar teken */
         regel[ok] = test[0];
         ok++;
      }
      else
      {
         if ( ok < 4 )
            /* Te weinig tekens in string */
            ok = 0;
         else
            /* Genoeg tekens in string */
            ok = REGLEN;
      }

      regel[ok] = '\0';
      if (ok < REGLEN)
         gelezen = bestand_lees_bytes(test,1);
   }

   if (gelezen > 0)
      *ptr_bytes_skipped = in - 1L - strlen(regel);
   else
      *ptr_bytes_skipped = in - strlen(regel);

   if (ok < 4)
      retcd = BESTAND_EINDE;
   else
      retcd = BESTAND_GELUKT;

   return(retcd);
}
