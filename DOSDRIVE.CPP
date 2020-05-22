/* ------------------------------------------------------------------------- */
/* DOSDRIVE   : Disk en drive operaties                                      */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#ifdef MSDOS
   #include <dos.h>
#else
   #include <os2.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <ctype.h>

#include "dosdrive.h"

DOSDRIVE::DOSDRIVE(char letter)
{
    drive = letter;
}


/* ---------------------------------------------------------------------- */
/* dosdrive_is_drive_aanwezig                                               */
/* Taak   : Kijken of de drive aanwezig is                                */
/* Begin  : driveletter gevuld                                            */
/* Eind   : OK en drive aanwezig                                          */
/*          NOT_OK en drive niet aanwezig                                 */
/* ---------------------------------------------------------------------- */

DOSDRIVE::is_aanwezig()
{
#ifdef MSDOS
   union   REGS regs;
#else
   ULONG DriveNr, DriveMap;
   int bitnr;
#endif
   int retcd = 0;
#ifdef MSDOS
   if (_osmajor > 3 || (_osmajor == 3 && _osminor >= 10))
   {
      /* Vraag of drive lokaal of netwerk is */
      regs.h.ah = 0x44;
      regs.h.al = 0x09;
      regs.h.bl = (unsigned char)(toupper(drive) - 'A' + 1);
      int86(0x21,&regs,&regs);

      /* test op fout indicator */
      if (!(regs.x.cflag & 1))
	 retcd = 1;
      else
	 retcd = 0;
   }
   else
   {
      /* Floppies altijd aanwezig, anderen alleen indien leesbaar */
      switch(toupper(drive))
      {
	 case 'A' :
	 case 'B' : retcd = 1;
		    break;
	 default:   retcd = is_disk_leesbaar();
      }
   }
#else
   bitnr = (toupper(drive) - 'A');
   DosQueryCurrentDisk(&DriveNr,&DriveMap);
   if ((0x01 << bitnr) & DriveMap)
      retcd = 1;
   else
      retcd = 0;
#endif

   return retcd;
}

/* ---------------------------------------------------------------------- */
/* dosdrive_is_drive_lokaal                                                 */
/* Taak   : Kijken of de drive lokaal/netwerk is                          */
/* Begin  : driveletter gevuld                                            */
/* Eind   : OK en drive lokaal                                            */
/*          NOT_OK en drive niet lokaal                                   */
/*          SYSTEM_ERROR en drive onbekend                                */
/* ---------------------------------------------------------------------- */

DOSDRIVE::is_lokaal()
{
#ifdef MSDOS
   union   REGS regs;
#endif
   int retcd = 0;

#ifdef MSDOS
   if (_osmajor > 3 || (_osmajor == 3 && _osminor >= 10))
   {
      /* Vraag of drive lokaal of netwerk is */
      regs.h.ah = 0x44;
      regs.h.al = 0x09;
      regs.h.bl = (unsigned char)(toupper(drive) - 'A' + 1);
      int86(0x21,&regs,&regs);

      /* test op fout indicator */
      if (!(regs.x.cflag & 1))
      {
	 if (regs.x.dx & 0x1000)
	    /* Netwerk drive */
	    retcd = 0;
	 else
	    /* Lokale drive */
	    retcd = 1;
      }
   }
   else
#endif
      /* Voor DOS 3.10 geen netwerken */
      retcd = 1;

   return retcd;
}

/* ---------------------------------------------------------------------- */
/* dosdrive_is_drive_aktief                                               */
/* Taak   : Kijken of de logische drive ook de fysieke drive is           */
/* Begin  : driveletter gevuld                                            */
/* Eind   : OK en drive aktief                                            */
/*          NOT_OK en drive niet aktief                                   */
/* ---------------------------------------------------------------------- */

