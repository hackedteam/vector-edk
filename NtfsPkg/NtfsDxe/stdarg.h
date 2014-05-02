#ifndef __STDARG_H_
#define __STDARG_H_

#define va_list	void *

void _va_start (va_list ap, void *);
void * _va_arg(va_list ap, void *, int size);

#define va_start(ap, paramN) _va_start(ap, (void *) paramN)
#define va_arg(ap, type) (type) _va_arg(ap, NULL, sizeof(type))
#define va_copy(dst, src) dst = src
void va_end(va_list ap);

#endif