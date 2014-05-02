#ifndef __STRING_H_
#define __STRING_H_

#ifndef __cplusplus
	// ANSI C!
	#define wchar_t	short
#endif

int strlen(const char *str);
int wstrlen(const wchar_t *str);

#define size_t int
#define wchar_t short

char *strdup(const char *s);
char *strcpy(char *destination, const char *source);
char *strchr(const char *src, int character);
int strcmp(const char *str1, const char *str2);
char *strcat(char *destination, const char *source);
char * strncpy ( char * destination, const char * source, size_t num );
const char * strrchr ( const char * str, int character );
char *strstr(const char *src, const char *src2);

wchar_t *wstrcpy_s(wchar_t *destination, int size, wchar_t *source);

char *strncasecmp(char *src1, char *src2, size_t size);
const char *strncmp( const char * destination, const char * source, size_t num );

// char function
char toupper(char c);
char tolower(char c);

int stricmp(const char *s1, const char *s2);
int wstricmp(const wchar_t *w1, const wchar_t *w2);
int wstricmp_s(const wchar_t *w1, int l1, const wchar_t *w2, int l2);

char *strerror(int errnum);

#endif
