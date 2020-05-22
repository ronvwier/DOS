/* ------------------------------------------------------------------------- */
/* DMMENU     : I/O op menu bestand                                          */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dmmenu.h"
#ifdef __MSDOS__
   #include "dos.h"
#endif
#include "dm.h"
#include "assert.h"

#define STATUS_INIT      0
#define STATUS_READ      1
#define STATUS_UPDATE    2

#define ID_GROEP   1
#define ID_KEUZE   10
#define ID_DIR     11
#define ID_PROG    12
#define ID_WACHT   13
#define ID_TYPE    21
#define ID_TEKST   22
#define ID_VRAAG   31

#define dmmenu_VERSIE      2

#define MAX_KEUZE 99

static unsigned char id_groep = ID_GROEP;
static unsigned char id_keuze = ID_KEUZE;
static unsigned char id_dir   = ID_DIR;
static unsigned char id_prog  = ID_PROG;
static unsigned char id_wacht = ID_WACHT;
static unsigned char id_type  = ID_TYPE;
static unsigned char id_tekst = ID_TEKST;
static unsigned char id_vraag = ID_VRAAG;

struct keuze_rec
{
   char titel[SIZE_TITEL+1];
   char dir[SIZE_DIR+1];
   char prog[SIZE_PROG+1];
   unsigned char wacht;
   char * parm[8];
};

static struct groep_rec
{
   char               titel[SIZE_TITEL];
   int                aantal_keuze;
   struct keuze_rec * keuze[MAX_KEUZE];
}  groep[25];

static int aantal_groep = 0;

static status = STATUS_INIT;

static char dmmenu_naam[64] = "";

int    dmmenu_open(char naam[])
{
   register int i,j,p;
   FILE * menu_file = NULL;
   int retcode = DMMENU_MISLUKT;
   unsigned char versie;
   unsigned char id;
   unsigned char lengte;
   char buffer[256];
   char hulp[80];
   char hulp2[80];
#ifdef MSDOS
   union REGS regs;
   struct SREGS sregs;
#endif
   unsigned char parm_type;

   menu_file = fopen(naam,"r+b");
   if ( menu_file != NULL )
   {
      retcode = DMMENU_UPDATE;
      status  = STATUS_UPDATE;
      fclose(menu_file);

      /* kijk of een update wel mogelijk is */
#ifdef MSDOS
      segread(&sregs);
      sregs.ds  = FP_SEG(naam);
      regs.x.dx = FP_OFF(naam);
      regs.h.ah = 0x43;
      regs.h.al = 1;
      regs.x.cx = 0x20;
      intdosx(&regs,&regs,&sregs);
      if ( regs.x.cflag & 1)
      {
	 menu_file = fopen(naam,"rb");
	 retcode = DMMENU_READ_ONLY;
	 status  = STATUS_READ;
      }
      else
#endif
	 menu_file = fopen(naam,"r+b");
   }
   if (menu_file == NULL)
   {
      menu_file = fopen(naam,"rb");
      if ( menu_file != NULL )
      {
         retcode = DMMENU_READ_ONLY;
         status  = STATUS_READ;
      }
   }
   if ( menu_file != NULL )
   {
      fflush(menu_file);
      setvbuf(menu_file,NULL,_IOFBF,8000);
      assert( fread(&versie,1,1,menu_file) > 0);

      if (versie == 1)
      {
         while ( fread(&id,1,1,menu_file) > 0 )
         {
            fread(&lengte,1,1,menu_file);
            fread(buffer,lengte,1,menu_file);

            switch(id)
            {
               case ID_GROEP :
                     i = dmmenu_nwe_groep();
                     dmmenu_zet_groep(i,buffer);
                  break;
               case ID_KEUZE :
                     j = dmmenu_nwe_keuze(i,0);
                     p = -1;
                     dmmenu_zet_keuze(i,j,buffer);
                  break;
               case ID_DIR   :
                     dmmenu_zet_dir(i,j,buffer);
                  break;
               case ID_PROG  :
                     dmmenu_zet_prog(i,j,buffer);
                  break;
               case ID_WACHT :
                     dmmenu_zet_wacht(i,j,buffer[0]);
                  break;
               case ID_TYPE :
                     parm_type = buffer[0];
                     if (parm_type == PARM_TYPE_VRAAG)
                       p++;
                  break;
               case ID_TEKST:
                     if (parm_type == PARM_TYPE_TEKST)
                     {
                        strcpy(hulp,dmmenu_prog(i,j));
                        strcat(hulp," ");
                        strcat(hulp,buffer);
                        dmmenu_zet_prog(i,j,hulp);
                     }
                     else
                     {
                        strcpy(hulp,dmmenu_prog(i,j));
                        sprintf(hulp2," %%%d",(p+1));
                        strcat(hulp,hulp2);
                        dmmenu_zet_prog(i,j,hulp);
                        dmmenu_zet_vraag(i,j,p,buffer);
                     }
                  break;
            }
         }
      }
      if (versie == 2)
      {
         while ( fread(&id,1,1,menu_file) > 0 )
         {
            fread(&lengte,1,1,menu_file);
            fread(buffer,lengte,1,menu_file);

            switch(id)
            {
               case ID_GROEP :
                     i = dmmenu_nwe_groep();
                     dmmenu_zet_groep(i,buffer);
                  break;
               case ID_KEUZE :
                     j = dmmenu_nwe_keuze(i,0);
                     p = -1;
                     dmmenu_zet_keuze(i,j,buffer);
                  break;
               case ID_DIR   :
                     dmmenu_zet_dir(i,j,buffer);
                  break;
               case ID_PROG  :
                     dmmenu_zet_prog(i,j,buffer);
                  break;
               case ID_WACHT :
                     dmmenu_zet_wacht(i,j,buffer[0]);
                  break;
               case ID_VRAAG:
                     p++;
                     dmmenu_zet_vraag(i,j,p,buffer);
                  break;
            }
         }
      }
      fclose(menu_file);
      strcpy(dmmenu_naam,naam);
   }

   return retcode;
}

