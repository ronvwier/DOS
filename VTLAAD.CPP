/* ------------------------------------------------------------------------- */
/* VTLAAD     : Download van telesoftware                                    */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include "time.h"
#include "conio.h"
#include "toets.h"
#include "commport.h"
#include "modem.h"
#include "vtdisld.h"
#include "vtlaad.h"

#define OK      0
#define FOUT    1

typedef char BYTE;

static FILE *uit = NULL;
static BYTE cb_waarde;

static int next_blok;
static int next_file;
static long tot_lengte;
static long file_lengte;
static BYTE l_herhaal[40] = "*00";
static BYTE l_verder[40]  = "_";
static BYTE l_terug[40]   = "_*";
static int tot_fouten;

static char dir[80] = "";

static CommPort *Port;

static lees_vt_teken(BYTE &teken);
static lees_vt_teken(BYTE &teken)
{
   long huidig,start;

   /* Wacht maximaal 10 sec. op de ontvangst van 1 teken */
   time(&start);
   while(!Port->Receive(teken))
   {
      /* Kijk of we al 10 seconden verder zijn */
      time(&huidig);
      if ((huidig - start) > 10) return FOUT;
   }

   return OK;
}

static lees_byte(BYTE &teken);
static lees_byte(BYTE &teken)
{
   BYTE teken1, teken2, fout = 0;

   /* Lees het eerste 7 bits teken */
   if (lees_vt_teken(teken1) != OK) return FOUT;

   /* Vertaal 1..2 7-bits tekens naar 1 byte */
   switch(teken1)
   {
      case 0x7C : /* 00..1F = 7C + t+40 */
		  if (lees_vt_teken(teken2) != OK) return FOUT;
		  teken = teken2 - (BYTE)0x40;
		  if (teken > 0x1F)
		     fout = 1;
		  break;

      case 0x7B : /* 20 = 7B */
		  teken = 0x20;
		  break;

      case 0x7D : /* 7B..7F = 7D + t */
		  if (lees_vt_teken(teken2) != OK) return FOUT;
		  teken = teken2;
		  if (teken < 0x7B || teken > 0x7F)
		     fout = 1;
		  break;

      case 0x7E : /* 80..BF = 7E + t-40 */
		  if (lees_vt_teken(teken2) != OK) return FOUT;
		  teken = teken2 + (BYTE)0x40;
		  if (teken < 0x80 || teken > 0xBF)
		     fout = 1;
		  break;

      case 0x7F : /* C0..FF = 7F + t-80 */
		  if (lees_vt_teken(teken2) != OK) return FOUT;
		  teken = teken2 + (BYTE)0x80;
		  if (teken < 0xC0)
		     fout = 1;
		  break;

      default:    /* 21..7A = t */
		  teken = teken1;
		  if (teken < 0x21 || teken > 0x7A)
		     fout = 1;
   }

   /* Verwerk de byte in de checksum */
   cb_waarde ^= teken;

   if (fout)
      return FOUT;
   else
      return OK;
}

static int lees_string(BYTE string[]);
static int lees_string(BYTE string[])
{
   BYTE teken;
   register int i, lengte;

   /* Lees het lengte byte */
   if (lees_byte(teken) != OK) return FOUT;
   lengte = teken - 0x20;

   /* Lees de rest van de string */
   for (i = 0; i < lengte; i++)
   {
      if (lees_byte(teken) != OK) return FOUT;
      string[i] = teken;
   }
   string[lengte] = '\0';

   return OK;
}

static int lees_long(long * waarde);
static int lees_long(long * waarde)
{
   BYTE lengte;
   long faktor = 1;
   long resultaat = 0;
   register BYTE i;
   BYTE teken;

   /* Lees de lengte van het getal */
   if (lees_byte(teken) != OK) return FOUT;
   lengte = teken - (BYTE)0x20;

   /* Lees de bytes van het getal (LSB first) */
   for (i = 0; i < lengte; i++)
   {
      if (lees_byte(teken) != OK) return FOUT;
      resultaat += teken * faktor;
      faktor *= 256;
   }

   *waarde = resultaat;

   return OK;
}

