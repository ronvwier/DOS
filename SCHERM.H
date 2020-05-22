/* ------------------------------------------------------------------------- */
/* SCHERM     : Afhandelen scherm i/o                                        */
/* ------------------------------------------------------------------------- */

void scherm_init(void);
void scherm_einde(void);
int  scherm_monochroom(void);
int  scherm_lcd(void);
int  scherm_aantal_reg(void);
int  scherm_aantal_kol(void);
int  scherm_kleur(int regel,int kol);
void scherm_cls(int kleur);
void scherm_bewaar(void);
void scherm_herstel(void);
void scherm_herstel_na_exec(void);
void scherm_breedte(int aantal_kol);

void scherm_cursor_plaats(int regel,int kol);
void scherm_cursor_pos(int * regel,int * kol);
void scherm_cursor_kleur(int kleur);
void scherm_cursor(int cursor_aan);   /* 0=Nee, 1=Ja */

void scherm_schrijf_teken(char teken);
void scherm_schrijf_tekst(char tekst[]);

void scherm_plaats_teken(int regel,int kol,int kleur,char teken);
void scherm_plaats_tekst(int regel,int kol,int kleur,char tekst[]);

void scherm_tekst(int regel,int kol,int lengte,int kleur,char tekst[]);

void scherm_vervang_kleur(int regel,int kol,int kleur);
void scherm_vervang_teken(int regel,int kol,char teken);
void scherm_vervang_tekst(int regel,int kol,char tekst[]);

void scherm_geef_kleurenset(char kleuren[16]);
void scherm_zet_kleurenset(char kleuren[16]);

/* ------------------------------------------------------------------------- */
/* SCHERM KLEUREN (STANDAARD)                                                */
/* ------------------------------------------------------------------------- */

#define SCHERM_VG_FEL          8
#define SCHERM_VG_KNIPPER    128

#define SCHERM_VG_ZWART        0
#define SCHERM_VG_BLAUW        1
#define SCHERM_VG_GROEN        2
#define SCHERM_VG_CYAAN        3
#define SCHERM_VG_ROOD         4
#define SCHERM_VG_PAARS        5
#define SCHERM_VG_GEEL         6
#define SCHERM_VG_GRYS         7

#define SCHERM_AG_ZWART        0
#define SCHERM_AG_BLAUW       16
#define SCHERM_AG_GROEN       32
#define SCHERM_AG_CYAAN       48
#define SCHERM_AG_ROOD        64
#define SCHERM_AG_PAARS       80
#define SCHERM_AG_GEEL        96
#define SCHERM_AG_GRYS       112
