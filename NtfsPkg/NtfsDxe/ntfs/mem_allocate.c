//#include "mem_allocate.h"
#include <mem.h>
void* ntfs_alloc (size_t size) {
	void *r = malloc(size);

    return r;
}

void* ntfs_align (size_t size) {
    return malloc(size);
}

void ntfs_free (void* mem) {
	//Print(L"ntfs_free(%x)\n", mem);
    free(mem);
}