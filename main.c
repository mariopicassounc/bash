#include <stdlib.h>
#include <stdio.h>
#include "command.h"

/*
TARGET=mybash
CC=gcc
CPPFLAGS=`pkg-config --cflags glib-2.0`
CFLAGS=-std=gnu11 -Wall -Wextra -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -Werror=vla -g -pedantic
LDFLAGS=`pkg-config --libs glib-2.0`
*/

int main(void){
    char *command = "wc";
    char *command2 = "ls";
    const scommand self = scommand_new();
    unsigned int zero_len = scommand_length(self);
    printf("%u",zero_len);
    scommand_push_back(self, command);
    scommand_push_back(self, command2);
    printf("\n%u\n", scommand_length(self));
    scommand_destroy(self);
    return 0;
}

// para compilar: gcc -std=gnu11 -Wall -Wextra -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -Werror=vla -g -pedantic -c main.c command.c $(pkg-config --cflags --libs glib-2.0)
// gcc -std=gnu11 -Wall -Wextra -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -Werror=vla -g -pedantic main.o command.o -o test $(pkg-config --cflags --libs glib-2.0)