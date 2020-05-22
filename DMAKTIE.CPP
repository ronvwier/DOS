/* ------------------------------------------------------------------------- */
/* DMAKTIE    : Vraag de gebruiker om een aktie                              */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#ifdef MSDOS
   #include <dos.h>
   #include <bios.h>
#endif
#include <string.h>

#include "toets.h"
#include "scherm.h"
#include "dialoog.h"
#include "tekst.h"
#include "taak.h"
#include "tijd.h"
#include "cua.h"
#include "geluid.h"

#include "dm.h"
#include "dmparm.h"
#include "dmgroep.h"
#include "dmaktie.h"
#include "dmkeuze.h"
#include "dmmenu.h"
#include "dmfind.h"

static char * titel = TITEL;
static char * recht = RECHT;

char * dmaktie_exec_parm(void) { return NULL;}

char * dmaktie_exec_prog(void) { return NULL;}

char * dmaktie_data_path(void) { return NULL;}

int    dmaktie_exec_wacht(void) { return 0;}

static void menu_naar_keuze(int grp,int i)
{
   int x;

   dmkeuze_zet_titel(dmmenu_keuze(grp,i));
   dmkeuze_zet_dir(dmmenu_dir(grp,i));
   dmkeuze_zet_prog(dmmenu_prog(grp,i));
   dmkeuze_zet_wacht(dmmenu_wacht(grp,i));
   for (x = 0; x < 4; x++ )
      dmkeuze_zet_optie_tekst(x,dmmenu_vraag(grp,i,x));
}
static void keuze_naar_menu(int grp,int i)
{
   int x;

   dmmenu_zet_keuze(grp,i,dmkeuze_titel());
   dmmenu_zet_dir(grp,i,dmkeuze_dir());
   dmmenu_zet_prog(grp,i,dmkeuze_prog());
   dmmenu_zet_wacht(grp,i,dmkeuze_wacht());
   for (x = 0; x < 4; x++ )
      dmmenu_zet_vraag(grp,i,x,dmkeuze_optie_tekst(x));
}

