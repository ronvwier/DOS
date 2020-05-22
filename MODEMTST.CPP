/* ------------------------------------------------------------------------- */
/* MODEMTST   : Testen waar een modem/muis aanwezig is                       */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <ctype.h>
#include <iostream.h>

#ifdef __MSDOS__
   #include "commhw.h"
#else
   #include "commdos.h"
#endif

#include "modem.h"
#include "muis.h"

#define MIN_IRQ 2
#define MAX_IRQ 7

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

main(int argc, char * argv[])
{
   int i,type,j;
   int standaard = 0, extended = 0;
   char interface = ' ';
   int poort = 0;

   if (argc > 1 && toupper(argv[1][0]) == 'S')
      standaard = 1;
   if (argc > 1 && toupper(argv[1][0]) == 'X')
      extended = 1;

   cout << "MODEMTST 2.2b : (c) 1991-1995, R.A. van Wier" << endl;
   cout << endl << endl;
#ifdef __MSDOS__
   cout << "Aanroep  : MODEMTST {opties}   voor volledige test met diverse IRQs" << endl;
   cout << "           MODEMTST S {opties} voor standaard instellingen test" << endl;
   cout << endl;
   cout << "opties   : /CB   Communicatie alleen via Bios/int14 interface" << endl;
   cout << "           /CF   Communicatie via Fossil interface" << endl;
   cout << "           /CH   Communicatie rechtstreeks via Hardware aansturing" << endl;
#else
   cout << "Aanroep  : MODEMTST voor volledige test" << endl;
#endif
   cout << endl;
   cout << "Returncode    : 0 of laatst gevonden modem poortnummer" << endl;

   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '/')
      {
         /* Verwerk de optie */
         switch(argv[i][1])
         {
#ifdef __MSDOS__
            case 'c':
                    case 'C':
                      switch(argv[i][2])
                      {
                         case 'b':
                         case 'B':
                              interface = 'B';
                              break;
                         case 'f':
                         case 'F':
                              interface = 'F';
                              break;
                         case 'h':
                         case 'H':
                              interface = 'H';
                              break;
                         default:
                              cout << "Optie " << argv[i] << "onbekend" << endl;
                              exit(1);
                      }
                      break;
#endif
            default:
                      cout << "Onbekende optie" << argv[i] << endl;
                      exit(1);
         }
      }
   }

   for (i = 1; i <= 9; i++)
   {
      /* Kijk of poort nummer i aanwezig is */
#ifdef __MSDOS__
      CommHW P(i);
#else
      CommDos P(i);
#endif
      P.Baud(1200);

      cout << "COM" << i << ":";

      if (P.IsOk())
      {
         /* Laat informatie voor poort nummer i zien */
         Modem M(P);

#ifdef __MSDOS__
         type = P.Type_UART();

         switch(type)
         {
            case COMMHW_UART_8250    : cout << " UART 8250 "; break;
            case COMMHW_UART_16450   : cout << " UART 16450 "; break;
            case COMMHW_UART_16550   : cout << " UART 16550 (GEEN FIFO) "; break;
            case COMMHW_UART_16550A  : cout << " UART 16550A (FIFO) "; break;
            case COMMHW_FOSSIL       : cout << " FOSSIL interface "; break;
            default                  : cout << " UART type onbekend "; break;
         }

         if (standaard || type == COMMHW_UART_ONBEKEND)
         {
            P.Open();
            if (M.IsOk())
               cout << " HAYES MODEM ", poort = i;
            else
               cout << " geen HAYES MODEM ";
            P.Close();
         }
         else
         {
            /* Loop alle interrupts af om een gekoppeld modem te vinden */
            int found = 0;
            j = MIN_IRQ;
            cout << " IRQ " << j;
            if (extended)
               cout << endl;
            while ( j <= MAX_IRQ && !found)
            {
               P.Irq(j);
               P.Open();
               if (M.IsOk())
                  found  = 1;
               P.Close();

               /* Volgende interrupt */
               if (!found)
                  j++;
               if (extended)
                  cout << endl << " IRQ  ";
               cout << "\b" << j;
               if (extended)
                  cout << endl;
            }

            if (j <= MAX_IRQ)
            {
               /* Gevonden interrupt komt overeen met een modem poort */
               if (extended)
                  cout << endl << " IRQ  ";
               cout << "\b" << j << " HAYES MODEM ";
               poort = i;
            }
            else
            {
               if (extended)
                  cout << endl << " IRQ  ";
               cout << "\b? geen HAYES MODEM ";
            }
          }
#else
          /* Test onder OS/2 */
          cout << " UART type onbekend ";
          P.Open();
          if (M.IsOk())
             cout << " HAYES MODEM ", poort = i;
          else
             cout << " geen HAYES MODEM ";
          P.Close();
#endif
      }
      else
         cout << " afwezig";
      cout << endl;
   }

   /* Vraag de interrupt van de muis op en toon deze */
   muis_init();
   if (muis_aanwezig())
      cout << endl << "MUIS: IRQ " << muis_irq() << endl;
   muis_einde();

   return poort;
}
