/* ------------------------------------------------------------------------- */
/* DOSDIR                                                                    */
/* Taak       : DOS directory operaties                                      */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <ctype.h>
#ifdef __MSDOS__
   #include <dos.h>
#endif

#include "dosdir.h"

/* --------------------------------------------------------------------- */
/* get_drive()                                                    */
/* return: nummer huidige drive. 'A' = 1                                 */
/* --------------------------------------------------------------------- */

static unsigned get_drive(void);
static unsigned get_drive()
{
   unsigned drive;

#ifdef __MSDOS__
   _dos_getdrive(&drive);
#endif
#ifdef __OS2__
   drive = 3;
#endif

   return(drive);
}

/* --------------------------------------------------------------------- */
/* set_drive(drive)                                               */
/* begin : drive = Naam van de drive. A, B .. Z (of verder)              */
/* einde : drive is actief, of error                                     */
/* --------------------------------------------------------------------- */

static int set_drive(unsigned char drivenum)
{  unsigned drive1,drive2,nrdrives;

   drive1=toupper(drivenum)-'A'+1;           /* bereken drivenr 1=A      */

#ifdef __MSDOS__
   _dos_setdrive(drive1,&nrdrives);          /* setdrive geeft geen fout */
#endif

   drive2=get_drive();                       /* kijken of goed gegaan is */

   return( (drive1 == drive2) ? 1 : 0);
}

/* --------------------------------------------------------------------- */
/* bewaar_huidige_pad                                             */
/* in   : buffer adres, grootte van de buffer                            */
/* einde: buffer is gevold met huidige pad, of                           */
/* --------------------------------------------------------------------- */

static int bewaar_huidige_pad(char *buffer, int size)
{
   return( (NULL==getcwd(buffer,size)) ? 0 : 1 );
}

/* --------------------------------------------------------------------- */
/* zet_pad                                                        */
/* in   : padspecificatie, eventueel drivespecificatie                   */
/* einde: de actieve, of de gespecificeerde drive wordt op het pad gezet */
/* --------------------------------------------------------------------- */

static int zet_pad(char *pad)
{
   int result=0;
   int deltadrive=0;
   unsigned prev_drive = 0;
   unsigned dummy;

   if ( pad[1]==':' )                        /* kijk of er een drive is  */
   {  prev_drive = get_drive();
      if (! (result = set_drive(pad[0])) )
      {
         pad+=2;                             /* zijn nu op andere drive  */
         deltadrive=1;
      }
   }
   if (!result)                              /* so far so good           */
      if (chdir(pad))                        /* verander van pad         */
	 result=0;               /* error is gebeurd         */

#ifdef __MSDOS__
   if (deltadrive) _dos_setdrive(prev_drive,&dummy);
#endif

   return(result);
}


DOSDIR::DOSDIR(char naam[])
{
    strcpy(dir_naam,naam);
}


/* --------------------------------------------------------------------- */
/* dosdir_volledige_naam                                                 */
/*        bereken volledig gekwalificeerd pad, gebruik van CHDIR!        */
/* in   : padspecificatie, eventueel drivespecificatie, adres buffer     */
/* einde: bestaanbaarheid is getest, en gekwalificeerde padnaam bekend   */
/*        actieve drive is onveranderd.                                  */
/* error:                                                                */
/* return 1, 0                                       */
/* --------------------------------------------------------------------- */

DOSDIR::volledige_naam(char * uit)
{
   int result = 1;
   unsigned drive,totdrv;
   char pad[DOSDIR_MAX_PAD_LEN];

   /* Evt. afsluitende \ verwijderen indien niet voorafgegaan door : */
   strcpy(pad,dir_naam);
   if (pad[strlen(pad) -1] == '\\'
      && strlen(pad) > 1
      && pad[strlen(pad)-2] != ':'
      )
      pad[strlen(pad) -1] = '\0';

   drive=get_drive();

   if (dir_naam[1]==':')                           /* drive is gespecificeerd  */
      /* Ga naar drive toe */
      result=set_drive(dir_naam[0]);

   if (result)                              /* nu verder gaan           */
   {
      char bewaar[DOSDIR_MAX_PAD_LEN];

      if (result=bewaar_huidige_pad(bewaar,sizeof(bewaar)))
      {
	 /* Ga naar de directory en vraag de volledige naam op */
	 if (pad[0] != '\0')
	    if (chdir(pad))                     /* chdir niet gelukt !      */
	       result=0;

	 if (result)                        /* succesvol gechdirt.      */
	    result=(NULL==getcwd(uit,DOSDIR_MAX_PAD_LEN)) ? 0 : 1;

	 /* Ga naar orginele pad */
	 zet_pad(bewaar);
      }
   }

   /* Ga naar orginele drive */
#ifdef __MSDOS__
   _dos_setdrive(drive,&totdrv);
#endif

   if (result != 1)
      result = 0;

   return(result);
}

DOSDIR::fysieke_naam(char * norm_pad_naam)
{
#ifdef MSDOS
   union  REGS  regs;
   struct SREGS sregs;
#endif

#ifdef MSDOS
   if (_osmajor >= 3)
   {
      regs.h.ah = 0x60;
      sregs.ds  = FP_SEG(dir_naam);
      regs.x.si = FP_OFF(dir_naam);
      sregs.es  = FP_SEG(norm_pad_naam);
      regs.x.di = FP_OFF(norm_pad_naam);
      int86x(0x21,&regs,&regs,&sregs);
   }
   else
#endif
      strcpy(norm_pad_naam,dir_naam);

   return 1;
}

DOSDIR::zoek_bestand(char bestandsnaam[])
{
   int retcd = 0;
#ifdef __MSDOS__
   struct find_t entry;
#endif
   char hulp[DOSDIR_MAX_PAD_LEN];
   char zoek[DOSDIR_MAX_PAD_LEN];
   char h_dir[DOSDIR_MAX_PAD_LEN];
   char file_naam[9];
   char file_ext[8];
   char file_tot[14];
   char h_drive[4];

   /* Zoek met volledige padnaam */
#ifdef __MSDOS__
   if (_dos_findfirst(bestandsnaam,_A_NORMAL,&entry) == 0)
#endif
#ifdef __OS2__
   if (1 == 1)
#endif
      retcd = 1;
   else
   {
      /* Zoek met alleen de file naam ook via PATH */
      _splitpath(bestandsnaam,hulp,hulp,file_naam,file_ext);
      _makepath(file_tot,"","",file_naam,file_ext);
      _searchenv(file_tot,"PATH",hulp);
      if (hulp[0] != '\0')
      {
	 strcpy(bestandsnaam,hulp);
	 retcd = 1;
      }
   }
   if (retcd == 1)
   {
      _splitpath(bestandsnaam,h_drive,h_dir,file_naam,file_ext);
      _makepath(zoek,h_drive,h_dir,"","");
      DOSDIR vertaal(zoek);
      if (vertaal.volledige_naam(hulp))
      {
	 _makepath(bestandsnaam,"",hulp,file_naam,file_ext);
	 strupr(bestandsnaam);
      }
   }


   return retcd;
}
