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

// interface functions for terminal function key input
// most VT100 keys are supported
// some VT52 keys (up, down, F1, F3, F4) are also recognised

#include <stdint.h>
#include <stdio.h>

#include "zmc.h"

// handle function keys starting with <ESC>
uint8_t parse_function_keys( uint8_t k ) {
    uint8_t loop = 1;
    // the VT100 CSI commands starting with <ESC>[
    if ( k == '[' ) {
        k = wait_key_hw();
        if ( k == 'A' ) { // "<ESC>[A" LINE_UP
            line_up();
        } else if ( k == 'B' ) { // "<ESC>[B" LINE_DOWN
            line_down();
        } else if ( k == '5' && wait_key_hw() == '~' ) { // "<ESC>[5~" PAGE_UP
            page_up();
        } else if ( k == '6' && wait_key_hw() == '~' ) { // "<ESC>[6~" PAGE_DOWN
            page_down();
        } else if ( k == 'H' ) { // <HOME> = "<ESC>[H"
            first_file();
        } else if ( k == 'F' ) { // <END> = "<ESC>[F"
            last_file();
        } else if ( k == '1' ) { // F5 = "<ESC>[15~" / F8 = "<ESC>[19~"
            k = wait_key_hw();
            if ( ( k == '5' || k == '6' ) && wait_key_hw() == '~' ) {
                // F5 = "<ESC>[15~" COPY (HACK: minicom sends "<ESC>[16~")
                copy_cmd();
            } else if ( k == '9' && wait_key_hw() == '~' ) { // F8 = "<ESC>[19~" DELETE
                delete_cmd();
            } else if ( k > '6' && k < '9' ) {
                wait_key_hw(); // remove '~'
            }
        } else if ( k == '2' ) {
            k = wait_key_hw();
            if ( k == '~' ) { // <INSERT> = "<ESC>[2~"
                select_file();
            } else if ( k == '1' && wait_key_hw() == '~' ) { // F10 = "<ESC>[21~"
                loop = 0;                                    // ready, leave loop
            } else if ( k >= '0' && k <= '9' ) {
                wait_key_hw(); // remove '~'
            }
        }
    }
    // the VT100 PF1 ... PF4 keys <ESC>OP ... <ESC>OS
    else if ( k == 'O' ) { // char OSCAR
        k = wait_key_hw();
        if ( k == 'P' ) { // F1 = "<ESC>OP" HELP
            help();
        } else if ( k == 'R' ) { // F3 = "<ESC>OR" VIEW
            view_file();
        } else if ( k == 'S' ) { // F4 = "<ESC>OS" DUMP
            dump_file();
        }
        // here comes the shirsch style extrapolation of F5, F8, F10
        else if ( k == 'T' ) { // F5 = "<ESC>OT" DUMP
            copy_cmd();
        } else if ( k == 'W' ) { // F8 = "<ESC>OW" DUMP
            delete_cmd();
        } else if ( k == 'Y' ) { // F10 = "<ESC>OY" DUMP
            loop = 0;            // ready, leave loop
        }
    } // now the VT52 cursor keys and F1, F3, F4
    else if ( k == 'A' ) { // "<ESC>A" LINE_UP
        line_up();
    } else if ( k == 'B' ) { // "<ESC>B" LINE_DOWN
        line_down();
    } else if ( k == 'P' ) { // "<ESC>P" F1
        help();
    } else if ( k == 'R' ) { // "<ESC>R" F3
        view_file();
    } else if ( k == 'S' ) { // "<ESC>R" F4
        dump_file();
    }
    return loop;
}
