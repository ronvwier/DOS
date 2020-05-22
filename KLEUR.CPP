/* ------------------------------------------------------------------------- */
/* KLEUR      : Standaard kleurenschema geven voor diverse schermonderdelen  */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/* ------------------------------------------------------------------------- */

#include "scherm.h"
#include "kleur.h"

static int pallette = 0;
static int voor_help_pallette = 0;

/* ------------------------------------------------------------------------- */
/* KLEUR_WINDOW_START                                                        */
/*                                                                           */
/* Taak       : Wijzigen kleurenpallette n.a.v. tonen popup window           */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : pallette gewijzigd                                           */
/* ------------------------------------------------------------------------- */
void kleur_window_start(void)
{
   if ( pallette == 0 )
      pallette = 1;
   else
   {
     if ( pallette == 1 )
        pallette = 0;
   }
}

/* ------------------------------------------------------------------------- */
/* KLEUR_WINDOW_HELP_START                                                   */
/*                                                                           */
/* Taak       : Wijzigen kleurenpallette n.a.v. tonen popup help window      */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : pallette gewijzigd                                           */
/* ------------------------------------------------------------------------- */
void kleur_window_help_start(void)
{
   voor_help_pallette = pallette;
   pallette = 2;
}

/* ------------------------------------------------------------------------- */
/* KLEUR_WINDOW_HELP_EINDE                                                   */
/*                                                                           */
/* Taak       : Terugzetten pallette n.a.v. einde help scherm                */
/*                                                                           */
/* Begin      : help pallette aktief                                         */
/*                                                                           */
/* Einde      : normaal pallette aktief                                      */
/* ------------------------------------------------------------------------- */
void kleur_window_help_einde(void)
{
   pallette = voor_help_pallette;
}

/* ------------------------------------------------------------------------- */
/* KLEUR_WINDOW_EINDE                                                        */
/*                                                                           */
/* Taak       : Terugzetten pallette n.a.v. einde popup window               */
/*                                                                           */
/* Begin      : popup pallette aktief                                        */
/*                                                                           */
/* Einde      : normaal pallette aktief                                      */
/* ------------------------------------------------------------------------- */
void kleur_window_einde(void)
{
   if ( pallette == 0 )
      pallette = 1;
   else
   {
     if ( pallette == 1 )
        pallette = 0;
   }
}

/* ------------------------------------------------------------------------- */
/* KLEUR_xxxxxx                                                              */
/*                                                                           */
/* Taak       : retourneren van een kleur voor het betreffende schermelement */
/*                                                                           */
/* Begin      : -                                                            */
/*                                                                           */
/* Einde      : returnwaarde is kleur                                        */
/* ------------------------------------------------------------------------- */
int kleur_titel_normaal(void)
{
   if ( scherm_monochroom() != 0 )
      return (SCHERM_VG_GRYS  + SCHERM_AG_ZWART);
   else
      return (SCHERM_AG_BLAUW + SCHERM_VG_CYAAN);
}
int kleur_titel_accent(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  + SCHERM_AG_ZWART + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
      return (SCHERM_VG_GRYS  + SCHERM_AG_ZWART + SCHERM_VG_FEL);
   else
      return (SCHERM_AG_BLAUW + SCHERM_VG_GRYS + SCHERM_VG_FEL);
}

int kleur_aktiebalk_keuze(void)
{
   if ( scherm_lcd() )
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_CYAAN + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_CYAAN + SCHERM_VG_ZWART);
   }
}
int kleur_aktiebalk_mnem(void)
{
   if ( scherm_lcd() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
   }
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_CYAAN + SCHERM_VG_ROOD);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ROOD);
      if ( pallette == 2 ) return (SCHERM_AG_CYAAN + SCHERM_VG_ROOD);
   }
}
int kleur_aktiebalk_accent(void)
{
   if ( scherm_lcd() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
   }
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_GRYS + SCHERM_VG_FEL + SCHERM_AG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_CYAAN + SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 2 ) return (SCHERM_AG_CYAAN + SCHERM_VG_GEEL + SCHERM_VG_FEL);
   }
}
int kleur_aktiebalk_cursor(void)
{
   if ( scherm_lcd() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
   }
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_AG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS );
      if ( pallette == 1 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS );
      if ( pallette == 2 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS );
   }
}

