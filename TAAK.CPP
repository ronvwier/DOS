/* ------------------------------------------------------------------------- */
/* TAAK       : Draaien taken bij aanroep vanuit toetsafhandeling            */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#ifdef __OS2__
   #define INCL_DOSPROCESS
   #include <os2.h>
#endif

#include "stdio.h"
#include "taak.h"

#define MAX_TAAK  8

static int laatste_taak = 0;

static TAAKPROC * taakproc[MAX_TAAK] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

TAAK::start(TAAKPROC * nwe_taak)
{
   register int i;


   if (procedure == NULL)
   {
      /* Zoek een vrije taak op in de tabel */
      for (i = 0; i < MAX_TAAK && taakproc[i] != NULL; i++);

      if (i < MAX_TAAK)
      {
	 /* Plaats de taak in de tabel */
	 taakproc[i] = nwe_taak;
	 if (i > laatste_taak)
	    laatste_taak = i;
      }
      procedure = nwe_taak;
   }
   return 1;
}

TAAK::stop()
{
   register int i;

   if (procedure != NULL)
   {
       /* Zoek de oude taak op in de tabel */
       for (i = 0; i < MAX_TAAK && taakproc[i] != procedure; i++);

       if (i < MAX_TAAK)
       {
	  /* Verwijder de taak uit de tabel */
	  taakproc[i] = NULL;
       }

       procedure = NULL;
    }
    return 1;
}

TAAK::run(void)
{
    static int aktief = 0;

    if (taakproc[aktief] != NULL)
       (*taakproc[aktief])();

    aktief++;
    aktief %= (laatste_taak + 1);

#ifdef __OS2__
    /* Geef het OS wat tijd */
    DosSleep(1);
#endif

    return 1;
}
