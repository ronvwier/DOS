#define DOSDIR_MAX_PAD_LEN 64+13+1+44

class DOSDIR
{
   public:
   DOSDIR(char naam[] = "" );
   fysieke_naam(char norm_dir_naam[]);
   volledige_naam(char norm_dir_naam[]);
   zoek_bestand(char bestandsnaam[]);

   private:
   char dir_naam[DOSDIR_MAX_PAD_LEN];
} ;