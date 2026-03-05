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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "zmc.h"


// draw one line with file name, attributes, size, date etc.
void draw_file_info( Panel *p, int16_t f_idx ) {
    if ( App.active_panel == p && p->num_files && f_idx == p->current_idx)
        set_invers();

    printf("%c%-12s %c%c%c",
           p->files[f_idx].attrib & B_SEL ? '*' : ' ',
           p->files[f_idx].cpmname,
           p->files[f_idx].attrib & B_RO ? 'R' : ' ',
           p->files[f_idx].attrib & B_SYS ? 'S' : ' ',
           p->files[f_idx].attrib & B_ARCH ? 'A' : ' '
    );

    if ( !*(p->files[f_idx].cpmname) )
        printf( "      " );
    else if ( p->files[f_idx].extent < 512) // file size < 64K
        printf( "%6u", p->files[f_idx].extent << 7 ); // *128
    else if ( p->files[f_idx].extent < 7812) // 64K <= file size < 1E6
        printf( "%6lu", (uint32_t)p->files[f_idx].extent << 7 ); // *128 -> uint32_t
    else // 1E6 <= file size < 8 MB
        printf( "%5uK", (uint16_t)(p->files[f_idx].extent + 7) >> 3 ); // *128/1024

    uint8_t w;
    if ( p->show_date ) {
        if ( p->files[f_idx].date) { // date and time defined
            printf(" %04d%s%02d%s%02d %02X%s%02X",
                p->files[f_idx].date,
                PANEL_WIDTH < 42 ? "" : "-",
                p->files[f_idx].month,
                PANEL_WIDTH < 42 ? "" : "-",
                p->files[f_idx].day,
                p->files[f_idx].hour,
                PANEL_WIDTH < 42 ? "" : ":",
                p->files[f_idx].minute
            );
        } else { // spaces instead of date
            w = PANEL_WIDTH < 42 ? 14 : 17;
            while ( w-- )
                putchar( ' ' );
        }
        if ( App.active_panel == p && f_idx == p->current_idx)
            set_normal();
        // fill to end of panel line
        w = PANEL_WIDTH < 42 ? PANEL_WIDTH - 39 : PANEL_WIDTH - 42;
        while ( w-- )
            putchar( ' ' );
    } else { // no date for complete drive or panel too narrow
        if ( App.active_panel == p && f_idx == p->current_idx)
            set_normal();
        w = PANEL_WIDTH - 25;
        while ( w-- )
            putchar( ' ' );
    }
}


// draw the empty wire frame
void draw_frame( Panel *p) {
    uint8_t i;

    if ( &App.left == p )
        goto_xy( 1, 1 );
    else
        goto_xy( PANEL_WIDTH + 1, 1 );
    putchar(' ');
    i = PANEL_WIDTH - 2;
    while ( i-- )
        putchar('_');
    putchar(' ');
    for (i = 0; i < VISIBLE_ROWS; i++) {
        int16_t f_idx = i + p->scroll_offset;
        if ( &App.left == p ) {
            putchar_xy( PANEL_WIDTH , i + 2, '|' );
            goto_xy( PANEL_WIDTH - 1 , i + 2 );
            clr_line_left();
            putchar_xy( 1, i + 2, '|' );
        } else {
            putchar_xy( PANEL_WIDTH + 1, i + 2, '|' );
            clr_line_right();
            putchar_xy( 2 * PANEL_WIDTH, i + 2, '|' );
        }
    }
    if ( &App.left == p )
        goto_xy( 1, PANEL_HEIGHT );
    else
        goto_xy( PANEL_WIDTH + 1, PANEL_HEIGHT );
    i = PANEL_WIDTH - 2;
    putchar( '|' );
    while ( i-- )
        putchar( '_' );
    putchar('|');
}


// fill the prepared panel with the file lines
void fill_panel( Panel *p ) {
    uint8_t x_offset = p == &App.left ? 2 : PANEL_WIDTH + 2;
    uint8_t i;

    if (p->current_idx < p->scroll_offset) {
        p->scroll_offset = p->current_idx;
    }
    if (p->current_idx >= p->scroll_offset + VISIBLE_ROWS) {
        p->scroll_offset = p->current_idx - (VISIBLE_ROWS - 1);
    }
    set_normal();
    for (i = 0; i < VISIBLE_ROWS; i++) {
        int f_idx = i + p->scroll_offset;
        if (f_idx < p->num_files) {
            goto_xy( x_offset, i + 2 );
            draw_file_info( p, f_idx );
        }
    }
}


