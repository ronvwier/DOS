/* ------------------------------------------------------------------------- */
/* DIALOOG                                                                   */
/* Taak       : Voeren dialogen met gebruiker                                */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "toets.h"
#include "scherm.h"
#include "muis.h"
#include "geluid.h"
#include "dialoog.h"
#include "cua.h"

#define DIALOOG_BUTTON_AAN      kleur_body_keuze_aan()
#define DIALOOG_BUTTON_UIT      kleur_body_keuze_uit()
#define DIALOOG_BUTTON_AAN_MNEM kleur_body_keuze_aan_mnem()
#define DIALOOG_BUTTON_UIT_MNEM kleur_body_keuze_uit_mnem()

static int button_aktief = 0;

static int insert_mode   = 0;

static char * keuze_doorgaan[] = {"Doorgaan",NULL};

static void schaduw(int regel,int kol)
{
   register int kleur;

   /* Verlaag de helderheid van de voorgrondkleur
      en maak de achtergrond zwart */
   kleur = scherm_kleur(regel,kol) % 16;
   if (kleur > 8)
      kleur -= 8;
   else
      kleur = 7;  /* WAS 8 */
   scherm_vervang_kleur(regel,kol,kleur);
}

static void bepaal_breedte_hoogte(char tekst[],int * ptr_breedte,int * ptr_hoogte)
{
   register int i,j;
   int breedte, hoogte;

   j = 0;
   breedte = 0;
   for (i = 0; i < (int)strlen(tekst) ; i++)
   {
      j++;
      if (tekst[i] == '\n')
         j = 0;
      if (j > breedte)
         breedte = j;
   }

   /* Bepaal de tonen hoogte */
   i = 0;
   hoogte = 0;
   while ( tekst[i] != '\0' )
   {
      hoogte++;
      while( tekst[i] != '\n' && tekst[i] != '\0' ) i++;
      if (tekst[i]) i++;
   }

   *ptr_breedte = breedte;
   *ptr_hoogte  = hoogte;
}

static void aktie_of_keuze_tekst(int regel,int kol,int lengte,char tekst[],int kleur_norm,int kleur_accent)
{
   register int i;

   scherm_plaats_teken(regel,kol-1,kleur_norm,' ');
   if (strcmp(tekst,"-") == 0)
   {
      /* Schrijf een scheidingsregel */
      scherm_tekst(regel,kol,lengte,kleur_norm,
      "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ");
   }
   else
   {
      scherm_tekst(regel,kol,lengte,kleur_norm,tekst);
      for ( i = 0; tekst[i] != '\0' && i < lengte ; i++ )
      {
	 if (    ( tekst[i] >= 'A' && tekst[i] <= 'Z' )
	      || (i == 0 && tekst[0] >= '1' && tekst[0] <= '9')
	    )
	 {
	    /* Toon hoofdletters en keuzenummers als mnemonics */
	    scherm_plaats_teken(regel,kol+i,kleur_accent,tekst[i]);
	 }
      }
   }
   scherm_plaats_teken(regel,kol+lengte,kleur_norm,' ');
}

static void aktie_tekst(int regel,int kol,int lengte,char tekst[])
{
   aktie_of_keuze_tekst(regel,kol,lengte,tekst,
                        kleur_aktiebalk_keuze(),kleur_aktiebalk_mnem());
}

static void aktie_tekst_cursor(int regel,int kol,int lengte,char tekst[])
{
   aktie_of_keuze_tekst(regel,kol,lengte,tekst,
                        kleur_aktiebalk_cursor(),kleur_aktiebalk_cursor_mnem());
}

static void aktie_tekst_accent(int regel,int kol,int lengte,char tekst[])
{
   aktie_of_keuze_tekst(regel,kol,lengte,tekst,
                        kleur_aktiebalk_accent(),kleur_aktiebalk_mnem());
}

static void keuze_tekst(int regel,int kol,int lengte,char tekst[])
{
   aktie_of_keuze_tekst(regel,kol,lengte,tekst,
                        kleur_body_keuze_uit(),kleur_body_keuze_uit_mnem());
}

static void keuze_tekst_cursor(int regel,int kol,int lengte,char tekst[])
{
   aktie_of_keuze_tekst(regel,kol,lengte,tekst,
                        kleur_aktiebalk_cursor(),kleur_aktiebalk_cursor_mnem());
}

