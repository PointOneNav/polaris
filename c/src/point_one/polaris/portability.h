/**************************************************************************/ /**
 * @brief Portability helpers.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef P1_FREERTOS // FreeRTOS

#include <string.h> // For strerror()}
#include "FreeRTOS.h"

# ifndef P1_printf
#  define P1_printf(format, ...) do {} while(0)
# endif

# define P1_fprintf(stream, format, ...) P1_printf(format, ##__VA_ARGS__)

// FreeRTOS does not use errno and does not support perror(). Instead, they
// include the error condition in the return code itself. To get around this,
// our perror macro also takes the function return code and prints it.
//
// For POSIX systems, the return code argument is ignored. While it may be
// omitted, it is required for FreeRTOS compilation.
# define P1_perror(format, ret) \
  P1_printf(format ". [error=%s (%d)]\n", strerror(-ret), ret)

typedef TickType_t P1_TimeValue_t;

static inline void P1_SetTime(int time_ms, P1_TimeValue_t* result) {
  *result = pdMS_TO_TICKS(time_ms);
}

#else // POSIX

# include <stdio.h>
# include <sys/time.h>

# define P1_printf printf
# define P1_fprintf fprintf
# define P1_perror(format, ...) perror(format)

typedef struct timeval P1_TimeValue_t;

static inline void P1_SetTime(int time_ms, P1_TimeValue_t* result) {
  result->tv_sec = time_ms / 1000;
  result->tv_usec = (time_ms % 1000) * 1000;
}

#endif // End OS selection

#ifdef __cplusplus
} // extern "C"
#endif
