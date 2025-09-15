#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gddread.h"
#include "shunting_yard.h"
#include "thompson.h"

int main(int argc, char **argv){
    char **line;

    line = gddread(argc, argv);

    if( line == NULL ) printf("BAD\n");
    else printf("GOOD\n");

    return 0;
}