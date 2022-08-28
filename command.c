#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "command.h"

struct scommand_s{
    GList * command_and_args;
    char * redirect_out;
    char * redirect_in;
};

scommand scommand_new(void){
    scommand result = malloc(sizeof(struct scommand_s));
    result->command_and_args = NULL;
    result->redirect_in = NULL;
    result->redirect_out = NULL;

    assert(result != NULL && scommand_is_empty(result) && 
    scommand_get_redir_in (result) == NULL &&
    scommand_get_redir_out (result) == NULL);
    return result;    
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);
    g_list_free(self->command_and_args);
    self->command_and_args = NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL && argument!=NULL);
    self->command_and_args = g_list_append(self->command_and_args, argument); /*Esto anda si el argumento es un puntero de 64 bits y el gpointer también*/
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self != NULL);
    self->redirect_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self != NULL);
    self->redirect_out = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);
    return (self->command_and_args == NULL) && 
    (self->redirect_in == NULL) &&
    (self->redirect_out == NULL);
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);
    unsigned int length;
    length = g_list_length(self->command_and_args);
    assert((length==0) == scommand_is_empty(self));
    return length;
}

char * scommand_get_redir_in(const scommand self){
    assert(self != NULL);
    return self->redirect_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self != NULL);
    return self->redirect_out;
}