DOSDRIVE::is_aktief()
{
#ifdef MSDOS
   union   REGS regs;
#endif
   int retcd = 0;
   unsigned char driveh;

   /* Een niet aanwezige drive is niet aktief */
   retcd = is_aanwezig();

   if (retcd)
   {
#ifdef MSDOS
      if (_osmajor > 3 || (_osmajor == 3 && _osminor >= 20))
      {
	 if (!is_lokaal())
	    /* Netwerk drive is altijd aktief */
	    retcd = 1;
	 else
	 {
	    /* Test FYSIEKE=LOGISCHE DRIVE, bijv. A: en B: bij 1 floppy systeem */
	    regs.h.ah = 0x44;
	    regs.h.al = 0x0E;
	    driveh = (unsigned char)(toupper(drive) - 'A' + 1) ;
	    regs.h.bl = driveh;
	    intdos(&regs,&regs);
	    if (regs.h.al == 0 || regs.h.al == driveh)
	       retcd = 1;
	    else
	       retcd = 0;
	 }
      }
      else
#endif
	 retcd = 1;
   }

   return retcd;
}

/* ---------------------------------------------------------------------- */
/* dosdrive_is_disk_leesbaar                                                */
/* Taak   : Controle of disk in drive gelezen kan worden                  */
/* Begin  : driveletter gevuld                                            */
/* Eind   : OK en disk leesbaar                                           */
/*          NOT_OK en disk niet leesbaar (afwezig, ongeformateerd)        */
/* ---------------------------------------------------------------------- */

DOSDRIVE::is_disk_leesbaar()
{
   int     ret_code= 0;
   unsigned char driveh;
#ifdef __MSDOS__
   struct diskfree_t dtable;
#endif

   /* Forceer FYSIEKE=LOGISCHE DRIVE, bijv. A: en B: bij 1 floppy systeem */
   aktiveer();

   /* Vraag vrije ruimte op disk op */
   driveh = (unsigned char)(toupper(drive) - 'A' + 1) ;
#ifdef __MSDOS__
   if (_dos_getdiskfree(driveh,&dtable) == 0)
#endif
      ret_code = 1;

   return(ret_code);
}

/* ---------------------------------------------------------------------- */
/* dosdrive_aktiveer_drive                                                */
/* Taak   : Maak logische en fysieke drive gelijk                         */
/* Begin  : driveletter gevuld                                            */
/* Eind   : drive is aktief, DOS melding plaats diskette in drive         */
/*          zal niet voorkomen                                            */
/* ---------------------------------------------------------------------- */

DOSDRIVE::aktiveer()
{
#ifdef MSDOS
   union   REGS regs;
#endif

#ifdef MSDOS
   if (_osmajor > 3 || (_osmajor == 3 && _osminor >= 20))
   {
      /* Forceer FYSIEKE=LOGISCHE DRIVE, bijv. A: en B: bij 1 floppy systeem */
      regs.h.ah = 0x44;
      regs.h.al = 0x0F;
      regs.h.bl = (unsigned char)(toupper(drive) - 'A' + 1) ;
      intdos(&regs,&regs);
   }
#endif

   return 1;
}

DOSDRIVE::geef_disk_naam(char naam[])
{
   int retcd = 0;
#ifdef __MSDOS__
   struct find_t entry;
#endif
   char vol_zoek[14];
   register int i;

   vol_zoek[0] = drive;
   vol_zoek[1] = '\0';
   strcat(vol_zoek,":\\*.*");

   if (is_disk_leesbaar())
   {
#ifdef __MSDOS__
      if (_dos_findfirst(vol_zoek,_A_VOLID,&entry) == 0)
      {
	 retcd = 1;
	 if (strlen(entry.name) > 8)
	 {
	    for (i = 0; i < 8; i++)  naam[i]   = entry.name[i];
	    for (i = 9; i < 13; i++) naam[i-1] = entry.name[i];
	 }
	 else
	    for (i = 0; i < 9; i++)  naam[i]   = entry.name[i];
      }
#endif
#ifdef __OS2__
   retcd = 0;
#endif
   }
   return retcd;
} 
