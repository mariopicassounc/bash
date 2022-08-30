#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "strextra.h"
#include "command.h"

struct scommand_s{
    GList * cmd_args;
    char * redirect_out;
    char * redirect_in;
};

scommand scommand_new(void){
    scommand result = malloc(sizeof(struct scommand_s));

    if(result== NULL){
        fprintf(stderr, "invalid allocated memory");
        exit(EXIT_FAILURE);
    }

    result->cmd_args = NULL;
    result->redirect_in = NULL;
    result->redirect_out = NULL;

    assert(result != NULL && scommand_is_empty(result) && 
    scommand_get_redir_in (result) == NULL &&
    scommand_get_redir_out (result) == NULL);
    return result;    
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);
    
    g_list_free(self->cmd_args);
    self->cmd_args = NULL;

    free(self);
    self = NULL;
    
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL && argument!=NULL);
    self->cmd_args = g_list_append(self->cmd_args, argument); /*Esto anda si el argumento es un puntero de 64 bits y el gpointer también*/
    assert(!scommand_is_empty(self));
}

/*Función auxiliar: g_list_free_front
* Elimina el primer elemento de una función no vacía
* Requires: list != NULL
*/
static GList* g_list_free_front(GList *list){
    assert(list != NULL);
    gpointer first = g_list_nth_data(list,0u);
    GList* l = g_list_remove(list,first);
    return l;
}

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    self->cmd_args = g_list_free_front(self->cmd_args);
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
    return (self->cmd_args == NULL) && 
    (self->redirect_in == NULL) &&
    (self->redirect_out == NULL);
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);
    unsigned int length;
    length = g_list_length(self->cmd_args);
    assert((length==0) == scommand_is_empty(self));
    return length;
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    char* result = g_list_nth_data(self->cmd_args, 0u);
    assert(result != NULL);
    return result;
}

char * scommand_get_redir_in(const scommand self){
    assert(self != NULL);
    return self->redirect_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self != NULL);
    return self->redirect_out;
}

char * scommand_to_string(const scommand self){
    assert(self!=NULL);
    
    char * result = strdup("");
    GList * l = self->cmd_args;

    if(l != NULL){
        result = strmerge(result, l->data);
        for(l = g_list_next(l); l != NULL; l = g_list_next(l)){
            result = strmerge(result, " ");
            result = strmerge(result, l->data);
        }
    }

    if (self->redirect_in != NULL){
        result = strmerge(result, " < ");
        result = strmerge(result, self->redirect_in);
    }
    
    if(self->redirect_out != NULL){
        result = strmerge(result, " > ");
        result = strmerge(result, self->redirect_out);
    }

    assert(scommand_is_empty(self) ||
    scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL ||
    strlen(result)>0);
    
    return result;
}


/*   ---  pipeline  ---   */


typedef struct pipeline_s{
    GList * sc;
    bool wait;
}pipeline_s;

pipeline pipeline_new(void)
{
    pipeline result = malloc(sizeof(pipeline_s));
    if (result==NULL){
        fprintf(stderr, "invalid allocated memory");
        exit(EXIT_FAILURE);
    }

    result->sc = NULL;
    result->wait = TRUE;

    assert(result != NULL 
    && pipeline_is_empty(result)
    && pipeline_get_wait(result));

    return result;
}


pipeline pipeline_destroy(pipeline self)
{
    assert(self != NULL);
    
    g_list_free(self->sc);
    self->sc = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc)
{
    assert(self != NULL && sc != NULL);
    self->sc = g_list_append(self->sc, sc);
    assert(!(pipeline_is_empty(self)));
}


void pipeline_pop_front(pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));

}

void pipeline_set_wait(pipeline self, const bool w)
{
    assert(self != NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
    return self->sc == NULL;
}

bool pipeline_get_wait(const pipeline self)
{
    assert(self != NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self){
    assert(self!=NULL);
    
    char * result = strdup("");
    GList * l = self->sc;
    char * frst_sc_str = NULL;
    char * snd_sc_str = NULL;

    if(l != NULL){
        frst_sc_str = scommand_to_string(l->data);
        result = strmerge(result, frst_sc_str );
        free(frst_sc_str);

        for(l = g_list_next(l); l != NULL; l = g_list_next(l)){
            snd_sc_str = scommand_to_string(l->data);
            result = strmerge(result, " | ");
            result = strmerge(result, snd_sc_str);
            free(snd_sc_str);
        }
    }

    if(self->wait){
        result = strmerge(result, " &");
    }

    assert(pipeline_is_empty(self) 
        || pipeline_get_wait(self) 
        || strlen(result)>0);
    
    return result;
}