static void vertaal_toets(int * toets_in,char * menu[],int * keuze)
{
   int uit_keuze;
   register int i, toets;

   uit_keuze = *keuze;
   toets = *toets_in;

   if ( (toets >= 'a' && toets <= 'z')
     || (toets >= 'A' && toets <= 'Z') )
   {
      i = 0;
      while ( (menu[i] != NULL)
           && (strchr(menu[i],toupper(toets)) == NULL ) )
         i++;
      if ( menu[i] != NULL )
         uit_keuze = i, toets = CUA_TABULEREN;
   }
   if (toets >= '1' && toets <= '9')
   {
      i = 0;
      while ( menu[i] != NULL && menu[i][0] != (char)toets )
         i++;
      if ( menu[i] != NULL )
         uit_keuze = i, toets = CUA_TABULEREN;
   }

   *keuze = uit_keuze;
   *toets_in = toets;
}

int dialoog_window(char vraag[],char * antwoord[])
{
   int toets;
   int max_keuze;
   int ki;
   int kol_start;
   int regel;
   int breedte_antwoord,breedte_vraag,breedte;
   int pos_vraag,pos_antw;
   int i;
   int keuzenr = -1;
   int hoogte_vraag;

   breedte_antwoord = 0;
   for (i = 0; antwoord[i] != NULL ; i++)
   {
      breedte_antwoord += strlen(antwoord[i]) + 4;
   }
   max_keuze = i - 1;

   regel = (scherm_aantal_reg() / 2) - 3;
   bepaal_breedte_hoogte(vraag,&breedte_vraag,&hoogte_vraag);
   if (breedte_antwoord > breedte_vraag)
      breedte = breedte_antwoord;
   else
      breedte = breedte_vraag;
   kol_start        = (scherm_aantal_kol() - breedte) / 2;
   pos_vraag        = ((breedte - breedte_vraag) / 2) + kol_start;
   pos_antw         = ((breedte - breedte_antwoord) / 2) + kol_start;

   ki = 0;

   dialoog_popup_start(regel,kol_start-2,breedte+4,hoogte_vraag+4);
   regel++;

   i = 0;
   while ( i < (int)strlen(vraag) )
   {
      scherm_cursor_plaats(regel,pos_vraag);
      scherm_cursor_kleur(DIALOOG_WINDOW);

      while( vraag[i] != '\n' && vraag[i] != '\0' )
      {
         scherm_schrijf_teken(vraag[i]);
         i++;
      }
      i++;
      regel++;
   }
   regel++;

   do
   {
      /* Vraag een keuze */
      toets = dialoog_button_vraag(regel,pos_antw,&ki,antwoord);
   }
   while (  (toets != CUA_ENTER)
         && (toets != CUA_TABULEREN)
         && (toets != CUA_AFBREKEN) );

   if (toets != CUA_AFBREKEN)
      keuzenr = ki;
   else
      keuzenr = -1;

   dialoog_popup_einde();

   return keuzenr;
}

int dialoog_vraag(char vraag[],char antwoord[],int antwoord_lengte)
{
   int toets;
   int kol_start = 30;
   int regel     =  7;
   int hoogte    = 8;
   int breedte_antwoord,breedte_vraag,breedte;
   int pos_vraag,pos_antw;
   int i;

   regel = (scherm_aantal_reg() / 2) - 3;
   breedte_antwoord = antwoord_lengte + 2;

   bepaal_breedte_hoogte(vraag,&breedte_vraag,&hoogte);
   hoogte += 2;
   if (breedte_antwoord > breedte_vraag)
      breedte = breedte_antwoord;
   else
      breedte = breedte_vraag;
   kol_start        = (scherm_aantal_kol() - breedte) / 2;
   pos_vraag        = ((breedte - breedte_vraag) / 2) + kol_start;
   pos_antw         = ((breedte - breedte_antwoord) / 2) + kol_start + 1;

   dialoog_popup_start(regel,kol_start-2,breedte+4,hoogte+2);

   regel++;

   i = 0;
   while ( i < (int)strlen(vraag) )
   {
      scherm_cursor_plaats(regel,pos_vraag);
      scherm_cursor_kleur(DIALOOG_WINDOW);

      while( vraag[i] != '\n' && vraag[i] != '\0' )
      {
         scherm_schrijf_teken(vraag[i]);
         i++;
      }
      i++;
      regel++;
   }
   regel++;

   scherm_tekst(regel,pos_antw-1,1,DIALOOG_WINDOW,"[");
   scherm_tekst(regel,pos_antw+breedte_antwoord-2,1,DIALOOG_WINDOW,"]");
   toets = dialoog_lees_veld(regel,pos_antw,antwoord_lengte,DIALOOG_WINDOW_DATA,antwoord);

   dialoog_popup_einde();

   return toets;
}

