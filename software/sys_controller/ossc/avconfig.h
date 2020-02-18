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

#ifndef AVCONFIG_H_
#define AVCONFIG_H_

#include <stdint.h>

#define SIGNED_NUMVAL_ZERO  128

#define SCANLINESTR_MAX     15
#define SL_HYBRIDSTR_MAX    28
#define H_MASK_MAX          255
#define V_MASK_MAX          63
#define HV_MASK_MAX_BR      15
#define VIDEO_LPF_MAX       5
#define SAMPLER_PHASE_MAX   31
#define SYNC_VTH_MAX        31
#define VSYNC_THOLD_MIN     10
#define VSYNC_THOLD_MAX     200
#define SD_SYNC_WIN_MAX     255
#define PLL_COAST_MAX       5
#define REVERSE_LPF_MAX     31
#define COARSE_GAIN_MAX     15
#define ALC_H_FILTER_MAX    7
#define ALC_V_FILTER_MAX    10
#define CLAMP_OFFSET_MIN    (SIGNED_NUMVAL_ZERO-100)
#define CLAMP_OFFSET_MAX    (SIGNED_NUMVAL_ZERO+100)

#define SL_MODE_MAX         2
#define SL_TYPE_MAX         2

#define AUDIO_GAIN_M12DB    0
#define AUDIO_GAIN_0DB      12
#define AUDIO_GAIN_12DB     24
#define AUDIO_GAIN_MAX      AUDIO_GAIN_12DB

#define L5FMT_1920x1080     0
#define L5FMT_1600x1200     1
#define L5FMT_1920x1200     2

// In reverse order of importance
typedef enum {
    NO_CHANGE           = 0,
    SC_CONFIG_CHANGE    = 1,
    MODE_CHANGE         = 2,
    TX_MODE_CHANGE      = 3,
    ACTIVITY_CHANGE     = 4
} status_t;

typedef struct {
    uint8_t r_f_off;
    uint8_t g_f_off;
    uint8_t b_f_off;
    uint8_t r_f_gain;
    uint8_t g_f_gain;
    uint8_t b_f_gain;
    uint8_t c_gain;
} __attribute__((packed)) color_setup_t;

typedef struct {
    uint8_t sl_mode;
    uint8_t sl_type;
    uint8_t sl_hybr_str;
    uint8_t sl_method;
    uint8_t sl_altern;
    uint8_t sl_altiv;
    uint8_t sl_str;
    uint8_t sl_id;
    uint8_t sl_cust_l_str[5];
    uint8_t sl_cust_c_str[6];
    uint8_t linemult_target;
    uint8_t l2_mode;
    uint8_t l3_mode;
    uint8_t l4_mode;
    uint8_t l5_mode;
    uint8_t l5_fmt;
    uint8_t pm_240p;
    uint8_t pm_384p;
    uint8_t pm_480i;
    uint8_t pm_480p;
    uint8_t pm_1080i;
    uint8_t pm_ad_240p;
    uint8_t pm_ad_480p;
    uint8_t adapt_lm;
    uint8_t ar_256col;
    uint8_t h_mask;
    uint8_t v_mask;
    uint8_t mask_br;
    uint8_t mask_color;
    uint8_t tx_mode;
    uint8_t hdmi_itc;
    uint8_t s480p_mode;
    uint8_t s400p_mode;
    uint8_t upsample2x;
    uint8_t ypbpr_cs;
    uint8_t sync_vth;
    uint8_t linelen_tol;
    uint8_t vsync_thold;
    uint8_t sync_lpf;
    uint8_t stc_lpf;
    uint8_t video_lpf;
    uint8_t pre_coast;
    uint8_t post_coast;
    uint8_t full_tx_setup;
    uint8_t reverse_lpf;
    uint8_t audio_dw_sampl;
    uint8_t audio_gain;
    uint8_t default_vic;
    uint8_t clamp_offset;
    uint8_t alc_h_filter;
    uint8_t alc_v_filter;
    color_setup_t col;
} __attribute__((packed)) avconfig_t;

int reset_target_avconfig();

avconfig_t* get_current_avconfig();

int set_default_avconfig(int update_cc);

status_t update_avconfig();

#endif
