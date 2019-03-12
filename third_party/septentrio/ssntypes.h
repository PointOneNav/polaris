/*
 * ssntypes.h: 
 *  Declarations of Septentrio types and implementation of common C types which
 *  are not implemented by every compiler.
 *
 *  If your compiler does not support the standard C99 types from \p stdint.h
 *  and \p stdbool.h, please define them for your platform. \n
 *  This is already done for the Microsoft C/C++ compiler.
 *
 *
 * Septentrio grants permission to use, copy, modify, and/or distribute
 * this software for any purpose with or without fee.  
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND SEPTENTRIO DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * SEPTENTRIO BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SSNTYPES_H
#define SSNTYPES_H 1 /**< To avoid multiple inclusions. */

#ifdef __linux__
# ifndef FW_EXPORT
#  ifdef ENABLE_VISIBILITY
#   define FW_EXPORT __attribute__((visibility("default"))) 
#  else
#   define FW_EXPORT
#  endif
# endif
#endif

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__GNUC__) || defined(__ARMCC__)
#  include <stdint.h>
#  include <stdbool.h>
#  include <inttypes.h>
#else
#  ifdef _MSC_VER
#    include "mscssntypes.h"
#    ifndef _USE_MATH_DEFINES /* used to define M_PI and so*/
#      define _USE_MATH_DEFINES
#    endif
#  endif
#endif

#define  I8_NOTVALID   (int8_t)  (0x80)
#define UI8_NOTVALID   (uint8_t) (0xFF)
#define I16_NOTVALID   (int16_t) (0x8000)
#define U16_NOTVALID   (uint16_t)(0xFFFF)
#define I32_NOTVALID   (int32_t) (0x80000000)
#define U32_NOTVALID   (uint32_t)(0xFFFFFFFF)
#define I64_NOTVALID   (int64_t) (0x8000000000000000LL)
#define U64_NOTVALID   (uint64_t)(0xFFFFFFFFFFFFFFFFULL)
#define F32_NOTVALID             (-2e10F)
#define F64_NOTVALID             (-2e10)

#ifndef FW_EXPORT
#  define FW_EXPORT /**< For making a DLL with the Microsoft C/C++ compiler. */
#endif

#endif
/* End of "ifndef SSNTYPES_H" */
