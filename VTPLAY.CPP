/* ------------------------------------------------------------------------- */
/* VTPLAY     : Afspelen videotex sessie vanuit logfile                      */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include "scherm.h"
#include "toets.h"
#include "geluid.h"
#include "cua.h"
#include "vt.h"

typedef unsigned char BYTE;

#define MAX_PAGE    512

static long page_pos[MAX_PAGE];
static int  cur_page = 0;
static int  first_page = 0;

static int aantal_kol = 40;

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(int argc, char * argv[])

{
   unsigned char teken;
   struct VIDEOTEX_MUT mut;
   FILE *in;
   int dis = 0;
   register int i, grootte;
   int toets;
   GELUID geluid;

   memset(page_pos,0,sizeof(page_pos));

   if (argc < 2)
   {
      printf("VTPLAY : VideoTex logfile replay\n");
      printf("========\n\n");
      printf("     Aanroep: VTPLAY logfile {opties}\n");
      printf("\n");
      printf("     Bijv.  : VTPLAY VT.LOG\n");
      printf("                                 \n");
      printf("     opties : /S0  scherm met 80 pos., waarvan 40 in gebruik\n");
      printf("              /S1  scherm met 40 pos., met brede letters en figuren\n");
      printf("              /S2  scherm met 80 pos., met alleen brede figuren\n");
      exit(1);
   }


   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '/')
      {
         switch(argv[i][1])
         {
	    case 's':
            case 'S': dis = argv[i][2] - '0';
                      if (dis < 0 || dis > 2)
                         dis = 0;
                      break;
            default:
                      printf("Onbekende optie %s\n",argv[i]);
                      exit(1);
         }
      }
   }

   in = fopen(argv[1],"rb");
   if (in == NULL)
   {
      printf("Onbekend bestand %s\n",argv[1]);
      exit(1);
   }

   scherm_init();
   vtdis_init_replay(dis);

   /* Lees t/m eerste clear-screen */
   teken = (unsigned char)fgetc(in);
   while (!videotex_is_cls(teken) && !feof(in))
      teken = (unsigned char)fgetc(in);
   videotex_cls();
   page_pos[cur_page] = ftell(in);
   grootte = 0;

   while (toets != CUA_EINDE)
   {
      if (!feof(in))
         teken = (unsigned char)fgetc(in);
      if ((videotex_is_cls(teken) && grootte > 160) || feof(in))
      {
         toets = toets_wacht();
         switch(toets)
         {
            case CUA_TERUG_1:
	    case CUA_TERUG_2:
		 if (cur_page > first_page)
		    cur_page--;
		 else
		    geluid.beep();
		 break;
	    case CUA_VERDER_1:
	    case CUA_VERDER_2:
		 if (!feof(in))
		 {
		    cur_page++;
		    page_pos[cur_page] = ftell(in);
		 }
		 else
		    geluid.beep();
                 break;
            case CUA_EINDE:
                 break;
            default:
		 geluid.beep();
		 break;
	 }
	 rewind(in);
	 fseek(in,page_pos[cur_page],SEEK_SET);
         videotex_cls();
         grootte = 0;
      }

      if (!feof(in))
      {
         videotex_teken(teken,&mut);
         if (mut.aantal_kol != aantal_kol)
         {
            first_page = cur_page;
            aantal_kol = mut.aantal_kol;
         }
         vtdis_toon(&mut);
         grootte++;
      }
   }

   vtdis_einde();
   scherm_einde();

   return 0;
}
