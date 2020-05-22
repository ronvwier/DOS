#include "kleur.h"

#define TITEL  "Dos Menu"
#define VERSIE "3.5a"
#define RECHT  "(c) 1992 RvW"

#define KLEUR_TEKST   kleur_body_tekst_normaal()
#define KLEUR_DATA    kleur_body_veld_normaal()
#define KLEUR_PF      kleur_pf()

#define KLEUR_MENU    kleur_titel_normaal()
#define KLEUR_TITEL   kleur_titel_accent()
#define KLEUR_RECHT   kleur_titel_normaal()

#define KLEUR_LOGO    kleur_body_tekst_normaal()

#define SIZE_TITEL    50
#define SIZE_PROG     50
#define SIZE_DIR      50
#define SIZE_PARM     50

#ifdef __OS2__
   int dmdlg(void * ptr_save_area,char * menu_arg, char * logo_arg);
#endif

