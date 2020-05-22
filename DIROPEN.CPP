/* ------------------------------------------------------------------------- */
/* DIROPEN                                                                   */
/* Taak       : Interaktief selecteren bestand                               */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <malloc.h>

#include "diropen.h"
#include "scherm.h"
#include "dialoog.h"
#include "toets.h"
#include "cua.h"
#include "dosdir.h"
#include "dosdrive.h"
#include "tekst.h"
#include "dosfind.h"

#define KLEUR_TEKST  kleur_body_tekst_normaal()
#define KLEUR_DATA   kleur_body_veld_normaal()

#define MAX_SUBDIR   128
#define MAX_FILE    1024

static char * dir_lijst[MAX_SUBDIR +1];
static char * file_lijst[MAX_FILE +1];
static char vol_naam[13] = "";

#ifndef __MSDOS__
_Optlink vergelijk(const void *, const void *);
#endif
/* ---------------------------------------------------------------------- */
/* VERGELIJK                                                              */
/* Taak       Vergelijk twee string pointers                              */
/*                                                                        */
/* ---------------------------------------------------------------------- */
int vergelijk(const void * string1, const void *string2)
{

   return strcmp(*(char **)(string1),*(char **)(string2));
}

static int lees_dir(char pad[],char bestanden[]);
static int lees_dir(char pad[],char bestanden[])
{
   register int i;
   int j;
   char zoek[65];
   static int eerste = 1;
   char h_drive[3];
   char h_dir[128];
   char h_fname[16];
   char h_ext[16];
   int retcd = DIROPEN_OK;

   tekst_trim(bestanden);
   if (   strchr(bestanden,':') != NULL
       || strchr(bestanden,'\\') != NULL)
   {
      /* Haal padnamen uit de ingetoetste bestandsnaam */
      _splitpath(bestanden,h_drive,h_dir,h_fname,h_ext);
      if (h_dir[0] == '\0')
	 strcpy(h_dir,".");
      _makepath(zoek,h_drive,h_dir,"","");
      _makepath(bestanden,"","",h_fname,h_ext);
      DOSDIR dir(zoek);
      if (!dir.volledige_naam(pad))
	 retcd = DIROPEN_NOT_OK;
   }
   if (retcd == DIROPEN_OK)
   {
      strcpy(zoek,pad);
      if (strlen(pad) > 3)
      {
	 if (pad[strlen(pad) -1] == '\\')
	    zoek[strlen(pad) -1] = '\0';
      }

      DOSDIR dir(zoek);
      if (!dir.volledige_naam(pad))
	 retcd = DIROPEN_NOT_OK;
   }
   if (retcd == DIROPEN_OK)
   {
      if (pad[strlen(pad) -1] != '\\')
	 strcat(pad,"\\");

      for (i = 1; dir_lijst[i] != NULL; i++)
      {
	 free(dir_lijst[i]);
	 dir_lijst[i] = NULL;
      }

      /* Haal de volume naam op */
      {
	 DOSDRIVE volume(pad[0]);
	 if (!volume.geef_disk_naam(vol_naam))
	    strcpy(vol_naam,"");
      }

      /* Lees de subdirectories onder de root */
      strcpy(zoek,pad);
      strcat(zoek,"*.*");

      if (strlen(pad) > 3)
      {
	 dir_lijst[0] = "[..]";
	 j = 1;
      }
      else j = 0;
      dir_lijst[j] = NULL;

      {
	 DOSFIND entry(zoek);

	 while (entry.search() && j < MAX_SUBDIR)
	 {
	    if (entry.is_subdir())
	    {
	       if (    strcmp(entry.name(),".")  != 0
		    && strcmp(entry.name(),"..") != 0
		  )
	       {
		  /* Plaats subdirectory in lijst */
		  dir_lijst[j] = (char *)malloc(strlen(entry.name()) +3);
		  strcpy(dir_lijst[j],"[");
		  strcat(dir_lijst[j],entry.name());
		  strcat(dir_lijst[j],"]");
		  strlwr(dir_lijst[j]);
		  j++;
		  dir_lijst[j] = NULL;
	       }
	    }
	 }
      }

      /* Voeg alle drives als pseudo-directories toe */
      for (i = 'A'; i <= 'Z'; i++)
      {
	 DOSDRIVE vol((unsigned char)i);

	 if (vol.is_aanwezig())
	 {
	    /* Plaats de drive in lijst */
	    dir_lijst[j] = (char *)malloc(6);
	    strcpy(dir_lijst[j],"[Ä");
	    dir_lijst[j][2] = (char)i;
	    dir_lijst[j][3] = '\0';
	    strcat(dir_lijst[j],"Ä]");
	    j++;
	    dir_lijst[j] = NULL;
	 }
      }

      /* Sorteer de directory lijst */
      if (j > 1)
	 qsort(&dir_lijst[0],(unsigned)j,(unsigned)sizeof(char *),vergelijk);

      /* Lees de file namen */
      for (i = 0; file_lijst[i] != NULL; i++)
      {
	 free(file_lijst[i]);
	 file_lijst[i] = NULL;
      }

      /* Lees de files in de directory */
      strcpy(zoek,pad);
      strcat(zoek,bestanden);

      {
	 DOSFIND entry(zoek);

	 j = 0;

	 while (entry.search() && j < MAX_FILE)
	 {
	    if (entry.is_file())
	    {
	       /* Plaats file in lijst */
	       file_lijst[j] = (char *)malloc(strlen(entry.name()) +1);
	       strcpy(file_lijst[j],entry.name());
	       strlwr(file_lijst[j]);
	       j++;
	       file_lijst[j] = NULL;
	    }
	 }
      }

      /* Sorteer de file lijst */
      if (j > 1)
	 qsort(&file_lijst[0],(unsigned)j,(unsigned)sizeof(char *),vergelijk);
   }

   return retcd;
}

