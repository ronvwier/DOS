#define DMAKTIE_KEUZE 1
#define DMAKTIE_NIETS 0

typedef char LOGO[20][81];
#define LOGO_HOOGTE    20

int dmaktie(LOGO logo);
char * dmaktie_exec_parm(void);
char * dmaktie_exec_prog(void);
char * dmaktie_data_path(void);
int    dmaktie_exec_wacht(void);
