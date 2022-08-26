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

int main(){
    char *command = "wc";
    char *command2 = "ls";
    scommand self = scommand_new();
    scommand_push_back(self, command);
    scommand_push_back(self, command2);
    scommand_destroy(self);
    return 0;
}