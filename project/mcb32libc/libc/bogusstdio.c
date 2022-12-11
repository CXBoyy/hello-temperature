#include <stdlib.h>

#if STDIO == 0

void * const stdin = NULL;
void * const stdout = NULL;
void * const stderr = NULL;

#endif
