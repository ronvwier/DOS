#include "kleur.h"

#define DIALOOG_TOON    0
#define DIALOOG_LEES    1

#define DIALOOG_MENU_GRP   20
#define DIALOOG_MENU_SUB   20

typedef char * DIALOOG_MENU[DIALOOG_MENU_GRP+2][DIALOOG_MENU_SUB+2];

#define SCHERM_SAA_KEUZE_AAN    kleur_aktiebalk_cursor()
#define SCHERM_SAA_KEUZE_AKTIEF kleur_aktiebalk_accent()
#define SCHERM_SAA_KEUZE_UIT    kleur_aktiebalk_keuze()

#define DIALOOG_WINDOW          kleur_body_tekst_normaal()
#define DIALOOG_WINDOW_DATA     kleur_body_veld_normaal()

int  dialoog_lees_veld(int regel,int kol,int lengte,int kleur,char veld[]);
void dialoog_toon_menu(int regel,int kol_start,int kol_eind,DIALOOG_MENU menu);
int  dialoog_kies_menu(int regel,int kol_start,int kol_eind,DIALOOG_MENU menu,int * grp_keuze,int * sub_keuze);
int  dialoog_vertikaal_menu(int regel,int kol,int max_reg,char * keuze[],int * keuzenr);
int  dialoog_vertikaal_sel(int aktie,int regel,int kol,int breedte,int hoogte,char * keuze[],int * keuzenr);

int  dialoog_window(char vraag[],char * antwoord[]);
int  dialoog_vraag(char vraag[],char antwoord[],int antwoord_lengte);
void dialoog_button_toon(int regel,int kol,int waarde,char * antwoord[]);
int  dialoog_button_vraag(int regel,int kol,int * waarde,char * antwoord[]);
int  dialoog_help(char tekst[]);

int  dialoog_popup_start(int regel, int kol,int breedte, int hoogte);
int  dialoog_popup_einde(void);

int  dialoog_popup_menu(char * keuze[],int * keuzenr);
