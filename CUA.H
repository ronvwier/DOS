#define CUA_TABULEREN             TOETS_TAB
#define CUA_TAB_TERUG             TOETS_BACKTAB
#define CUA_AFBREKEN              TOETS_ESC
#define CUA_WISSEN                TOETS_DEL
#define CUA_WIS_VELD              TOETS_CTRL_DEL
#define CUA_SPATIE_TERUG          TOETS_BACKSPACE
#define CUA_INVOEGEN              TOETS_INS
#define CUA_NAAR_RECHTS           TOETS_RIGHT
#define CUA_NAAR_LINKS            TOETS_LEFT
#define CUA_NAAR_BENEDEN          TOETS_DOWN
#define CUA_NAAR_BOVEN            TOETS_UP
#define CUA_ENTER                 TOETS_ENTER
#define CUA_AKTIE                 TOETS_F10
#define CUA_HELP                  TOETS_F1
#define CUA_EINDE                 TOETS_F3
#define CUA_TERUG_1               TOETS_F7
#define CUA_TERUG_2               TOETS_PGUP
#define CUA_VERDER_1              TOETS_F8
#define CUA_VERDER_2              TOETS_PGDN
#define CUA_WOORD_RECHTS          TOETS_ALT_RIGHT
#define CUA_WOORD_LINKS           TOETS_ALT_LEFT
#define CUA_VELD_EINDE            TOETS_END
#define CUA_VELD_BEGIN            TOETS_HOME
#define CUA_MUIS_LINKS            1
#define CUA_MUIS_RECHTS           2

#define cua_afsluiten(_toets)     (_toets == TOETS_F3 || _toets == TOETS_ALT_F4)
#define cua_opdracht(_toets)      (_toets == TOETS_SHIFT_F9)
#define cua_object_menu(_toets)   (_toets == TOETS_SHIFT_F10)
#define cua_tabuleren(_toets)     (_toets == TOETS_TAB)
#define cua_tab_terug(_toets)     (_toets == TOETS_BACKTAB)
#define cua_annuleren(_toets)     (_toets == TOETS_ESC)
#define cua_wissen(_toets)        (_toets == TOETS_DEL)
#define cua_wis_veld(_toets)      (_toets == TOETS_CTRL_DEL)
#define cua_spatie_terug(_toets)  (_toets == TOETS_BACKSPACE)
#define cua_invoegen(_toets)      (_toets == TOETS_INS)
#define cua_rechts(_toets)        (_toets == TOETS_RIGHT)
#define cua_links(_toets)         (_toets == TOETS_LEFT)
#define cua_naar_beneden(_toets)  (_toets == TOETS_DOWN
#define cua_naar_boven(_toets)    (_toets == TOETS_UP)
#define cua_enter(_toets)         (_toets == TOETS_ENTER)
#define cua_actiebalk(_toets)     (_toets == TOETS_F10)
#define cua_help(_toets)          (_toets == TOETS_F1)
#define cua_terug(_toets)         (_toets == TOETS_F7 || _toets == TOETS_PGUP)
#define cua_verder(_toets)        (_toets == TOETS_F8 || _toets == TOETS_PGDN)
#define cua_woord_rechts(_toets)  (_toets == TOETS_CTRL_RIGHT)
#define cua_woord_links(_toets)   (_toets == TOETS_CTRL_LEFT)
#define cua_einde_regel(_toets)   (_toets == TOETS_END)
#define cua_begin_regel(_toets)   (_toets == TOETS_HOME)
#define cua_einde_veld(_toets)    (_toets == TOETS_CTRL_END)
#define cua_begin_veld(_toets)    (_toets == TOETS_CTRL_HOME)