void dialoog_button_toon(int regel,int kol,int waarde,char * antwoord[])
{
   register int i,j;

   scherm_cursor_plaats(regel,kol);

   for (i = 0; antwoord[i] != NULL ; i++)
   {
      scherm_cursor_kleur(DIALOOG_BUTTON_UIT);
      if (i == waarde)
      {
         if ( button_aktief )
            scherm_cursor_kleur(DIALOOG_BUTTON_AAN);
         scherm_schrijf_teken('>');
      }
      else
         scherm_schrijf_teken(' ');

      scherm_schrijf_teken(' ');
      for (j = 0; antwoord[i][j]; j++)
      {
         if (    (antwoord[i][j] >= 'A' && antwoord[i][j] <= 'Z')
              || (antwoord[i][0] >= '1' && antwoord[i][0] <= '9' && j == 0)
            )
         {
           if (i == waarde && button_aktief)
           {
              scherm_cursor_kleur(DIALOOG_BUTTON_AAN_MNEM);
              scherm_schrijf_teken(antwoord[i][j]);
              scherm_cursor_kleur(DIALOOG_BUTTON_AAN);
           }
           else
           {
              scherm_cursor_kleur(DIALOOG_BUTTON_UIT_MNEM);
              scherm_schrijf_teken(antwoord[i][j]);
              scherm_cursor_kleur(DIALOOG_BUTTON_UIT);
           }
         }
         else
            scherm_schrijf_teken(antwoord[i][j]);
      }
      scherm_schrijf_teken(' ');
      scherm_cursor_kleur(DIALOOG_BUTTON_UIT);
      scherm_schrijf_teken(' ');
   }
}

int  dialoog_button_vraag(int regel,int kol,int * waarde,char * antwoord[])
{
   int i,toets;
   int max_keuze;
   int ki;

   ki = *waarde;

   max_keuze = 0;
   for (i = 0; antwoord[i] != NULL ; i++ ) max_keuze = i;
   button_aktief = 1;

   dialoog_button_toon(regel,kol,ki,antwoord);

   do
   {
      /* Vraag een keuze */
      toets = toets_wacht();
      vertaal_toets(&toets,antwoord,&ki);

      /* Verwerk toetsaanslag */
      if (toets == CUA_NAAR_RECHTS)
         if (ki < max_keuze)   ki++; else ki = 0;
      if (toets == CUA_NAAR_LINKS)
         if (ki > 0)           ki--; else ki = max_keuze;

      dialoog_button_toon(regel,kol,ki,antwoord);
   }
   while (  (toets == CUA_NAAR_LINKS)
         || (toets == CUA_NAAR_RECHTS) );

   button_aktief = 0;
   *waarde = ki;
   dialoog_button_toon(regel,kol,*waarde,antwoord);

   return toets;
}

