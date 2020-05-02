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

#ifndef AV_CONTROLLER_H_
#define AV_CONTROLLER_H_

#include "sysconfig.h"
#include "controls.h"
#include "video_modes.h"

// sys_ctrl
#define SCTRL_ISL_RESET_N     (1<<1)
#define SCTRL_HDMIRX_RESET_N  (1<<2)
#define SCTRL_EMIF_HWRESET_N  (1<<3)
#define SCTRL_EMIF_SWRESET_N  (1<<4)
#define SCTRL_CAPTURE_SEL     (1<<5)
#define SCTRL_ISL_VS_POL      (1<<6)
#define SCTRL_ISL_VS_TYPE     (1<<7)
#define SCTRL_AUDMUX_SEL      (1<<8)
#define SCTRL_VGTP_ENABLE     (1<<9)

// sys_status
#define SSTAT_RC_MASK   0x0000ffff
#define SSTAT_RC_OFFS   0
#define SSTAT_RRPT_MASK 0x00ff0000
#define SSTAT_RRPT_OFFS 16
#define SSTAT_BTN_MASK  0x03000000
#define SSTAT_BTN_OFFS  24

typedef enum {
    AV_TESTPAT      = 0,
    AV1_RGBS        = 1,
    AV1_RGsB        = 2,
    AV1_YPbPr       = 3,
    AV1_RGBHV       = 4,
    AV1_RGBCS       = 5,
    AV2_YPbPr       = 6,
    AV2_RGsB        = 7,
    AV3_RGBHV       = 8,
    AV3_RGBCS       = 9,
    AV3_RGBS        = 10,
    AV3_RGsB        = 11,
    AV3_YPbPr       = 12,
    AV4             = 13,
    AV_LAST         = 14
} avinput_t;

void chardisp_write_status();

void switch_input(rc_code_t code, btn_vec_t pb_vec);

void switch_audmux(uint8_t audmux_sel);

void switch_tp_mode(rc_code_t code);

#endif
