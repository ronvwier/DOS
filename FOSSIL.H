/* ------------------------------------------------------------------------- */
/* FOSSIL     : Interface voor datacommunicatie via een FOSSIL interface     */
/*                                                                           */
/* (c) 1992   : R.A. van Wier RI, Amsterdam.                                 */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#define FOSSIL_PS_RXCHARS      0x0100          /* Characters in input buffer */
#define FOSSIL_PS_RXOVER       0x0200          /* Receive buffer overrun */
#define FOSSIL_PS_MASK         0x0008          /* Bits always set */
#define FOSSIL_PS_TXCHARS      0x2000          /* Transmit buffer not full */
#define FOSSIL_PS_TXEMPTY      0x4000          /* Transmit buffer is empty */
#define FOSSIL_PS_CARRIER      0x0080          /* Carrier detect is high */

int           fossil_aktief(void);
int           fossil_init(int poort);
int           fossil_deinit(void);

unsigned int  fossil_request_status(void);
int           fossil_transmit(unsigned char teken);
unsigned char fossil_receive(void);
unsigned int  fossil_peek(void);
int           fossil_lower_dtr(void);
int           fossil_set_baud(long baud, int bits);
