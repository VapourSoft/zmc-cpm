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

// strip the MS-DOS protection header
#pragma output noprotectmsdos

// do not insert the file redirection option while parsing the command line arguments
#pragma output noredir

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <cpm.h>
#include "zmc.h"


extern AppState App;


uint8_t wait_key_bios( void ) {
    // use raw BIOS CONIO (fkt 3) to ignore XON/XOFF (^Q and ^S are used as fkt keys)
    uint8_t k = bios( BIOS_CONIN, 0, 0 ); // function, BC, DE, returns A
    return k;
}


uint8_t wait_key_bdos( void ) {
    // use BDOS RAWIO to ignore XON/XOFF (^Q and ^S are used as fkt keys)
    uint8_t k = bdos( 6, 0xFD ); // C_RAWIO, wait for char, returns A
    return k;
}


// function pointer, default is BIOS, can be switched to BDOS for CP/M3
uint8_t (*wait_key_hw)(void) = &wait_key_bios;


void other_panel() {
    // change focus
    if ( App.active_panel == &App.left )
        App.active_panel = &App.right;
    else
        App.active_panel = &App.left;
    draw_header( &App.left );
    draw_header( &App.right );

    // chirurgical update: refresh only the lines with cursors
    draw_file_line(&App.left, 1, App.left.current_idx);
    draw_file_line(&App.right, PANEL_WIDTH+1, App.right.current_idx);
    show_prompt();
}


void change_drive( char k ) {
    App.active_panel->drive = k;
    load_directory(App.active_panel);
    refresh_ui( PAN_ACTIVE );
}


void select_file() {
    if ( !App.active_panel->num_files )
        return;
    int idx = App.active_panel->current_idx;
    int offset = (App.active_panel == &App.left) ? 1 : PANEL_WIDTH+1;

    // A. invert the selection state in memory
    App.active_panel->files[idx].attrib ^= B_SEL;

    // B. redraw current line to show '*'
    // IMPORTANT: current_idx was not changed, line is drawn with cursor.
    draw_file_line(App.active_panel, offset, idx);

    // C. move the cursor to the next line
    line_down();
}


void line_up() {
    if (App.active_panel->current_idx > 0) {
        int old_idx = App.active_panel->current_idx;
        App.active_panel->current_idx--;

        // if scrolling, redraw everything; if not, only two lines
        if (App.active_panel->current_idx < App.active_panel->scroll_offset) {
            refresh_ui( PAN_ACTIVE );
        } else {
            int offset = (App.active_panel == &App.left) ? 1 : PANEL_WIDTH+1;
            draw_file_line(App.active_panel, offset, old_idx);
            draw_file_line(App.active_panel, offset, App.active_panel->current_idx);
        }
    }
}


void line_down() {
    if (App.active_panel->current_idx + 1 < App.active_panel->num_files ) {
        int old_idx = App.active_panel->current_idx;
        App.active_panel->current_idx++;

        // if scrolling, redraw everything; if not, only two lines
        if (App.active_panel->current_idx >= App.active_panel->scroll_offset + VISIBLE_ROWS) {
            refresh_ui( PAN_ACTIVE );
        } else {
            int offset = (App.active_panel == &App.left) ? 1 : PANEL_WIDTH+1;
            draw_file_line(App.active_panel, offset, old_idx);
            draw_file_line(App.active_panel, offset, App.active_panel->current_idx);
        }
    }
}


void page_up() {
    if (App.active_panel->current_idx >= VISIBLE_ROWS)
        App.active_panel->current_idx -= VISIBLE_ROWS;
    else
        App.active_panel->current_idx = 0;
    refresh_ui( PAN_ACTIVE );
}


void page_down() {
    App.active_panel->current_idx += VISIBLE_ROWS;
    if (App.active_panel->current_idx >= App.active_panel->num_files)
        App.active_panel->current_idx = App.active_panel->num_files - 1;
    refresh_ui( PAN_ACTIVE );
}


