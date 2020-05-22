/* ------------------------------------------------------------------------- */
/* BIOSCOMM   : Interface voor datacommunicatie via BIOS/INT14               */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

int           bioscomm_is_poort(int poortnummer);

int           bioscomm_init(int poort);
int           bioscomm_einde(void);

int           bioscomm_set_baud(long snelheid, int bits);

int           bioscomm_carrier(void);

int           bioscomm_teken_aanwezig(void);
unsigned char bioscomm_lees_teken(void);

int           bioscomm_zend(unsigned char teken);
