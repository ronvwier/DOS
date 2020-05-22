/* ------------------------------------------------------------------------- */
/* DBASE      : I/O op DBASE II/III/IV bestanden                             */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dbase.h"

static FILE *ptr_dbf;

#pragma pack(1)

	 struct DBASEIII {
	  unsigned char ID;                     /*  03 = III, 83 = IV */
	  unsigned char mut_jaar;
	  unsigned char mut_maand;
	  unsigned char mut_dag;
	  unsigned long aantal_records;
	  unsigned int  data_offset;
	  unsigned int  record_lengte;
	  unsigned char X2[20];
	  struct
	  {
	     char naam[11];
	     char type;
	     char X3[4];
	     unsigned char lengte;
	     unsigned char decimalen;
	     char X4[14];
	  }  veld[256];
       };
	struct DBASEII {
	  unsigned char ID;                     /* 02 */
	  unsigned int aantal_records;
	  unsigned char mut_dag;
	  unsigned char mut_jaar;
	  unsigned char mut_maand;
	  unsigned int  record_lengte;
	  struct
	  {
	     char naam[11];
	     char type;
	     unsigned char lengte;
	     char X5[2];
	     unsigned char decimalen;
	  }  veld[32];
	  unsigned char sluit;
	  };

static union DBF_HEADER
    {
       struct DBASEIII dbaseIII;
       struct DBASEII  dbaseII;
    } *ptr_header;

#pragma pack()

static unsigned int  data_offset;
static unsigned int  record_lengte;
static unsigned long aantal_records;

static unsigned char * ptr_record;

static int veld_lengte;
static int veld_offset;

int dbase_geef_veldinfo(int best_nr,int veldnr,char naam[],char * type,int * lengte, int * decimalen)
{
   register int i, v;

   v = veldnr - 1;

   if (ptr_header->dbaseII.ID == 0x02)
   {
      /* Copieer naam zonder afsluitende spaties */
      memcpy(naam,ptr_header->dbaseII.veld[v].naam,11);
      for (i = 11; i > 0 && naam[i] == ' '; i--);
      i++;
      naam[i] = '\0';

      *type      = ptr_header->dbaseII.veld[v].type;
      *lengte    = ptr_header->dbaseII.veld[v].lengte;
      *decimalen = ptr_header->dbaseII.veld[v].decimalen;

      if (naam[0] == '\0' || naam[0] == '\x0d')
         return DBASE_NOT_OK;
      else
         return DBASE_OK;
   }
   else
   {
      /* Copieer naam zonder afsluitende spaties */
      memcpy(naam,ptr_header->dbaseIII.veld[v].naam,11);
      for (i = 11; i > 0 && naam[i] == ' '; i--);
      i++;
      naam[i] = '\0';

      *type      = ptr_header->dbaseIII.veld[v].type;
      *lengte    = ptr_header->dbaseIII.veld[v].lengte;
      *decimalen = ptr_header->dbaseIII.veld[v].decimalen;

      if (naam[0] == '\0' || naam[0] == '\x0d')
         return DBASE_NOT_OK;
      else
         return DBASE_OK;
   }
}

static void lees_def()
{
   unsigned char versie;

   fseek(ptr_dbf,0,SEEK_SET);
   fread(&versie,1,1,ptr_dbf);
   if (versie == 2)
   {
      ptr_header = (DBF_HEADER *)malloc(sizeof(struct DBASEII));
      fseek(ptr_dbf,0,SEEK_SET);
      fread(ptr_header,sizeof(struct DBASEII),1,ptr_dbf);
      ptr_record = (char *)malloc(ptr_header->dbaseII.record_lengte);
      data_offset    = sizeof(struct DBASEII);
      aantal_records = ptr_header->dbaseII.aantal_records;
      record_lengte  = ptr_header->dbaseII.record_lengte;
   }
   else
   {
      fseek(ptr_dbf,8,SEEK_SET);
      fread(&data_offset,sizeof(data_offset),1,ptr_dbf);
      fseek(ptr_dbf,0,SEEK_SET);
      ptr_header = (DBF_HEADER *)malloc(data_offset);
      fread(ptr_header,data_offset,1,ptr_dbf);
      ptr_record = (char *)malloc(ptr_header->dbaseIII.record_lengte);
      aantal_records = ptr_header->dbaseIII.aantal_records;
      record_lengte  = ptr_header->dbaseIII.record_lengte;
   }
}

int dbase_formaat(char bestand[])
{
   unsigned char versie;
   int retcd;

   ptr_dbf = fopen(bestand,"rb");
   if (ptr_dbf == NULL)
      retcd = DBASE_FOUT;
   else
   {
      fread(&versie,1,1,ptr_dbf);
      fclose(ptr_dbf);
      if (versie == 0x02 || versie == 0x03 || versie == 0x83)
         retcd = DBASE_OK;
      else
         retcd = DBASE_NOT_OK;
   }

   return retcd;
}

int dbase_open_lezen(char bestand[],int * best_nr)
{
   ptr_dbf = fopen(bestand,"rb");

   if (ptr_dbf == NULL)
      return DBASE_FOUT;

   lees_def();

   return DBASE_OK;
}

int dbase_sluit(int best_nr)
{
   fclose(ptr_dbf);
   free(ptr_header);

   return DBASE_OK;
}

int dbase_lees_record(int best_nr,long record_nummer)
{
   if (record_nummer <= aantal_records)
   {
      fseek(ptr_dbf,((record_nummer - 1) * record_lengte) + data_offset,SEEK_SET);
      fread(ptr_record,record_lengte,1,ptr_dbf);
      if (ptr_record[0] == ' ')
         return DBASE_OK;
      else
         return DBASE_DELETED;
   }
   else
      return DBASE_EINDE;
}

int dbase_geef_veld(int best_nr,int veldnr,char waarde[])
{
   char naam[24];
   char type;
   int lengte;
   int decimalen;
   int veld_offset;
   register int i;

   /* Bepaal de record offset van het veld */
   veld_offset = 1;
   for (i = 1; i < veldnr; i++)
   {
      if (dbase_geef_veldinfo(best_nr,i,naam,&type,&lengte,&decimalen) != DBASE_OK)
         return DBASE_FOUT;
      veld_offset += lengte;
   }

   /* Haal veldinformatie op */
   if (dbase_geef_veldinfo(best_nr,veldnr,naam,&type,&lengte,&decimalen) != DBASE_OK)
      return DBASE_FOUT;

   /* Copieer data */
   memcpy(waarde,&ptr_record[veld_offset],lengte);
   waarde[lengte] = '\0';

   return DBASE_OK;
}

