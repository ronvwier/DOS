/* ------------------------------------------------------------------------- */
/* DMPARM     : Benaderen globaal werkgebied                                 */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stdio.h"
#include "string.h"
#ifdef __MSDOS__
   #include "dos.h"
#endif
#include "dm.h"
#include "dmparm.h"

static struct DMPARM_AREA *ptr_save_area = NULL;

int dmparm_push_menu()
{
    int retcode = DMPARM_MISLUKT;

    if (ptr_save_area->menu_level < 3)
    {
       /* Bewaar het huidige menu */
       ptr_save_area->menu_level++;
       strcpy(ptr_save_area->logo_path[ptr_save_area->menu_level],
              ptr_save_area->logo_path[ptr_save_area->menu_level-1]);
       retcode = DMPARM_GELUKT;
    }
    return retcode;
}

int dmparm_pop_menu()
{
    int retcode = DMPARM_MISLUKT;

    if (ptr_save_area->menu_level > 0)
    {
       /* Haal het vorige menu terug */
       ptr_save_area->menu_level--;
       retcode = DMPARM_GELUKT;
    }
    return retcode;
}

int    dmparm_geheugen(void  * adres,int grootte)
{
   int retcd = DMPARM_MISLUKT;

   if (grootte >= sizeof(struct DMPARM_AREA))
   {
      /* Zet de save area pointer */
      ptr_save_area = (DMPARM_AREA *)adres;
      retcd = DMPARM_GELUKT;
   }
   return retcd;
}

char  * dmparm_get_exec_parm()
{
   return(ptr_save_area->exec_parm);
}

char  * dmparm_get_exec_prog()
{
   return(ptr_save_area->exec_prog);
}

int dmparm_get_exec_wacht()
{
   return(ptr_save_area->wacht);
}

char * dmparm_get_default_dir()
{
   return(ptr_save_area->default_dir);
}

char * dmparm_get_target_dir()
{
   return(ptr_save_area->target_dir);
}

int dmparm_get_video_mode()
{
   return(ptr_save_area->video_mode);
}

char  * dmparm_get_data_path()
{
   return(ptr_save_area->data_path);
}

char  * dmparm_get_menu_path()
{
   return(ptr_save_area->menu_path[ptr_save_area->menu_level]);
}

char  * dmparm_get_logo_path()
{
   return(ptr_save_area->logo_path[ptr_save_area->menu_level]);
}

int dmparm_set_exec_parm(char  * exec_parm)
{
   strcpy(ptr_save_area->exec_parm,exec_parm);
   return DMPARM_GELUKT;
}

int dmparm_set_exec_prog(char  * exec_prog)
{
   strcpy(ptr_save_area->exec_prog,exec_prog);
   strupr(ptr_save_area->exec_prog);
   return DMPARM_GELUKT;
}

int dmparm_set_exec_wacht(int wacht)
{
   ptr_save_area->wacht = wacht;
   return DMPARM_GELUKT;
}

int dmparm_set_data_path(char  * data_path)
{
   strcpy(ptr_save_area->data_path,data_path);
   return DMPARM_GELUKT;
}

int dmparm_set_menu_path(char  * menu_path)
{
   strcpy(ptr_save_area->menu_path[ptr_save_area->menu_level],menu_path);
   return DMPARM_GELUKT;
}

int dmparm_set_logo_path(char  * menu_path)
{
   strcpy(ptr_save_area->logo_path[ptr_save_area->menu_level],menu_path);
   return DMPARM_GELUKT;
}

int dmparm_set_default_dir(char  * naam)
{
   strcpy(ptr_save_area->default_dir,naam);
   return DMPARM_GELUKT;
}

int dmparm_set_target_dir(char  * naam)
{
   strcpy(ptr_save_area->target_dir,naam);
   return DMPARM_GELUKT;
}

int dmparm_set_video_mode(int mode)
{
   ptr_save_area->video_mode = mode;
   return DMPARM_GELUKT;
}
