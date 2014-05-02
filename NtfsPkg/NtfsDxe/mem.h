#ifndef __MEM_H_
#define __MEM_H_

#ifndef size_t
#define size_t int
#endif

void free(void *ptr);
void *malloc(int size);
void *realloc(void *ptr, int newsize);
void *calloc(int num, int size);

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void * memmove ( void * destination, const void * source, size_t num );
void * memchr ( const void * ptr, int value, size_t num);

#define RtlZeroMemory(s, n) memset(s, 0, n)

#endif