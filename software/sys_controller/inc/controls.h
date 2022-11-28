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

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <stdint.h>
#include "sysconfig.h"

typedef enum {
    /* Keypad */
    RC_BTN1                 = 0,
    RC_BTN2,
    RC_BTN3,
    RC_BTN4,
    RC_BTN5,
    RC_BTN6,
    RC_BTN7,
    RC_BTN8,
    RC_BTN9,
    RC_BTN0,
    /* Menu navigation */
    RC_MENU,
    RC_OK,
    RC_BACK,
    RC_UP,
    RC_DOWN,
    RC_LEFT,
    RC_RIGHT,
    /* Essential special keys */
    RC_INFO,
    RC_STANDBY,
    /* Scanline hotkeys */
    RC_SL_MODE,
    RC_SL_TYPE,
    RC_SL_ALIGNM,
    RC_SL_PLUS,
    RC_SL_MINUS,
    /* LM hotkeys */
    RC_LM_PURE,
    RC_LM_ADAPT,
    RC_LM_PROC,
    RC_LM_DIL,
    RC_PHASE_PLUS,
    RC_PHASE_MINUS,
    /* Scaler hotkeys */
    RC_SCL,
    RC_SCL_RES,
    RC_SCL_FL,
    RC_SCL_SCL_ALG,
    RC_SCL_DIL_ALG,
    RC_SCL_AR,
    RC_SCL_ROT,
    RC_SCL_SIZE,
    RC_SCL_POS,
    /* Other special keys */
    RC_RED,
    RC_GREEN,
    RC_YELLOW,
    RC_BLUE,
    RC_OSD,
    RC_SCRSHOT,
    RC_PROF_HOTKEY
} rc_code_t;

typedef enum {
    BC_HOLD                 = 0,
    BC_OK,
    BC_PRESS,
    BC_UP,
    BC_DOWN,
    BC_LEFT,
    BC_RIGHT
} btn_code_t;

typedef enum {
    JOY_PRESS       = (1<<0),
    JOY_UP          = (1<<1),
    JOY_RIGHT       = (1<<2),
    JOY_DOWN        = (1<<3),
    JOY_LEFT        = (1<<4),
    PB_PRESS        = (1<<5),
} btn_vec_t;

#define REMOTE_MAX_KEYS (RC_PROF_HOTKEY+1)

#define CONTROLS_RC_MASK                   0x0000ffff
#define CONTROLS_RC_OFFS                   0
#define CONTROLS_RRPT_MASK                 0x00ff0000
#define CONTROLS_RRPT_OFFS                 16
#define CONTROLS_BTN_MASK                  0x3f000000
#define CONTROLS_BTN_OFFS                  24

int setup_rc();
int prompt_yesno(rc_code_t rem_yes, btn_code_t btn_yes, rc_code_t rem_no, btn_code_t btn_no);
void set_default_keymap();
void read_controls();
void parse_control();

#endif
