//#include "types.h"

#include "Ntfs.h"

#define size_t int
#define wchar_t	short
//#define NULL 0


// ffs
int ffs(int x)
{
	/*int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;*/

int r = 1;
  if (!x) return 0;
  if (!(x & 0xffff)) { x >>= 16; r += 16; }
  if (!(x &   0xff)) { x >>= 8;  r += 8;  }
  if (!(x &    0xf)) { x >>= 4;  r += 4;  }
  if (!(x &      3)) { x >>= 2;  r += 2;  }
  if (!(x &      1)) { x >>= 1;  r += 1;  }

  return r;
}

// mem.cpp
int memcmp(void *dst, void *src, int size)
{
	char *pDst = (char *) dst;
	char *pSrc = (char *) src;
	int i;

	for(i =0; i < size; i++)
	{
		if (*pDst == *pSrc)
		{
			pDst++;
			pSrc++;
		}
		else
			return -1;
	}
	
	return 0;
}

void* memcpy(void *dst, void *src, int size)
{
	char *pDst = (char *) dst;
	char *pSrc = (char *) src;

	while(size-- > 0)
	{
		*pDst++ = *pSrc++;
	}

	return dst;
}

void *memset(void *dst, int pattern, int size)
{
	char *pDst = (char *) dst;

	while(size-- > 0)
	{
		*pDst++ = pattern;
	}

	return dst;
}

void *memchr(void *src, int c)
{
	char *pSrc = (char *) src;

	while(*pSrc != c)
		pSrc++;

	return (void *) pSrc;
}


void free(void *ptr)
{
	int *x;

	if (ptr == NULL)
		return;	// nothing to free!!!

	// force! fix!
	x = (int *) ptr;

	x -= 4;

	if (*x == 0x30726670)
	{
		//Print(L"invalid pointer %x\n", ptr);
		return;
	}
	FreePool(ptr);
	return;
}

/////////////
void *malloc(size_t size)
{
	return AllocateZeroPool(size);
}

void * calloc(size_t nmemb, size_t lsize)
{
	return AllocateZeroPool(nmemb * lsize);
}

void *realloc(void *ptr, size_t size)
{
	void *nb = malloc(size);

	if (ptr != NULL && nb != NULL)
	{	// transfer
		memcpy(nb, ptr, size);
		free(ptr);
	}

	return nb;
}


/** function strchr
 *  const char * strchr ( const char * str, int character );
 *        char * strchr (       char * str, int character );
 *  Locate first occurrence of character in string
 **/
char *strchr(const char *src, int character)
{
	while(*src != 0x00)
        if (*src == character)
            return (char *) src;
        else
            src++;

    return NULL;
}

/** function strcpy
 *  char * strcpy ( char * destination, const char * source );
 *  Copy string
 *  Copies the C string pointed by source into the array pointed by destination, including the terminating null character.
**/
char *strcpy(char *destination, const char *source)
{
    char *d = destination;

    while(*source != 0x00)
        *d++ = *source++;

    return destination;
}

/**
 *  size_t strlen ( const char * str );
 *  Get string length
 *  Returns the length of str.
 **/
size_t strlen(const char *str)
{
    size_t size = 0;

    while(*str++ != 0x00)
        size++;

    return size;
}

size_t wstrlen(const wchar_t *str)
{
	size_t size = 0;

	while(*str++ != 0x0)
		size++;

	return size;
}

char *strcat(char *destination, const char *source)
{
	char *p = destination;

	while(*p++ != 0x00);	// end of string

	for(; *source != 0x00; source++, p++);
		*p = *source;

	return destination;
}

int strcmp(const char *str1, const char *str2)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);

	if (l1 < l2)
		return -1;

	if (l2 > l1)
		return 1;

	for(; *str1 != 0x00; str1++, str2++)
		if (*str1 != *str2)
			return -1;

	return 0;
}

/**
 *	char *strncpy( char * destination, const char * source, size_t num )
 *	Copy characters from string
 **/
char *strncpy( char * destination, const char * source, size_t num )
{
	char *r = destination;

	while(*source != 0x00 && num-- > 0)
		*destination++ = *source++;

	*destination = 0x00;	// end of string

	return r;
}

/**
 *	char *strncpy( char * destination, const char * source, size_t num )
 *	Copy characters from string
 **/
const char *strncmp( const char * destination, const char * source, size_t num )
{
	while(*destination == *source && num > 0)
	{
		destination++;
		source++;
		num--;
	}

	if (num == 0)
		return destination;

	return NULL;
}

const char * strrchr ( const char * str, int character )
{
	int l = strlen(str);

	if (l > 0)
	{
		for(; l > 0; l--)
			if (str[l-1] == character)
				return &str[l-1];
	}

	return NULL;
}

