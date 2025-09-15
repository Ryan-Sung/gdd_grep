#include <stdio.h>
#include <stdlib.h>

/* strlen */
#include <string.h>

/*
point out 'ss|' bad regex error ?
*/

enum
{
    MATCH = 0x00,
    CONCAT = 0x01, // 0x01
	SPLIT = 0x02 // 0x02 
};

char*
re2post(char *re){
    int nalt, natom;

    // static -> init to 0x00 which is '\0'
    static char buf[10000];

    // index of postfix string
    char *idx; 

    // for parentheses recur
    struct{
        int nalt;
        int natom;
    } paren[100], *p;

    p = paren;
    idx = buf;
    nalt = natom = 0;

    if( strlen(re) >= sizeof(buf)/2 ) 
        return NULL;

    // while -> clear natom for a new frag(?
    #define natomcheck(a, b, ifwhile)                   \
        if( a == 1 && natom == 0 )                      \
            return NULL;                                \
        if( b == 1 ){                                   \
            if( ifwhile ){                              \
                while(--natom)                          \
                    *idx++ = CONCAT;                    \
            }                                           \
            else{                                       \
                if( natom > 1 ){                        \
                    natom--;                            \
                    *idx++ = CONCAT;                    \
                }                                       \
            }                                           \
        }                                               
                                        
    #define altfill()                                   \
        for(; nalt; nalt--)                             \
            *idx++ = '|';

    // '\0' = 0x00 = false :D
    // or its NULL ?
    for(; *re; re++){
        switch(*re){
        case '(':
            natomcheck(0, 1, 0);
            if( p >= paren+100 )
                return NULL;
            p->nalt = nalt;
            p->natom = natom;
            p++;
            nalt = natom = 0;
            break;
        case '|':
            natomcheck(1, 1, 1);
            nalt++;
            break;
        case ')':
            if( p == paren )
                return NULL;
            natomcheck(1, 1, 1);
            altfill();

            --p;
            nalt = p->nalt;
            natom = p->natom;
            natom++;
            break;
        case '*':
        case '?':
        case '+':
            natomcheck(1, 0, 0);
            *idx++ = *re;
            break;

        // check if '.' = 
        case '.':
        default:
            natomcheck(0, 1, 0);
            *idx++ = *re;
            natom++;
            break;
        }       
    }

    if( p != paren ) 
        return NULL;
    natomcheck(0, 1, 1);
    altfill();
    *idx = 0x00;

    return buf;
}

int 
main(int argc, char *argv[])
{
    int i;
    char *post, *p;

    if( argc != 2 ){
        fprintf(stderr, "Run Time Error!\n");
        // fprintf(stderr, "Usage : %s <regexp> <input1> <input2>...\n", argv[0]);
        fprintf(stderr, "Usage : %s <regexp>\n", argv[0]);
        fprintf(stderr, "For safety, please enclose input in quotation marks :)\n");
        return 1;
    }

    // check if input regex illegal
    // check if truely 257 = 0x01

    post = re2post(argv[1]);
    if( post == NULL ){
        fprintf(stderr, "bad regexp : \'%s\'\n", argv[1]);
        return 1;
    }

    printf("%s\n", post);

    return 0;
}
 
/*
is 's(s|c)+?' valid ?

if return 1, need to tell thompson ?
*/


/* OLD VERSION */
/*
#define natomcheck(a, b, ifwhile)                       \
        if( a == 1 && natom == 0 )                      \
            return NULL;                                \
        if( b == 1 ){                                   \
            if( ifwhile ){                              \
                while(--natom)                          \
                    *idx++ = CONCAT;                    \
            }                                           \
            else{                                       \
                if( natom > 1 ){                        \
                    natom--;                            \
                    *idx++ = CONCAT;                    \
                }                                       \
            }                                           \
        }                                               \
        do{                                             \
            // important that b == 1 before --natom   \
            // is there need for prevent natom = 0 ?  \
            if( b == 1 && natom && --natom > 0 ){       \
                // natom--;                           \
                *idx++ = CONCAT;                        \
            }                                           \
        }while(ifwhile && b == 1 && natom);             \
*/

/*
(((((((((((((((((((((((((((((((((((((((())))))))))))))))))))(((((((((((((((((((())))))))))))))))))))(((((((((((((((((((())))))))))))))))))))(((((((((((((((((((())))))))))))))))))))(((((((((((((((((((())))))))))))))))))))(((((((((((((((((((())))))))))))))))))))))))))))))))))))))))
*/