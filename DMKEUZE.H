#define DMKEUZE_GELUKT   0
#define DMKEUZE_MISLUKT  1

int    dmkeuze_vraag(void);
void   dmkeuze_show(void);
void   dmkeuze_leeg(void);

char * dmkeuze_titel(void);
void   dmkeuze_zet_titel(char titel[]);
char * dmkeuze_dir(void);
void   dmkeuze_zet_dir(char dir[]);
char * dmkeuze_prog(void);
void   dmkeuze_zet_prog(char prog[]);
int    dmkeuze_wacht(void);
void   dmkeuze_zet_wacht(int wacht);
char * dmkeuze_optie_tekst(int optie);
void   dmkeuze_zet_optie_tekst(int optie,char tekst[]);
