/* ------------------------------------------------------------------------- */
/* DM         : Dos Menu sturing, semi-resident deel                         */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <process.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif

#include "dm.h"
#include "dmparm.h"

static char parm_seg[12];
static char parm_off[12];
static char menu_dialoog[64];
static char * comspec;
static char  * hlp_ptr;
#ifdef MSDOS
   static char  * menu_arg = NULL;
   static char  * logo_arg = NULL;
   static union REGS regs;
#endif
#ifdef __OS2__
   static char  * menu_arg = "";
   static char  * logo_arg = "";
#endif

static struct DMPARM_AREA save_area;         /* Communicatie gebied */

static void lh_off(void);
static void lh_off()
{
#ifdef MSDOS
   if (_osmajor >= 3)
   {
      /* Zet het automatisch UMB gebruik AF */
      regs.x.ax = 0x5800;
      int86(0x21,&regs,&regs);
      if (regs.x.ax > 0x0002)
      {
	 regs.x.ax = 0x5801;
	 regs.x.bx = 0;
	 int86(0x21,&regs,&regs);
      }
   }
#endif
}

static void menu(void);
static void menu()
{
   /* LoadHigh afzetten om problemen te voorkomen */
   lh_off();

   /* Vraag de gebruiker een menu optie */
#ifdef MSDOS
   if (spawnlp(P_WAIT,menu_dialoog,menu_dialoog,parm_seg,parm_off,VERSIE,menu_arg,logo_arg,NULL) < 0)
      exit(1);
#endif
#ifdef __OS2__
   dmdlg(&save_area,menu_arg,logo_arg);
#endif
}


int main(int argc,char * argv[])
{
   register int i, modeflag;

   comspec = getenv("COMSPEC");

   /* Initialiseer de save area */
   memset(&save_area,'\0',sizeof(save_area));

   /* Converteer het adres van de save area */
   hlp_ptr = (char *)&save_area;
#ifdef MSDOS
   itoa(FP_SEG(hlp_ptr),parm_seg,10);
   itoa(FP_OFF(hlp_ptr),parm_off,10);
#endif

   /* Verwerk de meegeleverde parameters */
   if (argc > 1)
      menu_arg = argv[1];
   if (argc > 2)
      logo_arg = argv[2];

   /* Gebruik het execute pad om DMDLG te vinden */
   if ( strchr(argv[0],'.') != NULL)
   {
      strcpy(menu_dialoog,argv[0]);
      for (i = strlen(menu_dialoog); menu_dialoog[i] != '.' ; i-- );
      menu_dialoog[i] = '\0';
   }
   else
      strcpy(menu_dialoog,"DM");

   strcat(menu_dialoog,"DLG");


   /* Vraag de gebruiker een menu optie */
   menu();

   while( save_area.exec_prog[0] != '\0')
   {
      /* Voer het gekozen programma uit */

      if (save_area.wacht == 2)
	 /* Draag besturing over en stop DM */
	 modeflag = P_OVERLAY;
      else
	 modeflag = P_WAIT;

      /* Probeer programma via COMMAND te starten */
#ifdef MSDOS
      if (strcmp(save_area.exec_prog,"COMMAND") == 0)
	 spawnlp(modeflag,comspec,comspec,save_area.exec_parm,NULL);
      else
	 spawnlp(modeflag,comspec,comspec,"/C",save_area.exec_prog,save_area.exec_parm,NULL);
#endif
#ifdef __OS2__
      if (save_area.wacht == 1)
      {
	 if (strcmp(save_area.exec_prog,"COMMAND") == 0)
	    spawnlp(modeflag,comspec,comspec,"/C start /win /max /f",save_area.exec_parm,NULL);
	 else
	    spawnlp(modeflag,comspec,comspec,"/c start","/k /win /max /f",save_area.exec_prog,save_area.exec_parm,NULL);
      }
      else
      {
	 if (strcmp(save_area.exec_prog,"COMMAND") == 0)
	    spawnlp(modeflag,comspec,comspec,"/C start /win /max /f",save_area.exec_parm,NULL);
	 else
	    spawnlp(modeflag,comspec,comspec,"/c start","/c /win /max /f",save_area.exec_prog,save_area.exec_parm,NULL);
      }
#endif


      /* Vraag de gebruiker een menu optie */
      menu();
   }

   return 0;
}
