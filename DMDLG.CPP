/* ------------------------------------------------------------------------- */
/* DMDLG      : Menu dialoog programma                                       */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif
#include <string.h>

#include "scherm.h"
#include "toets.h"
#include "muis.h"

#include "dm.h"
#include "dmfind.h"
#include "dmaktie.h"
#include "dmparm.h"
#include "dmcd.h"

static unsigned int *ptr_iap;

static LOGO logo;

static LOGO groot_logo = {
  " ",
  " ",
  " ",
  " ",
  " ",
  "                      ÚÄÄÄÄÄÄÄÄÄÄÄ¿   ÚÄÄ¿        ÚÄÄ¿",
  "                      ³ ÚÄÄÄÄÄÄÄ¿ À¿  ³  À¿      ÚÙ  ³",
  "                      ³ ³       À¿ ³  ³ Ú¿À¿    ÚÙÚ¿ ³",
  "                      ³ ³        ³ ³  ³ ³À¿À¿  ÚÙÚÙ³ ³",
  "                      ³ ³        ³ ³  ³ ³ À¿À¿ÚÙÚÙ ³ ³",
  "                      ³ ³        ³ ³  ³ ³  À¿ÀÙÚÙ  ³ ³",
  "                      ³ ³        ³ ³  ³ ³   ÀÄÄÙ   ³ ³",
  "                      ³ ³  DOS   ³ ³  ³ ³   MENU   ³ ³",
  "                      ³ ³       ÚÙ ³  ³ ³          ³ ³",
  "                      ³ ÀÄÄÄÄÄÄÄÙ ÚÙ  ³ ³          ³ ³",
  "                      ÀÄÄÄÄÄÄÄÄÄÄÄÙ   ÀÄÙ          ÀÄÙ",
  " ",
  " ",
  " "};

static LOGO klein_logo = {
  "    ÚÄÄÄÄÄÄÄÄÄÄÄ¿   ÚÄÄ¿        ÚÄÄ¿",
  "    ³ ÚÄÄÄÄÄÄÄ¿ À¿  ³  À¿      ÚÙ  ³",
  "    ³ ³       À¿ ³  ³ Ú¿À¿    ÚÙÚ¿ ³",
  "    ³ ³        ³ ³  ³ ³À¿À¿  ÚÙÚÙ³ ³",
  "    ³ ³        ³ ³  ³ ³ À¿À¿ÚÙÚÙ ³ ³",
  "    ³ ³        ³ ³  ³ ³  À¿ÀÙÚÙ  ³ ³",
  "    ³ ³        ³ ³  ³ ³   ÀÄÄÙ   ³ ³",
  "    ³ ³  DOS   ³ ³  ³ ³   MENU   ³ ³",
  "    ³ ³       ÚÙ ³  ³ ³          ³ ³",
  "    ³ ÀÄÄÄÄÄÄÄÙ ÚÙ  ³ ³          ³ ³",
  "    ÀÄÄÄÄÄÄÄÄÄÄÄÙ   ÀÄÙ          ÀÄÙ",
  " ",
  " ",
  " ",
  " ",
  " "};

static char logo_file_naam[65] = "dmlogo.txt";

