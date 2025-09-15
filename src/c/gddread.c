/*
at most 100 consecutive char
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gddread.h"

/*
以行為單位輸入
每行切成很多字
match 的話就印出整行
*/

char**
readline(FILE* fildes)
{
    char **line;
    const int t = 10;
    int c = 0x00, cnt;
    char **p, *q;
    
    line = malloc(t * sizeof(char *));
    for(p = line; c != EOF; p++){
        if( p == line+t ){
            fprintf(stderr, "A line can only contain at most %d words!\n", t);
            return NULL;
        }

        c = 0x00;
        cnt++;
        *p = malloc(t * sizeof(char));
        for(q = *p; c != ' ' && c != EOF; q++){
            if( q == *p+t ){
                fprintf(stderr, "A word can only contain at most %d chars!\n", t);
                return NULL;
            }
            c = fgetc(fildes);
            if( c == ' ' || c == EOF ) *q = 0x00;
            else *q = c;
        }
    }

    *++p = NULL;
    return line;
}

char**
gddread(int argc, char **argv)
{
    if( !isatty(STDIN_FILENO) && argc > 1 ){
        fprintf(stderr, "Error : %s doesnt support multi input stream!\n", argv[0]);
        return NULL;
    }

    FILE *fildes;
    char **line, *path;

    if( isatty(STDIN_FILENO) ){ /* argv input */
        if( argc == 1 ){
            fprintf(stderr, "need input\n");
            return NULL;
        }
        /* -> check if argv a file or string */
        int i;
        for(i = 1; i < argc; i++){
            path = argv[i];

            if( access(path, F_OK) == 0 ){ /* file exist */
                if( access(path, R_OK) == -1 ){
                    fprintf(stderr, "file isn't readable\n");
                    return NULL;
                }
                printf("read file\n");
                // OK and reads file
                // getline -> fprintf split ' '

                fildes = fopen(path, O_RDONLY);
                fclose(fildes);
            }
            else{
                // just argv string
                // check each word with no lines
                printf("argv input\n");
            }
        }
    }
    else{ /* pipe input */

        printf("STDIN :)\n");
    }

    return line;
}