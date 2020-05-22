#ifndef __MSDOS__
struct  find_t {
    long            reserved;
    long            size;              /* size of file */
    unsigned long   attrib;            /* attribute byte for matched file */
    unsigned short  wr_time;           /* time of last write to file */
    unsigned short  wr_date;           /* date of last write to file */
    char            name[256];         /* asciiz name of matched file */
};
#endif


class DOSFIND
{
    public:
    DOSFIND(char filenames[]);
    search(void);
    const char * name(void);
    is_file(void);
    is_subdir(void);
    is_hidden(void);
    is_read_only(void);
    is_system(void);
    is_archived(void);
    last_written(short &year, short &month, short &day, short &hour,
		 short &minute, short &second);

    private:
    short  first;
    struct find_t entry;
    char search_name[255];
} ;
