#define VIDEOTEX_OK      0
#define VIDEOTEX_FOUT   -1

struct VIDEOTEX_MUT
{
   int  telesoftware;     /* 0=Nee, 1=Ja */
   int  regel;            /* 0..24 */
   int  kollom;           /* 0..39 */
   int  lengte;           /* -1=Gehele scherm, 0=Geen */
   int  cursor_regel;     /* -1 is uit, of 0..24 */
   int  cursor_kollom;    /* 0..39 */
   int  aantal_kol;       /* 40/80 */
};

int  videotex_cls(void);
int  videotex_teken(unsigned char teken,struct VIDEOTEX_MUT * uit);
int  videotex_regel(int regel,unsigned char tekst[40],unsigned char kleur[40]);
int  videotex_lange_regel(int regel,unsigned char tekst[80],unsigned char kleur[80]);
unsigned
char videotex_vertaal_teken(int toets);
int  videotex_is_cls(unsigned char teken);

char * videotex_cmd_cls(void);
char * videotex_cmd_del_char(void);
char * videotex_cmd_ins_char(void);
char * videotex_cmd_del_line(void);
char * videotex_cmd_ins_line(void);
char * videotex_cmd_annuleer(void);
char * videotex_cmd_einde(void);
char * videotex_cmd_einde_sub(void);
char * videotex_cmd_hoofdmenu(void);
char * videotex_cmd_top_scherm(void);
char * videotex_cmd_eind_scherm(void);
char * videotex_cmd_terug(void);
char * videotex_cmd_verder(void);
char * videotex_cmd_return(void);
char * videotex_cmd_ververs(void);
char * videotex_cmd_naar_boven(void);
char * videotex_cmd_naar_beneden(void);
char * videotex_cmd_naar_links(void);
char * videotex_cmd_naar_rechts(void);
char * videotex_cmd_tab(void);
char * videotex_cmd_tab_terug(void);
char * videotex_cmd_help(void);
