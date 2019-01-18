#include <supportingUtilities/new.h>
#ifdef __cplusplus
#ifdef __GNUC__
#ifndef new
void * operator new(size_t size)
{
  return malloc(size);
}
#endif

#ifndef delete
void operator delete(void * ptr)
{
  free(ptr);
}
#endif

#ifndef new
void * operator new[](size_t size)
{
  return malloc(size);
}
#endif

#ifndef delete
void operator delete[](void * ptr)
{
  free(ptr);
}
#endif
#endif
#endif
