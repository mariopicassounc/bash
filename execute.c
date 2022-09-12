#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "tests/syscall_mock.h"

#include "execute.h"
#include "command.h"
#include "builtin.h"

#include <sys/wait.h>

#define READ_END    0    /* index pipe extremo lectura */
#define WRITE_END   1    /* index pipe extremo escritura */

/*
Ejecuta un comando simple identificando si es interno o no
*/
static void execute_scommand(pipeline apipe){

    scommand cmd = pipeline_front(apipe);
    
    // Es comando interno
    if(builtin_is_internal(cmd)){
        builtin_run(cmd); 
    }
    
    // No es comando interno
    else{
        char **argv = scommand_to_vector(cmd);

        int pid = fork();
        if (pid < 0){	// No funcionó bien el fork
            fprintf(stderr, "Error fork.");
            exit(EXIT_FAILURE);	
        }
        else if(pid == 0){	//Ejecuta el hijo
            execvp(argv[0],argv);
        }
        else if(pid > 0 && pipeline_get_wait(apipe)){ //Proc padre. No contiene &, debe esperar al hijo
            wait(NULL);
        }

        /* Liberar memoria de **argv */
        for (int i = 0; argv[i] != NULL; i++){
            free(argv[i]);
        }
        free(argv);
    }
}

void execute_pipeline(pipeline apipe){
    
    assert(apipe != NULL);
    
    if(pipeline_length(apipe) == 1){
        execute_scommand(apipe);
    }


    else if(pipeline_length(apipe) == 2){

        scommand cmd1 = pipeline_front(apipe);  //1er comando
        scommand cmd2 = NULL;                   //2do comando 
        
        if(builtin_is_internal(cmd1)){
            if (pipeline_get_wait(apipe)){	//no contiene &
                builtin_run(cmd1);
                wait(NULL);
            }
            else if (!pipeline_get_wait(apipe)){ //contiene &
                builtin_run(cmd1);
            }

            pipeline_pop_front(apipe);
            cmd2 = pipeline_front(apipe);
            execute_scommand(apipe);
            /*
            Pruebas en shell ejecutan sólo el segundo comando si hay un internal en el pipe
            */
        }
        else if(!builtin_is_internal(cmd1)){
            int fd[2];                           // file descriptor 
            pipe(fd);                            // pipe que conecta la salida de cmd1 con la entrada del cmd2, fd con extremos abiertos  

            int pid = fork();                    // Hijos heredan extremos de escritura y lectura abiertos

			if(pid < 0){	                     //No funcionó bien el fork
			    fprintf(stderr, "Error fork.");
		 		exit(EXIT_FAILURE);			
            }
            else if(pid == 0){ //Hijo 1
                close(fd[READ_END]); 

                dup2(fd[WRITE_END], STDOUT_FILENO); 
                close(fd[WRITE_END]);   

                char **argv = scommand_to_vector(cmd1);
                execvp(argv[0], argv);
            }
            else if(pid > 0){                      //Proc padre
                close(fd[WRITE_END]);
                pid = fork();
	
                if(pid < 0){
			        fprintf(stderr, "Error fork.");
		 		    exit(EXIT_FAILURE);	
                }
                else if(pid == 0){                 //Hijo 2
                    pipeline_pop_front(apipe);
                    cmd2 = pipeline_front(apipe);

                    dup2(fd[READ_END], STDIN_FILENO);
                    close(fd[READ_END]);

                    char **argv = scommand_to_vector(cmd2); 
                    execvp(argv[0], argv);
                }
                else if(pid > 0 && pipeline_get_wait(apipe)){ //Proc padre. Sin &, debe esperar a los hijos
                    wait(NULL);
                    wait(NULL);
                }
            }
        }
    }
}