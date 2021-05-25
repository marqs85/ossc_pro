#include <stdio.h>
#include "string.h"
#include "file.h"

FATFS fs;

FRESULT file_mount()
{
    return f_mount(&fs, "", 1);
}

FRESULT file_open(FIL* fil, char* path)
{
    return f_open(fil, path, FA_READ);
}

TCHAR* file_get_string(FIL* fil, char* buff, int len)
{
    return f_gets(buff, len, fil);
}

FRESULT file_close(FIL* fil)
{
    return f_close(fil);
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}
