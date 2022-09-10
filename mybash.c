#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void) {
    printf ("mybash> ");
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool exit_bash = false;

    input = parser_new(stdin);
    while (!exit_bash) {
        show_prompt();
        /* Parseamos el pipeline */
        pipe = parse_pipeline(input);

        /* Chequeamos que se haya parseado la completitud del comando */
        if(!parser_at_eof(input)){
            fprintf(stderr, "Error parsing command");
            exit(EXIT_FAILURE);
        }

        /* Imprimo el comando. Esto despues se borra */    
        char * pipe_str = pipeline_to_string(pipe);
        printf("%s\n", pipe_str);
        free(pipe_str); pipe_str = NULL;

        /* Ejecutamos el comando */
        execute_pipeline(pipe);

        /* Libero memoria de la estructura de datos */
        pipeline_destroy(pipe); pipe = NULL;
    }

    parser_destroy(input); input = NULL;    
    return EXIT_SUCCESS;
}

