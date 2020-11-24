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
    RC_MENU,
    RC_OK,
    RC_BACK,
    RC_UP,
    RC_DOWN,
    RC_LEFT,
    RC_RIGHT,
    RC_INFO,
    RC_STANDBY,
    RC_SL_MODE,
    RC_SL_TYPE,
    RC_SL_PLUS,
    RC_SL_MINUS,
    RC_LM_MODE,
    RC_PHASE_PLUS,
    RC_PHASE_MINUS,
    RC_PROF_HOTKEY
} rc_code_t;

typedef enum {
    PB_BTN0             = (1<<0),
    PB_BTN1             = (1<<1)
} btn_vec_t;

#define REMOTE_MAX_KEYS (RC_PROF_HOTKEY-RC_BTN1+1)

#define CONTROLS_RC_MASK                   0x0000ffff
#define CONTROLS_RC_OFFS                   0
#define CONTROLS_RRPT_MASK                 0x00ff0000
#define CONTROLS_RRPT_OFFS                 16
#define CONTROLS_BTN_MASK                  0x3f000000
#define CONTROLS_BTN_OFFS                  24

//void setup_rc();
void set_default_keymap();
void read_controls();
void parse_control();

#endif