int dmaktie(LOGO logo)
{
   char        regel[256];
   char *      ptr_hulp;
   register
      int      i,j;
   DIALOOG_MENU menu;
   int         grp,sub;
   int         toets;
   int         update = 0;
   int         update_kan = 0;
   static char * keuze_aanmaak[]   = {"Aanmaken","Annuleren",NULL};
   static char * keuze_verwijder[] = {"Verwijderen","Annuleren",NULL};
   static char * keuze_del_groep[] = {"Annuleren",NULL};
   static char * keuze_doorgaan[]  = {"Annuleren","Doorgaan",NULL};
   static char * keuze_ja_nee[]    = {"Ja","Nee",NULL};
   static char * keuze_a_c_v[]     = {"Achteraan","Cursor","Vooraan",NULL};
#ifdef MSDOS
   union REGS  regs;
   struct SREGS sregs;
#endif
   char        hulp_parm[132];
   char        hulp_vraag[512];
   char        antwoord[50];
   static char tussen_tekst[200][50];
   int         tussen_tel = 0;

   scherm_cls(KLEUR_LOGO);
   scherm_tekst(0,0,2,KLEUR_TITEL," ");
   scherm_tekst(0,2,scherm_aantal_kol() - 2,KLEUR_TITEL,TITEL);
   scherm_tekst(0,11,scherm_aantal_kol() - 11,KLEUR_TITEL,VERSIE);
   scherm_tekst(0,scherm_aantal_kol() - 11 - strlen(recht),strlen(recht),KLEUR_RECHT,recht);

   tijd_permanent_tonen(0,scherm_aantal_kol() -10);
   {
      /* Toon de naam van het menu */
      char drive[4],dir[128],file[12],ext[12];
      _splitpath(dmparm_get_menu_path(),drive,dir,file,ext);
      scherm_tekst(0,16,8,KLEUR_MENU,file);
   }
   if (scherm_aantal_kol() >= 80)
   {
      /* Toon de naam van de gebruiker */
      char * hulp;
      hulp = getenv("LOGIN_NAME");
      if (hulp != NULL)
         scherm_tekst(0,30,22,KLEUR_MENU,hulp);
   }
   scherm_tekst(1,0 ,scherm_aantal_kol(),kleur_pf()," ");
   if (scherm_aantal_kol() >= 80)
   {
      scherm_tekst(scherm_aantal_reg() - 2,0 ,scherm_aantal_kol(),kleur_pf(),"  F1=Help  F3=Einde  Shift-F10=Info");
      scherm_tekst(scherm_aantal_reg() - 1,0 ,scherm_aantal_kol(),7," ");
   }
   else
      scherm_tekst(scherm_aantal_reg() - 1,0 ,scherm_aantal_kol(),kleur_pf(),"  F1=Help  F3=Einde  Shift-F10=Info");

   scherm_cursor_kleur(KLEUR_LOGO);
   for (i = 0; i < LOGO_HOOGTE; i++ )
   {
      if (scherm_aantal_reg() > (3 + i))
      {
         scherm_cursor_plaats(3+i,0);
         scherm_schrijf_tekst(logo[i]);
      }
   }

   i = dmmenu_open(dmparm_get_menu_path());
   if ( i == DMMENU_MISLUKT)
   {
      j = dialoog_window("Menu bestand niet gevonden",keuze_aanmaak);
      if ( j != 0 ) return DMAKTIE_NIETS;

      dmmenu_maak(dmparm_get_menu_path());
      update_kan = 1;
   }
   else
      if (i == DMMENU_UPDATE)
         update_kan = 1;

   if (update_kan)
   {
      if (scherm_aantal_kol() >= 80)
         scherm_tekst(scherm_aantal_reg() - 2,0 ,scherm_aantal_kol(),kleur_pf(),"  Ins=Toevoegen  Del=Verwijder  F3=Einde  Shift-F10=Wijzig");
      else
         scherm_tekst(scherm_aantal_reg() - 1,0 ,scherm_aantal_kol(),kleur_pf(),"  Ins/Del  F3=Einde  Shift-F10=Wijzig");
   }

   grp = 0;
   sub = 0;

   do
   {
      /* Converteer het menu voor dialoog */
      memset(menu,0,sizeof(menu));
      tussen_tel = 0;
      for (i = 0; i < dmmenu_aantal_groep() ; i++)
      {
         int aantal = 0;

         menu[i][0] = dmmenu_groep(i);
         /* Tel het aantal werkelijke keuzes (dus geen - regels) */
         for ( j = 0; j < dmmenu_aantal_keuze(i) ; j++ )
           if (strcmp(dmmenu_keuze(i,j),"-") != 0)
              aantal++;

         if (aantal <= 9)
         {
            aantal = 0;
            for ( j = 0; j < dmmenu_aantal_keuze(i) ; j++ )
            {
               if (strcmp(dmmenu_keuze(i,j),"-") != 0)
               {
                  aantal++;
                  sprintf(tussen_tekst[tussen_tel],"%d. %s",aantal,dmmenu_keuze(i,j));
                  menu[i][j+1] = tussen_tekst[tussen_tel];
               }
               else
                  menu[i][j+1] = dmmenu_keuze(i,j);
               tussen_tel++;
            }
         }
         else
         {
            for ( j = 0; j < dmmenu_aantal_keuze(i) ; j++ )
               menu[i][j+1] = dmmenu_keuze(i,j);
         }
      }

      /* Vraag een keuze */
      dialoog_toon_menu(1,0,scherm_aantal_kol() -1,menu);
      toets = dialoog_kies_menu(1,0,scherm_aantal_kol() -1,menu,&grp,&sub);

      if (cua_enter(toets))
      {
	 strcpy(regel,dmmenu_dir(grp,sub-1));
	 if (   (regel[0] != '\0')
	     && !(regel[1] == ':' && regel[2] == '\0')
	     && !(regel[1] == ':' && regel[2] == '\\' && regel[3] == '\0') )
	 {
	    /* kijk of de directory aanwezig is */
	    ptr_hulp  = regel;
#ifdef MSDOS
	    sregs.ds  = FP_SEG(ptr_hulp);
	    regs.x.dx = FP_OFF(ptr_hulp);
	    regs.h.ah = 0x43;
	    regs.h.al = 0;
	    intdosx(&regs,&regs,&sregs);
	    if (( regs.x.cflag & 1) || ((regs.x.cx & _A_SUBDIR) == 0))
	    {
	       sprintf(regel,"Directory %s niet gevonden",dmmenu_dir(grp,sub-1));
	       j = dialoog_window(regel,keuze_doorgaan);
	       if ( j < 1 ) toets = 0;
	    }
#endif
	 }
      }
      if (update_kan && toets == CUA_INVOEGEN)
      {
         if (sub < 1)
         {
            /* Vraag de nieuwe groep gegevens */
            if (dmgroep_vraag() == DMGROEP_GELUKT)
            {
               if (dmkeuze_vraag() == DMKEUZE_GELUKT)
               {
                  /* Nieuwe groep */
                  grp = dmmenu_nwe_groep();
                  dmmenu_zet_groep(grp,dmgroep_titel());

                  /* Nieuwe keuze */
                  i = dmmenu_nwe_keuze(grp,0);
                  keuze_naar_menu(grp,i);

                  update = 1;
               }
            }
            dmgroep_leeg();
         }
         else
         {
            /* Vraag naar plaats van nieuwe keuze */
            i = dialoog_window("Waar de nieuwe keuze toevoegen ?",keuze_a_c_v);
            if (i >= 0)
            {
               if (i == 0)
                  sub = 0;
               if (i == 2)
                  sub = 1;

               /* Vraag de nieuwe keuze gegevens */
               if (dmkeuze_vraag() == DMKEUZE_GELUKT)
               {
                   i = dmmenu_nwe_keuze(grp,sub);
                   keuze_naar_menu(grp,i);

                   update = 1;
               }
               sub = i + 1;
            }
         }
      }
      if (toets == TOETS_F4 || cua_object_menu(toets))
      {
         if (sub < 1)
         {
            if (update_kan)
            {
               /* Vraag de nieuwe groep gegevens */
               dmgroep_zet_titel(dmmenu_groep(grp));
               if (dmgroep_vraag() == DMGROEP_GELUKT)
               {
                  dmmenu_zet_groep(grp,dmgroep_titel());
                  update = 1;
               }
               dmgroep_leeg();
            }
         }
         else
         {
            i = sub - 1;
            menu_naar_keuze(grp,i);

            if ( update_kan )
            {
               /* Vraag de nieuwe keuze gegevens */
               if (dmkeuze_vraag() == DMKEUZE_GELUKT)
               {
                   keuze_naar_menu(grp,i);
                   update = 1;
               }
            }
            else
               dmkeuze_show();
         }
      }
      if (update_kan && toets == CUA_WISSEN )
      {
         if (sub < 1)
            j = dialoog_window("Groep bevat nog keuzes",keuze_del_groep);
         else
         {
            /* Vraag een akkoord */
            i = sub - 1;
            strcpy(regel,"Keuze '");
            strcat(regel,dmmenu_keuze(grp,i));
            strcat(regel,"' wordt verwijderd");
            j = dialoog_window(regel,keuze_verwijder);
            if ( j == 0 )
            {
               /* Bewaar de keuze gegevens */
               i = sub - 1;
               menu_naar_keuze(grp,i);
               dmmenu_weg_keuze(grp,i);
               update = 1;
            }
         }
         grp = 0;
         sub = 0;
      }
      if (cua_afsluiten(toets))
      {
         j = dialoog_window("Menu verlaten ?",keuze_ja_nee);
         if (j != 0)
            toets = 0;
         else
         {
            while ( dmparm_pop_menu() == DMPARM_GELUKT );
         }
      }
      if (cua_annuleren(toets))
      {
         if (dmparm_pop_menu() == DMPARM_GELUKT)
            dmparm_push_menu();
         else
         {
            j = dialoog_window("Menu verlaten ?",keuze_ja_nee);
            if (j != 0)
               toets = 0;
         }
      }
      if (cua_enter(toets) && strcmp(dmmenu_keuze(grp,sub-1),"-") == 0)
      {
	 GELUID geluid;
	 geluid.beep();
         toets = 0;
      }
      if (cua_enter(toets))
      {
         /* Splits optie in programma en parameter */
         strcpy(regel,dmmenu_prog(grp,sub-1));
         for (i = 0; regel[i] != '\0' && regel[i] != ' '; i++);
         if (regel[i] == ' ')
            strcpy(hulp_parm,&regel[i+1]);
         else
            strcpy(hulp_parm,"");
         regel[i] = '\0';
         for (j = 0; j < 4 && toets == CUA_ENTER ; j++)
         {
             if (strlen(dmmenu_vraag(grp,sub-1,j)) > 0)
             {
                if (strlen(hulp_parm) > 0)
                   strcat(hulp_parm," ");

                /* Stel de vraag */
                strcpy(antwoord,"");
                strcpy(hulp_vraag,dmmenu_keuze(grp,sub-1));
                strcat(hulp_vraag,"\n\n");
                strcat(hulp_vraag,dmmenu_vraag(grp,sub-1,j));
                do
                  toets = dialoog_vraag(hulp_vraag,antwoord,30);
                while ( !cua_enter(toets) && !cua_annuleren(toets));
                tekst_trim(antwoord);
                hulp_vraag[0] = '%';
                hulp_vraag[1] = j+'1';
                hulp_vraag[2] = '\0';
                tekst_vervang(hulp_parm,hulp_vraag,antwoord);
             }
         }
         if (regel[0] == '\0')
	 {
	    GELUID geluid;
	    geluid.beep();
            toets = 0;
         }
         if ( cua_enter(toets))
         {
            dmparm_set_exec_prog(regel);
            dmparm_set_exec_parm(hulp_parm);
            dmparm_set_data_path(dmmenu_dir(grp,sub-1));
            dmparm_set_exec_wacht(dmmenu_wacht(grp,sub-1));
         }
         else
            toets = -1;
      }
   }
   while ( !cua_enter(toets) && !cua_annuleren(toets) && !cua_afsluiten(toets));


   /* Einde scherm en toetsafhandeling */
   if (update)
      dmmenu_schrijf();

   dmmenu_sluit();

   return DMAKTIE_NIETS;
}
