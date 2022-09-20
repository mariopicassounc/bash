#include "parsing.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "command.h"
#include "parser.h"

static scommand
parse_scommand (Parser p)
{
    /* Devuelve NULL cuando hay un error de parseo */
    scommand cmd;
    arg_kind_t arg_type;
    char *arg = NULL;

    cmd = scommand_new ();
    arg = parser_next_argument (p, &arg_type);

    /* Chequea si el input está vacío o es inválido */
    /* Condición mutuamente excluyente con la condición del while */
    if (arg == NULL)
        {
            scommand_destroy (cmd);
            cmd = NULL;
        }

    while (arg != NULL)
        {
            if (arg_type == ARG_INPUT)
                {
                    scommand_set_redir_in (cmd, arg);
                }
            else if (arg_type == ARG_OUTPUT)
                {
                    scommand_set_redir_out (cmd, arg);
                }
            else if (arg_type == ARG_NORMAL)
                {
                    scommand_push_back (cmd, arg);
                }
            arg = parser_next_argument (p, &arg_type);
        }

    return cmd;
}

pipeline
parse_pipeline (Parser p)
{
    assert (p != NULL && !parser_at_eof (p));

    pipeline result = pipeline_new ();
    scommand cmd = NULL;
    bool error = false, another_pipe = true, op_background = false,
         garbage = false;

    cmd = parse_scommand (p);
    error = (cmd == NULL); /* Comando inválido al empezar */

    while (another_pipe && !error)
        {
            pipeline_push_back (result, cmd);
            parser_op_pipe (p, &another_pipe);
            if (another_pipe)
                {
                    cmd = parse_scommand (p);
                    error = (cmd == NULL);
                }
        }
    parser_op_background (p, &op_background);

    if (op_background)
        {
            pipeline_set_wait (result, !op_background);
        }

    parser_garbage (p, &garbage);

    /* Chequea si hay caracteres inválidos luego de & */
    if ((op_background && garbage) || error)
        {
            pipeline_destroy (result);
            result = NULL;
        }

    return result;
}