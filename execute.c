#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "tests/syscall_mock.h"

#include "execute.h"
#include "command.h"
#include "builtin.h"

#include <sys/wait.h>

#define READ_END 0  /* index pipe extremo lectura */
#define WRITE_END 1 /* index pipe extremo escritura */

/* Funciones auxiliares */

/* Liberar memoria de argv */
static void free_argv(char **argv)
{
    assert(argv != NULL);
    for (int i = 0; argv[i] != NULL; i++)
    {
        free(argv[i]);
    }
    free(argv);
    argv = NULL;
}

/* Se encarga de redireccionar la entrada de cmd */
static int set_fd_in(scommand cmd)
{
    int redirected_in = 0;
    if (scommand_get_redir_in(cmd) != NULL)
    {
        int file_to_redirect_in = open(scommand_get_redir_in(cmd), O_DIRECTORY, O_RDONLY);
        if (file_to_redirect_in == -1)
        {
            /* En caso de error, open seetea el mansaje de perror */
            fprintf(stderr, "open error");
            exit(EXIT_FAILURE);
        }

        redirected_in = dup2(file_to_redirect_in, STDIN_FILENO);
        if (redirected_in == -1)
        {
            fprintf(stderr, "dup2 error");
            exit(EXIT_FAILURE);
        }

        int close_file = close(file_to_redirect_in);
        if(close_file == -1)
        {
            fprintf(stderr, "close error");
            exit(EXIT_FAILURE);
        }
    }
    /* Si redirected_in == 0 -> No hay redirección de entrada */
    return (EXIT_SUCCESS);
}

/* Se encarga de redireccionar la salida de cmd */
static int set_fd_out(scommand cmd)
{
    int redirected_out = 0;
    if (scommand_get_redir_out(cmd) != NULL)
    {
        // O_CREAT para crear el archivo de salida si este no existe
        int file_to_redirect_out = open(scommand_get_redir_out(cmd), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (file_to_redirect_out == -1)
        {
            // En caso de error, open seetea el mansaje de perror
            fprintf(stderr, "open error");
            exit(EXIT_FAILURE);
        }

        redirected_out = dup2(file_to_redirect_out, STDOUT_FILENO);
        if (redirected_out == -1)
        {
            fprintf(stderr, "dup2 error");
            exit(EXIT_FAILURE);
        }

        int close_file = close(file_to_redirect_out);
        if (close_file == -1)
        {
            fprintf(stderr, "close error");
            exit(EXIT_FAILURE);
        }
    }
    // Si redirected_out == 0 -> No hay redirección de salida
    return (EXIT_SUCCESS);
}

/* Ejecuta un comando simple identificando si es interno o no*/
static void execute_single_pipe(pipeline apipe)
{
    scommand cmd = pipeline_front(apipe);

    /* Es comando interno */
    if (builtin_is_internal(cmd))
    {
        builtin_run(cmd);
    }

    /* No es comando interno */
    else
    {
        char **argv = scommand_to_vector(cmd);

        int pid = fork();
        if (pid < 0)
        { /* No funcionó bien el fork */
            fprintf(stderr, "Error fork.");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { /* Ejecuta el hijo */
            int status = set_fd_in(cmd);
            if (status != 0)
            {
                fprintf(stderr, "error setting file descriptors");
                exit(EXIT_FAILURE);
            }
            status = set_fd_out(cmd);
            if (status != 0)
            {
                fprintf(stderr, "error setting file descriptors");
                exit(EXIT_FAILURE);
            }
            execvp(argv[0], argv);
        }
        else if (pid > 0 && pipeline_get_wait(apipe))
        { /* Proc padre. No contiene &, debe esperar al hijo */
            wait(NULL);
        }

        free_argv(argv);
    }
}

/* Se encarga de ejecutar un comando externo sin forkear */
static void execute_external_cmd(scommand cmd)
{
    int status = set_fd_in(cmd);
    if (status != 0)
    {
        fprintf(stderr, "error setting file descriptors");
        exit(EXIT_FAILURE);
    }
    status = set_fd_out(cmd);
    if (status != 0)
    {
        fprintf(stderr, "error setting file descriptors");
        exit(EXIT_FAILURE);
    }
    char **argv = scommand_to_vector(cmd);
    execvp(argv[0], argv);
    free_argv(argv);
}

/* Ejecuta dos comandos unidos por un pipe identificando si son internos o no */

static void execute_double_pipe(pipeline apipe)
{
    scommand cmd1 = pipeline_front(apipe); /* 1er comando */

    int fd[2]; /* file descriptor */
    pipe(fd);  /* pipe que conecta la salida de cmd1 con la entrada del cmd2, fd con extremos abiertos */

    int pid = fork(); /* Hijos heredan extremos de escritura y lectura abiertos */

    if (pid < 0)
    { /* No funcionó bien el fork */
        fprintf(stderr, "Error fork.");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { /* Hijo 1 */
        close(fd[READ_END]);

        dup2(fd[WRITE_END], STDOUT_FILENO);

        close(fd[WRITE_END]);
        
        if (builtin_is_internal(cmd1))
        {
            builtin_run(cmd1);
            pipeline_pop_front(apipe);
        }
        else
        {
            execute_external_cmd(cmd1);
        }
        
    }
    else if (pid > 0)
    { /* Proc padre */
        close(fd[WRITE_END]);
        pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "Error fork.");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {   /* Hijo 2 */
            pipeline_pop_front(apipe);
            scommand cmd2 = pipeline_front(apipe);

            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[READ_END]);

            if (builtin_is_internal(cmd2))
            {
                builtin_run(cmd2);
                pipeline_pop_front(apipe);
            }
            else {
                execute_external_cmd(cmd2);
            }
        }
        else if (pid > 0 && pipeline_get_wait(apipe))
        { /* Proc padre. Sin &, debe esperar a los hijos */
            wait(NULL);
            wait(NULL);
            close(fd[READ_END]);
        }
    }
}

void execute_pipeline(pipeline apipe)
{

    assert(apipe != NULL);

    if (pipeline_length(apipe) == 1)
    {
        execute_single_pipe(apipe);
    }

    else if (pipeline_length(apipe) == 2)
    {
        execute_double_pipe(apipe);
    }
}