/* ---------------------------------------------------------------------- */
/* LEES_VELD                                                              */
/* Taak   : Het wegschrijven en inlezen van een schermveld                */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int dialoog_lees_veld(int regel,int kol,int lengte,int kleur,char veld[])
{
   register
   int         i,j;
   unsigned int toets;
   int einde;
   GELUID geluid;

   i = strlen(veld);
   while ( i < lengte )
   {
      veld[i] = ' ';
      i++;
   }
   veld[lengte] = '\0';

   scherm_cursor_plaats(regel,kol);
   scherm_cursor_kleur(kleur);
   scherm_schrijf_tekst(veld);

   j = 0;
   if (insert_mode)
      scherm_cursor(2);
   else
      scherm_cursor(1);
   einde = 0;

   while ( einde == 0)
   {
      scherm_cursor_plaats(regel,kol+j);
      toets = toets_wacht();

      if (( toets >= ' ') && ( toets <= 0xff ))
      {
         if (j < lengte)
         {
            if (insert_mode)
            {
               if (veld[lengte -1] == ' ')
	       {
                  for (i = lengte -1; i > j; i--)
                     veld[i] = veld[i-1];
                  scherm_schrijf_teken((unsigned char)toets);
                  veld[j] = (unsigned char)toets;
                  j++;
                  scherm_cursor_plaats(regel,kol);
                  scherm_schrijf_tekst(veld);
               }
               else
		  geluid.beep();
	    }
	    else
	    {
	       scherm_schrijf_teken((unsigned char)toets);
	       veld[j] = (unsigned char)toets;
	       j++;
	    }
	 }
	 else
	    geluid.beep();
      }
      else
      {
	 switch(toets)
         {
            case CUA_VELD_BEGIN :
                    j = 0;
                 break;
            case CUA_VELD_EINDE :
                    j = lengte - 1;
                    while ( j > 0 && veld[j] == ' ' ) j--;
                    if (veld[j] != ' ' && j < (lengte - 1))
                       j++;
                 break;
            case CUA_WOORD_LINKS :
                    i = j;
                    while ( j > 0 && veld[j] != ' ' ) j--;
                    while ( j > 0 && veld[j] == ' ' ) j--;
                    if ( j > 0 )
		    {
                       while ( j > 0 && veld[j] != ' ' ) j--;
                       if (veld[j] == ' ') j++;
                    }
                    else j = i;
                 break;
            case CUA_WOORD_RECHTS :
                    i = j;
                    while ( j < lengte && veld[j] != ' ' ) j++;
                    while ( j < lengte && veld[j] == ' ' ) j++;
                    if ( j >= lengte )
                       j = i;
                 break;
            case CUA_NAAR_LINKS :
                    if ( j > 0 )
                       j--;
                    else
		       geluid.beep();
		 break ;
	    case CUA_NAAR_RECHTS :
		    if ( j < (lengte -1))
		       j++;
		    else
		       geluid.beep();
		 break;
	    case CUA_WISSEN :
		    if ( j < lengte )
		    {
		       for (i = j ; i < (lengte -1) ; i++)
			  veld[i] = veld[i+1];
		       veld[lengte-1] = ' ';
		       scherm_cursor_plaats(regel,kol);
		       scherm_schrijf_tekst(veld);
		    }
		    else
		       geluid.beep();
		 break;
	    case CUA_WIS_VELD :
		       scherm_cursor_plaats(regel,kol+j);
		       for (i = j ; i < lengte ; i++)
		       {
			  veld[i] = ' ';
			  scherm_schrijf_teken(veld[i]);
		       }
		 break;
	    case CUA_SPATIE_TERUG:
		    if ( j <= lengte && j > 0 )
		    {
		       for (i = j-1 ; i < (lengte -1) ; i++)
			  veld[i] = veld[i+1];
		       veld[lengte-1] = ' ';
		       scherm_cursor_plaats(regel,kol);
		       scherm_schrijf_tekst(veld);
		       j--;
		    }
		    else
		       geluid.beep();
		 break;
	    case CUA_INVOEGEN :
		    /* Flip de insert-mode */
		    if (insert_mode)
		       scherm_cursor(1), insert_mode = 0;
		    else
		       scherm_cursor(2), insert_mode = 1;
		 break;
	    default :
	      einde = 1;
	 }
      }

   }
   scherm_cursor(0);
   veld[lengte] = '\0';

   return toets;
}

/* ---------------------------------------------------------------------- */
/* Taak   : Vraag een keuze uit vertikaal selektie veld                   */
/* ---------------------------------------------------------------------- */

