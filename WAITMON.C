/* ------------------------------------------------------------------------- */
/* WAITMON                                                                   */
/*                                                                           */
/* Taak       : Laat op het scherm zien hoeveel procent van onze tijd op de  */
/*              PC zitten te wachten, handig om je baas ervan te overtuigen  */
/*              dat je nu echt een snelle PC nodig hebt.                     */
/*                                                                           */
/* (c) 1990   : R.A. van Wier RI, Amsterdam.                                 */
/*                                                                           */
/*              Gebruik en distributie van deze software voor niet-          */
/*              commercie‰le doeleinden is kosteloos toegestaan.             */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <dos.h>

#define INT_TIMER   0x1C
#define INT_DOS     0x21
#define kb_pos      70

static int gebruiker_wacht = 0;

/* We maken geen gebruik van de standaard argument routines */
void _getargv() { };
void _setenvp() { };

/* Adressen van de oude interrupt handlers */
void (interrupt  *(old_timer_handler))();
void (interrupt  *(old_dos_handler))();

/* Adres van het scherm gebied */
static  int      *scherm;

/* ---------------------------------------------------------------------- */
/* RESSTAY                                                                */
/*                                                                        */
/* Taak   : Het programma afbreken maar wel in het geheugen laten staan   */
/*          z.g.n. TSR                                                    */
/*                                                                        */
/* Begin  : -                                                             */
/*                                                                        */
/* Einde  : programma resident (achtergrond) aktief                       */
/*                                                                        */
/* ---------------------------------------------------------------------- */
void resstay()
{
   static unsigned int *ptr_int;

   /* Geef de programma environment ruimte vrij */
   FP_SEG(ptr_int) = _psp;
   FP_OFF(ptr_int) = 0;
   _dos_freemem(ptr_int[0x2c / 2]);

   /* Haal de loadsize op uit het Memory Control Block en stay resident */
   FP_SEG(ptr_int) = (_psp - 1);
   FP_OFF(ptr_int) = 3;
   _dos_keep(0, *ptr_int);
}

/* ---------------------------------------------------------------------- */
/* TIMER_HANDLER                                                          */
/*                                                                        */
/* Taak   : Laat eens per seconde de wachtstatus en het percentage op     */
/*          het scherm zien.                                              */
/*                                                                        */
/* Begin  : Timer-tick verstreken                                         */
/*                                                                        */
/* Einde  : status op scherm indien ongeveer een seconde verstreken       */
/*                                                                        */
/* ---------------------------------------------------------------------- */
void interrupt timer_handler(void)
{
    static int tikken = 0;
    static int gewacht = 0;
    static int totaal = 0;
    static int procent = 0;
    static char hulp[16];
    static int i;

    /* Tel het aantal tikken */
    tikken++;

    if (tikken >= 18)                    /* Ongeveer 18 tikken per seconde */
    {
       /* Verhoog gewacht aantal secondes indien er een DOS functie is
          uitgevoerd in de afgelopen seconde */
       if (gebruiker_wacht != 0)
          gewacht++;

       /* Laat zien hoeveel procent van de tijd er al gewacht is */
       totaal++;
       procent = 100 * gewacht / totaal;
       itoa((100 + procent),hulp,10);
       if (gebruiker_wacht != 0)
          strcat(hulp,"% WAIT");
       else
          strcat(hulp,"% wait");
       for ( i = 1; hulp[i]; i++ )
          scherm[kb_pos+i] = 0x0F00 | hulp[i];

       /* Zet de tellers weer op 0 */
       gebruiker_wacht = 0;
       tikken = 0;
    }

    /* Ga naar de oude timer interrupt handler */
    _chain_intr(old_timer_handler);
}

/* ---------------------------------------------------------------------- */
/* DOS_HANDLER                                                            */
/*                                                                        */
/* Taak   : Zet gebruiker_wacht aan bij een aanroep naar DOS via INT21    */
/*                                                                        */
/* Begin  : DOS is aangeroepen via INT21                                  */
/*                                                                        */
/* Einde  : gebruiker_wacht staat aan behalve voor speciale functies      */
/*          en DOS aangeroepen                                            */
/*                                                                        */
/* ---------------------------------------------------------------------- */
void interrupt dos_handler(es, ds, di, si, bp, sp, bx, dx, cx, ax, ip, cs, flags)
unsigned es, ds, di, si, bp, sp, bx, dx, cx, ax, ip, cs, flags;
{
    static unsigned char ah;

    /* Bepaal de DOS functie */
    ah = (unsigned char)(ax >> 8);
    if (    ah != 0x01           /* Read keyboard with echo */
         && ah != 0x06           /* Direct console I/O */
         && ah != 0x07           /* Direct console input */
         && ah != 0x08           /* Read keyboard without echo */
         && ah != 0x0A           /* Buffer keyboard input */
         && ah != 0x0B           /* Check keyboard status */
         && ah != 0x0C           /* Flush buffer, read keyboard */
         && ah != 0x2C           /* Get time */
       )
       /* Markeer dat we op DOS gewacht hebben */
       gebruiker_wacht = 1;

    /* Ga naar de echte DOS interrupt handler */
    _chain_intr(old_dos_handler);
}

/* ---------------------------------------------------------------------- */
/* MAIN                                                                   */
/*                                                                        */
/* Taak   : Activeer het programma                                        */
/*                                                                        */
/* Begin  : -                                                             */
/*                                                                        */
/* Einde  : programma resident aktief                                     */
/* ---------------------------------------------------------------------- */
void main(void);
void main()
{
    static  union REGS regs;

    /* Bepaal videoscherm adres, monochroom (7) = B000:xxxx, kleur = B800:xxxx */
    regs.h.ah = 15;
    int86(0x10, &regs, &regs);
    FP_SEG(scherm) = (regs.h.al == 7 ? 0xb000 : 0xb800);
    FP_OFF(scherm) = 0;

    /* Installeer de nieuwe timer-interrupt handler */
    old_timer_handler = _dos_getvect(INT_TIMER);
    _dos_setvect(INT_TIMER, timer_handler);

    /* Installeer de DOS-call handler */
    old_dos_handler = _dos_getvect(INT_DOS);
    _dos_setvect(INT_DOS,dos_handler);

    /* En blijf aktief */
    resstay();
}
