/***
 *  ]HackingTeam[ s.r.l.
 *  (c) 2012
 *
 *  Configuration file!
 **/

#ifndef _CONFIG_H_
    #define _CONFIG_H_

#pragma warning (disable : 4200)
#pragma warning (disable : 4341)	// signed value is out of range for enum constant
#pragma warning (disable : 4309)	// truncation of constant value
#pragma warning (disable : 4244)	// conversion from x to y possible loss of data
#pragma warning (disable : 4018)	// signed/unsigned mismatch


/***
 *  ]HackingTeam[ s.r.l.
 *  (c) 2012
 *
 *  Setup Configuration File
 **/

#ifndef __SETUP_H__
    #define __SETUP_H__

// Disk access method
#define USE_CHS 0x1

// disk access method
// #define USE_LBA 0x1


// mm/memory
#define USE_PMM

//#define USE_STRING  1
//#define USE_MEM     1

//  INT64 setup
#define _NATIVE_INT64 1
//#define _EMULATE_INT64 1

// Support for C++ features
//#define __SUPPORT_CPP_MEM_ALLOC
#endif


#ifndef __MACRO_H_
    #define __MACRO_H_

#define CALC_OFFSET(type, x, y) (type) ((VOID *) (((unsigned long) x) + y))
#define OFFSET(x, y) (void *) (((UINTN) x) + y)
#endif


#define size_t	int
#define _CRT_SECURE_NO_WARNINGS
//#define HAVE_SYS_TYPES_H
#define __LITTLE_ENDIAN	1
#define __BYTE_ORDER	__LITTLE_ENDIAN
#define HAVE_SYS_STAT_H	1
#define HAVE_STRING_H	1
#define HAVE_STDLIB_H	1
#define HAVE_TIME_H		1
#define HAVE_FCNTL_H	1
#define HAVE_STDIO_H	1
#define HAVE_STDARG_H	1
#define HAVE_LIMITS_H	1
#define HAVE_STDINT_H	1

#define PATH_MAX 255

#include <sys/errno.h>
#include <sys/types.h>
#include <stdint.h>

//#include <types.h>
#define ptrdiff_t(x) ((int)x)
#endif

