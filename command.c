#include <glib-2.0/glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "strextra.h"
#include "command.h"

struct scommand_s{
    GSList * cmd_args;
    char * redirect_out;
    char * redirect_in;
};

scommand scommand_new(void){
    scommand result = malloc(sizeof(struct scommand_s));

    if(result== NULL){
        fprintf(stderr, "Invalid allocated memory");
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
    
    /* This funtion pass as a argument the function to free all the elements of the list */
    g_slist_free_full(self->cmd_args, free);
    self->cmd_args = NULL;

    free(self->redirect_in);
    self->redirect_in = NULL;
    free(self->redirect_out);
    self->redirect_out = NULL;

    free(self);
    self = NULL;
    
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL && argument!=NULL);
    self->cmd_args = g_slist_append(self->cmd_args, argument); /*Esto anda si el argumento es un puntero de 64 bits y el gpointer también*/
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    
    GSList * head = self->cmd_args;
    self->cmd_args = g_slist_remove_link(self->cmd_args, head);

    free(head->data);
    head->data = NULL;

    g_slist_free_1(head);
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self != NULL);
    
    /* Free past redirectors dynamic string */
    if(self->redirect_in){
        free(self->redirect_in);
    }

    self->redirect_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self != NULL);

    if(self->redirect_out){
        free(self->redirect_out);
    }

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
    length = g_slist_length(self->cmd_args);
    assert((length==0) == scommand_is_empty(self));
    return length;
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    char* result = g_slist_nth_data(self->cmd_args, 0u);
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
    GSList * l = self->cmd_args;

    if(l != NULL){
        result = strmerge(result, l->data);
        for(l = g_slist_next(l); l != NULL; l = g_slist_next(l)){
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

char * scommand_front_and_pop(scommand self){
    assert(self != NULL && !scommand_is_empty(self));

    char* result = g_slist_nth_data(self->cmd_args, 0u);

    self->cmd_args = g_slist_remove(self->cmd_args, result);

    assert(result != NULL);
    return (result);
}



char ** scommand_to_vector(scommand self){ 
    assert(!scommand_is_empty(self));
    
    int n = scommand_length(self);
    char **vector = calloc(sizeof(char*), n+1); 

    if(vector == NULL){
        fprintf(stderr, "Invalid allocated memory");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < n; i++){
        char *elem = scommand_front_and_pop(self); 
        vector[i] = elem;
        
        assert(vector[i] != NULL);  
    }
    vector[n] = NULL;

    return vector;
}

/*   ---  pipeline  ---   */


typedef struct pipeline_s{
    GSList * list_sc;
    bool wait;
}pipeline_s;

pipeline pipeline_new(void)
{
    pipeline result = malloc(sizeof(pipeline_s));
    if (result==NULL){
        fprintf(stderr, "invalid allocated memory");
        exit(EXIT_FAILURE);
    }

    result->list_sc = NULL;
    result->wait = TRUE;

    assert(result != NULL 
    && pipeline_is_empty(result)
    && pipeline_get_wait(result));

    return result;
}


pipeline pipeline_destroy(pipeline self)
{
    assert(self != NULL);
    
    for(GSList * l = self->list_sc; l != NULL; l = g_slist_next(l)){
        l->data = scommand_destroy(l->data);
    }

    g_slist_free(self->list_sc);
    self->list_sc = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc)
{
    assert(self != NULL && sc != NULL);
    self->list_sc = g_slist_append(self->list_sc, sc);
    assert(!(pipeline_is_empty(self)));
}

void pipeline_pop_front(pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));
    GSList *head = self->list_sc;
    self->list_sc = g_slist_remove_link(self->list_sc, head);

    head->data = scommand_destroy(head->data);
    head->next = NULL;

    g_slist_free_1(head);
}

void pipeline_set_wait(pipeline self, const bool w)
{
    assert(self != NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self != NULL);
    return (g_slist_length(self->list_sc) == 0) || self == NULL;
}

unsigned int pipeline_length(const pipeline self){
	
	assert(self!=NULL);
	unsigned int l;
	l = g_slist_length(self->list_sc);
	assert((l == 0) == pipeline_is_empty(self));
	return l;
}

scommand pipeline_front(const pipeline self){
	
	assert(self!=NULL && !pipeline_is_empty(self));
	scommand result = g_slist_nth_data(self->list_sc,0u);
    assert(result!=NULL);
    return result;
}

bool pipeline_get_wait(const pipeline self)
{
    assert(self != NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self){
    assert(self!=NULL);
    
    char * result = strdup("");
    GSList * l = self->list_sc;
    char * frst_sc_str = NULL;
    char * snd_sc_str = NULL;

    if(l != NULL){
        frst_sc_str = scommand_to_string(l->data);
        result = strmerge(result, frst_sc_str );
        free(frst_sc_str);

        for(l = g_slist_next(l); l != NULL; l = g_slist_next(l)){
            snd_sc_str = scommand_to_string(l->data);
            result = strmerge(result, " | ");
            result = strmerge(result, snd_sc_str);
            free(snd_sc_str);
        }
    }

    if(!(self->wait)){
        result = strmerge(result, " &");
    }

    assert(pipeline_is_empty(self) 
        || pipeline_get_wait(self) 
        || strlen(result)>0);
    
    return result;
}