static int lees_cb(void);
static int lees_cb()
{
   BYTE teken;

   /* Lees de check-byte en kijk of checksum daarna 0 is */
   if (lees_byte(teken) != OK)   return FOUT;
   if (cb_waarde != 0)
      return FOUT;

   return OK;
}

static int zoek_start(void);
static int zoek_start()
{
   BYTE teken;

   /* Zoek het START teken = ESC en '>' */
   do
   {
      /* Zoek het ESC teken */
      do
      {
	 if (lees_vt_teken(teken) != OK) return FOUT;
      } while (teken != 0x1B);
      if (lees_vt_teken(teken) != OK) return FOUT;
   } while (teken != '>');

   return OK;
}

static int verwerk_file_header(void);
static int verwerk_file_header()
{
   char pad[128];
   BYTE soort[80];
   BYTE naam[80];
   BYTE nummer;
   BYTE type;
   BYTE vervolg;
   BYTE herken;
   BYTE blok;
   long begin;
   long run;
   long omv;

   /* Lees de diverse velden uit de file-header in */
   if (lees_string(soort) != OK)     return FOUT;
   if (lees_byte(nummer) != OK)     return FOUT;
   if (next_file > 0 && next_file != (int)nummer) return FOUT;
   next_file = (int)nummer - 1;
   if (lees_string(naam) != OK)     return FOUT;
   if (lees_byte(type) != OK)     return FOUT;
   if (lees_byte(vervolg) != OK)     return FOUT;
   if (lees_byte(herken) != OK)     return FOUT;
   if (lees_byte(blok) != OK)     return FOUT;
   next_blok = blok;
   if (lees_long(&begin) != OK)     return FOUT;
   if (lees_long(&run) != OK)     return FOUT;
   if (lees_long(&omv) != OK)     return FOUT;
   file_lengte = omv;
   tot_lengte  = 0L;

   /* Toon de naam en het nummer van de file */
   vtdisld_file(naam,nummer);

   /* Maak de file aan */
   strcpy(pad,dir);
   strcat(pad,naam);
   uit = fopen(pad,"wb");
   if (uit == NULL)
      return FOUT;

   return OK;
}

static int verwerk_blok_header(int * data_lengte);
static int verwerk_blok_header(int * data_lengte)
{
   BYTE herken, bloknr, aant1, aant2;
   int aant_bytes;
   long laad_adres;

   /* Lees de diverse velden in de blockheader */
   if (lees_byte(herken) != OK)  return FOUT;
   if (lees_byte(bloknr) != OK)  return FOUT;
   if (next_blok != (int)bloknr)
      return FOUT;
   if (lees_byte(aant1) != OK)  return FOUT;
   if (lees_byte(aant2) != OK)  return FOUT;
   aant_bytes = aant1 + aant2 * 256;
   if (lees_long(&laad_adres) != OK) return FOUT;

   *data_lengte = aant_bytes;

   /* Toon het bloknummer */
   vtdisld_blok(bloknr);

   return OK;
}

static int verwerk_route_label(void);
static int verwerk_route_label()
{
   BYTE terug[255];
   BYTE herhaal[255];
   BYTE verder[255];

   /* Lees de diverse velden in het routeringslabel */
   if (lees_string(terug) != OK) return FOUT;     /* De scherm-terug code */
   if (terug[0]) strcpy(l_terug,terug);
   if (lees_string(herhaal) != OK) return FOUT;   /* De herhaal-scherm code */
   if (herhaal[0]) strcpy(l_herhaal,herhaal);
   if (lees_string(verder) != OK) return FOUT;    /* De volgend-scherm code */
   if (verder[0]) strcpy(l_verder,verder);

   return OK;
}

static int verwerk_index_label(void);
static int verwerk_index_label()
{
   BYTE soort[255];
   BYTE prog[255];

   /* Dit is informatie over waar het volgende routeringslabel op slaat */
   if (lees_string(soort) != OK) return FOUT;
   if (lees_string(prog) != OK)  return FOUT;

   /* Hierna hoort dus een routeringslabel te volgen */

   return OK;
}

