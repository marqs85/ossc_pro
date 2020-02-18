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
    VIDEO_LDTV      = (1<<0),
    VIDEO_SDTV      = (1<<1),
    VIDEO_EDTV      = (1<<2),
    VIDEO_HDTV      = (1<<3),
    VIDEO_PC        = (1<<4),
} video_type;

typedef enum {
    GROUP_NONE      = 0,
    GROUP_240P      = 1,
    GROUP_384P      = 2,
    GROUP_480I      = 3,
    GROUP_480P      = 4,
    GROUP_1080I     = 5,
} video_group;

typedef enum {
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

typedef enum tagHDMI_Video_Type {
    HDMI_Unknown = 0 ,
    HDMI_640x480p60 = 1 ,
    HDMI_480p60,
    HDMI_480p60_16x9,
    HDMI_720p60,
    HDMI_1080i60,
    HDMI_480i60,
    HDMI_480i60_16x9,
    HDMI_240p60,
    HDMI_1080p60 = 16,
    HDMI_576p50,
    HDMI_576p50_16x9,
    HDMI_720p50 = 19,
    HDMI_1080i50,
    HDMI_576i50,
    HDMI_576i50_16x9,
    HDMI_288p50,
    HDMI_1080p50 = 31,
    /*HDMI_1080p24,
    HDMI_1080p25,
    HDMI_1080p30,
    HDMI_1080i120 = 46,*/
} HDMI_Video_Type ;

typedef enum {
    TX_1X   = 0,
    TX_2X   = 1,
    TX_4X   = 2
} HDMI_pixelrep_t;

typedef enum {
    STDMODE_240p   = 7,
    STDMODE_480p   = 24,
    STDMODE_720p   = 30,
    STDMODE_960p   = 32,
    STDMODE_1080p  = 36,
    STDMODE_1200p  = 38,
    STDMODE_1440p  = 39
} stdmode_t;

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint16_t h_total;
    uint8_t  h_total_adj:5;
    uint16_t v_total:11;
    uint16_t h_backporch:9;
    uint16_t v_backporch:9;
    uint16_t h_synclen:9;
    uint8_t v_synclen:5;
} __attribute__((packed)) sync_timings_t;

typedef struct {
    char name[10];
    HDMI_Video_Type vic:8;
    sync_timings_t timings;
    uint8_t sampler_phase;
    video_type type:5;
    video_group group:3;
    mode_flags flags;
    HDMI_pixelrep_t tx_pixelrep:2;
    HDMI_pixelrep_t hdmitx_pixr_ifr:2;
    // for generation from 27MHz clock
    uint8_t si_pclk_mult:4;
    si5351_ms_config_t si_ms_conf;
} mode_data_t;

typedef struct {
    stdmode_t mode_idx_i;
    sync_timings_t timings_i;
    uint8_t sampler_phase;
    video_type type:5;
    video_group group:3;
    uint8_t y_rpt;
    int16_t x_offset_i;
    int16_t y_offset_i;
    si5351_ms_config_t si_ms_conf;
} ad_mode_data_t;

typedef struct {
    uint8_t x_rpt;
    uint8_t y_rpt;
    uint8_t x_skip;
    int16_t x_offset;
    int16_t y_offset;
    uint16_t x_size;
    uint16_t y_size;
    uint16_t framesync_line;
    int8_t linebuf_startline;
} vm_mult_config_t;


void set_default_vm_table();

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz);

int get_adaptive_mode(uint16_t totlines, uint8_t progressive, uint16_t hz_x100, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out);

int get_mode_id(uint16_t totlines, uint8_t progressive, uint16_t hz_x100, video_type typemask, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out);

int get_standard_mode(unsigned stdmode_idx_arr_idx, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out);

#endif /* VIDEO_MODES_H_ */
