//---------------------------------------------------------------------------
#ifndef __stk_MAIN_H
#define __stk_MAIN_H
//---------------------------------------------------------------------------
//		asm(".intel_syntax noprefix\n");
//		asm(".att_syntax prefix\n");

#define HEXATOR(h) 0x##h
//  BYTE B(1010,1001)
#define  B(h1,h2) ((((HEXATOR(h1)>>12) & 0x0001)<<7) + (((HEXATOR(h1)>>8) & 0x0001)<<6) + (((HEXATOR(h1)>>4) & 0x0001)<<5) + (((HEXATOR(h1)>>0) & 0x0001)<<4) + (((HEXATOR(h2)>>12) & 0x0001)<<3) + (((HEXATOR(h2)>>8) & 0x0001)<<2) + (((HEXATOR(h2)>>4) & 0x0001)<<1) + (((HEXATOR(h2)>>0) & 0x0001)<<0))
//  WORD W(1010,1001,0110, etc)
#define  W(h1,h2,h3,h4) (B(h1,h2)<<8 + B(h3,h4))
#define  D(h1,h2,h3,h4,h5,h6,h7,h8) (W(h1,h2,h3,h4)<<16 + W(h5,h6,h7,h8))
#define  Q(h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,h13,h14,h15) (D(h1,h2,h3,h4,h5,h6,h7,h8)<<16 + D(h9,h10,h11,h12,h13,h14,h15))
#define SBI(val,bit) (val |  (0x00000001<<bit))
#define CBI(val,bit) (val & ~(0x00000001<<bit))
#define GBI(val,bit) (val &  (0x00000001<<bit))

//__STRICT_ANSI__
/*__BYTE_ORDER__
__ORDER_LITTLE_ENDIAN__
__ORDER_BIG_ENDIAN__
__ORDER_PDP_ENDIAN__*/

//---------------------------------------------------------------------------
#ifdef __CLANG__
//#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
//#pragma clang diagnostic pop
#endif
//---------------------------------------------------------------------------
#ifndef WIN32
#define WIN32
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <winsock2.h>
#include <cstddef>
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
//---------------------------------------------------------------------------
#define __restrict__
// Borland C++
// ...
// ...
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
// GCC etc.
// ...
// ...
#include <inttypes.h>

#ifndef int8_t
typedef int8_t  BYTE;
typedef int8_t UBYTE;
#endif

#ifndef int16_t
typedef int16_t  SHORT;
typedef int16_t USHORT;
#endif

#ifndef int32_t
typedef int32_t  LONG;
typedef int32_t ULONG;
#endif

#ifndef int64_t
typedef int64_t   LONGLONG;
typedef uint64_t ULONGLONG;
#endif

#include <omp.h>
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
// UNICODE
// ...

#ifndef __x
#define __x(s) L ## s
#endif

#ifndef _T
#define _T(s) __x(s)
#endif

#ifndef _TEXT
#define _TEXT(s) __T(s)
#endif

#ifndef _TEOF
#define _TEOF EOF
#endif

//---------------------------------------------------------------------------
// INCLUDE
// ...
// ...
//---------------------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#include <time.h>
//---------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <direct.h>
//---------------------------------------------------------------------------
#include <fcntl.h>
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
/*#ifndef __MMX__
#define __MMX__
#endif
#ifndef __SSE__
#define __SSE__
#endif
#ifndef __SSE2__
#define __SSE2__
#endif
*/
#ifndef MAKEWORD
#define MAKEWORD(a,b)   ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))
#endif

//---------------------------------------------------------------------------
#define __ASM_OPT__
#define __DEBUG__
#define __STASM_DO_NOT_WARN
//---------------------------------------------------------------------------
#if defined(__DEBUG__)
//---------------------------------------------------------------------------
//#define __DEBUG_MEM32__
//#define __DEBUG_MEM32_ALLOC__
//#define __DEBUG_CSTR__
//#define __DEBUG_CSTR_CLASS__
//#define __DEBUG_CSTR_STACK__
#define __DEBUG_DATABASE_ITEMS__
#define __DEBUG_DATABASE__
#define __DEBUG_KOP32_CLASS__
#define __DEBUG_KOP32_CONSOLE__
#define __DEBUG_KOP32_PROGRESS_CONTROLER__
#define __DEBUG_PHARSER__
#define __DEBUG_MATH_PHARSER__
#define __DEBUG_VECTOR__
#define __DEBUG_LIST__
#define __DEBUG_HASH_CHAIN__
#define __DEBUG_SET__
#define __DEBUG_LINKED_VECTOR__
#define __DEBUG_HASHED_LINKED_VECTOR__
#define __DEBUG_COMPRESSOR_API__
#define __DEBUG_LZSS_COMPRESSOR__
#define __DEBUG_HUFF_COMPRESSOR__
#define __DEBUG_STATIC_HUFF_COMPRESSOR__
#define __DEBUG_ARI_COMPRESSOR__
#define __DEBUG_CIPHER_API__
#define __DEBUG_MTF_CIPHER__
#define __DEBUG_XOR_CIPHER__
#define __DEBUG_ROT_CIPHER__
#define __DEBUG_HASHER_API__
#define __DEBUG_CRC32_HASHER__
#define __DEBUG_MD5_HASHER__
#define __DEBUG_SSC1_HASHER__

#define __DEBUG_SOCKET__
#define __DEBUG_RS232__
#define __DEBUG_FILE_IO__
#define __DEBUG_FILE_INI__
//#define __DEBUG_TIME__

#define __DEBUG_FUNC_ENTER__
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#if defined(__DEBUG_FUNC_ENTER__)
#define __DEBUG_CALLED("") \
static const  int __entered_line = __LINE__;\
static int __entered_time = 1;\
	char *__file__ = __FILE__;\
	char *__file_name__ = ::strrchr(__file__,'/');\
	  if (__file_name__==NULL) __file_name__ = ::strrchr(__file__,'\\');\
	  if (__file_name__==NULL) __file_name__ = __file__;\
	printf("DEBUG: Entered func() @%s %d, time %d\n",__file_name__,__entered_line,__entered_time++);
#define __DEBUG_FUNC_RET__\
	printf("DEBUG: Ret. from func @%s %d at line %d\n",__file_name__,__entered_line,__LINE__);
#else
#define __DEBUG_CALLED("")
#define __DEBUG_FUNC_RET__
#endif
//---------------------------------------------------------------------------
#if !defined(__GNUC__) && !defined(__clang__)
#define __builtin_prefetch(x1,x2,x3)   asm { push esi; mov esi,x1; prefetchnta esi+0; pop esi; }
#define __builtin___clear_cache(x1,x2) asm { SFENCE; }
#define __PTRDIFF_TYPE__ int8_t*
#define __PTRDIFF_MAX__ ((__PTRDIFF_TYPE__)(~0))
#endif
#include "./__stasm.h"
//---------------------------------------------------------------------------
#endif