int dialoog_vertikaal_sel(int aktie,int regel,int kol,int breedte,int hoogte,char * keuze[],int * keuzenr)
{
   int         toets;
   int         ki;
   int         aantal,toon_aantal, basis, vbasis;
   int         i,max_keuze,j;


   /* Bepaal breedte en hoogte */
   for (i = 0; keuze[i] != NULL; i++);
   aantal = i;

   if (aantal < hoogte)
      toon_aantal = aantal;
   else
      toon_aantal = hoogte;

   max_keuze = aantal - 1;

   ki = *keuzenr;
   if (ki >= aantal)
      ki = aantal -1;
   if (ki >= toon_aantal)
      basis = ki - toon_aantal + 1;
   else
      basis = 0;

   /* Plaats bovenste kader rand */
   scherm_plaats_teken(regel,kol-2,kleur_body_tekst_normaal(),'Ú');
   for (j = 0; j < breedte+2;j++)
      scherm_plaats_teken(regel,kol+j-1,kleur_body_tekst_normaal(),'Ä');
   scherm_plaats_teken(regel,kol+breedte+1,kleur_body_tekst_normaal(),'¿');

   /* Plaats onderste kader rand */
   scherm_plaats_teken(regel+hoogte+1,kol-2,kleur_body_tekst_normaal(),'À');
   for (j = 0; j < breedte+2;j++)
      scherm_plaats_teken(regel+hoogte+1,kol+j-1,kleur_body_tekst_normaal(),'Ä');
   scherm_plaats_teken(regel+hoogte+1,kol+breedte+1,kleur_body_tekst_normaal(),'Ù');

   vbasis = -1;

   do
   {
      /* Zorg ervoor dat de keuze zichtbaar is */
      if (ki < basis)
         basis = ki;
      if (ki >= (basis + toon_aantal))
         basis = ki - toon_aantal + 1;

      if (basis != vbasis)
      {
         /* Laat alle keuze's zien in pull-down kader */
         for (i = 0; i < toon_aantal; i++)
         {
            scherm_plaats_teken(regel+i+1,kol-2,kleur_body_tekst_normaal(),'³');
            keuze_tekst(regel+i+1,kol,breedte,keuze[i+basis]);
            scherm_plaats_teken(regel+i+1,kol+breedte,kleur_body_tekst_normaal(),' ');
            scherm_plaats_teken(regel+i+1,kol+breedte+1,kleur_body_tekst_normaal(),'³');
         }
         /* Toon pijltje omhoog */
         if (basis > 0)
            scherm_plaats_teken(regel+1,kol+breedte+1,kleur_body_tekst_accent(),24);
         else
            scherm_plaats_teken(regel+1,kol+breedte+1,kleur_body_tekst_normaal(),'³');
         /* Toon pijltje omlaag */
         if ((basis + toon_aantal) < aantal)
            scherm_plaats_teken(regel+toon_aantal,kol+breedte+1,kleur_body_tekst_accent(),25);
         else
            scherm_plaats_teken(regel+toon_aantal,kol+breedte+1,kleur_body_tekst_normaal(),'³');

         vbasis = basis;
      }

      if (aktie == DIALOOG_LEES)
      {
         /* Vraag een keuze */
         keuze_tekst_cursor(regel+ki+1-basis,kol,breedte,keuze[ki]);

         /* Muis aanzetten en flush buffer */
         muis_aan();
         muis_toets();
         toets = 0;

         while( toets == 0)
         {
            /* Vraag om een toets */
            toets = toets_lees();
            if (toets == 0 && muis_aanwezig())
               toets = muis_toets();
         }
         muis_uit();

         keuze_tekst(regel+ki+1-basis,kol,breedte,keuze[ki]);
         vertaal_toets(&toets,keuze,&ki);

         /* Verwerk toetsaanslag */
         if (toets == CUA_MUIS_RECHTS)
            toets = CUA_AFBREKEN;

         if (toets == CUA_MUIS_LINKS)
         {
            int r,k;

            /* Vertaal positie naar keuzenr */
            muis_geef_positie(&r,&k);
            if (  k >= (kol -2) && k <= (kol+ breedte + 1)
                  && r >= regel && r <= (regel + toon_aantal + 1)
               )
            {
               /* Voor dit menu bedoeld */
               if ( toets == CUA_MUIS_LINKS && k == (kol + breedte + 1) )
               {
                  /* Op "scroll-bar" */
                  if (r == (regel + 1))
                     toets = CUA_TERUG_1;
                  if (r == (regel + toon_aantal))
                     toets = CUA_VERDER_1;
               }
               else
               if (  k >= kol && k < (kol+ breedte)
                     && r > regel && r <= (regel + toon_aantal)
                  )
               {
                  /* Selekteer de keuze */
                  ki = basis + (r - regel) -1;
                  if (toets == CUA_MUIS_LINKS)
                     toets = CUA_ENTER;
               }
               else
                  toets = 0;
            }
         }
         if (toets == CUA_NAAR_BENEDEN)
         {
           if (ki < max_keuze)
              ki++;
           else
              ki = 0;
         }
         if (toets == CUA_NAAR_BOVEN)
         {
            if (ki > 0)
               ki--;
            else
               ki = max_keuze;
         }
         if (toets == CUA_VELD_EINDE)
            ki = max_keuze;
         if (toets == CUA_VELD_BEGIN)
            ki = 0;
         if (   toets == CUA_TERUG_1
             || toets == CUA_TERUG_2)
         {
            ki -= toon_aantal;
            if (ki < 0)
               ki = 0;
            basis -= toon_aantal;
            if (basis < 0)
               basis = 0;
         }
         if (   toets == CUA_VERDER_1
             || toets == CUA_VERDER_2)
         {
            ki += toon_aantal;
            if (ki > max_keuze)
               ki = max_keuze;
            basis += toon_aantal;
            if (basis > (max_keuze - toon_aantal + 1))
               basis = max_keuze - toon_aantal + 1;
         }
      }
      else toets = -1;
   }
   while (  (toets == CUA_NAAR_BENEDEN)
         || (toets == 0)
         || (toets == CUA_VERDER_1)
         || (toets == CUA_VERDER_2)
         || (toets == CUA_TERUG_1)
         || (toets == CUA_TERUG_2)
         || (toets == CUA_VELD_EINDE)
         || (toets == CUA_VELD_BEGIN)
         || (toets == CUA_NAAR_BOVEN) );

   *keuzenr = ki;

   return toets;
}

