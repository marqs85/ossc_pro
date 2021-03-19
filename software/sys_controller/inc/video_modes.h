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
#include "si5351.h"
#include "hdmi.h"
#include "sysconfig.h"

#define H_TOTAL_MIN 300
#define H_TOTAL_MAX 2800
#define H_TOTAL_ADJ_MAX 19
#define H_SYNCLEN_MIN 10
#define H_SYNCLEN_MAX 255
#define H_BPORCH_MIN 1
#define H_BPORCH_MAX 255
#define H_ACTIVE_MIN 200
#define H_ACTIVE_MAX 1920
#define V_SYNCLEN_MIN 1
#define V_SYNCLEN_MAX 7
#define V_BPORCH_MIN 1
#define V_BPORCH_MAX 63
#define V_ACTIVE_MIN 160
#define V_ACTIVE_MAX 1200

#define DEFAULT_SAMPLER_PHASE 0

typedef enum {
    FORMAT_RGBS = 0,
    FORMAT_RGBHV = 1,
    FORMAT_RGsB = 2,
    FORMAT_YPbPr = 3
} video_format;

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
    GROUP_1080I     = 8,
} video_group;

typedef enum {
    //deprecated
    MODE_INTERLACED     = (1<<0),
    MODE_PLLDIVBY2      = (1<<1),
    //at least one of the flags below must be set for each mode
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
} mode_flags;

typedef enum {
    STDMODE_240p         = 7,
    STDMODE_288p         = 15,
    STDMODE_480i         = 23,
    STDMODE_480p         = 24,
    STDMODE_576i         = 27,
    STDMODE_576p         = 28,
    STDMODE_720p_50      = 30,
    STDMODE_720p_60      = 31,
    STDMODE_1280x1024_60 = 34,
    STDMODE_1080i_50     = 36,
    STDMODE_1080i_60     = 37,
    STDMODE_1080p_50     = 38,
    STDMODE_1080p_60     = 39,
    STDMODE_1080p_120    = 40,
    STDMODE_1600x1200_60 = 41,
    STDMODE_1920x1200_50 = 42,
    STDMODE_1920x1200_60 = 43,
    STDMODE_1920x1440_50 = 44,
    STDMODE_1920x1440_60 = 45,
    STDMODE_2560x1440_50 = 46,
    STDMODE_2560x1440_60 = 47
} stdmode_t;

typedef enum {
    /* Generic */
    SMPPRESET_GEN_720x240    = 0,
    SMPPRESET_GEN_960x240,
    SMPPRESET_GEN_1280x240,
    SMPPRESET_GEN_1600x240,
    SMPPRESET_GEN_1920x240,
    SMPPRESET_GEN_720x288,
    SMPPRESET_GEN_1536x288,
    SMPPRESET_GEN_1920x288,
    SMPPRESET_GEN_720x480i,
    SMPPRESET_GEN_1280x480i,
    SMPPRESET_GEN_1920x480i,
    SMPPRESET_GEN_1280x480i_WS,
    SMPPRESET_GEN_1707x480i_WS,
    SMPPRESET_GEN_720x576i,
    SMPPRESET_GEN_1536x576i,
    SMPPRESET_GEN_720x480,
    SMPPRESET_GEN_1280x480,
    SMPPRESET_GEN_1920x480,
    SMPPRESET_GEN_1280x480_WS,
    SMPPRESET_GEN_1707x480_WS,
    SMPPRESET_GEN_720x576,
    SMPPRESET_GEN_1536x576,
    /* DTV / PC */
    SMPPRESET_DTV480I,
    SMPPRESET_DTV480I_WS,
    SMPPRESET_DTV576I,
    SMPPRESET_DTV576I_WS,
    SMPPRESET_VGA480P60,
    SMPPRESET_DTV480P,
    SMPPRESET_DTV480P_WS,
    SMPPRESET_DTV576P,
    SMPPRESET_DTV576P_WS,
    SMPPRESET_DTV720P50,
    SMPPRESET_DTV720P60,
    SMPPRESET_DTV1080I50,
    SMPPRESET_DTV1080I60,
    SMPPRESET_DTV1080P50,
    SMPPRESET_DTV1080P60,
    /* console-specific */
    SMPPRESET_SNES_256x240,
    SMPPRESET_SNES_512x240,
    SMPPRESET_MD_256x224,
    SMPPRESET_MD_320x224,
    SMPPRESET_PSX_256x240,
    SMPPRESET_PSX_320x240,
    SMPPRESET_PSX_384x240,
    SMPPRESET_PSX_512x240,
    SMPPRESET_PSX_640x240,
    SMPPRESET_N64_320x240,
    SMPPRESET_N64_640x240,
} smp_preset_id_t;

