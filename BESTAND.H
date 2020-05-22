/* ---------------------------------------------------------------------- */
/* BESTAND: gebufferd lezen van bestand                                   */
/* ---------------------------------------------------------------------- */

#define BESTAND_MISLUKT   1
#define BESTAND_GELUKT    2
#define BESTAND_EINDE     3

int           bestand_open(char naam[]);
int           bestand_set_stdin(void);
char *        bestand_naam(void);
int           bestand_sluit(void);
int           bestand_lees_regel(char regel[]);
int           bestand_lees_filter(char regel[],unsigned long * ptr_bytes_skipped);
int           bestand_lees_bytes(char regel[],int lengte);
unsigned long bestand_get_positie(void);
void          bestand_set_positie(unsigned long positie);
