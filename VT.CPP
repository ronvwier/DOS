/* ------------------------------------------------------------------------- */
/* VT         : Videotex communicatie pakket                                 */
/*                                                                           */
/* (c) 1992   : R.A. van Wier, Amsterdam.                                    */
/*              Verspreiding van deze source naar derden is niet toegestaan. */
/*              Voor verspreiding van programmatuur die gebruik maakt van    */
/*              deze source zijn geen licentierechten verschuldigd.          */
/* ------------------------------------------------------------------------- */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <time.h>
#include <string.h>
#include <iostream.h>

#include "toets.h"
#include "scherm.h"
#include "cua.h"
#include "dialoog.h"
#include "geluid.h"
#include "patroon.h"
#include "wekker.h"
#include "taak.h"
#ifndef NO_ESC_MENU
   #include "vtesc.h"
#endif
#ifdef __MSDOS__
   #include "commhw.h"
#else
   #include "commdos.h"
#endif
#include "modem.h"
#ifdef ONLY_HWCOMM
   #include "hwcomm.h"
#endif
#include "vt.h"

#ifndef LIGHT
   #include "vtlaad.h"
#endif

#define MAX_POORT 9

typedef char BYTE;

static struct VIDEOTEX_MUT mut;

#ifndef NO_DOWNLOAD
   static char tele_dir[80] = "";
#endif
#ifndef NO_MACRO
   static unsigned int  macro_code[32];
   static char macro_tekst[32][32];
   static int  macro_aantal = 0;
#endif
static FILE *print = NULL;

static int blijf_aktief = 0;

static void help(void);
static void help()
{
   printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
   printf("³%-70.70s      ³\n",TITEL);
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
#ifdef LIGHT
   printf("³ Aanroep: VTL compoort snelheid Toon/Pulse telefoonnr {opties}              ³\n");
#else
   printf("³ Aanroep: VT compoort snelheid Toon/Pulse telefoonnr {opties}               ³\n");
#endif
   printf("³                                                                            ³\n");
   printf("³ Opties : /S0       Scherm met 80 posities, waarvan 40 in gebruik           ³\n");
   printf("³          /S1       Scherm met 40 posities                                  ³\n");
   printf("³          /S2       Scherm met 80 posities, met alleen brede figuren        ³\n");
   printf("³          /V        Ververs scherm automatisch meteen na starten            ³\n");
#ifdef __MSDOS__
   printf("³          /Ix       gebruik Interrupt x voor COM poort                      ³\n");
#endif
   printf("³          /Pxx      Printbestand xx aanmaken                                ³\n");
   printf("³          /Lxx      Logbestand xx aanmaken voor VTPLAY                      ³\n");
#ifndef NO_MACRO
   printf("³          /Mxx      Macro file xx gebruiken                                 ³\n");
#endif
   printf("³          /Att      bestand tt gebruiken voor Automatische verwerking       ³\n");
#ifndef NO_DOWNLOAD
   printf("³          /Dzz      Directory zz gebruiken voor opslag telesoftware         ³\n");
#endif
#ifndef ONLY_HWCOMM
  #ifdef __MSDOS__
   printf("³          /CB       Communicatie alleen via Bios/int14 interface            ³\n");
   printf("³          /CF       Communicatie via Fossil interface                       ³\n");
   printf("³          /CH       Communicatie rechtstreeks via Hardware aansturing       ³\n");
 #endif
#endif
   printf("³          /GC       Geen CARRIER signaal gebruiken voor controle verbinding ³\n");
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
   printf("³ (c) 1992-1999 Amsterdam, R.A. van Wier. Alle rechten voorbehouden.         ³\n");
#ifdef LICENTIE
   printf("³ Licentie verleend aan %-45.45s        ³\n",LICENTIE);
#else
   printf("³ Gebruik voor niet commercieele doeleinden is kosteloos toegestaan.         ³\n");
#endif
   printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
}

