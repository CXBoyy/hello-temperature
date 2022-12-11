#include <stddef.h>

#pragma weak stdin
#pragma weak stdout
#pragma weak stderr

void *stdin = NULL;
void *stdout = NULL;
void *stderr = NULL;