static void tonen(char titel[],char bestanden[], char dir[]);
static void tonen(char titel[],char bestanden[], char dir[])
{
    int nummer;

    scherm_plaats_tekst( 4,28,KLEUR_TEKST,titel);
    scherm_plaats_tekst( 6,14,KLEUR_TEKST,"Bestandsnaam [                                     ]");
    scherm_plaats_tekst( 6,28,KLEUR_DATA,bestanden);
    scherm_plaats_tekst( 7,14,KLEUR_TEKST,"Directory                                           ");
    scherm_plaats_tekst( 7,28,KLEUR_TEKST,dir);
    scherm_plaats_tekst( 8,14,KLEUR_TEKST,"                                                    ");
    scherm_plaats_tekst( 9,14,KLEUR_TEKST,"              Bestanden:          Directories:      ");
    scherm_plaats_tekst( 9,14,KLEUR_TEKST,vol_naam);
    scherm_plaats_tekst(10,14,KLEUR_TEKST,"            ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");
    scherm_plaats_tekst(11,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(12,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(13,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(14,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(15,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(16,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(17,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(18,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(19,14,KLEUR_TEKST,"            ³                ³  ³                  ³");
    scherm_plaats_tekst(20,14,KLEUR_TEKST,"            ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");

    nummer = 0;
    if (file_lijst[0] != NULL)
       dialoog_vertikaal_sel(DIALOOG_TOON,10,28,14,9,file_lijst,&nummer);
    nummer = 0;
    if (dir_lijst[0] != NULL)
    dialoog_vertikaal_sel(DIALOOG_TOON,10,48,16,9,dir_lijst,&nummer);
}

int diropen(char titel[],char bestanden[], char dir[],char bestand[])
{
   int toets = 0;
   int veld = 0;
   int nummer;
   char hulp[255];

   dialoog_popup_start(3,12,56,19);

   dir_lijst[0] = NULL;
   file_lijst[0] = NULL;
   file_lijst[1] = NULL;
   dir_lijst[1] = NULL;

   strcpy(hulp,dir);
   DOSDIR zoekdir(dir);
   zoekdir.volledige_naam(dir);
   strcpy(bestand,"");
   lees_dir(dir,bestanden);
   tonen(titel,bestanden,dir);

   while ( toets != CUA_AFBREKEN && veld < 3)
   {
      switch(veld)
      {
	  case 0 :
	       toets = dialoog_lees_veld(6,28,37,KLEUR_DATA,bestanden);
               if (toets == CUA_ENTER)
               {
                  lees_dir(dir,bestanden);
                  tonen(titel,bestanden,dir);
	       }
               break;
          case 1:
               if (file_lijst[0] == NULL)
                  toets = CUA_TABULEREN;
               else
               {
                  nummer = 0;
                  toets = dialoog_vertikaal_sel(DIALOOG_LEES,10,28,14,9,file_lijst,&nummer);
		  if (toets == CUA_ENTER)
                  {
                     strcpy(bestand,dir);
		     strcat(bestand,file_lijst[nummer]);
                     veld = 99;
                  }
               }
               break;
	  case 2:
               if (dir_lijst[0] == NULL)
                  toets = CUA_TABULEREN;
               else
               {
                  nummer = 0;
		  toets = dialoog_vertikaal_sel(DIALOOG_LEES,10,48,16,9,dir_lijst,&nummer);
                  if (toets == CUA_ENTER)
                  {
		     strcpy(hulp,dir);
                     if (strcmp(dir_lijst[nummer],"[..]") == 0)
                     {
			int i;
                        for (i = strlen(dir) - 2; i > 0 && dir[i] != '\\'; i--);
                        dir[i+1] = '\0';
                     }
                     else
		     if (dir_lijst[nummer][1] == 'Ä')
                     {
                        dir[0] = dir_lijst[nummer][2];
                        dir[1] = ':';
                        dir[2] = '.';
                        dir[3] = '\0';
                     }
                     else
                     {
                        strcat(dir,&dir_lijst[nummer][1]);
                        dir[strlen(dir)-1] = '\0';
                        strcat(dir,"\\");
		     }
                     if (lees_dir(dir,bestanden) != DIROPEN_OK)
                        strcpy(dir,hulp);
		     tonen(titel,bestanden,dir);
                  }
	       }
               break;
      }
      if (toets == CUA_TABULEREN || toets == CUA_NAAR_RECHTS || toets == CUA_NAAR_BENEDEN)
      {
         if (veld < 2)
            veld++;
         else
            veld = 0;
      }
      if (toets == CUA_TAB_TERUG || toets == CUA_NAAR_LINKS || toets == CUA_NAAR_BOVEN)
      {
	 if (veld > 0)
            veld--;
         else
            veld = 2;
      }
      if (   toets == CUA_ENTER
          && strchr(bestanden,'?') == NULL
          && strchr(bestanden,'*') == NULL)
      {
	 DOSDIR zoek(dir);
	 strcpy(bestand,dir);
	 strcat(bestand,bestanden);
	 if (zoek.zoek_bestand(bestand))
            veld = 99;
      }
   }
   dialoog_popup_einde();

   tekst_trim(bestand);
   tekst_trim(bestanden);
   tekst_trim(dir);

   if (toets == CUA_AFBREKEN)
      return DIROPEN_NOT_OK;
   else
      return DIROPEN_OK;
}


