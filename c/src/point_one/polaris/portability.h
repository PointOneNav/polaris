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

#include <string.h> // For strerror()

#include "FreeRTOS.h"

# ifndef P1_printf
#  define P1_printf(format, ...) do {} while(0)
# endif

# ifndef P1_fprintf
#  define P1_fprintf(stream, format, ...) P1_printf(format, ##__VA_ARGS__)
# endif

// FreeRTOS does not use errno and does not support perror(). Instead, they
// include the error condition in the return code itself. To get around this,
// our perror macro also takes the function return code and prints it.
//
// For POSIX systems, the return code argument is ignored. While it may be
// omitted, it is required for FreeRTOS compilation.
# ifndef P1_perror
#  define P1_perror(format, ret) \
  P1_printf(format ". [error=%s (%d)]\n", strerror(-ret), ret)
# endif

typedef TickType_t P1_TimeValue_t;

static inline void P1_GetCurrentTime(P1_TimeValue_t* result) {
  *result = xTaskGetTickCount();
}

static inline void P1_SetTimeMS(int time_ms, P1_TimeValue_t* result) {
  *result = pdMS_TO_TICKS(time_ms);
}

static inline int P1_GetElapsedMS(const P1_TimeValue_t* start,
                                  const P1_TimeValue_t* end) {
  return (int)((*end - *start) * portTICK_PERIOD_MS);
}

#else // POSIX

# include <errno.h>
# include <stdio.h>
# include <sys/time.h>

# ifndef P1_printf
#  define P1_printf printf
# endif

# ifndef P1_fprintf
#  define P1_fprintf fprintf
# endif

// The standard POSIX perror() does not include the numeric error code in the
// printout, which is often very useful, so we do not use it.
//
// Note that we ignore the ret argument here, which is meant for use with
// FreeRTOS, and instead get the actual error value from errno on POSIX systems.
# ifndef P1_perror
#  define P1_perror(format, ret) \
  P1_fprintf(stderr, format ". [error=%s (%d)]\n", strerror(errno), errno)
# endif

typedef struct timeval P1_TimeValue_t;

static inline void P1_GetCurrentTime(P1_TimeValue_t* result) {
  gettimeofday(result, NULL);
}

static inline void P1_SetTimeMS(int time_ms, P1_TimeValue_t* result) {
  result->tv_sec = time_ms / 1000;
  result->tv_usec = (time_ms % 1000) * 1000;
}

static inline int P1_GetElapsedMS(const P1_TimeValue_t* start,
                                  const P1_TimeValue_t* end) {
  P1_TimeValue_t delta;
  timersub(end, start, &delta);
  return (int)((delta.tv_sec * 1000) + (delta.tv_usec / 1000));
}

#endif // End OS selection

#ifdef __cplusplus
} // extern "C"
#endif