//
//
char *strncasecmp(char *src1, char *src2, size_t size)
{
	return NULL;
}


// Convert a character to uppercase
// TODO: Re-do how filename conversions are done.
char toupper(char c) {
	if(c >= 'a' && c <= 'z') {
		return c + ('A'-'a');
	}
	return c;
}

char tolower(char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - ('A'-'a');
	}

	return c;
}


int stricmp(const char *s1, const char *s2)
{
	char c1,c2;
	
	do 
	{
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
	} while((c1 == c2) && (c1 != '\0'));

	return (int) c1-c2;
}

int wstricmp(const wchar_t *w1, const wchar_t *w2)
{
	wchar_t c1,c2;
	int len1 = wstrlen(w1);
	int len2 = wstrlen(w2);

	do 
	{
		c1 = (wchar_t) tolower((char) *w1++);
		c2 = (wchar_t) tolower((char) *w2++);
		len1--;
		len2--;
	} while((c1 == c2) && (c1 != '\0') && (len1 > 0) && (len2 > 0));

	return (int) c1-c2;
}

int wstricmp_s(const wchar_t *w1, int l1, const wchar_t *w2, int l2)
{
	wchar_t c1,c2;
	
	do 
	{
		c1 = (wchar_t) tolower((char) *w1++);
		c2 = (wchar_t) tolower((char) *w2++);
		l1--;
		l2--;
	} while((c1 == c2) && (c1 != '\0') && (l1 > 0) && (l2 > 0));

	return (int) c1-c2;
}


wchar_t *wstrcpy_s(wchar_t *destination, int size, wchar_t *source)
{
	wchar_t *d = destination;

	while(*source != 0x00 && size > 0)
	{
		*d++ = *source++;
		size--;
	}

	*d = 0x00;

	return destination;
}

char *strdup(const char *str)
{
	char *d = malloc(strlen(str) + 1);

	/*AsciiPrint("strdup ");
	AsciiPrint(str);
	

	AsciiPrint(" malloc %x %x\n\r", d, strlen(str));*/
	strcpy(d, str);
	return d;
}

char *strstr(const char *str, const char *target)
{
	char *p1, *p2, *p1Begin;
	
	*p2 = NULL;
	*p1 = (char *) str;
	*p1Begin = NULL;

	if (!*target)
		return str;

	while(*p1)
	{
		p1Begin = p1;
		p2 = (char *) target;

		while(*p1 && *p2 && *p1 == *p2)
		{
			p1++;
			p2++;
		}
		if (!*p2)
			return p1Begin;

		p1 = p1Begin + 1;
	}

	return NULL;
}


int errno = 0;

//
////typedef struct _LONGLONG
////{
////	DWORD	Lower;
////	DWORD	Highest;
////} LONGLONG;
//
//// math!
//UINT64 _allmul(UINT64 Multiplicand, UINT64 Multiplier)
//{
//	//UINTN *r;
//
//	UINT64 result = MultU64x64(Multiplicand, Multiplier);
//
//	//r = &Multiplicand;
//	//AsciiPrint("_allmul Multiplicand %x%x\n\r", r[1], r[0]);
//	//r = &Multiplier;
//	//AsciiPrint("_allmul Multiplier %x%x\n\r", r[1], r[0]);
//
//	//r = (UINTN *) &result;
//
//	//AsciiPrint("_allmul result %x%x\n\r", r[1], r[0]);
//
//	return result;
//}
//
//UINT64 _alldiv(UINT64 Dividend, UINT64 Divisor)
//{
//	UINT64 Remainder = 0;
//
//	return DivS64x64Remainder(Dividend, Divisor, Remainder);
//}
//
//void _allshr(UINT64 multiplier, UINT64 multiplicand)
//{
//	
//	AsciiPrint("_allshr:\n\r");
//}
//
//UINT64 _allshl(UINT64 Operand, UINTN Count)
//{
//	AsciiPrint("_allshl:\n\r");
//	return InternalMathLShiftU64(Operand, Count);
//}
//
//void _aullrem(UINT64 dividend, UINT64 divisor)
//{
//	AsciiPrint("_aullrem:\n\r");
//}
//
//void _aullshr(UINT64 dividend, UINT64 divisor)
//{
//	AsciiPrint("_aullshr:\n\r");
//}
//
//UINT64 _allrem(UINT64 Dividend, UINT64 Divisor)
//{
//	UINT64 Remainder = 0;
//
//	DivS64x64Remainder(Dividend, Divisor, Remainder);
//
//	return Remainder;
//}