typedef enum {
    ADMODE_240p      = 0,
    ADMODE_288p,
    ADMODE_480p,
    ADMODE_576p,
    ADMODE_720p_60,
    ADMODE_1280x1024_60,
    ADMODE_1080i_50_CR,
    ADMODE_1080i_60_LB,
    ADMODE_1080p_50_CR,
    ADMODE_1080p_60_LB,
    ADMODE_1080p_60_CR,
    ADMODE_1600x1200_60,
    ADMODE_1920x1200_50,
    ADMODE_1920x1200_60,
    ADMODE_1920x1440_50,
    ADMODE_1920x1440_60,
    ADMODE_2560x1440_50,
    ADMODE_2560x1440_60,
} ad_mode_id_t;

typedef enum {
    SM_GEN_4_3      = 0,
    SM_GEN_16_9,
    SM_OPT_DTV480I,
    SM_OPT_DTV480I_WS,
    SM_OPT_DTV576I,
    SM_OPT_DTV576I_WS,
    SM_OPT_DTV480P,
    SM_OPT_DTV480P_WS,
    SM_OPT_VGA480P60,
    SM_OPT_DTV576P,
    SM_OPT_DTV576P_WS,
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
    SM_OPT_SAT_704COL,
    SM_OPT_N64_320COL,
    SM_OPT_N64_640COL,
} smp_mode_t;

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint8_t v_hz_max;
    uint16_t h_total;
    uint8_t  h_total_adj;
    uint16_t v_total;
    uint16_t h_backporch;
    uint16_t v_backporch;
    uint16_t h_synclen;
    uint8_t v_synclen;
    uint8_t interlaced;
} sync_timings_t;

typedef struct {
    char name[14];
    HDMI_vic_t vic;
    sync_timings_t timings;
    uint8_t sampler_phase;
    video_type type;
    video_group group;
    mode_flags flags;
    HDMI_pixelrep_t tx_pixelrep;
    HDMI_pixelrep_t hdmitx_pixr_ifr;
    // for generation from 27MHz clock
    uint8_t si_pclk_mult;
    si5351_ms_config_t si_ms_conf;
} mode_data_t;

typedef struct {
    char name[14];
    smp_mode_t sm;
    sync_timings_t timings_i;
    uint8_t h_skip;
    uint8_t sampler_phase;
    video_type type;
    video_group group;
} smp_preset_t;

typedef struct {
    ad_mode_id_t id;
    smp_preset_id_t smp_preset_id;
    uint16_t v_total_override;
    uint8_t x_rpt;
    uint8_t y_rpt;
    int16_t x_offset_i;
    int16_t y_offset_i;
    si5351_ms_config_t si_ms_conf;
} ad_mode_data_t;

typedef struct {
    uint8_t x_rpt;
    uint8_t y_rpt;
    uint8_t h_skip;
    int16_t x_offset;
    int16_t y_offset;
    uint16_t x_size;
    uint16_t y_size;
    uint16_t framesync_line;
    uint8_t x_start_lb;
    int8_t y_start_lb;
} vm_mult_config_t;


void set_default_vm_table();

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz);

int get_scaler_mode(mode_data_t *vm_in, mode_data_t *vm_out, vm_mult_config_t *vm_conf, int *framelock);

int get_adaptive_lm_mode(mode_data_t *vm_in, mode_data_t *vm_out, vm_mult_config_t *vm_conf);

int get_pure_lm_mode(mode_data_t *vm_in, mode_data_t *vm_out, vm_mult_config_t *vm_conf);

int get_standard_mode(unsigned stdmode_idx_arr_idx, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out);

#endif /* VIDEO_MODES_H_ */