int    dmmenu_maak(char naam[])
{
   FILE * dmmenu_file;
   int retcode = DMMENU_MISLUKT;
   int i,j;

   dmmenu_file = fopen(naam,"wb");
   if ( dmmenu_file != NULL )
   {
      fclose(dmmenu_file);
      strcpy(dmmenu_naam,naam);
      status  = STATUS_UPDATE;
      retcode = DMMENU_UPDATE;
      aantal_groep = 0;

      /* Maak voorlopig maar een dummy entry */
      i = dmmenu_nwe_groep();
      dmmenu_zet_groep(i,"Groep");
      j = dmmenu_nwe_keuze(i,0);
      dmmenu_zet_keuze(i,j,"Keuze");

      dmmenu_schrijf();
   }

   return retcode;
}

int    dmmenu_schrijf(void)
{
   register int i,j,x;
   FILE * dmmenu_file;
   int retcode = DMMENU_MISLUKT;
   unsigned char versie = dmmenu_VERSIE;
   int lengte;
   unsigned char wacht;

   dmmenu_file = fopen(dmmenu_naam,"wb");
   if ( dmmenu_file != NULL )
   {
      /* Schrijf het menu weg */
      fwrite(&versie,1,1,dmmenu_file);
      for (i = 0; i < aantal_groep; i++)
      {
         /* Schrijf groep gegevens */
         fwrite(&id_groep,1,1,dmmenu_file);
         lengte = strlen(groep[i].titel) + 1;
         fwrite(&lengte,1,1,dmmenu_file);
         fwrite(groep[i].titel,lengte,1,dmmenu_file);

         for ( j = 0; j < groep[i].aantal_keuze; j++ )
         {
            /* Schrijf keuze gegevens */
            fwrite(&id_keuze,1,1,dmmenu_file);
            lengte = strlen(groep[i].keuze[j]->titel) + 1;
            fwrite(&lengte,1,1,dmmenu_file);
            fwrite(groep[i].keuze[j]->titel,lengte,1,dmmenu_file);

            fwrite(&id_dir,1,1,dmmenu_file);
            lengte = strlen(groep[i].keuze[j]->dir) + 1;
            fwrite(&lengte,1,1,dmmenu_file);
            fwrite(groep[i].keuze[j]->dir,lengte,1,dmmenu_file);

            fwrite(&id_prog,1,1,dmmenu_file);
            lengte = strlen(groep[i].keuze[j]->prog) + 1;
            fwrite(&lengte,1,1,dmmenu_file);
            fwrite(groep[i].keuze[j]->prog,lengte,1,dmmenu_file);

            fwrite(&id_wacht,1,1,dmmenu_file);
            lengte = 1;
            fwrite(&lengte,1,1,dmmenu_file);
            wacht = groep[i].keuze[j]->wacht;
            fwrite(&wacht,lengte,1,dmmenu_file);

            for (x = 0; x < 8 ; x++)
            {
                if ( groep[i].keuze[j]->parm[0] != '\0' )
                {
                   fwrite(&id_vraag,1,1,dmmenu_file);
                   lengte = strlen(groep[i].keuze[j]->parm[x]) + 1;
                   fwrite(&lengte,1,1,dmmenu_file);
                   fwrite(groep[i].keuze[j]->parm[x],lengte,1,dmmenu_file);
                }
            }
         }
      }

      /* Sluit het menu bestand */
      fclose(dmmenu_file);
      retcode = DMMENU_GELUKT;
   }

   return retcode;
}

int    dmmenu_sluit(void)
{
    aantal_groep = 0;
    return DMMENU_GELUKT;
}

