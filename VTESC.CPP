/* ------------------------------------------------------------------------- */
/* VTESC      : Afhandelen speciale escape code scherm                       */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stddef.h>
#include <ctype.h>

#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "geluid.h"

#include "videotex.h"

#include "vtesc.h"

static GELUID geluid;

static char * esc_menu[] = { "1. afbreken scherm     (*0#)",
                             "2. begin invulscherm  (home)",
                             "3. einde invulscherm   (end)",
                             "4. opmaak codes",
                             "5. tekst kleur",
                             "6. grafische kleur",
                             "7. grafische codes",
                             "8. achtergrond kleur",
                             "9. tekst corrigeren",
                             NULL };

static char * opm_menu[] = { "1. Knipperend      (esc h)",
                             "2. Verborgen       (esc x)",
                             "3. Normaal         (esc i)",
                             "4. Gewone  hoogte  (esc l)",
                             "5. Dubbele hoogte  (esc m)",
                             NULL };

static char * klt_menu[] = { "1. Rood     (esc a)",
                             "2. Groen    (esc b)",
                             "3. gEel     (esc c)",
                             "4. Blauw    (esc d)",
                             "5. Magenta  (esc e)",
                             "6. Cyaan    (esc f)",
                             "7. Wit      (esc g)",
                             NULL };

static char * klg_menu[] = { "1. Rood     (esc q)",
                             "2. Groen    (esc r)",
                             "3. gEel     (esc s)",
                             "4. Blauw    (esc t)",
                             "5. Magenta  (esc u)",
                             "6. Cyaan    (esc v)",
                             "7. Wit      (esc w)",
                             NULL };

static char * opg_menu[] = { "1. Aaneengesloten  (esc y)",
                             "2. Gescheiden      (esc z)",
                             "3. Houdend         (esc ^)",
                             "4. Release         (esc #)",
                             NULL };

static char * ach_menu[] = { "1. Voorgrondkleur overnemen (esc ])",
                             "2. Zwart achtergrond        (esc \\)",
                             NULL };

static char * cor_menu[] = { "1. Spatie invoegen        (ins)",
                             "2. Teken verwijderen      (del)",
                             "3. Invoegen regel     (alt+ins)",
                             "4. Verwijderen regel  (alt+del)",
                             "5. Leegmaken scherm            ",
                             NULL };

/* ------------------------------------------------------------------------- */
/* VTESC_CODE                                                                */
/*                                                                           */
/* Taak       : Afhandelen speciale esc code ingave                          */
/* ------------------------------------------------------------------------- */

char * vtesc_code()
{
   int keuze, toets;
   char * terug = "";
   static char hulp[8];

   keuze = 0;
   toets = dialoog_popup_menu(esc_menu,&keuze);
   toets = toupper(toets);

   if ((toets >= '@' && toets <= '^') || toets == '#')
   {
      hulp[0] = 0x1B;
      hulp[1] = videotex_vertaal_teken(toets);
      hulp[2] = '\0';
      terug = hulp;
   }
   else
   {
      if (toets == CUA_TABULEREN || toets == CUA_ENTER)
      {
         switch(keuze)
         {
            case 0: terug = videotex_cmd_annuleer();    break;
            case 1: terug = videotex_cmd_top_scherm();  break;
            case 2: terug = videotex_cmd_eind_scherm(); break;
            case 3: /* opmaak codes */
                    keuze = 0;
                    toets = dialoog_popup_menu(opm_menu,&keuze);
                    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
                    {
                       switch(keuze)
                       {
                          case 0: terug = "H";  break;
                          case 1: terug = "X";  break;
                          case 2: terug = "I";  break;
                          case 3: terug = "L";  break;
                          case 4: terug = "M";  break;
			  default: geluid.beep();
		       }
		    }
		    else
		      if (toets != CUA_AFBREKEN)
			 geluid.beep();
		    break;
	    case 4: /* tekst kleur */
		    keuze = 0;
		    toets = dialoog_popup_menu(klt_menu,&keuze);
		    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
		    {
		       switch(keuze)
		       {
			  case 0: terug = "A";  break;
			  case 1: terug = "B";  break;
			  case 2: terug = "C";  break;
			  case 3: terug = "D";  break;
			  case 4: terug = "E";  break;
			  case 5: terug = "F";  break;
			  case 6: terug = "G";  break;
			  default: geluid.beep();
		       }
		    }
		    else
		      if (toets != CUA_AFBREKEN)
			 geluid.beep();
		    break;
	    case 5: /* grafische kleur */
		    keuze = 0;
		    toets = dialoog_popup_menu(klg_menu,&keuze);
		    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
		    {
		       switch(keuze)
		       {
			  case 0: terug = "Q";  break;
                          case 1: terug = "R";  break;
                          case 2: terug = "S";  break;
                          case 3: terug = "T";  break;
                          case 4: terug = "U";  break;
                          case 5: terug = "V";  break;
                          case 6: terug = "W";  break;
			  default: geluid.beep();
		       }
		    }
		    else
		      if (toets != CUA_AFBREKEN)
			 geluid.beep();
		    break;
	    case 6: /* grafische codes */
                    keuze = 0;
                    toets = dialoog_popup_menu(opg_menu,&keuze);
                    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
                    {
                       switch(keuze)
                       {
                          case 0: terug = "Y";  break;
                          case 1: terug = "Z";  break;
                          case 2: terug = "^";  break;
                          case 3: terug = "_";  break;
                          default: geluid.beep();
                       }
                    }
                    break;
            case 7: /* Achtergrond kleur */
		    keuze = 0;
		    toets = dialoog_popup_menu(ach_menu,&keuze);
		    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
                    {
                       switch(keuze)
                       {
                          case 0: terug = "]" ;  break;
                          case 1: terug = "\\";  break;
                          default: geluid.beep();
                       }
                    }
                    else
                      if (toets != CUA_AFBREKEN)
                         geluid.beep();
                    break;
            case 8: /* Tekst corrigeren */
                    keuze = 0;
                    toets = dialoog_popup_menu(cor_menu,&keuze);
                    if (toets == CUA_TABULEREN || toets == CUA_ENTER)
                    {
                       switch(keuze)
                       {
                          case 0: terug = videotex_cmd_ins_char();  break;
                          case 1: terug = videotex_cmd_del_char();  break;
                          case 2: terug = videotex_cmd_ins_line();  break;
                          case 3: terug = videotex_cmd_del_line();  break;
                          case 4: terug = videotex_cmd_cls();       break;
                          default: geluid.beep();
                       }
                    }
                    break;
            default: geluid.beep();
         }
      }
      else
         if (toets != CUA_AFBREKEN)
	    geluid.beep();
   }

   return terug;
}