int kleur_aktiebalk_cursor_mnem(void)
{
   if ( scherm_lcd() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
   }
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL + SCHERM_AG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_ROOD + SCHERM_VG_CYAAN);
      if ( pallette == 1 ) return (SCHERM_AG_ROOD + SCHERM_VG_CYAAN);
      if ( pallette == 2 ) return (SCHERM_AG_ROOD + SCHERM_VG_CYAAN);
   }
}

int kleur_body_tekst_normaal(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS );
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS );
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_BLAUW);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_BLAUW);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_CYAAN + SCHERM_VG_FEL);
   }
}
int kleur_body_tekst_accent(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_GEEL + SCHERM_VG_FEL);
   }
}

int kleur_body_keuze_uit(void)
{
   return kleur_body_veld_normaal();
}
int kleur_body_keuze_uit_mnem(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_BLAUW);
   else
   if ( scherm_monochroom() )
      return (SCHERM_VG_BLAUW);
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_ROOD);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_ROOD);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_ROOD);
   }
}

int kleur_body_keuze_aan(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
      return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
   else
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_CYAAN+ SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW+ SCHERM_AG_CYAAN);
   }
}
int kleur_body_keuze_aan_mnem(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_BLAUW );
   else
   if ( scherm_monochroom() )
      return (SCHERM_VG_BLAUW );
   else
   {
      if ( pallette == 0 ) return (SCHERM_VG_ROOD + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_ROOD + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_ROOD + SCHERM_AG_CYAAN);
   }
}

int kleur_body_veld_normaal(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_GRYS + SCHERM_VG_FEL);
   }
}
int kleur_body_veld_accent(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_GEEL + SCHERM_VG_FEL);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_GEEL + SCHERM_VG_FEL);
   }
}

int kleur_pf(void)
{
   if ( scherm_lcd() )
      return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   else
   if ( scherm_monochroom() )
      return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_ZWART);
   }
}

int kleur_selektie(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
      return (SCHERM_VG_BLAUW + SCHERM_VG_FEL);
   else
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS + SCHERM_AG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_CYAAN+ SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_BLAUW+ SCHERM_AG_CYAAN);
   }
}

int kleur_fout(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_ROOD);
      if ( pallette == 1 ) return (SCHERM_AG_CYAAN+ SCHERM_VG_ROOD);
      if ( pallette == 2 ) return (SCHERM_AG_BLAUW+ SCHERM_VG_ROOD);
   }
}
int kleur_mededeling(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  );
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS  );
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS + SCHERM_VG_ZWART);
   }
}
int kleur_waarschuwing(void)
{
   if ( scherm_lcd() )
      return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_VG_GRYS  + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_GEEL + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_AG_GEEL + SCHERM_VG_ZWART);
      if ( pallette == 2 ) return (SCHERM_AG_GEEL + SCHERM_VG_ZWART);
   }
}
int kleur_kritiek(void)
{
   if ( scherm_lcd() )
      return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
   else
   if ( scherm_monochroom() )
   {
      if ( pallette == 0 ) return (SCHERM_AG_GRYS  + SCHERM_VG_ZWART);
      if ( pallette == 1 ) return (SCHERM_VG_GRYS  + SCHERM_AG_ZWART);
      if ( pallette == 2 ) return (SCHERM_VG_GRYS  + SCHERM_AG_ZWART);
   }
   else
   {
      if ( pallette == 0 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS + SCHERM_VG_FEL);
      if ( pallette == 1 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS + SCHERM_VG_FEL);
      if ( pallette == 2 ) return (SCHERM_AG_ROOD + SCHERM_VG_GRYS + SCHERM_VG_FEL);
   }
}
