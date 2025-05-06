/**************************************************************************/ /**
 * @brief Portability helpers.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/portability.h"

#ifdef P1_FREERTOS // FreeRTOS

#else // POSIX

#include <time.h>

int P1_GetUTCOffsetSec(P1_TimeValue_t* time) {
  time_t now_sec = (time_t)time->tv_sec;
  struct tm local_time = *localtime(&now_sec);
#if __GLIBC__ && __USE_MISC
  return local_time.tm_gmtoff;
#else // Not glibc
  // The tm_gmtoff field is a non-standard glibc extension. If it is not
  // available, use the `timezone` global variable defined in time.h by tzset().
  // It does _not_ get adjusted for daylight savings time, so we need to
  // manually offset it if tm_isdst is set.
  int utc_offset_sec = -timezone;
  if (local_time.tm_isdst) {
    utc_offset_sec += 3600;
  }
  return utc_offset_sec;
#endif
}

#endif // End OS selection
