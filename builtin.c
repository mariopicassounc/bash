#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "strextra.h"

bool exit_bash;

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    return strcmp(scommand_front(cmd), "cd") == 0 ||
           strcmp(scommand_front(cmd), "exit") == 0 ||
           strcmp(scommand_front(cmd), "help") == 0
           ;
} //strcmp devuelve 0 si las cadenas a comparar son iguales

bool builtin_alone(pipeline p){
    assert(p != NULL);
    return pipeline_length(p) == 1 &&
           builtin_is_internal(pipeline_front(p))
           ;
}

/*Funciones auxiliares*/

/*Ejecuta el comando "cd"
  Requires: cmd != NULL* && strcmp(scommand_front(cmd), "cd") == 0
*/
static void builtin_run_cd (scommand cmd){
    assert(cmd != NULL && strcmp(scommand_front(cmd), "cd") == 0);
    scommand_pop_front(cmd);
    const char* cmd_path = scommand_front(cmd);
    int result = chdir(cmd_path);
    if(result != 0){
        fprintf(stderr, "invalid result");
        exit(EXIT_FAILURE);
    }
}

/*Ejecuta el comando "exit"
  Requires: cmd != NULL && strcmp(scommand_front(cmd), "exit") == 0
*/
static void builtin_run_exit (scommand cmd){
    assert(cmd != NULL && strcmp(scommand_front(cmd), "exit") == 0);
    exit_bash = true;
}

/*Ejecuta el comando "help"
  Requires: cmd != NULL && strcmp(scommand_front(cmd), "help") == 0
*/
static void builtin_run_help (scommand cmd){
    printf("Bienvenido a MyBash\n");
    printf("Autores: Mateo Malpassi, Mario Picasso, Facundo Coria, Bruno Espinossa\n");
    printf("Comandos internos disponibles: \n cd (Cambia del directorio actual al cual desees) \n exit (Salir del bash) help (Obtén información sobre el bash y los comandos que este tiene implementados)\n");   
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));
    if(strcmp(scommand_front(cmd), "cd") == 0){
        builtin_run_cd(cmd);
    }else if(strcmp(scommand_front(cmd), "exit") == 0){
        builtin_run_exit(cmd);
    }else if(strcmp(scommand_front(cmd), "help") == 0){
        builtin_run_help(cmd);
    }
}