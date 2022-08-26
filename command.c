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
    free(self->redirect_in);
    self->redirect_in = NULL;
    free(self->redirect_out);
    self->redirect_out=NULL;
    free(self);
    self = NULL;
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL && argument!=NULL);
    GList *aux = g_list_append(self->command_and_args, argument); /*Esto anda si el argumento es un puntero de 64 bits y el gpointer también*/
    self->command_and_args = aux;
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);
    return (self->command_and_args == NULL) && 
    (self->redirect_in == NULL) &&
    (self->redirect_out == NULL);
}

char * scommand_get_redir_in(const scommand self){
    assert(self != NULL);
    return self->redirect_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self != NULL);
    return self->redirect_out;
}