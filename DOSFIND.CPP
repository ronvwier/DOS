#ifdef __MSDOS__
   #include <dos.h>
#endif
#ifdef __OS2__
   #define INCL_DOSFILEMGR   /* File Manager values */
   #include <os2.h>
#endif
#include <string.h>
#include "dosfind.h"

DOSFIND::DOSFIND(char filenames[])
{
    strcpy(search_name,filenames);
    first = 1;
}

DOSFIND::search()
{
#ifdef __MSDOS__
   int rc;

   if (first)
      rc = _dos_findfirst(search_name,_A_SUBDIR,&entry);
   else
      rc = _dos_findnext(&entry);
#endif
#ifdef __OS2__
   HDIR          FindHandle;
   FILEFINDBUF3  FindBuffer;
   ULONG         FindCount;
   APIRET        rc;           /* Return code */
 
   FindHandle = HDIR_SYSTEM ; /*0x0001;*/
   FindCount = 1;

   if (first)
   {
      rc = DosFindFirst((PSZ)search_name,            /* File pattern */
                      &FindHandle,           /* Directory search handle */
                      FILE_DIRECTORY,        /* Search attribute */
                      (PVOID) &FindBuffer,   /* Result buffer */
                      sizeof(FindBuffer),    /* Result buffer length */
                      &FindCount,            /* Number of entries to find */
                      FIL_STANDARD);        /* Return level 1 file info */
   }
   else
   {
      rc = DosFindNext(FindHandle,         /* Directory handle */
                     (PVOID) &FindBuffer,  /* Result buffer */
                     sizeof(FindBuffer), /* Result buffer length */
                     &FindCount);        /* Number of entries to find */
   }
   if (rc == 0) 
   {
      strcpy(entry.name,FindBuffer.achName);
      entry.attrib = FindBuffer.attrFile;
   }
#endif

   first = 0;

   return (rc == 0) ? 1 : 0;
}

const char * DOSFIND::name()
{
   return entry.name;
}

DOSFIND::is_file()
{
#ifdef __MSDOS__
    return !(entry.attrib & _A_SUBDIR);
#else
    return !(entry.attrib & FILE_DIRECTORY);
#endif
}

DOSFIND::is_subdir()
{
#ifdef __MSDOS__
    return entry.attrib & _A_SUBDIR;
#else
    return entry.attrib & FILE_DIRECTORY;
#endif
}

DOSFIND::is_hidden()
{
#ifdef __MSDOS__
    return entry.attrib & _A_HIDDEN;
#else
    return 0;
#endif
}

DOSFIND::is_read_only()
{
#ifdef __MSDOS__
    return entry.attrib & _A_RDONLY;
#else
   return 0;
#endif
}

DOSFIND::is_system()
{
#ifdef __MSDOS__
    return entry.attrib & _A_SYSTEM;
#else
   return 0;
#endif

}

DOSFIND::is_archived()
{
#ifdef __MSDOS__
    return !(entry.attrib & _A_ARCH);
#else
   return 0;
#endif
}

DOSFIND::last_written(short &year, short &month, short &day, short &hour,
		      short &minute, short &second)
{
    year   = (entry.wr_date >> 9) + 1980;
    month  = (entry.wr_date >> 5) & 0x000F;
    day    = entry.wr_date & 0x001F;
    hour   = entry.wr_time >> 11;
    minute = (entry.wr_time >> 5) & 0x003F;
    second = (entry.wr_time & 0x001F) * 2;

    return 1;
}

