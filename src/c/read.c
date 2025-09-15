/*
at most 100 consecutive char
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "read.h"

/*
以行為單位輸入
每行切成很多字
match 的話就印出整行
*/

/* return line -> words */
// char**
// void
// input(int fildes, char **words)
// {
//     char *line = NULL, *p, *w; //, words[100][100];
//     size_t len;
//     getline(&line, &len, fildes);

//     words = malloc(100 * sizeof(char *));

//     return;
// }

#if 0
char **
readline(int fildes){
    int i, len;
    char **words, line, *p;
    char buf[102]; 



    line = malloc(100 * sizeof(char *));
    for(p = *line; ; p++){
        /* edge test */
        if( p >= line+100 ){
            fprintf(stderr, "A line must contain \\lt 100 words\n");
            return 1;
        }

        *p = malloc(102 * sizeof(char)); /* 100*char + '\n' + '\0' = 102 */
        len = strlen(p);
        if( fgets(p, sizeof(p), stdin) == NULL ){ /* EOF */
            *p = 0x00;
            break;
        }
        else{

        }
    }
}
#endif

int
readline(char ***line, FILE* fildes)
{
    const int t = 10;
    int c = 0x00, cnt;
    char **p, *q;
    
    *line = malloc(t * sizeof(char *));
    for(p = *line; c != EOF; p++){
        if( p == *line+t ){
            fprintf(stderr, "A line can only contain at most %d words!\n", t);
            return -1;
        }

        c = 0x00;
        cnt++;
        *p = malloc(t * sizeof(char));
        for(q = *p; c != ' ' && c != EOF; q++){
            if( q == *p+t ){
                fprintf(stderr, "A word can only contain at most %d chars!\n", t);
                return -1;
            }
            c = fgetc(fildes);
            if( c == ' ' || c == EOF ) *q = 0x00;
            else *q = c;
        }
    }

    *++p = NULL;
    return cnt;
}


int 
main(int argc, char **argv){
    FILE *fildes;
    char **line;

    printf("%d\n", argc);

    if( !isatty(STDIN_FILENO) && argc > 1 ){
        fprintf(stderr, "Error : %s doesnt support multi input stream!\n", argv[0]);
        return 1;
    }

    if( isatty(STDIN_FILENO) ){ /* argv input */
        if( argc == 1 ){
            fprintf(stderr, "need input\n");
            return 1;
        }
        /* -> check if argv a file or string */
        int i;
        for(i = 1; i < argc; i++){
            if( access(argv[i], F_OK) == 0 ){ /* file exist */
                if( access(argv[i], R_OK) == -1 ){
                    fprintf(stderr, "file isn't readable\n");
                    return 1;
                }
                printf("read file\n");
                // OK and reads file
                // getline -> fprintf split ' '

                fildes = fopen(argv[i], O_RDONLY);
                close(fildes);
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

    return 0;
}

/*
pipe, read file 一起做
一樣 readline -> split
就是 fildes 不一樣
*/



#if 0
int main(int argc, char **argv){
    printf("%d\n", isatty(STDIN_FILENO));


    if( isatty(STDIN_FILENO) ){ /* argv input */
        if( argc == 1 ){
            fprintf(stderr, "need input\n");
            return 1;
        }
        /* -> check if argv a file or string */
        int i;
        for(i = 1; i < argc; i++){
            if( access(argv[i], F_OK) == 0 ){ /* file exist */
                if( access(argv[i], R_OK) == -1 ){
                    fprintf(stderr, "file isn't readable\n");
                    return -1;
                }
                printf("read file\n");
                // OK and reads file
                // getline -> fprintf split ' '
            }
            else{
                // just argv string
                printf("argv input\n");
            }
        }
    }
    else{ /* pipe input */
        printf("STDIN :)\n");
    }

    return 0;
}
#endif 