static int verwerk_header(BYTE type);
static int verwerk_header(BYTE type)
{
   int data_lengte;
   register int i;

   cb_waarde = type;

   /* Verwerk een specifieke header soort */
   switch(type)
   {
      case 0x46 : verwerk_file_header();
                  break;
      case 0x42 : verwerk_blok_header(&data_lengte);
                  break;
      case 0x52 : verwerk_route_label();
                  break;
      case 0x49 : verwerk_index_label();
                  break;
      default:    return FOUT;
   }

   /* Controleer de check-byte */
   if (lees_cb() != OK)   return FOUT;

   if (type == 0x42)
   {
      BYTE data[24*40];

      /* Lees de data van een data blok in */
      for (i = 0; i < data_lengte; i++)
	 if (lees_byte(data[i]) != OK) return FOUT;

      /* Controleer de check-byte */
      if (lees_cb() != OK)   return FOUT;

      /* Schrijf de data naar het bestand */
      tot_lengte += data_lengte;
      fwrite(data,data_lengte,1,uit);
      next_blok--;
   }

   return OK;
}

static int verwerk_start_stop(void);
static int verwerk_start_stop()
{
   BYTE type;

   /* Zoek het START teken */
   if (zoek_start() != OK)         return FOUT;

   /* Lees het type van het blok in */
   if (lees_vt_teken(type) != OK) return FOUT;

   while (type != 0x1b)          /* Totdat STOP wordt ontdekt */
   {
      /* Verwerk het blok */
      if (verwerk_header(type) != OK)  return FOUT;

      /* Lees het type van het blok in */
      if (lees_vt_teken(type) != OK)  return FOUT;
   }

   return OK;
}

static int laad_blok(void);
static int laad_blok()
{
   register int fouten = 0, toets;

   /* Kijk of er ondertussen op de ESC toets is gedrukt */
   do {
      toets = toets_lees();
      if (toets == TOETS_ESC) return FOUT;
   } while (toets);

   while (verwerk_start_stop() != OK)
   {
      /* Kijk of er ondertussen op de ESC toets is gedrukt */
      do {
	 toets = toets_lees();
	 if (toets == TOETS_ESC) return FOUT;
      } while (toets);

      /* Vraag herhaling van het scherm */
      if (!Port->Send(l_herhaal)) return FOUT;

      /* Laat het aantal fouten op het scherm zien */
      fouten++;
      tot_fouten++;
      vtdisld_fout(tot_fouten);

      /* Na 4 fouten op dezelfde pagina => afbreken */
      if (fouten > 4)
	 return FOUT;
   }

   /* Vraag de volgende beeldpagina op */
   if (!Port->Send(l_verder)) return FOUT;

   return OK;
}

static int laad_file(void);
static int laad_file()
{
   register int retcd;

   /* Lees alle beeldpagina's behorende bij 1 file in */
   next_blok = -1;
   do
   {
      /* Verwerk een beeldpagina (START blok STOP) */
      retcd = laad_blok();
   } while( next_blok != 0 && retcd == OK);

   if (uit != NULL)
   {
      /* Einde file */
      fclose(uit);
      uit = NULL;
   }

   return retcd;
}

int vtlaad(CommPort &P,const char * tele_dir)
{
   register int retcd;

   /* Plak een \ achter de teledirectory naam */
   strcpy(dir,tele_dir);
   if (dir[0] != '\0')
      strcat(dir,"\\");

   Port = &P;

   /* Toon het telesoftware scherm */
   vtdisld_start();

   /* Vraag de huidige pagina nogmaals op */
   if (!Port->Send("*00")) return FOUT;


   next_file = -1;
   tot_fouten = 0;

   do
   {
      /* Laad 1 file in */
      retcd = laad_file();
   } while (next_file != 0 && retcd == OK);

   /* Sluit de verwerking af */
   if (retcd != OK)
      Port->Send("*0_");
   vtdisld_einde();

   return retcd;
}
