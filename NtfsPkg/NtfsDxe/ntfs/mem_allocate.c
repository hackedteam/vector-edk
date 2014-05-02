//#include "mem_allocate.h"
#include <mem.h>
void* ntfs_alloc (size_t size) {
    return malloc(size);
}

void* ntfs_align (size_t size) {
    return malloc(size);
}

void ntfs_free (void* mem) {
    free(mem);
}