#ifndef NO_MACRO
static int  verwerk_macro_file(char naam[]);
static int  verwerk_macro_file(char naam[])
{
   FILE *macro;
   unsigned int code;
   char regel[40];
   register int i;

   macro = fopen(naam,"r");
   if (macro == NULL)
      return 0;
   else
   {
      while ( fgets(regel,32,macro) != NULL )
      {
	 /* Verwijder New-line */
	 regel[strlen(regel) - 1] = '\0';
	 for (i = 1; regel[i] != '=' && regel[i] ; i++);
	 if (regel[i] == '=')
	 {
	    regel[i] = '\0';
	    code = toets_tekst_naar_code(regel);
	    if (code == 0)
	    {
	       printf("Macro file %s toets '%s' onbekend\n",naam,regel);
	       exit(1);
	    }
	    /* Bewaar de macro */
	    macro_code[macro_aantal] = code;
	    strcpy(macro_tekst[macro_aantal],&regel[i+1]);
	    macro_aantal++;
	 }
	 else
	 {
	    printf("Macro file %s regel '%s' foutief\n",naam,regel);
	    exit(1);
	 }
      }
      fclose(macro);
   }

   return 1;
}
#endif


static void verzend(char tekst[],CommPort &P);
static void verzend(char tekst[],CommPort &P)
{
   register int i;

   for (i = 0; tekst[i]; i++)
      P.Send(videotex_vertaal_teken(tekst[i]));
}


class Automaat
{
   public:
   Automaat()
   {
      aktief = 0;   
      aantal = 0;
      aantal_patronen = 0;
      curreg = -1;
   }
   void Open(char naam[])
   {
      FILE *automaat;
      char hulp[250]; 

      automaat = fopen(naam,"r");
      if (automaat == NULL)
      {
         printf("Automaat file %s niet gevonden\n",naam);
         exit(1);
      }

      while (fgets(hulp,sizeof(hulp),automaat) != NULL)
      {
         /* Haal newline teken weg */
         hulp[strlen(hulp) - 1] = '\0';
         regel[aantal] = new char[strlen(hulp)+1];
         strcpy(regel[aantal],hulp);
         aantal++;
      }
      fclose(automaat);
   }
   void Volgende()
   {
      int i,j,chars; 
      char verwerk[250]; 
      char rest[250];

      /* Sluit alle nog lopende patronen af */
      for (i = 0; i < aantal_patronen; i++)
          patroon_einde(patroon_nr[i]);
      aantal_patronen = 0;

      /* Ga naar volgende regel */
      curreg++;
      if (curreg >= aantal)  
         aktief = 0;
      wacht[0][0] = '\0';

      while ( aktief && aantal_patronen < 1)
      {
         /* Kijk of er een ^ in de regel voorkomt */
         for (i = 0; i < strlen(regel[curreg]) && regel[curreg][i] != '^'; i++);
         if (regel[curreg][i] == '^')
         {
            strcpy(rest,regel[curreg]);  
            do {
               strcpy(verwerk,rest);
               for (i = 0; i < strlen(rest) && rest[i] != '|'; i++);
               if (rest[i] == '|')
               { 
                  verwerk[i] = '\0';
                  chars = strlen(rest) - i - 1;
                  for (j = 0; j <= chars; j++)
                     rest[j] = rest[i+j+1];
               }
               else
               {
                  rest[0] = '\0';
               } 

               wacht[aantal_patronen][0] = '\0';
               for (i = 0; i < strlen(verwerk) && verwerk[i] != '^'; i++)
               {
                  wacht[aantal_patronen][i] = verwerk[i];
                  wacht[aantal_patronen][i+1] = '\0';
               }
               zend[aantal_patronen][0] = '\0';
               if (verwerk[i] == '^')
               {
                  for (j = i + 1; verwerk[j] ; j++)
                  {
                     zend[aantal_patronen][j-i-1] = verwerk[j];
                     zend[aantal_patronen][j-i]   = '\0';
                  }
               }
               if (wacht[aantal_patronen][0] != '\0')
               {      
                  patroon_nr[aantal_patronen] = patroon_tekst(wacht[aantal_patronen]);
                  aantal_patronen++;
               }

            } while( rest[0] != '\0');

            if (wacht[0][0] == '\0')
            { 
               /* Niet wachten maar direkt zenden */
               Verzend(zend[0]);

               /* Sluit alle patronen af */
               for (i = 0; i < aantal_patronen; i++)
               patroon_einde(patroon_nr[i]);
               aantal_patronen = 0;

               /* Op naar de volgende regel */
               curreg++;
               if (curreg >= aantal)  
                  aktief = 0;
            }
         }
         else
         {
            /* commentaar of label regel */  
            curreg++;
            if (curreg >= aantal)  
               aktief = 0;
         }
      }
   }
   void Start(Modem &MP,CommPort &PP)
   {
      M = &MP;  
      P = &PP;
      aktief = 1;

      Volgende();
   } 
   void Verwerk(char teken)
   {
      int i; 
      if (aktief)
      {
         if (patroon_teken(teken))
         {
            /* De gezochte tekst is ontvangen, verstuur het antwoord */
            for (i = 0; !patroon_gevonden(patroon_nr[i]) && i < aantal_patronen; i++);  
            Verzend(zend[i]);

            /* Lees de volgende zoek tekst in */
            Volgende();
         }
      }
   }
   void Verzend(char tekst[])
   {
      char * ptr_hulp;
      register int i;

      for (i = 0; tekst[i] == '^'; i++);
      ptr_hulp = &tekst[i];

      if (i > 0)
      {
         /* Wacht I seconden */
         vtdis_wacht_start();
         wekker_wacht(i*1000L);
         vtdis_wacht_einde();
      }

      if (ptr_hulp[0] == '~') 
      { 
         if (strcmp(ptr_hulp,"~") == 0)
         {
            /* Hangup */
            M->HangUp();
            blijf_aktief = 0;
            curreg = aantal;
         }
         else
         { 
            char label[80];

            strcpy(label,&ptr_hulp[1]);
            strcat(label,":"); 

            /* Sprongopdracht */
            for (curreg = 0;
                 curreg < aantal && strcmp(label,regel[curreg]) != 0;
                 curreg++);
             
         }
      }
      else
         if (ptr_hulp[0])
            verzend(ptr_hulp,*P);
   }

