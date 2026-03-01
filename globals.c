/*
Z80 Management Commander (ZMC)
Copyright (C) 2026 Volney Torres & Martin Homuth-Rosemann

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include "zmc.h"

// GLOBAL VARIABLES

// // the status of both panels
AppState App;

// default configuration, can be patched for the target terminal
// get the address of the config values with "zmc --config
const uint8_t CONFIG[] = { // 80x40
    80,  // Columns
    24   // Lines
};

const uint8_t *COLUMNS = CONFIG;
const uint8_t *LINES = CONFIG+1;


char cmdline[CMDLINELEN+1];

uint8_t DEBUG = 0; // increase with "zmc --debug", can be used to enable messages etc.
uint8_t DEVEL = 0; // increase with "zmc --devel", can be used to enable new features


uint16_t MAX_FILES = 0;


// GLOBAL FUNCTIONS

void print_cpm_attrib( uint8_t *ca) {
    // show file attributes
    printf( "%c%c%c",
            *ca++ > 0x7F ? 'R' : ' ', // READ ONLY
            *ca++ > 0x7F ? 'S' : ' ', // SYSTEM
            *ca++ > 0x7F ? 'B' : ' '  // file was BACKED UP
    );
}

void draw_header( Panel *p ) {
    uint8_t i;
    uint8_t x_offset = p == &App.left ? 1 : PANEL_WIDTH + 1;
    i = PANEL_WIDTH - 2;
    putchar_xy( x_offset, 1, ' ' );
    while ( i-- )
        putchar('_');
    putchar(' ');
    goto_xy( x_offset + 2 , 1 );
    if ( p == App.active_panel )
        set_invers();
    printf( "[ DISK %c: ]", p->drive );
    if ( p == App.active_panel )
        set_normal();
}


void show_prompt() {
    goto_xy( 1, PANEL_HEIGHT+1 );
    set_normal();
    printf("%c> %s", App.active_panel->drive, cmdline );
    show_cursor();
    clr_line_right();
}


void refresh_ui(uint8_t which_panel) {
    hide_cursor();
    if ( which_panel & 0b01) {
        if ( App.active_panel == &App.left )
            draw_panel(&App.left);
        else if ( App.active_panel == &App.right )
            draw_panel(&App.right);
    }
    if ( which_panel & 0b10) {
        if ( App.active_panel == &App.right )
            draw_panel(&App.left);
        else if ( App.active_panel == &App.left )
            draw_panel(&App.right);
    }
    draw_header( &App.left );
    draw_header( &App.right );

    goto_xy( 1, PANEL_HEIGHT+2 );
    set_invers();
    if ( PANEL_WIDTH >= 40 ) {
        printf("| A: - P: | TAB:Sw | F1:Help | F3:View | F4:Dump | F5:Copy | F8:Del | F10:Exit |");
    } else if ( PANEL_WIDTH >= 30 ) {
        printf("A:-P:|TAB:Sw|F1:Help|F3:View|F4:Dump|F5:Copy|F8:Del|F10:Exit");
    }
    show_prompt();
}

