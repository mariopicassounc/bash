#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

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

        /* Aseguramos matar los procesos zombies (en caso de que haya) */
        while(waitpid(-1,NULL,WNOHANG) > 0);

        /* Parseamos el pipeline */
        pipe = parse_pipeline(input);

        /* Si estoy al final del archivo me voy */
        exit_bash = parser_at_eof(input);

        /* Ejecutamos el comando */
        execute_pipeline(pipe);

        /* Libero memoria de la estructura de datos */
        pipeline_destroy(pipe); pipe = NULL;
    }

    parser_destroy(input); input = NULL;    
    return EXIT_SUCCESS;
}