   private:

   int aktief;
   int aantal;
   PATROON_NR patroon_nr[8];
   int aantal_patronen;
   char wacht[8][80];
   char zend[8][80]; 
   Modem *M;
   CommPort *P;
   char * regel[250];
   int curreg; 
};

static void verwerk_toets(unsigned int toets,Modem &M,CommPort &P);
static void verwerk_toets(unsigned int toets,Modem &M,CommPort &P)
{
   static char * einde_menu[] =
    { "1. Invoerscherm einde      (esc j)",
      "2. Annuleren invoerscherm    (*0#)",
      "3. Hoofdmenu oproepen        (*6#)",
      "4. Subdienst verlaten        (*9#)",
      "5. Dienst verlaten          (*90#)",
      "6. Verbinding verbreken",
       NULL };
   int keuze ;
   unsigned int ktoets;
   register int i;
   GELUID geluid;

#ifndef NO_MACRO
   for ( i = 0; i < macro_aantal && toets != macro_code[i] ; i++);
   if (i < macro_aantal)
      verzend(macro_tekst[i],P);
   else
#endif
   {
      switch(toets)
      {
#ifdef NO_ESC_MENU
	 case TOETS_ESC: P.Send(videotex_cmd_annuleer());  break;
#else
         case TOETS_ESC:   P.Send(vtesc_code());                break;
#endif
         case TOETS_ALT_F4:
         case TOETS_F3:
                       keuze = 0;
                       ktoets = dialoog_popup_menu(einde_menu,&keuze);
                       if (ktoets == CUA_TABULEREN || ktoets == CUA_ENTER)
                       {
                          switch(keuze)
			  {
                             case 0: P.Send(videotex_cmd_eind_scherm());  break;
                             case 1: P.Send(videotex_cmd_annuleer());  break;
                             case 2: P.Send(videotex_cmd_hoofdmenu());  break;
                             case 3: P.Send(videotex_cmd_einde_sub());  break;
                             case 4: P.Send(videotex_cmd_einde());  break;
                             case 5: M.HangUp();
                                     blijf_aktief = 0;
                                     break;
                             default: geluid.beep();
                          }
                       }
                       else
                         if (ktoets != CUA_AFBREKEN)
                            geluid.beep();
                       break;
         case TOETS_END:   P.Send(videotex_cmd_eind_scherm());  break;
	 case TOETS_HOME:  P.Send(videotex_cmd_top_scherm());   break;
         case TOETS_F8:
         case TOETS_PGDN:  P.Send(videotex_cmd_verder());       break;
         case TOETS_F7:
         case TOETS_PGUP:  P.Send(videotex_cmd_terug());        break;
         case TOETS_CTRL_ENTER:
                           P.Send(videotex_cmd_return());       break;
         case TOETS_F5:    P.Send(videotex_cmd_ververs());      break;
         case TOETS_BACKTAB: P.Send(videotex_cmd_tab_terug());   break;
         case TOETS_UP:    P.Send(videotex_cmd_naar_boven());   break;
         case TOETS_TAB:   P.Send(videotex_cmd_tab());          break;
         case TOETS_DOWN:  P.Send(videotex_cmd_naar_beneden()); break;
         case TOETS_BACKSPACE:
                           P.Send(videotex_cmd_naar_links());
                           P.Send(videotex_cmd_del_char());
                           break;
         case TOETS_INS:   P.Send(videotex_cmd_ins_char());     break;
	 case TOETS_DEL:   P.Send(videotex_cmd_del_char());     break;
         case TOETS_ALT_INS: P.Send(videotex_cmd_ins_line());   break;
         case TOETS_ALT_DEL: P.Send(videotex_cmd_del_line());   break;
         case TOETS_LEFT:  P.Send(videotex_cmd_naar_links());   break;
         case TOETS_RIGHT: P.Send(videotex_cmd_naar_rechts());  break;
         case TOETS_F1:    P.Send(videotex_cmd_help());         break;
         case TOETS_ENTER:
#ifndef NO_DOWNLOAD
                           if (mut.telesoftware)
                              vtlaad(P,tele_dir);
                           else
#endif
                              P.Send(videotex_vertaal_teken('#'));

                           break;
#ifndef NO_PROMPT
         case TOETS_SHIFT_F9:
                           scherm_bewaar();
                           scherm_breedte(80);
                           scherm_cls(SCHERM_VG_GRYS);
                           scherm_schrijf_tekst("Geef EXIT voor terugkeer naar VT");
			   scherm_cursor(1);
                           spawnlp(P_WAIT,getenv("COMSPEC"),getenv("COMSPEC"),NULL);
                           scherm_herstel_na_exec();
                           break;
#endif
         default:          P.Send(videotex_vertaal_teken(toets));
      }
   }
}

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*           T O T A L E   B E S T U R I N G                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int main(int argc, char * argv[])

