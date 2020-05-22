/* ------------------------------------------------------------------------- */
/* HWCOMM     : Interface voor datacommunicatie via hardware RS232 poort     */
/*                                                                           */
/* (c) 1991   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#define HWCOMM_UART_ONBEKEND   0
#define HWCOMM_UART_8250       1
#define HWCOMM_UART_16450      2
#define HWCOMM_UART_16550      3
#define HWCOMM_UART_16550A     4


int           hwcomm_is_poort(int poortnummer);
int           hwcomm_type_uart(int poortnummer);

int           hwcomm_init(int poort, int afwijkende_irq);
int           hwcomm_einde(void);

int           hwcomm_set_baud(long snelheid);
int           hwcomm_set_E71(void);
int           hwcomm_set_N81(void);

int           hwcomm_carrier(void);
int           hwcomm_lower_dtr(void);

int           hwcomm_teken_aanwezig(void);
unsigned char hwcomm_lees_teken(void);

int           hwcomm_zend(unsigned char teken);
