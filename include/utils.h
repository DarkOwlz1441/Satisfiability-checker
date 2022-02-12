#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
    This is the definition of a boolean type, using enums.
*/
typedef enum bool{FALSE, TRUE} bool;

/*
    This function is responsible for duplicating literal strings,
    in other implementations we use it to obtain the terms of the
    proposition, which we call token
*/

char *custom_strdup(const char *string)
{
    size_t string_len = strlen(string);
    char *duplicate   = (char *) malloc(string_len + 1);
    
    // Check if the duplicate pointer has been allocated 
    if(!duplicate)
    {
        return NULL;
    }

    /* 
        Returns the duplicate pointer with the same content/memory
        as the literary string passed as parameter 
    */
    return (char *) memcpy(duplicate, string, (string_len + 1));
}