/* ---------------------------------------------------------------------- */
/* PULL_MENU                                                              */
/* Taak   : Vraag een keuze uit een pull-down menu                        */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int dialoog_vertikaal_menu(int regel,int kol,int max_reg,char * keuze[],int * keuzenr)
{
   int         toets;
   int         breedte,aantal,toon_aantal;
   register    int i;

   scherm_bewaar();
   kleur_window_start();

   /* Bepaal breedte en hoogte */
   breedte = 0;
   for (i = 0; keuze[i] != NULL; i++)
       if ((int)strlen(keuze[i]) > breedte) breedte = (int)strlen(keuze[i]);
   aantal = i;

   if (aantal < max_reg)
      toon_aantal = aantal;
   else
      toon_aantal = max_reg;

   /* Korrigeer de kollom indien te ver naar links of rechts */
   if ((kol + breedte + 1) >= scherm_aantal_kol() )
      kol = scherm_aantal_kol() - breedte - 2;
   if (kol < 2 )
      kol = 2;

   toets = dialoog_vertikaal_sel(DIALOOG_LEES,regel,kol,breedte,toon_aantal,keuze,keuzenr);

   kleur_window_einde();
   scherm_herstel();

   return toets;
}

/* ---------------------------------------------------------------------- */
/* BALK_MENU                                                              */
/* Taak   : Vraag een keuze uit een balk menu                             */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int dialoog_hor_menu_pos(int kol,char * keuze[],int keuzenr)
{
   register int i,pos_keuze;

   pos_keuze = kol + 2;
   i = 1;
   while ( i <= keuzenr )
   {
      pos_keuze += strlen(keuze[i-1]) + 2;
      i++;
   }

   return pos_keuze;
}

int dialoog_hor_menu(int regel,int kol_start,int kol_eind,char * keuze[],int * keuzenr)
{
   int         toets;
   int         ki;
   int         aantal;
   int         i,max_keuze;
   int         pos[40];

   /* Vraag de posities van de keuze op */
   for (i = 0; i < 40; i++) pos[i] = 999;
   for (aantal = 0; keuze[aantal] != NULL; aantal++)
      pos[aantal] = dialoog_hor_menu_pos(kol_start,keuze,aantal);
   max_keuze = aantal - 1;

   ki = *keuzenr;

   /* Laat alle keuze's zien in een balk */
   scherm_tekst(regel,kol_start,(kol_eind - kol_start + 1),SCHERM_SAA_KEUZE_UIT,"");
   for (i = 0; keuze[i] != NULL; i++)
      aktie_tekst(regel,pos[i],strlen(keuze[i]),keuze[i]);

   do
   {
      /* Vraag een keuze */
      aktie_tekst_cursor(regel,pos[ki],strlen(keuze[ki]),keuze[ki]);

      /* Muis aanzetten en flush buffer */
      muis_aan();
      muis_toets();
      toets = 0;

      while( toets == 0)
      {
         /* Vraag om een toets */
         toets = toets_lees();
         if (toets == 0 && muis_aanwezig())
            toets = muis_toets();
      }
      muis_uit();

      aktie_tekst(regel,pos[ki],strlen(keuze[ki]),keuze[ki]);
      vertaal_toets(&toets,keuze,&ki);

      /* Verwerk toetsaanslag */
      if (toets == CUA_MUIS_RECHTS)
         toets = CUA_AFBREKEN;
      if (toets == CUA_MUIS_LINKS)
      {
         int r,k;

         /* Vertaal positie naar keuzenr */
         muis_geef_positie(&r,&k);
         if (  k >= kol_start
            && k <= kol_eind
            && r == regel
            )
         {
            toets = 0;
            for (i = 0; i <= max_keuze; i++)
            {
               if (k >= pos[i] && k < (pos[i] + strlen(keuze[i]) ) )
               {
                  ki = i;
                  toets = CUA_ENTER;
               }
            }
         }
      }
      if (toets == CUA_NAAR_RECHTS)
      {
         if (ki < max_keuze)
            ki++;
         else
            ki = 0;
      }
      if (toets == CUA_NAAR_LINKS)
      {
         if (ki > 0)
            ki--;
         else
            ki = max_keuze;
      }
   }
   while (  (toets == CUA_NAAR_LINKS)
         || (toets == CUA_NAAR_RECHTS) );

   *keuzenr = ki;

   return toets;
}

