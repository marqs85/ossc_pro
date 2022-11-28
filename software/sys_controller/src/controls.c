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
#include "userdata.h"
#include "us2066.h"

static const char *rc_keydesc[REMOTE_MAX_KEYS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
                                                   "MENU", "OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT",
                                                   "INFO", "STANDBY",
                                                   "SCANLINE_MODE", "SCANLINE_TYPE", "SCANLINE_ALIGNM", "SCANLINE_INT+", "SCANLINE_INT-",
                                                   "P-LINEMULT", "A-LINEMULT", "LINEMULT_PROC", "LINEMULT_DIL", "PHASE+", "PHASE-",
                                                   "SCALER", "SCALER_RES", "SCALER_FLOCK", "SCALER_SCL_ALG", "SCALER_DIL_ALG", "SCALER_ASPECT", "SCALER_ROT", "SCALER_ZOOM", "SCALER_PAN",
                                                   "RED", "GREEN", "YELLOW", "BLUE", "OSD", "SCREENSHOT", "PROFILE_HOTKEY"};
#ifndef DExx_FW
const uint16_t rc_keymap_default[REMOTE_MAX_KEYS] = {0x0ab0, 0x0a70, 0x0af0, 0x0a38, 0x0ab8, 0x0a78, 0x0af8, 0x0a20, 0x0aa0, 0x0a30,
                                                    0x0aca, 0x0acc, 0x0a26, 0x0a0e, 0x0a8e, 0x0ace, 0x0a4e,
                                                    0x0a08, 0x0ae8,
                                                    0x0a50, 0x0ad0, 0x0a35, 0x0a48, 0x0aa8,
                                                    0x0a90, 0x0a7e, 0x0a7a, 0x0afa, 0x0a98, 0x0a18,
                                                    0x0a1e, 0x0a84, 0x0a44, 0x0ac4, 0x0a24, 0x0a3c, 0x0a14, 0x0ae4, 0x0ac8,
                                                    0x0a32, 0x0aa2, 0x0a62, 0x0ae2, 0x0ac5, 0x0a10, 0x0a88};
#else
const uint16_t rc_keymap_default[REMOTE_MAX_KEYS] = {0x3E29, 0x3EA9, 0x3E69, 0x3EE9, 0x3E19, 0x3E99, 0x3E59, 0x3ED9, 0x3E39, 0x3EC9,
                                                    0x3E4D, 0x3E1D, 0x3EED, 0x3E2D, 0x3ECD, 0x3EAD, 0x3E6D,
                                                    0x3E65, 0x3E01,
                                                    0x1C48, 0x1C18, 0x0001, 0x1C50, 0x1CD0,
                                                    0x0001, 0x0001, 0x1CC8, 0x0001, 0x5E58, 0x5ED8,
                                                    0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x3EC1, 0x3ED1,
                                                    0x3EC1, 0x3E41, 0x3EA1, 0x3ED1, 0x3EB5, 0x0001, 0x3EB9};
#endif
uint16_t rc_keymap[REMOTE_MAX_KEYS];

uint32_t controls;
uint16_t remote_code_raw;
uint8_t remote_rpt, remote_rpt_prev;
uint8_t btn_vec, btn_vec_prev, btn_rpt;

rc_code_t r_code;
btn_code_t b_code;

extern int enable_tp, enable_isl;
extern uint8_t smp_cur, smp_edit, dtmg_cur, dtmg_edit;
extern oper_mode_t oper_mode;
extern char menu_row1[US2066_ROW_LEN+1], menu_row2[US2066_ROW_LEN+1];

extern menuitem_t menu_scanlines_items[];
extern menuitem_t menu_output_items[];
extern menuitem_t menu_settings_items[];
extern menuitem_t menu_advtiming_items[];
#ifdef VIP
extern menuitem_t menu_scaler_items[];
#endif

