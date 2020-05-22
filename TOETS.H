/* ------------------------------------------------------------------------- */
/* TOETS      : Toetsenbord uitlezen                                         */
/* ------------------------------------------------------------------------- */

unsigned int  toets_aanwezig(void);      /* 0=Nee, 1=Ja */
unsigned int  toets_lees(void);          /* 0=Niets, anders toets */
unsigned int  toets_wacht(void);

unsigned int  toets_tekst_naar_code(char tekst[]);
void          toets_code_naar_tekst(unsigned int code,char tekst[]);

/* ------------------------------------------------------------------------- */
/* DEFINITIE VAN SCANCODES VAN EEN AANTAL TOETSEN                            */
/* ------------------------------------------------------------------------- */

#define TOETS_ESC             0x001B
#define TOETS_DOWN            0x5000
#define TOETS_INS             0x5200
#define TOETS_DEL             0x5300
#define TOETS_UP              0x4800
#define TOETS_PGUP            0x4900
#define TOETS_PGDN            0x5100
#define TOETS_RIGHT           0x4D00
#define TOETS_LEFT            0x4B00
#define TOETS_HOME            0x4700
#define TOETS_END             0x4F00
#define TOETS_BACKSPACE       0x0008
#define TOETS_TAB             0x0009
#define TOETS_ENTER           0x000D
#define TOETS_BACKTAB         0x0F00
#define TOETS_CPGUP           0x8400
#define TOETS_CTRL_LEFT       0x7300
#define TOETS_CTRL_RIGHT      0x7400
#define TOETS_CPGDN           0x7600
#define TOETS_CTRL_DEL        0x9300
#define TOETS_CTRL_ENTER      0x000A
#define TOETS_ALT_LEFT        0x9B00
#define TOETS_ALT_RIGHT       0x9D00
#define TOETS_ALT_INS         0xA200
#define TOETS_ALT_DEL         0xA300

#define TOETS_ALT_A           0x1E00
#define TOETS_ALT_B           0x3000
#define TOETS_ALT_C           0x2E00
#define TOETS_ALT_D           0x2000
#define TOETS_ALT_E           0x1200
#define TOETS_ALT_F           0x2100
#define TOETS_ALT_G           0x2200
#define TOETS_ALT_H           0x2300
#define TOETS_ALT_I           0x1700
#define TOETS_ALT_J           0x2400
#define TOETS_ALT_K           0x2500
#define TOETS_ALT_L           0x2600
#define TOETS_ALT_M           0x3200
#define TOETS_ALT_N           0x3100
#define TOETS_ALT_O           0x1800
#define TOETS_ALT_P           0x1900
#define TOETS_ALT_Q           0x1000
#define TOETS_ALT_R           0x1300
#define TOETS_ALT_S           0x1F00
#define TOETS_ALT_T           0x1400
#define TOETS_ALT_U           0x1600
#define TOETS_ALT_V           0x2F00
#define TOETS_ALT_W           0x1100
#define TOETS_ALT_X           0x2D00
#define TOETS_ALT_Y           0x1500
#define TOETS_ALT_Z           0x2C00

#define TOETS_F1              0x3B00
#define TOETS_F2              0x3C00
#define TOETS_F3              0x3D00
#define TOETS_F4              0x3E00
#define TOETS_F5              0x3F00
#define TOETS_F6              0x4000
#define TOETS_F7              0x4100
#define TOETS_F8              0x4200
#define TOETS_F9              0x4300
#define TOETS_F10             0x4400
#define TOETS_F11             0x8500
#define TOETS_F12             0x8600

#define TOETS_SHIFT_F1        0x5400
#define TOETS_SHIFT_F2        0x5500
#define TOETS_SHIFT_F3        0x5600
#define TOETS_SHIFT_F4        0x5700
#define TOETS_SHIFT_F5        0x5800
#define TOETS_SHIFT_F6        0x5900
#define TOETS_SHIFT_F7        0x5A00
#define TOETS_SHIFT_F8        0x5B00
#define TOETS_SHIFT_F9        0x5C00
#define TOETS_SHIFT_F10       0x5D00
#define TOETS_SHIFT_F11       0x8700
#define TOETS_SHIFT_F12       0x8800

#define TOETS_CTRL_F1         0x5E00
#define TOETS_CTRL_F2         0x5F00
#define TOETS_CTRL_F3         0x6000
#define TOETS_CTRL_F4         0x6100
#define TOETS_CTRL_F5         0x6200
#define TOETS_CTRL_F6         0x6300
#define TOETS_CTRL_F7         0x6400
#define TOETS_CTRL_F8         0x6500
#define TOETS_CTRL_F9         0x6600
#define TOETS_CTRL_F10        0x6700
#define TOETS_CTRL_F11        0x8900
#define TOETS_CTRL_F12        0x8A00

#define TOETS_ALT_F1          0x6800
#define TOETS_ALT_F2          0x6900
#define TOETS_ALT_F3          0x6A00
#define TOETS_ALT_F4          0x6B00
#define TOETS_ALT_F5          0x6C00
#define TOETS_ALT_F6          0x6D00
#define TOETS_ALT_F7          0x6E00
#define TOETS_ALT_F8          0x6F00
#define TOETS_ALT_F9          0x7000
#define TOETS_ALT_F10         0x7100
#define TOETS_ALT_F11         0x8B00
#define TOETS_ALT_F12         0x8C00
