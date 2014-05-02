#ifndef __SYS_TYPES_H_
#define __SYS_TYPES_H_

//#define size_t		int
#define u_int8_t	unsigned char
#define uint8_t		unsigned char
#define int8_t		char
#define u_int16_t	unsigned short
#define uint16_t	unsigned short
#define int16_t		short
#define u_int32_t	unsigned int
#define uint32_t	unsigned int
#define int32_t		int
//#define u64			unsigned __int64
#define u_int64_t	unsigned __int64
#define	uint64_t	unsigned __int64
#define int64_t		__int64

#ifndef NULL
	#define NULL	0
#endif

#ifdef __cplusplus
#define BOOL		bool
#define TRUE		true
#define FALSE		false
#else
#define bool		char
#define	BOOL		char
#ifndef UEFI_BUILD
#ifndef TRUE
#define	TRUE		1
#define FALSE		0
#endif
#endif
#define true		1
#define false		0

#endif


#define time_t		long
#define size_t		int

#endif