#ifndef PCW_PLATFORM_H
#define PCW_PLATFORM_H

#include <stdint.h>

int pcw_platform_init( uint8_t cpmversion );
uint8_t pcw_wait_key( void );
uint8_t pcw_dispatch_key( uint8_t key );

#endif
