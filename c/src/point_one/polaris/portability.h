/**************************************************************************/ /**
 * @brief Portability helpers.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef P1_FREERTOS  // FreeRTOS

#  include <string.h>  // For strerror()

#  include "FreeRTOS.h"

#  ifndef P1_printf
#    define P1_printf(format, ...) \
      do {                         \
      } while (0)
#  endif

#  ifndef P1_fprintf
#    define P1_fprintf(stream, format, ...) P1_printf(format, ##__VA_ARGS__)
#  endif

typedef TickType_t P1_TimeValue_t;

static inline void P1_GetCurrentTime(P1_TimeValue_t* result) {
  *result = xTaskGetTickCount();
}

static inline void P1_SetTimeMS(int time_ms, P1_TimeValue_t* result) {
  *result = pdMS_TO_TICKS(time_ms);
}

static inline uint64_t P1_GetTimeMS(const P1_TimeValue_t* time) {
  return (uint64_t)pdTICKS_TO_MS(*time);
}

static inline int P1_GetElapsedMS(const P1_TimeValue_t* start,
                                  const P1_TimeValue_t* end) {
  return (int)((*end - *start) * portTICK_PERIOD_MS);
}

static inline int P1_GetUTCOffsetSec(P1_TimeValue_t* time) { return 0; }

static inline int P1_GetUTCOffsetHours(P1_TimeValue_t* time) {
  return P1_GetUTCOffsetSec(time) / 3600;
}

#else  // POSIX

#  include <errno.h>
#  include <stdio.h>
#  include <sys/time.h>

#  ifndef P1_printf
#    define P1_printf printf
#  endif

#  ifndef P1_fprintf
#    define P1_fprintf fprintf
#  endif

typedef struct timeval P1_TimeValue_t;

static inline void P1_GetCurrentTime(P1_TimeValue_t* result) {
  gettimeofday(result, NULL);
}

static inline void P1_SetTimeMS(uint64_t time_ms, P1_TimeValue_t* result) {
  result->tv_sec = (time_t)(time_ms / 1000);
  result->tv_usec = (suseconds_t)((time_ms % 1000) * 1000);
}

static inline uint64_t P1_GetTimeMS(const P1_TimeValue_t* time) {
  return (((uint64_t)(time->tv_sec)) * 1000) + (time->tv_usec / 1000);
}

static inline int P1_GetElapsedMS(const P1_TimeValue_t* start,
                                  const P1_TimeValue_t* end) {
  P1_TimeValue_t delta;
  timersub(end, start, &delta);
  return (int)((delta.tv_sec * 1000) + (delta.tv_usec / 1000));
}

int P1_GetUTCOffsetSec(P1_TimeValue_t* time);

static inline int P1_GetUTCOffsetHours(P1_TimeValue_t* time) {
  return P1_GetUTCOffsetSec(time) / 3600;
}

#endif  // End OS selection

#ifdef __cplusplus
}  // extern "C"
#endif
