/**************************************************************************/ /**
 * @brief Socket support wrapper for the Polaris client.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#ifdef P1_FREERTOS
#  include "socket_freertos.h"
#else
#  include "socket_posix.h"
#endif
