#include <cpm.h>
#include <stdint.h>
#include <stdio.h>

#include "pcw_platform.h"
#include "zmc.h"

#define PCW_FKEY_BASE 0x80
#define PCW_SHIFT 0x20

extern uint16_t pcw_get_key_nowait( void );
extern void pcw_put_key( uint16_t key ) __z88dk_fastcall;

static int wait_key_bios( void ) { return bios( BIOS_CONIN, 0, 0 ); }

uint8_t pcw_wait_key( void ) {
    for ( ;; ) {
        uint16_t raw = pcw_get_key_nowait();
        if ( raw == 0xFFFF )
            continue;

        uint8_t key = raw & 0xFF;
        uint8_t shifts = raw >> 8;

        if ( key == 2 )
            return PCW_FKEY_BASE + ( shifts & PCW_SHIFT ? 1 : 0 );
        if ( key == 0 )
            return PCW_FKEY_BASE + ( shifts & PCW_SHIFT ? 3 : 2 );
        if ( key == 73 )
            return PCW_FKEY_BASE + ( shifts & PCW_SHIFT ? 5 : 4 );
        if ( key == 77 )
            return PCW_FKEY_BASE + ( shifts & PCW_SHIFT ? 7 : 6 );

        if ( key == 0x0E )
            return 'E' - '@';
        if ( key == 0x4F )
            return 'X' - '@';
        if ( key == 0x0F || key == 0x06 )
            return TAB;

        if ( key == 21 || key == 70 || key == 74 || key == 80 )
            continue;

        pcw_put_key( raw );
        return wait_key_bios();
    }
    return 0;
}

int pcw_platform_init( uint8_t cpmversion ) {
    if ( cpmversion != 0x31 ) {
        puts( "This PCW build requires CP/M Plus." );
        return 0;
    }
    wait_key_hw = &pcw_wait_key;
    return 1;
}

uint8_t pcw_dispatch_key( uint8_t key ) {
    if ( key < PCW_FKEY_BASE || key >= PCW_FKEY_BASE + 8 )
        return 0;

    switch ( key - PCW_FKEY_BASE ) {
    case 0: help(); break;
    case 2: view_file(); break;
    case 3: dump_file(); break;
    case 4: copy_cmd(); break;
    case 7: delete_cmd(); break;
    default: break;
    }
    return 1;
}

