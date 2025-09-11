#include <stdio.h>

/* malloc */
#include <stdlib.h>

/* what if it's func not union ? */
enum
{
    MATCH = 0x00,
    CONCAT = 0x01, // 0x01
	SPLIT = 0x02 // 0x02 
};

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
    return 0;
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
    for(; l; l = l->next)
        l->s = s;

    return;
}

State*
post2nfa(char *post)
{   
#define push(x, y) *stackp++ = frag(x, y)
#define pop() *--stackp

    if( post == NULL )
        return NULL;
    
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
            push(s, append(e1.out, list1(&s->out2)));
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

    e = pop();
    // How come this ?
    if(stackp != stack)
        return NULL;

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

void addstate(List *l, State*s)
{
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
        printf("+1 ");
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
    printf("%d ", clist->n);
    for(i = 0; i < clist->n; i++){
        s = clist->s[i];
        if( valid(s->c) )
            addstate(nlist, s->out1);
            /* why there no s->out2 ? */
    }


#undef valid
}

int
match(State *start, char *s)
{
    int i, c;
    List *clist, *nlist, *temp;

    /* Why &l1 not l1 ? */
    clist = startlist(start, &l1);
    nlist = &l2;
    for(; *s; s++){
        c = *s;
        step(clist, c, nlist);

        /* bruh idk why Russ Cox do tri-swap here */
        clist = nlist;
    }

    return ismatch(clist);
}

int main(int argc, char **argv){
    if( argc < 2 ){
        fprintf(stderr, "lack of input string !!\n");
        return 1;
    }

    int i;
    static char re[10000];
    char *post = re; // regex in postfix
    State *start;

    // pipe input -> stding
    scanf("%s", post);
    char *p;
    // for(p = post; *p; p++){
    //     if( *p == CONCAT ) printf("CONCAT ");
    //     else printf("%c ", *p);
    // }
    // printf("\n");

    start = post2nfa(post);
    if( start == NULL ){
        fprintf(stderr, "Error in post2nfa %s\n", post);
        return 1;
    }

    /* wait is l1.s[0] a good parameter ? */
    /* it doesnt exist ?!?!*/
    l1.s = malloc(nstate * sizeof l1.s[0]);
    l2.s = malloc(nstate * sizeof l2.s[0]);
    for(i = 1; i < argc; i++){
        if( match(start, argv[i]) )
            printf("%s\n", argv[i]);
        printf("\n");
    }
    return 0;
}