int dialoog_kies_menu(int regel,int kol_start,int kol_eind,DIALOOG_MENU menu,int * grp_keuze,int * sub_keuze)
{
   int i,j,x,pos;
   int toets;
   int max_hor;

   static char * balk[DIALOOG_MENU_GRP];
   static char * keuze[DIALOOG_MENU_SUB];

   for (x = 0; x < DIALOOG_MENU_GRP; x++)
      balk[x] = menu[x][0];
   for (i = 0; balk[i] != NULL ; i++) max_hor = i;

   scherm_bewaar();

   if (*sub_keuze > 0)
   {
      i = *grp_keuze;
      j = - 1;
      toets = CUA_ENTER;
   }
   else
   {
      i = 0;
      j = -1;
      toets = dialoog_hor_menu(regel,kol_start,kol_eind,balk,&i);
   }

   while (   toets == CUA_ENTER
          || toets == CUA_NAAR_BOVEN
          || toets == CUA_NAAR_BENEDEN
          || toets == CUA_TABULEREN)
   {
      if ( menu[i][1] != NULL )
      {
         do
         {
            /* Markeer de aktieve keuze */
            pos = dialoog_hor_menu_pos(kol_start,balk,i);
            aktie_tekst_accent(regel,pos,strlen(balk[i]),balk[i]);

            /* Roep een submenu op */
            j = 0;
            for (x = 0; x < DIALOOG_MENU_SUB; x++)
               keuze[x] = menu[i][x+1];
            toets = dialoog_vertikaal_menu(regel+1,pos,scherm_aantal_reg() -5 -regel,keuze,&j);

            /* De-Markeer de aktieve keuze */
            aktie_tekst(regel,pos,strlen(balk[i]),balk[i]);

            if (toets == CUA_NAAR_LINKS)
            {
               if (i > 0)
                  i--;
               else
                  i = max_hor;
            }
            if (toets == CUA_NAAR_RECHTS)
            {
               if (i < max_hor)
                  i++;
               else
                  i = 0;
            }
            if (toets == CUA_MUIS_LINKS)
            {
               int r,k;

               /* Vertaal positie naar keuzenr */
               muis_geef_positie(&r,&k);
               if (  k >= kol_start
                  && k <= kol_eind
                  && r == regel
                  )
               {
                  int x,y;

                  /* Muis op horizontale balk */
                  toets = 0;
                  for (y = 0; balk[y] != NULL; y++)
                  {
                     x = dialoog_hor_menu_pos(kol_start,balk,y);
                     if (k >= x && k < (x + strlen(balk[y]) ) )
                     {
                        i = y;
                        toets = CUA_NAAR_LINKS;
                     }
                  }
               }
            }
         }
         while ((menu[i][1] != NULL) && (toets == CUA_NAAR_LINKS || toets == CUA_NAAR_RECHTS));

         if (toets == CUA_ENTER
             || toets == CUA_TABULEREN)
            toets = -1;

         if ( toets == CUA_AFBREKEN || menu[i][1] == NULL)
         {
            j = -1;
            toets = dialoog_hor_menu(regel,kol_start,kol_eind,balk,&i);
         }
      }
      else
      {
         toets = -1;
         j = -1;
      }
   }
   if (toets == -1)
      toets = CUA_ENTER;

   scherm_herstel();

   *grp_keuze = i;
   *sub_keuze = j+1;

   return toets;
}

