/**************************************************************************/ /**
 * @brief Portability helpers.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#ifdef P1_FREERTOS // FreeRTOS
# ifndef P1_printf
#  define P1_printf(x, ...) do {} while(0)
# endif

# define P1_fprintf(s, x, ...) P1_printf(x, ##__VA_ARGS__)
# define P1_perror(x) P1_printf(x ".\n")
#else // POSIX
# include <stdio.h>
# define P1_printf printf
# define P1_fprintf fprintf
# define P1_perror perror
#endif
