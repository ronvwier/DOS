/* ------------------------------------------------------------------------- */
/* AT         : Versturen modem commando                                     */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream.h>

#ifdef __MSDOS__
   #include "commhw.h"
#else
   #include "commdos.h"
#endif

#include "modem.h"
#include "strings.h"
#include "cmdline.h"

/* ---------------------------------------------------------------------- */
/* MODEMTST/MAIN                                                          */
/*                                                                        */
/* Taak       Testen of en waar een modem en/of muis aanwezig is          */
/*                                                                        */
/* Begin      -                                                           */
/*                                                                        */
/* Einde      Bekende informatie over COM1..4 gegeven en over de muis     */
/*                                                                        */
/*------------------------------------------------------------------------*/

main(const int argc, const char ** argv)
{
   int i;
   int poort = 0;
   char antwoord[2048];
   CmdLine Optie(argc,argv);
#ifdef __MSDOS__
   int irq = 0;
   char interface = ' ';
#endif

   cout << "AT 1.2a    versturen Hayes AT commando's naar een modem" << endl;
   cout << endl;
   cout << "           (c) 1992, R.A. van Wier RI" << endl << endl;

   if (argc < 2)
   {
      cout << "Aanroep  : AT poort {opties} commando {commando}..." << endl;
      cout << endl;
      cout << "poort    : nummer of '?'" << endl;
#ifdef __MSDOS__
      cout << "opties   : /Ix = gebruikt IRQx voor COM poort" << endl;
      cout << "           /CB   Communicatie alleen via Bios/int14 interface" << endl;
      cout << "           /CF   Communicatie via Fossil interface" << endl;
      cout << "           /CH   Communicatie rechtstreeks via Hardware aansturing" << endl;
#endif
      cout << endl;
      exit(1);
   }

   for (i = 2; i < argc; i++)
   {
      if (Optie.IsOption(i))
      {
	 /* Verwerk de optie */
	 switch(Optie.OptionID(i))
	 {
#ifdef __MSDOS__
	    case 'I': irq = Optie.OptionValue(i);
		      if (irq < 0 || irq > 9)
			 irq = 0;
		      break;
	    case 'C':
		      switch(toupper(((const char *)Optie.OptionValue(i))[0]))
		      {
			 case 'B': interface = 'B'; break;
			 case 'F': interface = 'F'; break;
			 case 'H': interface = 'H'; break;
			 default:
			      cout << "Optie " << Optie.Value(i) << " onbekend" << endl;
			      exit(1);
		      }
		      break;
#endif
	    default:  cout << "Onbekende optie " << Optie.Value(i) << endl;
		      exit(1);
	 }
      }
   }

   poort = Optie.Value(1);

   if (poort < 1)
   {
      /* Zoek de poort */
      int gevonden = 0;
      i = 1;
      while (i <= 8 && !gevonden)
      {

#ifdef __MSDOS__
	 CommHW P(i);
	 P.Irq(irq);
#else
	 CommDos P(i);
#endif
	 P.Baud(1200);
	 if (P.IsOk())
	 {
	    Modem M(P);
	    P.Open();
	    if (M.IsOk())
	       gevonden = 1;
	    P.Close();
	 }
	 if (!gevonden)
	    i++;
      }
      if (i <= 8)
	 poort = i;
      else
      {
	 cout << "Geen modem gevonden" << endl;
	 exit(1);
      }
   }

#ifdef __MSDOS__
   CommHW P(poort);
   P.Irq(irq);
#else
   CommDos P(poort);
#endif

   P.Baud(1200);
   if (!P.IsOk())
   {
      cout << "Poort " << poort << " niet bereikbaar" << endl;
      exit(1);
   }
   P.Open();

   Modem M(P);
   if (!M.IsOk())
   {
      cout << "Modem op poort " << poort << " geeft geen antwoord" << endl;
      exit(1);
   }

   for ( i = 2; i < Optie.Words(); i++)
   {
      if (!Optie.IsOption(i))
      {
	 cout << "Commando 'AT" << Optie.Value(i) << "' aan modem op poort " << poort << endl;
	 if (!M.Command(Optie.Value(i),antwoord,30000))
	    cout << "*** FOUT OPGETREDEN BIJ VERWERKING COMMANDO ***" << endl;
	 cout << antwoord << endl;
      }
   }

   P.Close();

   return 0;
}