void dialoog_toon_menu(int regel,int kol_start,int kol_eind,DIALOOG_MENU menu)
{
   register int i;
   char * balk[DIALOOG_MENU_GRP];

   /* Converteer menu naar balk menu */
   for (i = 0; i < DIALOOG_MENU_GRP; i++)
      balk[i] = menu[i][0];

   /* Laat alle keuze's zien in een balk */
   scherm_tekst(regel,kol_start,(kol_eind - kol_start + 1),SCHERM_SAA_KEUZE_UIT,"");
   for (i = 0; balk[i] != NULL; i++)
      aktie_tekst(regel,dialoog_hor_menu_pos(kol_start,balk,i),strlen(balk[i]),balk[i]);
}

int dialoog_help(char tekst[])
{
   int i;
   static char * keuze_doorgaan[] = {"Enter",NULL};

   kleur_window_help_start();
   i = dialoog_window(tekst,keuze_doorgaan);
   kleur_window_help_einde();

   return i;
}
int  dialoog_popup_start(int regel, int kol,int breedte, int hoogte)
{
   register int i,kleur;

   scherm_bewaar();
   kleur_window_start();
   scherm_cursor(0);
   kleur = kleur_body_tekst_normaal();
   scherm_cursor_kleur(kleur);

   /* Teken een kaderrand */
   for (i = 0; i < hoogte; i++)
   {
      scherm_tekst(regel + i,kol,breedte,kleur," ");
      scherm_vervang_teken(regel+i,kol,'º');
      scherm_vervang_teken(regel+i,kol+breedte-1,'º');
   }

   for (i = 0; i < breedte; i++)
   {
      scherm_vervang_teken(regel,kol+i,'Í');
      scherm_vervang_teken(regel+hoogte-1,kol+i,'Í');
   }
   scherm_vervang_teken(regel,kol,'É');
   scherm_vervang_teken(regel,kol+breedte-1,'»');
   scherm_vervang_teken(regel+hoogte-1,kol,'È');
   scherm_vervang_teken(regel+hoogte-1,kol+breedte-1,'¼');

   /* Toon een schaduw */
   if (scherm_aantal_kol() > 40)
   {
      for (i =  1; i < hoogte; i++)
      {
         /* Schaduw 2 posities breed om vierkant te lijken */
         schaduw(regel+i,kol+breedte);
         schaduw(regel+i,kol+breedte+1);
      }
      for (i = 2; i <= (breedte + 1); i++)
         schaduw(regel+hoogte,kol+i);
   }
   else
   {
      for (i =  1; i < hoogte; i++)
         schaduw(regel+i,kol+breedte);
      for (i = 1; i <= breedte; i++)
         schaduw(regel+hoogte,kol+i);
   }

   return 0;
}

int  dialoog_popup_einde(void)
{
   kleur_window_einde();
   scherm_herstel();

   return 0;
}

/* ---------------------------------------------------------------------- */
/* POPUP_MENU                                                             */
/* Taak   : Vraag een keuze uit een gecentreerd popup menu                */
/*                                                                        */
/* ---------------------------------------------------------------------- */

int dialoog_popup_menu(char * keuze[],int * keuzenr)
{
   int         toets;
   int         breedte,aantal;
   register    int i;
   int regel, kol;

   /* Bepaal breedte en hoogte */
   breedte = 0;
   for (i = 0; keuze[i] != NULL; i++)
       if ((int)strlen(keuze[i]) > breedte) breedte = (int)strlen(keuze[i]);
   aantal = i;

   /* Korrigeer de kollom indien te ver naar links of rechts */
   kol   = (scherm_aantal_kol() - breedte) /2;
   regel = (scherm_aantal_reg() - aantal -2)  /2;

   dialoog_popup_start(regel,kol-2,breedte+4,aantal+2);
   *keuzenr = 0;
   toets = dialoog_vertikaal_sel(DIALOOG_LEES,regel,kol,breedte,aantal,keuze,keuzenr);
   dialoog_popup_einde();

   return toets;
}

