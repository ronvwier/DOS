#include "videotex.h"

#define TITEL " VT 2.3b  VideoTex "

#define KLEUR_TITEL        kleur_titel_accent()
#define KLEUR_PFK          kleur_pf()
#define KLEUR_TEKST        kleur_body_tekst_normaal()

#define VT_OK     0
#define VT_NOT_OK 1
#define VT_FOUT  -1

int vtdis_init(int mode);
int vtdis_init_replay(int mode);
int vtdis_toon(struct VIDEOTEX_MUT * mutatie);
int vtdis_print(FILE * print);
int vtdis_einde(void);
int vtdis_wacht_start(void);
int vtdis_wacht_einde(void);

int vtlaad(char teledir[]);
