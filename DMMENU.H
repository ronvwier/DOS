#define DMMENU_GELUKT      0
#define DMMENU_MISLUKT     -1
#define DMMENU_READ_ONLY   1
#define DMMENU_UPDATE      2

#define PARM_TYPE_TEKST     0
#define PARM_TYPE_VRAAG     1

int    dmmenu_open(char naam[]);
int    dmmenu_maak(char naam[]);
int    dmmenu_schrijf(void);
int    dmmenu_sluit(void);

int    dmmenu_aantal_groep(void);
int    dmmenu_aantal_keuze(int groep);
char * dmmenu_groep(int groep);
int    dmmenu_wacht(int groep,int keuze);
char * dmmenu_keuze(int groep,int keuze);
char * dmmenu_dir(int groep,int keuze);
char * dmmenu_prog(int groep,int keuze);
char * dmmenu_vraag(int groep,int keuze,int parm);

int    dmmenu_nwe_groep(void);
int    dmmenu_weg_groep(int groep);
int    dmmenu_nwe_keuze(int groep,int pos);
int    dmmenu_weg_keuze(int groep,int keuze);

int    dmmenu_zet_groep(int groep,char titel[]);
int    dmmenu_zet_keuze(int groep,int keuze,char titel[]);
int    dmmenu_zet_dir(int groep,int keuze,char dir[]);
int    dmmenu_zet_prog(int groep,int keuze,char prog[]);
int    dmmenu_zet_wacht(int groep,int keuze,int wacht);
int    dmmenu_zet_vraag(int groep,int keuze,int parm,char tekst[]);
