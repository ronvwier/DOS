void muis_init(void);
void muis_einde(void);

void muis_aan(void);
void muis_uit(void);

int  muis_aanwezig(void);                     /* 0=Nee, 1=Ja */

int  muis_irq(void);                          /* Irq=0 of nummer */

int  muis_toets(void);                   /* 0=Nee, 1=Links, 2=Rechts, 3=Beide */
void muis_geef_positie(int *,int *);          /* Regel, Kolom */
void muis_zet_positie(int regel,int kolom);   /* Regel, Kolom */

void muis_zet_gebied(int van_reg,int van_kol,int tot_reg, int tot_kol);

#define MUIS_TOETS_LINKS   1
#define MUIS_TOETS_RECHTS  2
