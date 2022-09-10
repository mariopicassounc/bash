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
    exit_bash = false;

    input = parser_new(stdin);
    while (!exit_bash) {
        show_prompt();
        pipe = parse_pipeline(input);

        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);
        
        char * pipe_str = pipeline_to_string(pipe);
        printf("%s\n", pipe_str);
        free(pipe_str); pipe_str = NULL;
    }
    parser_destroy(input); input = NULL;
    pipeline_destroy(pipe); pipe = NULL;

    return EXIT_SUCCESS;
}

