//
// Copyright (C) 2015-2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include "controls.h"
#include "av_controller.h"

static const char *rc_keydesc[REMOTE_MAX_KEYS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", \
                                                   "MENU", "OK", "BACK", "UP", "DOWN", "LEFT", "RIGHT", "INFO", "LCD_BACKLIGHT", "SCANLINE_MODE", \
                                                   "SCANLINE_TYPE", "SCANLINE_INT+", "SCANLINE_INT-", "LINEMULT_MODE", "PHASE+", "PHASE-", "PROFILE_HOTKEY"};
const uint16_t rc_keymap_default[REMOTE_MAX_KEYS] = {0x3E29, 0x3EA9, 0x3E69, 0x3EE9, 0x3E19, 0x3E99, 0x3E59, 0x3ED9, 0x3E39, 0x3EC9, \
                                              0x3E4D, 0x3E1D, 0x3EED, 0x3E2D, 0x3ECD, 0x3EAD, 0x3E6D, 0x3E65, 0x3E01, 0x1C48, \
                                              0x1C18, 0x1C50, 0x1CD0, 0x1CC8, 0x5E58, 0x5ED8, 0x3EB9};
uint16_t rc_keymap[REMOTE_MAX_KEYS];

/*void setup_rc()
{
    int i, confirm;
    alt_u32 remote_code_prev = 0;

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

int parse_control(avinput_t *input, uint16_t remote_code, uint8_t btn_vec, uint8_t *ymult_new, unsigned *target_tp_stdmode_idx)
{
    int i, prof_x10=0, ret=0, retval;

    avinput_t next_input = (*input == AV4) ? AV1_RGBS : (*input+1);

    if (remote_code)
        printf("RC_CODE: 0x%.4lx\n", remote_code);

    if (btn_vec)
        printf("BTN_CODE: 0x%.2lx\n", btn_vec);

    for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
        if (remote_code == rc_keymap[i])
            break;
        if (i == REMOTE_MAX_KEYS - 1)
            goto Button_Check;
    }

    switch (i) {
        case RC_BTN1: *input = AV1_RGBS; break;
        case RC_BTN4: *input = (*input == AV1_RGsB) ? AV1_YPbPr : AV1_RGsB; break;
        case RC_BTN7: *input = (*input == AV1_RGBHV) ? AV1_RGBCS : AV1_RGBHV; break;
        case RC_BTN2: *input = (*input == AV2_YPbPr) ? AV2_RGsB : AV2_YPbPr; break;
        case RC_BTN3: *input = (*input == AV3_RGBHV) ? AV3_RGBCS : AV3_RGBHV; break;
        case RC_BTN6: *input = AV3_RGBS; break;
        case RC_BTN9: *input = (*input == AV3_RGsB) ? AV3_YPbPr : AV3_RGsB; break;
        case RC_BTN5: *input = AV4; break;
        case RC_BTN0: *input = AV_TESTPAT; break;

        case RC_LEFT:
            if (*input == AV_TESTPAT)
                *target_tp_stdmode_idx = *target_tp_stdmode_idx - 1;
            else
                *ymult_new = 2;
            break;
        case RC_RIGHT:
            if (*input == AV_TESTPAT)
                *target_tp_stdmode_idx = *target_tp_stdmode_idx + 1;
            else
                *ymult_new = 3;
            break;
        case RC_UP: *ymult_new = 4; break;
        case RC_DOWN: *ymult_new = 5; break;
        case RC_OK: *ymult_new = 1; break;
        /*case RC_MENU:
            menu_active = !menu_active;
            osd->osd_config.menu_active = menu_active;
            profile_sel_menu = profile_sel;

            if (menu_active)
                display_menu(1);
            else
                lcd_write_status();

            break;
        case RC_INFO:
            sc_status = sc->sc_status;
            sc_status2 = sc->sc_status2;
            sniprintf(menu_row1, LCD_ROW_LEN+1, "Prof.%u %9s", profile_sel, video_modes[cm.id].name);
            if (cm.sync_active) {
                sniprintf(menu_row2, LCD_ROW_LEN+1, "%4lu%c%c  %lu", (unsigned long)((sc_status.vmax+1)<<sc_status.interlace_flag)+sc_status.interlace_flag,
                                                                     sc_status.interlace_flag ? 'i' : 'p',
                                                                     sc_status.fpga_vsyncgen ? '*' : ' ',
                                                                     (unsigned long)sc_status2.pcnt_frame);
            }
            osd->osd_config.menu_active = 1;
            lcd_write_menu();
            break;
        case RC_LCDBL:
            sys_ctrl ^= LCD_BL;
            break;
        case RC_SL_MODE: tc.sl_mode = (tc.sl_mode < SL_MODE_MAX) ? (tc.sl_mode + 1) : 0; break;
        case RC_SL_TYPE: tc.sl_type = (tc.sl_type < SL_TYPE_MAX) ? (tc.sl_type + 1) : 0; break;
        case RC_SL_MINUS: tc.sl_str = tc.sl_str ? (tc.sl_str - 1) : 0; break;
        case RC_SL_PLUS: tc.sl_str = (tc.sl_str < SCANLINESTR_MAX) ? (tc.sl_str + 1) : SCANLINESTR_MAX; break;
        case RC_LM_MODE:
            strncpy(menu_row1, "Linemult mode:", LCD_ROW_LEN+1);
            strncpy(menu_row2, "press 1-5", LCD_ROW_LEN+1);
            osd->osd_config.menu_active = 1;
            lcd_write_menu();

            while (1) {
                btn_vec = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;
                for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
                    if (btn_vec == rc_keymap[i])
                        break;
                }

                if (video_modes[cm.id].group > GROUP_1080I) {
                    printf("WARNING: Corrupted mode (id %d)\n", cm.id);
                    break;
                }

                if (i <= RC_BTN5) {
                    if ((1<<i) & valid_pm[video_modes[cm.id].group]) {
                        *pmcfg_ptr[video_modes[cm.id].group] = i;
                    } else {
                        sniprintf(menu_row2, LCD_ROW_LEN+1, "%ux unsupported", i+1);
                        lcd_write_menu();
                        usleep(500000);
                    }
                    break;
                } else if (i == RC_BACK) {
                    break;
                }

                usleep(WAITLOOP_SLEEP_US);
            }
            osd->osd_config.menu_active = 0;
            lcd_write_status();
            menu_active = 0;
            break;
        case RC_PHASE_PLUS:
            video_modes[cm.id].sampler_phase = (video_modes[cm.id].sampler_phase < SAMPLER_PHASE_MAX) ? (video_modes[cm.id].sampler_phase + 1) : 0;
            update_cur_vm = 1;
            if (cm.id == vm_edit)
                tc_sampler_phase = video_modes[vm_edit].sampler_phase;
            break;
        case RC_PHASE_MINUS: video_modes[cm.id].sampler_phase = video_modes[cm.id].sampler_phase ? (video_modes[cm.id].sampler_phase - 1) : SAMPLER_PHASE_MAX;
            update_cur_vm = 1;
            if (cm.id == vm_edit)
                tc_sampler_phase = video_modes[vm_edit].sampler_phase;
            break;
        case RC_PROF_HOTKEY:
Prof_Hotkey_Prompt:
            strncpy(menu_row1, "Profile load:", LCD_ROW_LEN+1);
            sniprintf(menu_row2, LCD_ROW_LEN+1, "press %u-%u", prof_x10*10, ((prof_x10*10+9) > MAX_PROFILE) ? MAX_PROFILE : (prof_x10*10+9));
            osd->osd_config.menu_active = 1;
            lcd_write_menu();

            while (1) {
                btn_vec = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE) & RC_MASK;

                if ((btn_vec_prev == 0) && (btn_vec != 0)) {
                    for (i = RC_BTN1; i < REMOTE_MAX_KEYS; i++) {
                        if (btn_vec == rc_keymap[i])
                            break;
                    }

                    if ((i == RC_BTN0) || (i < (RC_BTN1 + (prof_x10 == (MAX_PROFILE/10)) ? (MAX_PROFILE%10) : 9))) {
                        profile_sel_menu = prof_x10*10 + ((i+1)%10);
                        retval = load_profile();
                        sniprintf(menu_row2, LCD_ROW_LEN+1, "%s", (retval==0) ? "Done" : "Failed");
                        lcd_write_menu();
                        usleep(500000);
                        break;
                    } else if (i == RC_PROF_HOTKEY) {
                        prof_x10 = (prof_x10+1) % ((MAX_PROFILE/10)+1);
                        btn_vec_prev = btn_vec;
                        goto Prof_Hotkey_Prompt;
                    } else if (i == RC_BACK) {
                        break;
                    }
                }

                btn_vec_prev = btn_vec;
                usleep(WAITLOOP_SLEEP_US);
            }

            osd->osd_config.menu_active = 0;
            lcd_write_status();
            menu_active = 0;
            break;*/
        /*case RC_RIGHT:
            if (!menu_active)
                *input = next_input;
            break;*/
        default: break;
    }

Button_Check:
    if (btn_vec & PB_BTN0)
        *input = next_input;
    /*if (btn_vec & PB_BTN1)
        tc.sl_mode = tc.sl_mode < SL_MODE_MAX ? tc.sl_mode + 1 : 0;*/

    return 0;
}
