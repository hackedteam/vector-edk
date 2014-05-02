#include "config.h"
#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <stdio.h>
#include <stdarg.h>

FILE *stdout;
FILE *stdin;
FILE *stderr;

int vasprintf  (char **ptr, const char *fmt, va_list ap);

int sprintf ( char * str, const char * format, ... )
{
	int r;

	va_list ap;
	va_start(ap, format);
	r = vasprintf(&str, format, ap);
	va_end(ap);

	return r;
}

int vfprintf ( FILE * stream, const char * format, ... )
{
	return 0;
}
int fprintf ( FILE * stream, const char * format, ... )
{
	return 0;
}

int fflush (FILE * stream)
{
	return 0;
}

//#define va_list void *

void va_end(va_list ap)
{
	return;
}

void _va_start(va_list ap, void *param)
{
	return;
}

void * _va_arg(va_list ap, void *dst, int size)
{
	return NULL;
}

long time(long *t)
{
	EFI_TIME curr;
	EFI_TIME_CAPABILITIES ignore;
	long result;

	gRT->GetTime(&curr, &ignore);
	
	result = (curr.Year - 1970) * 31556926;
	result += (curr.Month) * 2629743;
	result += (curr.Day) * 86400;
	result += (curr.Hour) * 3600;
	result += (curr.Minute) * 60;
	result += (curr.Second);
	
	return result;
}