void first_file() {
    App.active_panel->current_idx = 0;
    refresh_ui( PAN_ACTIVE );
}


void last_file() {
    App.active_panel->current_idx = App.active_panel->num_files - 1;
    refresh_ui( PAN_ACTIVE );
}


uint8_t yes_no() {
    char k = wait_key_hw();
    return (k == 'y' || k == 'Y');
}


void copy() {
    if ( App.left.drive == App.right.drive ) // cannot copy to same drive
        return;
    Panel *dest = (App.active_panel == &App.left) ? &App.right : &App.left;
    // clear dialog box and ask
    goto_xy( 1, PANEL_HEIGHT+1 );
    clr_line_right();
    printf(" COPY SELECTED FILE(S) TO %c:? (Y/N) ", dest->drive);
    if ( yes_no() )
        // Y: copy multiple files
        exec_multi_copy(App.active_panel, dest);
    // clear status line
    goto_xy( 1, PANEL_HEIGHT+1 );
    clr_line_right();
    refresh_ui( PAN_OTHER );
}


void delete() {
    // clear dialog box and ask
    goto_xy( 1, PANEL_HEIGHT+1 );
    clr_line_right();
    printf(" DELETE SELECTED FILE(S)? (Y/N) " );
    if ( yes_no() ) {
        // Y: call master function
        exec_multi_delete(App.active_panel);
        load_directory(App.active_panel);
        // if left == right update both panels
        if ( App.left.drive == App.right.drive )
            if ( App.active_panel == &App.left )
                copy_panel( &App.left, &App.right );
            else
                copy_panel( &App.right, &App.left );
    }
    // clear status line
    goto_xy( 1, PANEL_HEIGHT+1 );
    clr_line_right();
    refresh_ui( App.left.drive == App.right.drive ? PAN_BOTH : PAN_ACTIVE ); // file(s) deleted, refresh active panel
}


void help() {
    uint8_t line = 1;
    hide_cursor();
    set_normal();
    clr_scr();
    goto_xy( 1, line++ );
    if ( SCREEN_HEIGHT >= 20) {
        puts( "                           " );
        puts( " #######  #     #   #####  " );
        puts( "      #   ##   ##  #     # " );
        puts( "     #    # # # #  #       " );
        puts( "    #     #  #  #  #       " );
        puts( "   #      #     #  #       " );
        puts( "  #       #     #  #     # " );
        puts( " #######  #     #   #####  " );
        puts( "                           " );
        line = 12;
    }

#ifndef i8080
    puts( "= ZMC 1.3 - Volney Torres =" );
#else
    puts( "= ZMC 1.3 (8080 version) - Volney Torres =" );
#endif
    const uint8_t helppos = 40;
    goto_xy( 1, line );
    printf( "A: ... P:" );
    goto_xy( helppos, line++ );
    puts( "Select drive" );
    printf( "[TAB]" );
    goto_xy( helppos, line++ );
    puts( "Change panel" );
    printf( "[F3],  [ESC]3, TYPE, VIEW, CAT");
    goto_xy( helppos, line++ );
    puts( "Show file" );
    printf( "[F4],  [ESC]4, DUMP, HEX" );
    goto_xy( helppos, line++ );
    puts( "Hexdump file" );
    printf( "[F5],  [ESC]5, COPY, CP" );
    goto_xy( helppos, line++ );
    puts( "Copy file(s)");
    printf( "[F8],  [ESC]8, [ESC]3, DEL, ERA, RM" );
    goto_xy( helppos, line++ );
    puts( "Delete file(s)" );
    printf( "[F10], [ESC]0, [ESC][ESC], QUIT, EXIT" );
    goto_xy( helppos, line++ );
    puts( "Exit" );
    wait_key_hw();
    refresh_ui( PAN_BOTH );
}

typedef void (*command_func_t)(void);

typedef struct {
    const char *keyword;
    command_func_t func;
} command_t;