{
   register unsigned int toets = 0;
   char teken;
   long snelheid;
   int pos_parm = 0;
   short poort;
   short dis = 0;
   short irq = 0;
   register int i;
   FILE *log   = NULL;
   char Toon_Pulse;
   char nummer[40];
   int auto_ververs = 0;
   int retcd = 0;
   int print_eerste = 1;
#ifndef ONLY_HWCOMM
   TAAK verspil;
   static char interface = ' ';
#endif
   Automaat A;

#ifndef NO_MACRO
   verwerk_macro_file("vt.mac");
#endif

   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '/')
      {
	 /* Verwerk de optie */
	 switch(toupper(argv[i][1]))
	 {
	    case 'S': dis = argv[i][2] - '0';
		      if (dis < 0 || dis > 2)
			 dis = 0;
		      break;
	    case 'I': irq = argv[i][2] - '0';
		      if (irq < 0 || irq > 9)
			 irq = 0;
		      break;
	    case 'L':
		      if (argv[i][2] != '\0')
			 log = fopen(&argv[i][2],"wb");
		      else
			 log = fopen("vt.log","wb");
		      break;
	    case 'P':
		      if (argv[i][2] != '\0')
			 print = fopen(&argv[i][2],"w");
		      else
			 print = fopen("prn","w");
		      break;
	    case 'A':
		      A.Open(&argv[i][2]);
		      break;
#ifndef ONLY_HWCOMM
   #ifdef __MSDOS__
	    case 'C':
		      switch(toupper(argv[i][2]))
		      {
			 case 'B':
			      interface  = 'B';
			      break;
			 case 'F':
			      interface = 'F';
			      break;
			 case 'H':
			      interface = 'H';
			      break;
			 default:
			      printf("Optie %s onbekend\n",argv[i]);
			      exit(1);
		      }
		      break;
   #endif
#endif
#ifndef NO_DOWNLOAD
	    case 'D':
		      strcpy(tele_dir,&argv[i][2]);
		      break;
#endif
#ifndef NO_MACRO
	    case 'M':
		      if (!verwerk_macro_file(&argv[i][2]))
		      printf("Macro file %s niet gevonden\n",&argv[i][2]);
		      exit(1);
		      break;
#endif
	    case 'G':
		      switch(argv[i][2])
		      {
			 case 'c':
			 case 'C':
			      blijf_aktief = 1;
			      break;
			 default:
			      printf("Optie %s onbekend\n",argv[i]);
			      exit(1);
		      }
		      break;
	    case 'V':
		      auto_ververs = 1;
		      break;
	    case '?':
	    case 'H': help();
		      exit(1);
		      break;
	    default:
		      printf("Onbekende optie %s\n",argv[i]);
		      exit(1);
	 }
      }
      else
      {
	 /* Verwerk de positionele parameter */
	 pos_parm++;
	 switch(pos_parm)
	 {
	    case 1:
		    if (strcmp(argv[i],"?") == 0)
		       poort = 0;
		    else
		    {
		       poort    = atoi(argv[i]);
		       if (poort < 1 || poort > MAX_POORT)
		       {
			  printf("Alleen COM1..9 worden ondersteund\n");
			  exit(1);
		       }
		    }
		    break;
	    case 2:
		    snelheid = atol(argv[i]);
		    switch(snelheid)
		    {
		       case 0:
		       case 1200:
		       case 2400:
		       case 4800:
		       case 9600:
		       case 19200:
		       case 38400:
		       case 57600:
		       case 115200:
				 break;
		       default:
			    printf("Alleen 1200/2400/4800/9600/19200/38400/57600/115200 baud mogelijk\n");
			    exit(1);
		    }
		    break;
	    case 3:
		    strupr(argv[i]);
		    if (argv[i][0] != 'T' && argv[i][0] != 'P')
		    {
		       printf("Alleen Toon/Puls kiezen wordt ondersteund\n");
		       exit(1);
		    }
		    Toon_Pulse = argv[i][0];
		    break;
	    case 4:
		    strcpy(nummer,argv[i]);
		    break;
	    default:
		    printf("Teveel informatie meegegeven\n");
		    exit(1);
		    break;

	 }
      }
   }

   if (pos_parm != 1 && pos_parm != 4)
   {
      help();
      exit(1);
   }

   if (poort < 1)
   {
      /* Zoek de poort */
      int gevonden = 0;
      i = 1;
      while (i <= MAX_POORT && !gevonden)
      {
#ifdef __MSDOS__
	 CommHW P(i);
	 if (irq > 0)
	    P.Irq(irq);
#else
	 CommDos P(i);
#endif
	 if (snelheid > 0)
	    P.Baud(snelheid);
	 if (P.IsOk())
	 {
	    Modem M(P);
	    P.Open();
	    if (M.IsOk())
	       gevonden = 1;
	    P.Close();
	 }
	 if (!gevonden)
	    i++;
      }
      if (i <= MAX_POORT)
	 poort = i;
      else
      {
	 cout << "Geen modem gevonden" << endl;
	 exit(1);
      }
   }

