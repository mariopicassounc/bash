#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "execute.h"
#include "command.h"
#include "builtin.h"

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);
    
    // sin &
    if(pipeline_get_wait(apipe) ){
        if (pipeline_length(apipe) == 1){

        }
        else if (pipeline_length(apipe) == 2){

        }
    }
    // con &
    else if (!pipeline_get_wait(apipe)){
        if (pipeline_length(apipe) == 1) {

        }
        else if (pipeline_length(apipe) == 2){

        }
    }


    else{
        fprintf(stderr, "Error. Not covered case.");
        exit(EXIT_FAILURE);
    }

}