// parse zmc cmd line and get a fkt pointer
command_func_t find_command(const char *input, command_t commands[], int num_commands) {
    for (int i = 0; i < num_commands; i++) {
        if (strncmp(input, commands[i].keyword, strlen(commands[i].keyword)) == 0) {
            return commands[i].func;
        }
    }
    return NULL;
}


int main(int argc, char** argv) {
    uint8_t cpmversion = bdos( 12, NULL );
    // CP/M Plus has values for screen size in System Control Block
    if ( cpmversion == 0x31 ) { // version == CP/M Plus
        // handle BDOS errors internally, do not exit
        bdos( 45, 0xFF ); // set BDOS return error mode 1
        bdos( 109, 0x0A ); // C_MODE, ignore ^C and ^S
        wait_key_hw = &wait_key_bdos; // use BDOS RAWIO instead of BIOS CONIO
        uint8_t scbpb[ 4 ] = { 0x1A, 0, 0, 0 }; // SCB parameter block, get col - 1
        *COLUMNS = bdos( 49, scbpb ) + 1;
        scbpb[0] = 0x1C; // lines - 1
        *LINES = bdos( 49, scbpb ) + 1;
    }

    uint16_t total;
    uint16_t largest;
    // total   = address where the total number of free bytes in the heap will be stored
    // largest = address where the size of the largest available block in the heap will be stored
    mallinfo( &total, &largest );

    // calculate number of file entries
    MAX_FILES = largest / sizeof( FileEntry ) / 2 - 1;

    // Set current drive for panels
    char drive_left  = '@';
    char drive_right = '@';

    // cmd line argument "--config" shows address of screen size constants
    // in zmc.com to help the user to patch with a HEX editor, e.g. BE.
    while ( --argc ) {
        ++argv;
        if ( !strcmp( *argv, "--CONFIG" ) ) {
            printf( "CP/M version: %02X\n", cpmversion );
#ifdef i8080
            printf( "8080 code\n" );
#endif
            printf( "COLUMNS @ 0x%04X: %d\n", COLUMNS - 0x100, *COLUMNS );
            printf( "LINES @ 0x%04X: %d\n", LINES - 0x100, *LINES );
            printf( "MAX_FILES: %u\n", MAX_FILES );
            return 0;
        } else if ( !strcmp( *argv, "--DEVEL" ) ) {
            ++DEVEL;
        } else if ( !strcmp( *argv, "--DEBUG" ) ) {
            ++DEBUG;
        } else if ( !strcmp( *argv, "--KEY" ) ) {
            // test for terminal function keys, exit with <ESC><ESC>
            uint8_t k;
            printf( "CP/M version %02X: function key test - exit with <ESC><ESC>\n", cpmversion );
            for(;;) {
                static uint8_t esc = 0;
                k = wait_key_hw();
                printf( "0x%02X  ", k );
                if ( k == ESC )
                    puts( "ESC" );
                else if (k < ESC )
                    printf( "^%c\n", k + '@');
                else    // show printable chars, else '.'
                    printf( "%c\n", k >= ' ' && k < 128 ? k : '.' );
                if ( esc && k == ESC ) // <ESC><ESC>
                    return 0;
                esc = k == ESC; // remember <ESC>
            }
        } else if ( *argv[0] >= 'A' && *argv[0] <= 'P' ) {
             if ( argc == 1 )
                 drive_right = *argv[0];
             else
                 drive_left  = *argv[0];
        }
    }

    if ( DEBUG )
        --*LINES; // debugging output in the last line

    FileEntry *f_left;
    FileEntry *f_right;

    f_left = calloc( MAX_FILES, sizeof( FileEntry ) ); // reserve and init heap space
    if ( f_left == NULL ) {
        fprintf( stderr, "Not enough memory!\n" );
        return -1;
    }
    f_right = calloc( MAX_FILES, sizeof( FileEntry ) ); // reserve and init heap space
    if ( f_right == NULL ) {
        fprintf( stderr, "Not enough memory!\n" );
        return -1;
    }

    App.left.files = f_left;
    App.right.files = f_right;
    App.left.drive = drive_left;
    App.right.drive = drive_right;

    App.active_panel = &App.left;

    load_directory(&App.left);
    load_directory(&App.right);

    clr_scr();
    goto_xy( 1, 1 );
    hide_cursor();
    refresh_ui( PAN_BOTH ); // refresh/init both panels

    uint8_t loop = 1;
    uint8_t k;

    char *cp = cmdline;
    *cp = '\0';

    // list of text commands from prompt line and called function
    command_t commands[] = {
        { "HELP", help },

        { "VIEW", view_file },
        { "TYPE", view_file },
        { "CAT", view_file },

        { "DUMP", dump_file },
        { "HEX", dump_file },

        { "COPY", copy },
        { "CP", copy },

        { "DEL", delete },
        { "ERA", delete },
        { "RM", delete },

        { "TOP", first_file },
        { "POS1", first_file },

        { "BOT", last_file },
        { "END", last_file },
    };

    int num_commands = sizeof(commands) / sizeof(commands[0]);

    while( loop ) { // terminal key input loop
        k = wait_key_hw();
        show_cursor();
        // printable char go to the prompt line, BS/RUB deletes, CR executes
        if ( k > SPC && k < RUB ) {
            if ( cp < cmdline + CMDLINELEN ) {
                *cp++ = toupper( k );
                *cp = '\0';
            }
        } else if ( k == BS || k == RUB ) {
            if ( cp > cmdline )
                *--cp = '\0';
        } else if ( k == CR ) { // cmd line parser
            // "A:" .. "P:"
            if ( cmdline[1] == ':' && *cmdline >= 'A' && *cmdline <= 'P' ) {
                change_drive( *cmdline );
            }
            else if ( !strncmp( cmdline, "EXIT", 4 )
                || !strncmp( cmdline, "QUIT", 4 ) ) {
                loop = 0;
            }
            else if ( *cmdline ) { // scan cmd list and get function
                command_func_t function = find_command( cmdline, commands, num_commands );
                if ( function ) // if found ..
                    function(); // .. execute it
            }
            // clear cmd string
            cp = cmdline;
            *cp = '\0';
        }
        else if ( k == TAB ) { // TAB: OTHER_PANEL
            other_panel();
        }
        // here come the function keys, first the WS bindings
        else if (k == ' ' || k == 'V'-'@') { // ' ' or ^V -> SELECT
            select_file();
        } else if ( k == 'E'-'@' ) { // ^E
            line_up();
        } else if ( k == 'X'-'@' ) { // ^X
            line_down();
        } else if ( k == 'R'-'@' ) { // ^R
            page_up();
        } else if ( k == 'C'-'@' ) { // ^C
            page_down();
        } else if ( k == 'Q'-'@' ) { // ^Q
            k = wait_key_hw();
            if ( k == 'S'-'@' ) // ^Q^S
                first_file();
            else if ( k == 'D'-'@' ) // ^Q^D
                last_file();
        }
        // now the multi character function keys starting with <ESC>
        else if ( k == ESC ) { // ESC sequences
            k = wait_key_hw();
            if ( k == ESC ) { // <ESC><ESC>
                loop = 0; // quit program
            }
            // now comes the mc style coding ESC1...ESC0 as proposed by SvenMb
            else if ( k == '1' ) { // <ESC>1
                help();
            }
            else if ( k == '3' ) { // <ESC>3
                view_file();
            }
            else if ( k == '4' ) { // <ESC>4
                dump_file();
            }
            else if ( k == '5' ) { // <ESC>5
                copy();
            }
            else if ( k == '8' ) { // <ESC>8
                delete();
            }
            else if ( k == '0' ) { // <ESC>0 (ZERO)
                loop = 0;
            }
            else {
                loop = parse_function_keys( k ); // VT100 function keys
            }
        }
        if ( loop )
            show_prompt();
    } // while ( loop )
    clr_scr();
    goto_xy( 1, 1 );
    show_cursor();
    set_normal();
    return 0;
}

