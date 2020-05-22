#define MAXREG    16380   /* maximaal aantal regels */
#define REGLEN      255   /* maximale regellengte */

/* ---------------------------------------------------------------------- */
/* Object : Lees                                                          */
/* ---------------------------------------------------------------------- */

/* export */

#define ZIELEES_MISLUKT   1
#define ZIELEES_GELUKT    2

#define ZIELEES_MODE_TXT    1
#define ZIELEES_MODE_HEX    2
#define ZIELEES_MODE_FILTER 3

int           zielees_open(char naam[]);
int           zielees_set_stdin(void);
char *        zielees_naam(void);
int           zielees_sluit(void);

char *        zielees_regel(int regelnummer);
void          zielees_mode_set(int new_mode);
int           zielees_mode_get(void);
int           zielees_maxreg(void);
unsigned long zielees_offset(int regelnummer);