#ifdef __MSDOS__
   CommHW P(poort);
   if (irq > 0)
      P.Irq(irq);
#else
   CommDos P(poort);
#endif
   P.Bits(7);
   if (snelheid > 0)
      P.Baud(snelheid);

   if (!P.IsOk())
   {
      cout << "Poort " << poort << " niet bereikbaar" << endl;
      exit(1);
   }
   P.Open();
   Modem M(P);

   if (Toon_Pulse == 'T')
      M.DialTone(1);
   else
      M.DialTone(0);

   /* Kijk of de verbinding er al is */
   if (pos_parm > 1)
   {
      if (!M.IsOk())
      {
	 cout << "Modem op poort " << poort << " geeft geen antwoord" << endl;
	 exit(1);
      }
   }
   else if (!M.Connection())
	{
	   printf("Geen verbinding aanwezig\n");
	   exit(1);
	}

   scherm_init();
   vtdis_init(dis);

   if (pos_parm > 1)
   {
      /* Kijk of er een verbinding op te bouwen valt */
      if (!M.Dial(nummer))
	  retcd = 1;
   }

   if (M.Connection() || blijf_aktief)
   {
      A.Start(M,P);
   }

   videotex_cls();

   if ( (M.Connection() || blijf_aktief) && auto_ververs)
      /* Vraag het scherm nogmaals op */
      P.Send((const char *)videotex_cmd_ververs());

   while ( M.Connection() || blijf_aktief)
   {
      while (P.Receive(teken))
      {
	 /* Verwerk het ingelezen teken */
	 if (log != NULL)
	    fputc(teken,log);

	 if (print != NULL && videotex_is_cls(teken))
	 {
	    if (print_eerste)
	       print_eerste = 0;
	    else
	       vtdis_print(print);
	 }

	 /* Toon het gewijzigde scherm */
	 videotex_teken(teken,&mut);
	 vtdis_toon(&mut);

	 A.Verwerk(teken);

      }

      /* Verwerk een eventuele toets */
      toets = toets_lees();
      if (toets != 0)
	 verwerk_toets(toets,M,P);
   }

   if (retcd == 0)
   {
      /* Druk het laatste scherm af */
      if (print != NULL)
	 vtdis_print(print);
   }


   /* Sluit de logging af */
   if (log != NULL)
      fclose(log);
   if (print != NULL)
      fclose(print);

   /* Sluit alles af */
   P.Close();
   vtdis_einde();
   scherm_einde();

   return retcd;
}

