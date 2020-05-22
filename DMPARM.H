#define DMPARM_GELUKT  0
#define DMPARM_MISLUKT 1

struct DMPARM_AREA
{
   char exec_parm[128];
   char exec_prog[SIZE_PROG+1];
   char data_path[SIZE_DIR+1];
   char menu_path[4][65];
   char target_dir[128];
   char default_dir[128];
   int  video_mode;
   int  menu_level;
   int  wacht;
   char logo_path[4][65];
};

int    dmparm_geheugen(void  * adres,int grootte);

int     dmparm_push_menu(void);
int     dmparm_pop_menu(void);

char  * dmparm_get_target_dir(void);
char  * dmparm_get_default_dir(void);
int     dmparm_get_video_mode(void);

char  * dmparm_get_exec_parm(void);
char  * dmparm_get_exec_prog(void);
int     dmparm_get_exec_wacht(void);
char  * dmparm_get_data_path(void);
char  * dmparm_get_menu_path(void);
char  * dmparm_get_logo_path(void);

int dmparm_set_exec_parm(char  * exec_parm);
int dmparm_set_exec_prog(char  * exec_prog);
int dmparm_set_exec_wacht(int    wacht);
int dmparm_set_data_path(char  * data_path);
int dmparm_set_menu_path(char  * menu_path);
int dmparm_set_logo_path(char  * menu_path);
int dmparm_set_target_dir(char  * old_target_drive_dir);
int dmparm_set_default_dir(char  * old_default_drive_dir);
int dmparm_set_video_mode(int mode);
