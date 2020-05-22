#define DBASE_OK      0
#define DBASE_NOT_OK  1
#define DBASE_DELETED 2
#define DBASE_EINDE   3
#define DBASE_II      12
#define DBASE_III     13

#define DBASE_FOUT   -1

int dbase_formaat(char bestand[]);

int dbase_open_lezen(char bestand[],int * best_nr);
int dbase_sluit(int best_nr);

int dbase_geef_info(int best_nr,int * mut_dag,int * mut_maand,int * mut_jaar,
               unsigned long * aantal_records);

int dbase_geef_veldinfo(int best_nr,int veldnr,char naam[],char * type,int * lengte, int * decimalen);

int dbase_lees_record(int best_nr,long record_nummer);

int dbase_geef_veld(int best_nr,int veldnr, char waarde[]);

