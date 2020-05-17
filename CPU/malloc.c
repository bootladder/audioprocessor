#include "FreeRTOS.h"
void* malloc(size_t size)
{
  void* ptr = NULL;

  if(size > 0)
  {
    // We simply wrap the FreeRTOS call into a standard form
    ptr = pvPortMalloc(size);
  } // else NULL if there was an error

  return ptr;
}

void free(void* ptr)
{
  if(ptr)
  {
    // We simply wrap the FreeRTOS call into a standard form
    vPortFree(ptr);
  }
}