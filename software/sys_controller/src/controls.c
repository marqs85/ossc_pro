//
// Copyright (C) 2015-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <string.h>
#include <unistd.h>
#include "altera_avalon_pio_regs.h"
#include "controls.h"
#include "av_controller.h"
#include "menu.h"

static const char *rc_keydesc[REMOTE_MAX_KEYS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", \
                                                   "MENU", "OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT", "INFO", "STANDBY", "SCANLINE_MODE", \
                                                   "SCANLINE_TYPE", "SCANLINE_INT+", "SCANLINE_INT-", "LINEMULT_MODE", "PHASE+", "PHASE-", "PROFILE_HOTKEY"};
#ifndef DExx_FW
const uint16_t rc_keymap_default[REMOTE_MAX_KEYS] = {0x0ab0, 0x0a70, 0x0af0, 0x0a38, 0x0ab8, 0x0a78, 0x0af8, 0x0a20, 0x0aa0, 0x0a30, \
                                              0x0aca, 0x0acc, 0x0a26, 0x0a0e, 0x0a8e, 0x0ace, 0x0a4e, 0x0a08, 0x0ae8, 0x0a56, \
                                              0x0a86, 0x0a48, 0x0aa8, 0x0ae6, 0x0a98, 0x0a18, 0x0a88};
#else
const uint16_t rc_keymap_default[REMOTE_MAX_KEYS] = {0x3E29, 0x3EA9, 0x3E69, 0x3EE9, 0x3E19, 0x3E99, 0x3E59, 0x3ED9, 0x3E39, 0x3EC9, \
                                              0x3E4D, 0x3E1D, 0x3EED, 0x3E2D, 0x3ECD, 0x3EAD, 0x3E6D, 0x3E65, 0x3E01, 0x1C48, \
                                              0x1C18, 0x1C50, 0x1CD0, 0x1CC8, 0x5E58, 0x5ED8, 0x3EB9};
#endif
uint16_t rc_keymap[REMOTE_MAX_KEYS];

uint32_t controls;
uint16_t remote_code;
uint8_t remote_rpt, remote_rpt_prev;
uint8_t btn_vec, btn_vec_prev;

/*void setup_rc()
{
    int i, confirm;
    uint32_t remote_code_prev = 0;

    for (i=0; i<REMOTE_MAX_KEYS; i++) {
        strncpy(menu_row1, "Press", LCD_ROW_LEN+1);
        strncpy(menu_row2, rc_keydesc[i], LCD_ROW_LEN+1);
        lcd_write_menu();
        confirm = 0;

        while (1) {
            remote_code = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;
            btn_code = ~IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & PB_MASK;

            if (remote_code && (remote_code != remote_code_prev)) {
                if (confirm == 0) {
                    rc_keymap[i] = remote_code;
                    strncpy(menu_row1, "Confirm", LCD_ROW_LEN+1);
                    lcd_write_menu();
                    confirm = 1;
                } else {
                    if (remote_code == rc_keymap[i]) {
                        confirm = 2;
                    } else {
                        strncpy(menu_row1, "Mismatch, retry", LCD_ROW_LEN+1);
                        lcd_write_menu();
                        confirm = 0;
                    }
                }
            }

            if ((btn_code_prev == 0) && (btn_code == PB0_BIT)) {
                if (i == 0) {
                    memcpy(rc_keymap, rc_keymap_default, sizeof(rc_keymap));
                    i=REMOTE_MAX_KEYS;
                } else {
                    i-=2;
                }
                confirm = 2;
            }

            remote_code_prev = remote_code;
            btn_code_prev = btn_code;

            if (confirm == 2)
                break;

            usleep(WAITLOOP_SLEEP_US);
        }
    }
    write_userdata(INIT_CONFIG_SLOT);
}*/

void set_default_keymap() {
    memcpy(rc_keymap, rc_keymap_default, sizeof(rc_keymap));
}

void read_controls() {
    // Read remote control and PCB button status
    controls = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
    remote_code = (controls & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;
    remote_rpt = (controls & CONTROLS_RRPT_MASK) >> CONTROLS_RRPT_OFFS;
    btn_vec = (~controls & CONTROLS_BTN_MASK) >> CONTROLS_BTN_OFFS;

    if ((remote_rpt == 0) || ((remote_rpt > 1) && (remote_rpt < 6)) || (remote_rpt == remote_rpt_prev))
        remote_code = 0;

    remote_rpt_prev = remote_rpt;

    if (btn_vec_prev == 0) {
        btn_vec_prev = btn_vec;
    } else {
        btn_vec_prev = btn_vec;
        btn_vec = 0;
    }
}

void parse_control()
{
    rc_code_t c;
    btn_vec_t b = (btn_vec_t)btn_vec;
    int prof_x10=0, ret=0, retval;

    if (remote_code)
        printf("RC_CODE: 0x%.4x\n", remote_code);

    if (btn_vec)
        printf("BTN_CODE: 0x%.2x\n", btn_vec);

    for (c = RC_BTN1; c < REMOTE_MAX_KEYS; c++) {
        if (remote_code == rc_keymap[c])
            break;
    }

    if (sys_is_powered_on()) {
        if (!is_menu_active()) {
            if ((c <= RC_BTN0) || (c == RC_UP) || (c == RC_DOWN))
                switch_input(c, b);
            else if (c == RC_MENU)
                display_menu(c);
            if ((c == RC_LEFT) || (c == RC_RIGHT))
                switch_tp_mode(c);
            if (c == RC_INFO)
                print_vm_stats();
        } else {
            if (c <= RC_RIGHT)
                display_menu(c);
        }
    }

    if (c == RC_STANDBY)
        sys_toggle_power();
}
