/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Data types used in the UQM port
 * Draft 3
 * Compiled from various sources
 */

#ifndef _TYPES_H
#define _TYPES_H

#if defined(__MACOS__)

#	define UQM_INT16   short
#	define UQM_INT32   long

#elif defined (_WIN64) || defined(WIN64)

#	error No Win64 support yet (see types.h)

/* Add your OS support here */

#else
/* all other sane 32bit: WIN32, __MACOSX__, __BEOS__, __EMX__ */

#	define UQM_INT16   short
#	define UQM_INT32   int

#endif

/* Figure out how to support 64-bit datatypes */
#if !defined(__STRICT_ANSI__)
#if defined(__GNUC__) || defined(__MWERKS__) || defined(__SUNPRO_C)
#	define UQM_INT64    long long
#elif defined(_MSC_VER) || defined(__BORLANDC__)
#	define UQM_INT64    __int64
#endif
#endif /* !__STRICT_ANSI__ */

/* The 64-bit type isn't available on EPOC/Symbian OS */
#ifdef __SYMBIAN32__
#	undef UQM_INT64
#endif

typedef enum
{
	false = 0,
	true
} bool;

/* If it isn't char, what is it ?!*/
typedef unsigned char      uint8;
typedef signed char        sint8;

typedef unsigned UQM_INT16 uint16;
typedef signed UQM_INT16   sint16;

typedef unsigned UQM_INT32 uint32;
typedef signed UQM_INT32   sint32;

#if defined(UQM_INT64)

typedef unsigned UQM_INT64 uint64;
typedef signed UQM_INT64   sint64;

#undef UQM_INT64

#else /* dont have UQM_INT64 */

typedef struct
{
	uint32 hi;
	uint32 lo;
} uint64;
typedef struct
{
	sint32 hi;
	uint32 lo;
} sint64;
typedef struct
{
	int32  hi;
	uint32 lo;
} int64;

#endif /* UQM_INT64 */

#undef UQM_INT16
#undef UQM_INT32

/* Make sure the types really have the right sizes
 * Adapted from SDL
 * This will generate "negative subscript or subscript is too large"
 * error during compile, if the actual size of a type is wrong
 */
#define UQM_COMPILE_TIME_ASSERT(name, x) \
	typedef int UQM_dummy_##name [(x) * 2 - 1]

UQM_COMPILE_TIME_ASSERT(uint8, sizeof(uint8) == 1);
UQM_COMPILE_TIME_ASSERT(uint16, sizeof(uint16) == 2);
UQM_COMPILE_TIME_ASSERT(uint32, sizeof(uint32) == 4);
UQM_COMPILE_TIME_ASSERT(uint64, sizeof(uint64) == 8);

#undef UQM_COMPILE_TIME_ASSERT

#undef SINT8_MIN
#undef SINT8_MAX
#undef SINT16_MIN
#undef SINT16_MAX
#undef SINT32_MIN
#undef SINT32_MAX

#undef UINT8_MAX
#undef UINT16_MAX
#undef UINT32_MAX

#define SINT8_MIN  (-128)
#define SINT8_MAX  127
#define SINT16_MIN (-32767-1)
#define SINT16_MAX 32767
#define SINT32_MIN (-2147483647-1)
#define SINT32_MAX 2147483647

#define UINT8_MAX  0xff /* 255U */
#define UINT16_MAX 0xffff /* 65535U */
#define UINT32_MAX 0xffffffff /* 4294967295U */

#endif /* _TYPES_H */
