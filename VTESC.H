/* ------------------------------------------------------------------------- */
/* VTESC      : Afhandelen speciale escape code scherm                       */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* VTESC_CODE                                                                */
/*                                                                           */
/* Taak       : Afhandelen speciale esc code ingave                          */
/*                                                                           */
/* In         : -                                                            */
/*                                                                           */
/* Uit        : -                                                            */
/*                                                                           */
/* Returncode : "" of videotex code voor gekozen functie                     */
/*                                                                           */
/* Begin      : gebruiker vraagt om speciale code (ESC)                      */
/*                                                                           */
/* Einde      : subkeuze gevraagd en code vertaald naar vidotex commando's   */
/* ------------------------------------------------------------------------- */

char * vtesc_code(void);
