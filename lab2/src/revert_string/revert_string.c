#include "revert_string.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>


void RevertString(char *str)
{
    int L = strlen(str);
    char* str2 = malloc(sizeof(char) * (L + 1));
    int i;
    for (i = 0; i < L; i++)
    {
        str2[i] = str[L-1-i];
    }
    strcpy(str, str2);
    free(str2);
}