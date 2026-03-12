#include <stdio.h>
#include <stdint.h>
#include "mds_def.h"
#include "mds_utils.h"

static uint8_t g_heapTest[0x2000];
void *__HeapBase = &(g_heapTest[0]);
void *__HeapLimit = &(g_heapTest[0x2000]);

int main()
{
}