// put one file info at defined position
void draw_file_line(Panel *p, int16_t file_idx) {
    uint8_t x_offset = p == &App.left ? 2 : PANEL_WIDTH + 2;
    if (file_idx >= p->scroll_offset && file_idx < p->scroll_offset + VISIBLE_ROWS) {
        goto_xy( x_offset, file_idx - p->scroll_offset + 2 );
        draw_file_info( p, file_idx );
    }
}


void print_cpm_attrib( uint8_t *ca) {
    // show file attributes
    printf( "%c%c%c",
            *ca++ > 0x7F ? 'R' : ' ', // READ ONLY
            *ca++ > 0x7F ? 'S' : ' ', // SYSTEM
            *ca++ > 0x7F ? 'B' : ' '  // file was BACKED UP
    );
}


// show the disk on top of panel, mark the active panel
void draw_header( Panel *p ) {
    uint8_t x_offset = p == &App.left ? 3 : PANEL_WIDTH + 3;
    goto_xy( x_offset, 1 );
    if ( p == App.active_panel )
        set_invers();
    printf( "[ DISK %c: ]", p->drive );
    if ( p == App.active_panel )
        set_normal();
}


// show function key help
void draw_footer( void ) {
    goto_xy( 1, PANEL_HEIGHT+2 );
    set_invers();
    if ( PANEL_WIDTH >= 40 ) {
        printf("| A: - P: | TAB:Sw | F1:Help | F3:View | F4:Dump | F5:Copy | F8:Del | F10:Exit |");
    } else if ( PANEL_WIDTH >= 30 ) {
        printf("A:-P:|TAB:Sw|F1:Help|F3:View|F4:Dump|F5:Copy|F8:Del|F10:Exit");
    }
}


// cmd line with active drive
void show_prompt() {
    goto_xy( 1, PANEL_HEIGHT+1 );
    set_normal();
    printf("%c> %s", App.active_panel->drive, cmdline );
    show_cursor();
    clr_line_right();
}


// update none, one or both panels
void refresh_ui(uint8_t which_panel) {
    hide_cursor();
    if ( which_panel & 0b01) {
        draw_frame( App.active_panel );
        draw_header( App.active_panel );
        fill_panel( App.active_panel );
    }
    if ( which_panel & 0b10) {
        draw_frame( App.inactive_panel );
        draw_header( App.inactive_panel );
        fill_panel( App.inactive_panel );
    }
    draw_footer();
    show_prompt();
}

void other_panel() {
    // change focus
    Panel *tmp = App.active_panel;
    App.active_panel = App.inactive_panel;
    App.inactive_panel = tmp;
    draw_header( &App.left );
    draw_header( &App.right );

    // chirurgical update: refresh only the lines with cursors
    draw_file_line(&App.left, App.left.current_idx);
    draw_file_line(&App.right, App.right.current_idx);
    show_prompt();
}


void select_file() {
    if ( !App.active_panel->num_files )
        return;
    int16_t idx = App.active_panel->current_idx;

    // A. invert the selection state in memory
    App.active_panel->files[idx].attrib ^= B_SEL;

    // B. redraw current line to show '*'
    // IMPORTANT: current_idx was not changed, line is drawn with cursor.
    draw_file_line(App.active_panel, idx);

    // C. move the cursor to the next line
    line_down();
}


void goto_line( int16_t new_idx ) {
    int16_t old_idx = App.active_panel->current_idx;
    if ( new_idx < 0 ||  old_idx == new_idx ) // no files or already there
        return;
    App.active_panel->current_idx = new_idx;
    // if scrolling, redraw everything; if not, only two lines
    if ( new_idx < App.active_panel->scroll_offset
      || new_idx >= App.active_panel->scroll_offset + VISIBLE_ROWS )
        fill_panel( App.active_panel );
    else {
        draw_file_line(App.active_panel, old_idx); // deselect
        draw_file_line(App.active_panel, new_idx); // select
    }

}


void line_up() {
    if (App.active_panel->current_idx > 0)
        goto_line( App.active_panel->current_idx - 1 );
}


void line_down() {
    if (App.active_panel->current_idx + 1 < App.active_panel->num_files )
        goto_line( App.active_panel->current_idx + 1 );
}


void page_up() {
    if (App.active_panel->current_idx >= VISIBLE_ROWS)
        goto_line( App.active_panel->current_idx - VISIBLE_ROWS );
    else
        goto_line( 0 );
}


void page_down() {
    if ( App.active_panel->current_idx + VISIBLE_ROWS < App.active_panel->num_files )
        goto_line( App.active_panel->current_idx + VISIBLE_ROWS );
    else
        goto_line( App.active_panel->num_files - 1 );
}


void first_file() {
    goto_line( 0 );
}


void last_file() {
    goto_line( App.active_panel->num_files - 1 );
}

