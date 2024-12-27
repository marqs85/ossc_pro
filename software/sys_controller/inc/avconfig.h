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
#include "sysconfig.h"
#include "av_controller.h"
#include "isl51002.h"
#ifdef INC_ADV7513
#include "adv7513.h"
#endif
#ifdef INC_SII1136
#include "sii1136.h"
#endif
#ifdef INC_ADV761X
#include "adv761x.h"
#endif
#ifdef INC_PCM186X
#include "pcm186x.h"
#endif
#ifndef DExx_FW
#include "adv7280a.h"
#endif

#define SIGNED_NUMVAL_ZERO  128

#define SCANLINESTR_MAX     15
#define SL_HYBRIDSTR_MAX    28
#define H_MASK_MAX          255
#define V_MASK_MAX          63
#define HV_MASK_MAX_BR      15
#define VIDEO_LPF_MAX       5
#define SAMPLER_PHASE_MAX   64
#define SYNC_VTH_MAX        15
#define VSYNC_THOLD_MIN     10
#define VSYNC_THOLD_MAX     200
#define SD_SYNC_WIN_MAX     255
#define PLL_COAST_MAX       5
#define PLL_LOOP_GAIN_MAX   3
#define REVERSE_LPF_MAX     31
#define COARSE_GAIN_MAX     15
#define ALC_H_FILTER_MAX    3
#define ALC_V_FILTER_MAX    7
#define CLAMP_STR_MAX       7
#define CLAMP_POS_MIN       1
#define CLAMP_POS_MAX       250
#define CLAMP_WIDTH_MIN     1
#define CLAMP_WIDTH_MAX     100

#define SL_MODE_MAX         2
#define SL_TYPE_MAX         2

#define L5FMT_1920x1080     0
#define L5FMT_1600x1200     1
#define L5FMT_1920x1200     2

// In ascending order of importance
typedef enum {
    SC_CONFIG_CHANGE    = (1<<0),
    TP_MODE_CHANGE      = (1<<1),
    MODE_CHANGE         = (1<<2),
} status_t;

typedef enum {
    SCL_FL_ON           = 0,
    SCL_FL_ON_2X        = 1,
    SCL_FL_OFF_CLOSEST  = 2,
    SCL_FL_OFF_PRESET   = 3,
    SCL_FL_OFF_50HZ     = 4,
    SCL_FL_OFF_60HZ     = 5,
    SCL_FL_OFF_100HZ    = 6,
    SCL_FL_OFF_120HZ    = 7,
} scl_fl_mode_t;

typedef struct {
    /* P-LM mode options */
    uint8_t pm_240p;
    uint8_t pm_384p;
    uint8_t pm_480i;
    uint8_t pm_480p;
    uint8_t pm_1080i;
    uint8_t l2_mode;
    uint8_t l3_mode;
    uint8_t l4_mode;
    uint8_t l5_mode;
    uint8_t l6_mode;
    uint8_t l5_fmt;
    uint8_t s480p_mode;
    uint8_t s400p_mode;
    uint8_t upsample2x;
    /* A-LM mode options */
    uint8_t pm_ad_240p;
    uint8_t pm_ad_288p;
    uint8_t pm_ad_384p;
    uint8_t pm_ad_480i;
    uint8_t pm_ad_576i;
    uint8_t pm_ad_480p;
    uint8_t pm_ad_576p;
    uint8_t pm_ad_720p;
    uint8_t pm_ad_1080i;
    uint8_t pm_ad_1080p;
    uint8_t sm_ad_240p_288p;
    uint8_t sm_ad_384p;
    uint8_t sm_ad_480i_576i;
    uint8_t sm_ad_480p;
    uint8_t sm_ad_576p;
    /* Common LM mode options */
    uint8_t lm_mode;
    uint8_t ar_256col;
    /* Scaler mode options */
#ifdef VIP
    uint8_t scl_out_mode;
    uint8_t scl_crt_out_mode;
    uint8_t scl_out_type;
    uint8_t scl_framelock;
    uint8_t scl_aspect;
    uint8_t scl_alg;
    uint8_t scl_gen_sr;
    uint8_t sm_scl_240p_288p;
    uint8_t sm_scl_384p;
    uint8_t sm_scl_480i_576i;
    uint8_t sm_scl_480p;
    uint8_t sm_scl_576p;
#endif
    /* Common mode options */
    uint8_t oper_mode;
    uint8_t timing_1080p120;
    uint8_t timing_2160p60;
    uint8_t hdmi_pixeldecim_mode;

    /* Postprocessing settings */
    uint8_t sl_mode;
    uint8_t sl_type;
    uint8_t sl_hybr_str;
    uint8_t sl_method;
    uint8_t sl_altern;
    uint8_t sl_altiv;
    uint8_t sl_str;
    uint8_t sl_id;
    uint8_t sl_cust_l_str[10];
    uint8_t sl_cust_c_str[10];
    uint8_t sl_cust_iv_x;
    uint8_t sl_cust_iv_y;
    uint8_t h_mask;
    uint8_t v_mask;
    uint8_t mask_br;
    uint8_t mask_color;
    uint8_t bfi_enable;
    uint8_t bfi_str;
    uint8_t shmask_mode;
    uint8_t shmask_str;
    uint8_t reverse_lpf;
    uint8_t lumacode_mode;
    uint8_t ypbpr_cs;
    /* Common LM settings */
    uint8_t lm_deint_mode;
    uint8_t nir_even_offset;
    /* Scaler settings */
#ifdef VIP
    uint8_t scl_edge_thold;
    uint8_t scl_dil_motion_shift;
#ifndef VIP_DIL_B
    uint8_t scl_dil_alg;
#else
    uint8_t scl_dil_motion_scale;
    uint8_t scl_dil_cadence_detect_enable;
    uint8_t scl_dil_visualize_motion;
#endif
#endif

    /* Misc */
    uint8_t tp_mode;
    uint8_t audio_fmt;
    uint8_t audmux_sel;
    audinput_t audio_src_map[5];
    uint8_t exp_sel;
    uint8_t extra_av_out_mode;
    uint8_t isl_ext_range;
#ifdef INC_THS7353
    uint8_t syncmux_stc;
#endif
    isl51002_config isl_cfg __attribute__ ((aligned (4)));
#ifdef INC_ADV7513
    adv7513_config hdmitx_cfg __attribute__ ((aligned (4)));
#endif
#ifdef INC_SII1136
    sii1136_config hdmitx_cfg __attribute__ ((aligned (4)));
#endif
#ifdef INC_ADV761X
    adv761x_config hdmirx_cfg __attribute__ ((aligned (4)));
#endif
#ifdef INC_PCM186X
    pcm186x_config pcm_cfg __attribute__ ((aligned (4)));
#endif
#ifndef DExx_FW
    adv7280a_config sdp_cfg __attribute__ ((aligned (4)));
#endif
} __attribute__((packed)) avconfig_t;

avconfig_t* get_current_avconfig();
avconfig_t* get_target_avconfig();
status_t update_avconfig();
int set_default_profile(int update_cc);
int reset_profile();
int save_profile();
int load_profile();
int save_profile_sd();
int load_profile_sd();

#endif