#ifdef ONLY_HWCOMM
CommHW::CommHW(short PortNr) : CommPort(PortNr)  { IrqNr = 0; }
CommHW::~CommHW()  { }
CommHW::Open()
{
   /* Stel modem in */
   if (hwcomm_init(PortNr,IrqNr) == 0)
      return 0;
   if (BPS > 0)
   {
	   hwcomm_set_baud(BPS);
	   hwcomm_set_N81();
   }
   return 1;
}
CommHW::Close()  {   hwcomm_einde();   return 1; }
CommHW::IsOk() const {    if (hwcomm_is_poort(PortNr)) return 1; return 0;}
CommHW::Type_UART()
{
   int UART_type = COMMHW_UART_ONBEKEND;

   if (IsOk())
      UART_type = hwcomm_type_uart(PortNr);

   return UART_type;
}
CommHW::DTR(short On)
{
   /* DTR afzetten */
   if (!On)
   {
      hwcomm_lower_dtr();
   }
   return 1;
}
CommHW::Carrier() const
{
   if (hwcomm_carrier())
      return 1;

   return 0;
}
CommHW::Send(const char *tekst) {     return CommPort::Send(tekst); }
CommHW::Send(const char teken)
{
   if (hwcomm_zend(teken))
      return 1;

   return 0;
}

CommHW::Receive(char & Teken)
{
   if (IsData())
   {
      Teken = hwcomm_lees_teken();

      if (NrBits == 7)
      {
	 /* Strip het hoogste bitje */
	 Teken = (unsigned char)(Teken & 0x7F);
      }

      return 1;
   }

   return 0;
}
CommHW::IsData() const
{
   if (hwcomm_teken_aanwezig())
       return 1;
   return 0;
}
CommHW::Irq(short P_IrqNr)       { IrqNr = P_IrqNr;    return 1; }
CommHW::SetInterface(short type) { Interface = type;   return 1; }
#endif