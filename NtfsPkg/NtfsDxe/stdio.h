/***
 *  ]HackingTeam[ s.r.l.
 *  (c) 2012
 *
 *  STDIO implementation!
 **/

#ifndef __STDIO_H_
#define __STDIO_H_

#ifndef mode_t
	#define mode_t unsigned int
#else
	//#error "mode_t defined!"
#endif


#ifndef size_t
#define size_t int
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define SEEK_METHOD short

// mkdir - make a directory
int mkdir(const char *path, mode_t mode);

// rmdir - remove a directory
int rmdir(const char *path);

int sprintf ( char * str, const char * format, ... );
int snprintf(char *str, size_t size, const char *format, ...);


#define INVALID_HANDLE_VALUE 0

typedef struct _FILE
{
	int		*handle;	
} FILE;

extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;

int vfprintf ( FILE * stream, const char * format, ... );
int fprintf ( FILE * stream, const char * format, ... );
int fflush (FILE * stream);

#endif

