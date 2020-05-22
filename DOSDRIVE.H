/* ------------------------------------------------------------------------- */
/* DOSDRIVE : Operaties op DOS disk drive's en media                         */
/* ------------------------------------------------------------------------- */
class DOSDRIVE
{
    public:
    DOSDRIVE(char drive_letter);
    int is_aktief(void);
    int aktiveer(void);
    int is_aanwezig(void);
    int is_lokaal(void);
    int is_disk_leesbaar(void);
    int geef_disk_naam(char naam[]);

    private:
    char drive;
};
