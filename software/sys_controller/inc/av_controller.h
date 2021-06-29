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
#include "osd_generator_regs.h"

// sys_ctrl
#define SCTRL_POWER_ON          (1<<0)
#define SCTRL_ISL_RESET_N       (1<<1)
#define SCTRL_HDMI_RESET_N      (1<<2)
#define SCTRL_EMIF_HWRESET_N    (1<<3)
#define SCTRL_EMIF_SWRESET_N    (1<<4)
#define SCTRL_EMIF_POWERDN_REQ  (1<<5)
#define SCTRL_VIP_RESET_N       (1<<6)
#define SCTRL_CAPTURE_SEL       (1<<7)
#define SCTRL_ISL_HS_POL        (1<<8)
#define SCTRL_ISL_VS_POL        (1<<9)
#define SCTRL_ISL_VS_TYPE       (1<<10)
#define SCTRL_AUDMUX_SEL        (1<<11)
#define SCTRL_VGTP_ENABLE       (1<<12)
#define SCTRL_CSC_ENABLE        (1<<13)
#define SCTRL_FRAMELOCK         (1<<14)
#define SCTRL_HDMIRX_SPDIF      (1<<15)

// sys_status
#define SSTAT_EMIF_STAT_MASK            0x00000007
#define SSTAT_EMIF_STAT_OFFS            0
#define SSTAT_EMIF_STAT_INIT_DONE_BIT   0
#define SSTAT_EMIF_POWERDN_ACK_BIT      3
#define SSTAT_EMIF_PLL_LOCKED           4
#define SSTAT_SD_DETECT_BIT             5

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

#ifdef DExx_FW
typedef enum {
    AUD_AV1_ANALOG  = 0,
    AUD_SPDIF       = 1,
} audinput_t;
#else
typedef enum {
    AUD_AV1_ANALOG  = 0,
    AUD_AV2_ANALOG  = 1,
    AUD_AV3_ANALOG  = 2,
    AUD_SPDIF       = 3,
    AUD_AV4_DIGITAL = 4,
} audinput_t;
#endif

void ui_disp_menu(uint8_t osd_mode);
void ui_disp_status(uint8_t refresh_osd_timer);

void switch_input(rc_code_t code, btn_vec_t pb_vec);

void set_syncmux_biasmode(uint8_t syncmux_stc);

void switch_audmux(uint8_t audmux_sel);

void switch_audsrc(audinput_t *audsrc_map, HDMI_audio_fmt_t *aud_tx_fmt);

void switch_tp_mode(rc_code_t code);

int sys_is_powered_on();

void sys_toggle_power();

void print_vm_stats();

#endif