#ifdef MSDOS
int  main(int argc,char * argv[])
{
   char  *     ptr_save_area;
#endif
#ifdef __OS2__
int dmdlg(void * ptr_save_area,char * menu_arg, char * logo_arg)
{
#endif
   char  menu_file[80];
   char hulp[128];
   FILE * logo_file;
   register int i;
   int         terug = 0;
#ifdef MSDOS
   union REGS regs;
#endif
   static char oude_dft_dir[64],
	  oude_nwe_dir[64];

#ifdef MSDOS
   /* Test of de aanroep juist is */
   if (argc < 4)
   {
      printf("--- Aanroep DMDLG alleen via DM ---\n");
      exit(-1);
   }

   /* Zoek het geheugen op van de save area */
   FP_SEG(ptr_save_area) = atoi(argv[1]);
   FP_OFF(ptr_save_area) = atoi(argv[2]);
#endif

   dmparm_geheugen(ptr_save_area,9999);

#ifdef MSDOS
   if (dmparm_get_exec_wacht() == 1)
   {
      /* Wacht op een toetsaanslag alvorens het scherm te wijzigen */
      puts("--- druk op een toets ---");
      getch();
   }
#endif

   if (dmparm_get_video_mode() > 0)
   {
      /* Herstel video mode en directories van voorafgaande programma keuze */

      /* Herstel oude video mode */
#ifdef MSDOS
      regs.h.ah = 0x0f;
      int86(0x10,&regs,&regs);
      if ( regs.h.al != (unsigned char)dmparm_get_video_mode() )
      {
	 /* Set video mode */
	 regs.h.ah = 0x00;
	 regs.h.al = (unsigned char)dmparm_get_video_mode();
	 int86(0x10,&regs,&regs);
      }
#endif

      /* Herstel directories */
      dmcd(dmparm_get_target_dir(),hulp,hulp);
      dmcd(dmparm_get_default_dir(),hulp,hulp);
   }
   else
   {
      /* Intieele aanroep */
#ifdef MSDOS
      if (argc > 4)
	 dmparm_set_menu_path(argv[4]);
      /* LOGO filenaam meegegeven ? */
      if (argc > 5)
	 dmparm_set_logo_path(argv[5]);
      else
	 dmparm_set_logo_path(logo_file_naam);
#endif
#ifdef __OS2__
      if (strlen(menu_arg) > 0)
	 dmparm_set_menu_path(menu_arg);
      /* LOGO filenaam meegegeven ? */
      if (strlen(logo_arg) > 0)
	 dmparm_set_logo_path(logo_arg);
      else
	 dmparm_set_logo_path(logo_file_naam);
#endif
   }

   /* Init scherm en toetsafhandeling */
   scherm_init();
   muis_init();

   while ( terug == 0 )
   {
      /* Per default geen aktie */
      dmparm_set_exec_parm("");
      dmparm_set_exec_prog("");
      dmparm_set_data_path("");
      dmparm_set_exec_wacht(0);

      /* Vraag de naam van de menu file */
      strcpy(menu_file,dmparm_get_menu_path());
      if (strlen(menu_file) < 1)
         strcpy(menu_file,"start.dm");
#ifdef MSDOS
      dmfind(argv[0],menu_file);
#endif
#ifdef __OS2__
      dmfind("",menu_file);
#endif
      dmparm_set_menu_path(menu_file);
      strcpy(logo_file_naam,dmparm_get_logo_path());


      if (strlen(logo_file_naam) > 0)
         logo_file = fopen(logo_file_naam,"r");
      else
         logo_file = NULL;
      if (logo_file != NULL)
      {
         for (i = 0; i < LOGO_HOOGTE; i++ )
	 {
            if (fgets(logo[i],80,logo_file) != NULL )
               logo[i][strlen(logo[i])-1] = '\0';
         }
         fclose(logo_file);
      }
      else
      {
         if (scherm_aantal_kol() < 80)
         {
            for (i = 0; i < LOGO_HOOGTE; i++ )
              strcpy(logo[i],klein_logo[i]);
         }
	 else
         {
            for (i = 0; i < LOGO_HOOGTE; i++ )
              strcpy(logo[i],groot_logo[i]);
         }
      }
      /* Ga in dialoog met de gebruiker */
      dmaktie(logo);

      /* Test op aanroep ander menu */
      if (strcmp(dmparm_get_exec_prog(),"DM") == 0)
      {
         char * ptr_hulp;
         strcpy(hulp,dmparm_get_exec_parm());
         /* Roep het nieuwe menu op */
         dmparm_push_menu();
	 dmparm_set_menu_path(strtok(hulp," "));
         ptr_hulp = strtok(NULL," ");
         if (ptr_hulp != NULL)
            dmparm_set_logo_path(ptr_hulp);
      }
      else
      {
         /* Test op return uit menu */
         if ( strcmp(dmparm_get_exec_prog(),"") == 0)
         {
            if ( dmparm_pop_menu() != DMPARM_GELUKT )
               terug = 1;
         }
	 else terug = 1;
      }
   }
   muis_einde();

   /* Bewaar video mode */
#ifdef MSDOS
   regs.h.ah = 0x0f;
   int86(0x10,&regs,&regs);
   dmparm_set_video_mode(regs.h.al);
#endif

   /* Clear screen */
#ifdef MSDOS
   regs.h.ah = 0x00;
   int86(0x10,&regs,&regs);
#endif

   if ( dmparm_get_exec_prog()[0] != '\0')
   {
      /* Ga naar de opgegeven directory en bewaar oude namen */
      dmcd(dmparm_get_data_path(),oude_dft_dir,oude_nwe_dir);
      dmparm_set_target_dir(oude_nwe_dir);
      dmparm_set_default_dir(oude_dft_dir);

      return 0;
   }
   else
      return -1;
}
