#define DIAPULSE_OK    0
#define DIAPULSE_FOUT -1

int diapulse_init(int comm_poort);

int diapulse_vooruit(void);
int diapulse_achteruit(void);

int diapulse_einde(void);
