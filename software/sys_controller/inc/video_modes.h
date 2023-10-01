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

#ifndef VIDEO_MODES_H_
#define VIDEO_MODES_H_

#include <stdint.h>
#include "sysconfig.h"
#include "si5351.h"
#include "hdmi.h"
#include "avconfig.h"

#define H_TOTAL_MIN 300
#define H_TOTAL_MAX 2800
#define H_TOTAL_ADJ_MAX 19
#define H_SYNCLEN_MIN 10
#define H_SYNCLEN_MAX 255
#define H_BPORCH_MIN 0
#define H_BPORCH_MAX 511
#define H_ACTIVE_MIN 200
#define H_ACTIVE_MAX 2560
#define H_ACTIVE_SMP_MAX 2048
#define V_SYNCLEN_MIN 1
#define V_SYNCLEN_MAX 15
#define V_BPORCH_MIN 0
#define V_BPORCH_MAX 511
#define V_ACTIVE_MIN 160
#define V_ACTIVE_MAX 2160

typedef enum {
    VIDEO_SDTV      = (1<<0),
    VIDEO_EDTV      = (1<<1),
    VIDEO_HDTV      = (1<<2),
    VIDEO_PC        = (1<<3),
} video_type;

typedef enum {
    GROUP_NONE      = 0,
    GROUP_240P      = 1,
    GROUP_288P      = 2,
    GROUP_384P      = 3,
    GROUP_480I      = 4,
    GROUP_576I      = 5,
    GROUP_480P      = 6,
    GROUP_576P      = 7,
    GROUP_720P      = 8,
    GROUP_1080I     = 9,
    GROUP_1080P     = 10,
} video_group;

typedef enum {
    MODE_INTERLACED     = (1<<0), //deprecated
    MODE_CRT            = (1<<1),
    //at least one of the flags below must be set for each P-LM mode
    MODE_PT             = (1<<2),
    MODE_L2             = (1<<3),
    MODE_L2_512_COL     = (1<<4),
    MODE_L2_384_COL     = (1<<5),
    MODE_L2_320_COL     = (1<<6),
    MODE_L2_256_COL     = (1<<7),
    MODE_L2_240x360     = (1<<8),
    MODE_L3_GEN_16_9    = (1<<9),
    MODE_L3_GEN_4_3     = (1<<10),
    MODE_L3_512_COL     = (1<<11),
    MODE_L3_384_COL     = (1<<12),
    MODE_L3_320_COL     = (1<<13),
    MODE_L3_256_COL     = (1<<14),
    MODE_L3_240x360     = (1<<15),
    MODE_L4_GEN_4_3     = (1<<16),
    MODE_L4_512_COL     = (1<<17),
    MODE_L4_384_COL     = (1<<18),
    MODE_L4_320_COL     = (1<<19),
    MODE_L4_256_COL     = (1<<20),
    MODE_L5_GEN_4_3     = (1<<21),
    MODE_L5_512_COL     = (1<<22),
    MODE_L5_384_COL     = (1<<23),
    MODE_L5_320_COL     = (1<<24),
    MODE_L5_256_COL     = (1<<25),
    MODE_L6_GEN_4_3     = (1<<26),
    MODE_L6_512_COL     = (1<<27),
    MODE_L6_384_COL     = (1<<28),
    MODE_L6_320_COL     = (1<<29),
    MODE_L6_256_COL     = (1<<30),
} mode_flags;

typedef enum {
    STDMODE_240p_CRT,
    STDMODE_240p_WS_CRT,
    STDMODE_288p_CRT,
    STDMODE_288p_WS_CRT,
    STDMODE_480i_CRT,
    STDMODE_480i_WS_CRT,
    STDMODE_576i_CRT,
    STDMODE_576i_WS_CRT,
    STDMODE_480p_60_CRT,
    STDMODE_480p_100_CRT,
    STDMODE_480p_120_CRT,
    STDMODE_540p_50_CRT,
    STDMODE_540p_60_CRT,
    STDMODE_480i,
    STDMODE_480p,
    STDMODE_480p_WS,
    STDMODE_576i,
    STDMODE_576p,
    STDMODE_576p_WS,
    STDMODE_720p_50,
    STDMODE_720p_60,
    STDMODE_720p_100,
    STDMODE_720p_120,
    STDMODE_1024x768_60,
    STDMODE_1280x1024_60,
    STDMODE_1080i_50,
    STDMODE_1080i_60,
    STDMODE_1080p_50,
    STDMODE_1080p_60,
    STDMODE_1080p_100,
    STDMODE_1080p_120,
    STDMODE_1600x1200_60,
    STDMODE_1920x1200_50,
    STDMODE_1920x1200_60,
    STDMODE_1920x1440_50,
    STDMODE_1920x1440_60,
    STDMODE_2560x1440_50,
    STDMODE_2560x1440_60,
    STDMODE_2880x2160_50,
    STDMODE_2880x2160_60,
} stdmode_t;

