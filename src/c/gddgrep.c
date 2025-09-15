/* 
Combination of (ShuntYard.c -> Thompson.c)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct State State;
struct State
{
    int c; // char of the state
    State *out1;
    State *out2;
    int lastlist;
};

State matchstate = { MATCH };
int nstate;

State*
state(int c, State *out1, State *out2)
{
    State *s;

    nstate++;
    s = malloc(sizeof *s);
    s->lastlist = 0;
    s->c = c;
    s->out1 = out1;
    s->out2 = out2;
    return s;
}

typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
    State *start;
    Ptrlist *out;
};

/* init :D */
Frag
frag(State *start, Ptrlist *out)
{
    Frag f = {start, out};
    return f;
}

union Ptrlist
{
    Ptrlist *next;
    State *s;
};


Ptrlist*
list1(State **outp)
{
    Ptrlist *l;

    /* Pointer reinterpret >< */
    l = (Ptrlist*)outp;
    l->next = NULL;
    return l;
}

Ptrlist*
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *start = l1;

    while(l1->next)
        l1 = l1->next;
    l1->next = l2;

    return start;
}

void 
patch(Ptrlist *l, State *s)
{
    Ptrlist *next;
    for(; l; l = next){
        // hey why no HI
        next = l->next;
        l->s = s;
    }

    return;
}

State*
post2nfa(char *post)
{   
#define push(x, y) *stackp++ = frag(x, y)
#define pop() *--stackp

    if( post == NULL )
        return NULL;
    
    // printf("Check 1\n");
    
    char *p;
    Frag stack[1000], e, e1, e2;
    Frag *stackp = stack;
    State *s;

    for(p = post; *p; p++){
        switch(*p){
        default: /* normal char include '.' */
            s = state(*p, NULL, NULL);
            push(s, list1(&s->out1));
            break;
        case CONCAT: /* concat last 2 ORDERLY */
            e2 = pop();
            e1 = pop();
            patch(e1.out, e2.start);
            push(e1.start, e2.out);
            break;
        case '|':
            e2 = pop();
            e1 = pop();
            s = state(SPLIT, e1.start, e2.start);
            push(s, append(e1.out, e2.out));
            break;
        case '?':
            e = pop();
            s = state(SPLIT, e.start, NULL);
            push(s, append(e.out, list1(&s->out2)));
            break;
        case '*':
            e = pop();
            s = state(SPLIT, e.start, NULL);
            patch(e.out, s);
            push(s, list1(&s->out2));
            break;
        case '+':
            e = pop();
            s = state(SPLIT, e.start, NULL);
            patch(e.out, s);
            push(e.start, list1(&s->out2));
            break;
        }
    }

    // printf("Check 2\n");

    e = pop();
    // How come this ?
    if(stackp != stack){
        return NULL;
    }

    // printf("Check 3\n");

    patch(e.out, &matchstate);
    return e.start;

#undef push
#undef pop
}

typedef struct List List;
struct List
{
    State **s;
    int n;
};
List l1, l2;
static int listid;

void addstate(List*, State*);
void step(List*, int, List*);

/* Build init state list */
List*
startlist(State *start, List *l)
{

    l->n = 0;
    listid++;
    addstate(l, start);
    return l;
}

/* wait bro why there's no bool in C ?!? */
/* check if exist end state */
int
ismatch(List *l)
{
    int i;

    for(i = 0; i < l->n; i++)
        if( l->s[i] == &matchstate )
            return 1;

    return 0;
}

void addstate(List *l, State *s)
{
    if( s == NULL ) printf("WTF\n");

    if( s == NULL || s->lastlist == listid )
        return;

    s->lastlist = listid;
    if( s->c == SPLIT ){
        /* Split vamos :) */
        addstate(l, s->out1);
        addstate(l, s->out2);
    }
    else{
        l->s[l->n++] = s;
    }

    return;
}

void
step(List *clist, int c, List *nlist)
{
#define valid(x) ( x == c || x == '.' )

    int i;
    State *s;

    listid++;
    nlist->n = 0;
    for(i = 0; i < clist->n; i++){
        s = clist->s[i];
        if( valid(s->c) ){
            addstate(nlist, s->out1);
            /* why there no s->out2 ? */
        }
    }


#undef valid
}

int
match(State *start, char *s)
{
    int i, c;
    List *clist, *nlist, *t; //, *temp;

    /* Why &l1 not l1 ? */
    clist = startlist(start, &l1);
    nlist = &l2;
    for(; *s; s++){
        c = *s; // & 0xFF;
        step(clist, c, nlist);
        t = clist; clist = nlist; nlist = t;
    }

    return ismatch(clist);
}

int 
main(int argc, char **argv)
{
    int i, cnt;
    char *post, *p;
    State *start;

    if( argc < 3 ){
        // fprintf(stderr, "Run Time Error!\n");
        fprintf(stderr, "Usage : %s <regex> <input 1> <input 2> ... <input N>\n", argv[0]);
        fprintf(stderr, "For safety, please enclose input in quotation marks :)\n");
        return 1;
    }

    post = re2post(argv[1]);
    if( post == NULL ){
        fprintf(stderr, "bad regex : \'%s\'\n", argv[1]);
        return 1;
    }
    printf("postfix regex : %s\n", post);


    start = post2nfa(post);
    if( start == NULL ){
        fprintf(stderr, "Error in post2nfa %s\n", post);
        return 1;
    }

    /* wait is l1.s[0] a good parameter ? */
    /* it doesnt exist ?!?! */
    l1.s = malloc(nstate * sizeof start);
    l2.s = malloc(nstate * sizeof start);

    for(i = 2, cnt = 0; i < argc; i++){
        if( match(start, argv[i]) ){
            printf("find : %s\n", argv[i]);
            cnt++;
        }
    }

    if( cnt ) printf("totally find %d matches !\n", cnt);
    else printf("find nothing QAQ\n");

    /* inecessary for multi using (? */
    free(l1.s);
    free(l2.s);

    return 0;
}