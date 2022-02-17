#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
    This is the definition of a boolean type, using enums.
*/
typedef enum bool{FALSE, TRUE} bool;
/*
    This function is responsible for duplicating literal strings.
    it receives a string and returns a copy for it.

    There is the strdup() function that does the exact same thing,
    but it is rather a GCC function than a C Library one. So it is
    not so portable. That is why we implemented a custom one, for 
    using in this project.
*/
char *custom_strdup(const char *string)
{
    size_t string_len = strlen(string);
    char *duplicate   = (char *) malloc(string_len + 1);
    
    // Check if the duplicate pointer has been allocated 
    if(!duplicate)
        return NULL;

    // Returns the duplicate pointer with the same content/memory
    // as the literary string passed as parameter
    return (char *) memcpy(duplicate, string, (string_len + 1));
}