typedef enum {
    SM_GEN_4_3      = 0,
    SM_GEN_16_9,
    SM_OPT_DTV480I,
    SM_OPT_DTV480I_WS,
    SM_OPT_DTV576I,
    SM_OPT_DTV576I_WS,
    SM_OPT_DTV480P,
    SM_OPT_DTV480P_WS,
    SM_OPT_DTV576P,
    SM_OPT_DTV576P_WS,
    SM_OPT_VGA_640x350,
    SM_OPT_VGA_720x350,
    SM_OPT_VGA_640x400,
    SM_OPT_VGA_720x400,
    SM_OPT_VESA_640x480,
    SM_OPT_PC_HDTV,
    SM_OPT_SNES_256COL,
    SM_OPT_SNES_512COL,
    SM_OPT_MD_256COL,
    SM_OPT_MD_320COL,
    SM_OPT_PSX_256COL,
    SM_OPT_PSX_320COL,
    SM_OPT_PSX_384COL,
    SM_OPT_PSX_512COL,
    SM_OPT_PSX_640COL,
    SM_OPT_SAT_320COL,
    SM_OPT_SAT_352COL,
    SM_OPT_SAT_640COL,
    SM_OPT_SAT_704COL,
    SM_OPT_N64_320COL,
    SM_OPT_N64_640COL,
    SM_OPT_NG_320COL,
    SM_OPT_GBI_240COL,
    SM_OPT_PC98_640COL,
    SM_OPT_DC_640COL,
    SM_OPT_PS2_512COL,
    SM_OPT_X68K_256COL,
    SM_OPT_X68K_512COL,
    SM_OPT_X68K_768COL,
    SM_OPT_C64_4XXCOL,
} smp_mode_t;

typedef enum {
    OPERMODE_PURE_LM      = 0,
    OPERMODE_ADAPT_LM     = 1,
    OPERMODE_SCALER       = 2,
    OPERMODE_INVALID      = 255,
} oper_mode_t;

typedef enum {
    GEN_WIDTH_SMALLEST,
    GEN_WIDTH_LARGEST,
    GEN_WIDTH_CLOSEST_PREFER_UNDER,
    GEN_WIDTH_CLOSEST_PREFER_OVER,
} gen_width_mode_t;

#define NUM_VIDEO_GROUPS (GROUP_1080P+1)
#define NUM_VIDEO_MODES  (STDMODE_2880x2160_60+1)

typedef struct {
    stdmode_t stdmode_id;
    int8_t y_rpt;
} ad_mode_t;

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint16_t v_hz_x100;
    uint16_t h_total;
    uint8_t  h_total_adj;
    uint16_t v_total;
    uint16_t h_backporch;
    uint16_t v_backporch;
    uint8_t h_synclen;
    uint8_t v_synclen;
    uint8_t interlaced;
} sync_timings_t;

typedef struct {
    uint8_t h;
    uint8_t v;
} aspect_ratio_t;

typedef struct {
    uint8_t h;
    uint8_t v;
} mask_t;

typedef struct {
    char name[16];
    HDMI_vic_t vic;
    sync_timings_t timings;
    uint8_t sampler_phase;
    union {
        aspect_ratio_t ar;
        mask_t mask;
    };
    video_type type;
    video_group group;
    mode_flags flags;
    HDMI_pixelrep_t tx_pixelrep;
    HDMI_pixelrep_t hdmitx_pixr_ifr;
} mode_data_t;

typedef struct {
    char name[16];
    smp_mode_t sm;
    sync_timings_t timings_i;
    uint8_t h_skip;
    uint8_t sampler_phase;
    aspect_ratio_t ar;
    video_type type;
    video_group group;
} smp_preset_t;

typedef struct {
    int8_t x_rpt;
    int8_t y_rpt;
    uint8_t h_skip;
    uint8_t h_sample_sel;
    int16_t x_offset;
    int16_t y_offset;
    uint16_t x_size;
    uint16_t y_size;
    uint16_t framesync_line;
    uint8_t x_start_lb;
    int8_t y_start_lb;
    uint8_t framelock;
    // for generation from 27MHz clock
    int8_t si_pclk_mult;
} vm_proc_config_t;


void set_default_vm_table();

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz);

uint32_t calculate_pclk(uint32_t src_clk_hz, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

oper_mode_t get_operating_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

int get_scaler_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

int get_adaptive_lm_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

int get_pure_lm_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

int get_standard_mode(stdmode_t stdmode_id, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf);

#endif /* VIDEO_MODES_H_ */