int    dmmenu_aantal_groep(void)
{
   return aantal_groep;
}

int    dmmenu_aantal_keuze(int groepnr)
{
   return groep[groepnr].aantal_keuze;
}

char * dmmenu_groep(int groepnr)
{
   return groep[groepnr].titel;
}

char * dmmenu_keuze(int groepnr,int keuzenr)
{
   return groep[groepnr].keuze[keuzenr]->titel;
}

char * dmmenu_dir(int groepnr,int keuzenr)
{
   return groep[groepnr].keuze[keuzenr]->dir;
}

char * dmmenu_prog(int groepnr,int keuzenr)
{
   return groep[groepnr].keuze[keuzenr]->prog;
}

int dmmenu_wacht(int groepnr,int keuzenr)
{
   return groep[groepnr].keuze[keuzenr]->wacht;
}

int    dmmenu_nwe_groep(void)
{
   register int i;

   i = aantal_groep;
   aantal_groep++;
   groep[i].aantal_keuze = 0;
   strcpy(groep[i].titel,"");

   return (i);
}

int    dmmenu_weg_groep(int groepnr)
{
   register int i;

   /* Verwijder alleen lege groepen en laat er 1 over */
   if (aantal_groep > 1 && groep[groepnr].aantal_keuze < 1)
   {
      for ( i = groepnr; i < aantal_groep ; i++ )
         memcpy(&groep[i],&groep[i+1],sizeof(struct groep_rec));
      aantal_groep--;
   }

   return DMMENU_GELUKT;
}

int    dmmenu_nwe_keuze(int groepnr,int pos)
{
   register int i,j;

   /* Maak nieuwe keuze achteraan aan */
   i = groep[groepnr].aantal_keuze;
   groep[groepnr].aantal_keuze++;
   groep[groepnr].keuze[i] = (keuze_rec *)malloc(sizeof(struct keuze_rec));
   strcpy(groep[groepnr].keuze[i]->titel,"");
   strcpy(groep[groepnr].keuze[i]->dir,"");
   strcpy(groep[groepnr].keuze[i]->prog,"");
   groep[groepnr].keuze[i]->wacht = 0;

   for (j = 0; j < 8; j++)
      groep[groepnr].keuze[i]->parm[j]      = "";

   if (pos > 0)
   {
      struct keuze_rec *hulp;

      /* Verplaats keuzes 1 naar boven en plaats de nieuwe keuze op pos-1 */
      hulp = groep[groepnr].keuze[i];
      for (j = i; j > (pos -1); j--)
         groep[groepnr].keuze[j] = groep[groepnr].keuze[j-1];
      groep[groepnr].keuze[pos-1] = hulp;
      i = pos -1;
   }

   return (i);
}

int    dmmenu_weg_keuze(int groepnr,int keuzenr)
{
   register int i;

   if ( (groep[groepnr].aantal_keuze > 1)
      || (aantal_groep > 1 && groep[groepnr].aantal_keuze > 0))
   {
      for ( i = keuzenr; i < groep[groepnr].aantal_keuze ; i++ )
         groep[groepnr].keuze[i] = groep[groepnr].keuze[i+1];
      groep[groepnr].aantal_keuze--;
   }

   if (groep[groepnr].aantal_keuze < 1)
      dmmenu_weg_groep(groepnr);

   return DMMENU_GELUKT;
}

int    dmmenu_zet_groep(int groepnr,char titel[])
{
   strcpy(groep[groepnr].titel,titel);

   return DMMENU_GELUKT;
}

int    dmmenu_zet_keuze(int groepnr,int keuzenr,char titel[])
{
   strcpy(groep[groepnr].keuze[keuzenr]->titel,titel);

   return DMMENU_GELUKT;
}

int    dmmenu_zet_wacht(int groepnr,int keuzenr,int wacht)
{
   groep[groepnr].keuze[keuzenr]->wacht = (unsigned char)wacht;

   return DMMENU_GELUKT;
}

int    dmmenu_zet_dir(int groepnr,int keuzenr,char dir[])
{
   strcpy(groep[groepnr].keuze[keuzenr]->dir,dir);

   return DMMENU_GELUKT;
}

int    dmmenu_zet_prog(int groepnr,int keuzenr,char prog[])
{
   strcpy(groep[groepnr].keuze[keuzenr]->prog,prog);

   return DMMENU_GELUKT;
}

char * dmmenu_vraag(int groepnr,int keuzenr,int parm)
{
   return groep[groepnr].keuze[keuzenr]->parm[parm];
}

int    dmmenu_zet_vraag(int groepnr,int keuzenr,int parm,char tekst[])
{
   groep[groepnr].keuze[keuzenr]->parm[parm] = (char *)malloc(strlen(tekst) + 1);
   strcpy(groep[groepnr].keuze[keuzenr]->parm[parm],tekst);

   return DMMENU_GELUKT;
}