int setup_rc()
{
    int i, confirm, retval=0;
    uint32_t remote_code_raw_prev;

    remote_code_raw_prev = (IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;

    for (i=0; i<REMOTE_MAX_KEYS; i++) {
        sniprintf(menu_row1, US2066_ROW_LEN+1, "Press");
        strncpy(menu_row2, rc_keydesc[i], US2066_ROW_LEN+1);
        ui_disp_menu(1);
        confirm = 0;

        while (1) {
            controls = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
            remote_code_raw = (controls & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;
            btn_vec = (~controls & CONTROLS_BTN_MASK) >> CONTROLS_BTN_OFFS;

            if (remote_code_raw && (remote_code_raw != remote_code_raw_prev)) {
                if (confirm == 0) {
                    rc_keymap[i] = remote_code_raw;
                    sniprintf(menu_row1, US2066_ROW_LEN+1, "Confirm 0x%.4x", remote_code_raw);
                    sniprintf(menu_row2, US2066_ROW_LEN+1, "by pressing again");
                    ui_disp_menu(1);
                    confirm = 1;
                } else {
                    if (remote_code_raw == rc_keymap[i]) {
                        confirm = 2;
                    } else {
                        sniprintf(menu_row1, US2066_ROW_LEN+1, "Mismatch, retry");
                        strncpy(menu_row2, rc_keydesc[i], US2066_ROW_LEN+1);
                        ui_disp_menu(1);
                        confirm = 0;
                    }
                }
            }

            if ((btn_vec_prev & PB_PRESS) && !btn_vec) {
                if (i == 0) {
                    retval = 1;
                    i=REMOTE_MAX_KEYS;
                } else {
                    i-=2;
                }
                confirm = 2;
            }

            remote_code_raw_prev = remote_code_raw;
            btn_vec_prev = btn_vec;

            if (confirm == 2)
                break;

            usleep(MAINLOOP_INTERVAL_US);
        }
    }

    if (retval == 1) {
        sniprintf(menu_row1, US2066_ROW_LEN+1, "Load default map?");
#ifdef DE10N
        sniprintf(menu_row2, US2066_ROW_LEN+1, "Y:KEY0 N:KEY1");
#else
        sniprintf(menu_row2, US2066_ROW_LEN+1, "Y:> N:<");
#endif
        ui_disp_menu(1);

#ifdef DE10N
        if (prompt_yesno(RC_DOWN, JOY_DOWN, RC_MENU, PB_PRESS) == 1) {
#else
        if (prompt_yesno(RC_RIGHT, JOY_RIGHT, RC_LEFT, JOY_LEFT) == 1) {
#endif
            set_default_keymap();
            set_func_ret_msg("Default map set");
        } else {
            set_func_ret_msg("Cancelled");
        }
    }

#ifdef DE10N
    write_userdata_sd(SD_INIT_CONFIG_SLOT);
#else
    write_userdata(INIT_CONFIG_SLOT);
#endif

    return retval;
}

int prompt_yesno(rc_code_t rem_yes, btn_code_t btn_yes, rc_code_t rem_no, btn_code_t btn_no) {
    int retval = -1;
    uint32_t remote_code_raw_prev;

    remote_code_raw_prev = (IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;

    while (1) {
        controls = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
        remote_code_raw = (controls & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;
        btn_vec = (~controls & CONTROLS_BTN_MASK) >> CONTROLS_BTN_OFFS;

        if ((remote_code_raw && (remote_code_raw != remote_code_raw_prev)) || (btn_vec_prev && !btn_vec)) {
            if ((remote_code_raw == rc_keymap[rem_yes]) || (btn_vec_prev & btn_yes))
                retval = 1;
            else if ((remote_code_raw == rc_keymap[rem_no]) || (btn_vec_prev & btn_no))
                retval = 0;
        }

        remote_code_raw_prev = remote_code_raw;
        btn_vec_prev = btn_vec;

        if (retval >= 0)
            return retval;

        usleep(MAINLOOP_INTERVAL_US);
    }
}

void set_default_keymap() {
    memcpy(rc_keymap, rc_keymap_default, sizeof(rc_keymap));
}

void read_controls() {
    // Read remote control and PCB button status
    controls = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
    remote_code_raw = (controls & CONTROLS_RC_MASK) >> CONTROLS_RC_OFFS;
    remote_rpt = (controls & CONTROLS_RRPT_MASK) >> CONTROLS_RRPT_OFFS;
    btn_vec = (~controls & CONTROLS_BTN_MASK) >> CONTROLS_BTN_OFFS;

    if ((remote_rpt == 0) || ((remote_rpt > 1) && (remote_rpt < 6)) || (remote_rpt == remote_rpt_prev))
        remote_code_raw = 0;

    remote_rpt_prev = remote_rpt;

    for (r_code = RC_BTN1; r_code < REMOTE_MAX_KEYS; r_code++) {
        if (remote_code_raw == rc_keymap[r_code])
            break;
    }

    if (btn_vec & PB_PRESS)
        btn_rpt++;
    else
        btn_rpt = 0;

    if (btn_rpt > 100) {
        b_code = BC_HOLD;
        btn_rpt = 0;
    } else if (btn_vec_prev && !btn_vec) {
        if (btn_vec_prev & PB_PRESS)
            b_code = BC_PRESS;
        else if (btn_vec_prev & JOY_PRESS)
            b_code = BC_OK;
        else if (btn_vec_prev & JOY_LEFT)
            b_code = BC_LEFT;
        else if (btn_vec_prev & JOY_RIGHT)
            b_code = BC_RIGHT;
        else if (btn_vec_prev & JOY_UP)
            b_code = BC_UP;
        else if (btn_vec_prev & JOY_DOWN)
            b_code = BC_DOWN;
    } else {
        b_code = (btn_code_t)-1;
    }

    if (remote_code_raw)
        printf("RC_CODE: 0x%.4x\n", remote_code_raw);

    if (btn_vec && (btn_vec != btn_vec_prev))
        printf("BTN_CODE: 0x%.2x\n", btn_vec);

    if (b_code != (btn_code_t)-1)
        printf("BTN: %u\n", b_code);

    // keep joy press sticky
    if ((btn_vec_prev & JOY_PRESS) && btn_vec)
        btn_vec |= JOY_PRESS;

    btn_vec_prev = btn_vec;
}

void parse_control()
{
    int prof_x10=0, ret=0, retval;

    if (sys_is_powered_on()) {
        if (!is_menu_active()) {
            if ((r_code <= RC_BTN0) || (r_code == RC_UP) || (r_code == RC_DOWN) || (b_code == BC_UP) || (b_code == BC_DOWN))
                switch_input(r_code, b_code);
            else if ((r_code == RC_MENU) || (b_code == BC_PRESS))
                display_menu(r_code, b_code);
            if (enable_tp) {
                if ((r_code == RC_LEFT) || (b_code == BC_LEFT))
                    quick_adjust(&menu_output_items[1], -1);
                if ((r_code == RC_RIGHT) || (b_code == BC_RIGHT))
                    quick_adjust(&menu_output_items[1], 1);
            }
            if (enable_isl) {
                if ((r_code == RC_PHASE_PLUS) || (r_code == RC_PHASE_MINUS))
                    quick_adjust_phase(r_code == RC_PHASE_PLUS);
            }
            if (r_code == RC_INFO)
                print_vm_stats();
            if (r_code == RC_SL_MODE)
                enter_cstm(&menu_scanlines_items[0], 1);
            if (r_code == RC_SL_TYPE)
                enter_cstm(&menu_scanlines_items[5], 1);
            if (r_code == RC_SL_ALIGNM)
                quick_adjust(&menu_scanlines_items[4], 1);
            if ((r_code == RC_SL_PLUS) || (r_code == RC_SL_MINUS))
                quick_adjust(&menu_scanlines_items[1], (r_code == RC_SL_PLUS) ? 1 : -1);
#ifdef VIP
            if (oper_mode == OPERMODE_SCALER) {
                if (r_code == RC_SCL_RES)
                    enter_cstm(&menu_scaler_items[0], 1);
                if (r_code == RC_SCL_FL)
                    enter_cstm(&menu_scaler_items[3], 1);
                if (r_code == RC_SCL_SCL_ALG)
                    enter_cstm(&menu_scaler_items[5], 1);
                if (r_code == RC_SCL_DIL_ALG)
                    enter_cstm(&menu_scaler_items[7], 1);
                if (r_code == RC_SCL_AR)
                    enter_cstm(&menu_scaler_items[4], 1);
                /*if (r_code == RC_SCL_ROT)
                    TODO*/
            }
#endif
            if ((oper_mode == OPERMODE_ADAPT_LM) || (oper_mode == OPERMODE_SCALER)) {
                if ((r_code == RC_SCL_SIZE) || (r_code == RC_SCL_POS)) {
                    smp_edit = smp_cur;
                    dtmg_edit = dtmg_cur;
                    enter_cstm(&menu_advtiming_items[(r_code == RC_SCL_SIZE) ? 1 : 2], 1);
                }
            }
            if (r_code == RC_OSD)
                enter_cstm(&menu_settings_items[1], 1);
        } else {
            if ((r_code <= RC_RIGHT) || ((b_code >= BC_OK) && (b_code <= BC_RIGHT)))
                display_menu(r_code, b_code);
        }
    }

    if ((r_code == RC_STANDBY) || (b_code == BC_HOLD))
        sys_toggle_power();
}
