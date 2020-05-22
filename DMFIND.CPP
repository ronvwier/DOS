/* ------------------------------------------------------------------------- */
/* DMFIND     : Zoek menu bestand                                            */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#ifdef __MSDOS__
   #include "dos.h"
#endif
#include "dmfind.h"

int dmfind(char exec_path[],char file_naam[])
{
   FILE *file;
   int retcode = DMFIND_ONBEKEND;
   char split_drive[_MAX_DRIVE];
   char split_dir[_MAX_DIR];
   char split_fname[_MAX_FNAME];
   char split_ext[_MAX_EXT];
   char hulp[64];

   strupr(file_naam);
   _splitpath(file_naam,split_drive,split_dir,split_fname,split_ext);
   _makepath(file_naam,split_drive,split_dir,split_fname,".DM");

   file = fopen(file_naam,"rb");
   if (file != NULL)
      retcode = DMFIND_BEKEND;
   else
   {
      _splitpath(exec_path,split_drive,split_dir,split_fname,split_ext);
      strcpy(hulp,split_drive);
      strcat(hulp,split_dir);
      strcat(hulp,file_naam);
      file = fopen(hulp,"rb");
      if (file != NULL)
      {
         retcode = DMFIND_BEKEND;
         strcpy(file_naam,hulp);
      }
   }

   if (retcode == DMFIND_BEKEND)
      fclose(file);

   return retcode;
}
