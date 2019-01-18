#ifndef NEW_H
#define NEW_H

#include <stdlib.h>
#ifdef __cplusplus
#ifdef __GNUC__
void * operator new(size_t size);
void operator delete(void * ptr);
void * operator new[](size_t size);
void operator delete[](void * ptr);
#endif
#endif
#endif
