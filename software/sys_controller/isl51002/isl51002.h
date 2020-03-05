//
// Copyright (C) 2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef ISL51002_H_
#define ISL51002_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "video_modes.h"
#include "isl51002_regs.h"

//#define ISL_MEAS_HZ

typedef struct {
    uint16_t r_gain;
    uint16_t g_gain;
    uint16_t b_gain;
    uint16_t r_offs;
    uint16_t g_offs;
    uint16_t b_offs;
} color_setup_t;

typedef struct {
    color_setup_t col;
    uint8_t pre_coast;
    uint8_t post_coast;
    uint16_t clamp_alc_start;
    uint8_t clamp_alc_width;
    uint8_t coast_clamp;
    uint8_t alc_enable;
    uint8_t alc_h_filter;
    uint8_t alc_v_filter;
    uint8_t afe_bw;
    uint8_t hsync_vth;
    uint8_t sog_vth;
    uint8_t sync_gf;
} isl51002_config;

typedef struct {
    union {
        struct {
            uint8_t sync_type:2;
            uint8_t h_polarity:1;
            uint8_t v_polarity:1;
            uint8_t sog_trilevel:1;
            uint8_t interlace:1;
            uint8_t macrovision:1;
            uint8_t pll_locked:1;
        };
        uint8_t sync_params;
    };
    uint16_t v_total;
    uint8_t interlace_flag;
    uint32_t pcnt_frame;
} isl51002_sync_status;

typedef struct {
    uint16_t h_period_x16;
    uint16_t v_totlines;
    uint16_t h_synclen_x16;
    uint16_t v_synclen;
    uint16_t h_sync_backporch;
    uint16_t h_active;
    uint16_t v_sync_backporch;
    uint16_t v_active;
} isl51002_sync_meas;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint32_t xtal_freq;
    uint8_t powered_on;
    uint8_t sync_active;
    uint8_t auto_bw_sel;
    isl51002_config cfg;
    isl51002_sync_status ss;
    isl51002_sync_meas sm;
} isl51002_dev;

typedef enum {
    ISL_CH0 = 0,
    ISL_CH1 = 1,
    ISL_CH2 = 2
} isl_input_t;

typedef enum {
    SYNC_SOG = (1<<0),
    SYNC_HV = (1<<1),
    SYNC_CS = (1<<2)
} video_sync;


void isl_writereg(isl51002_dev *dev, uint8_t regaddr, uint8_t data);

uint8_t isl_readreg(isl51002_dev *dev, uint8_t regaddr);

int isl_init(isl51002_dev *dev);

void isl_get_default_cfg(isl51002_config *cfg);

void isl_enable_power(isl51002_dev *dev, int enable);

void isl_enable_outputs(isl51002_dev *dev, int enable);

void isl_source_sel(isl51002_dev *dev, isl_input_t input, video_sync syncinput, video_format fmt);

int isl_check_activity(isl51002_dev *dev, isl_input_t input, video_sync syncinput);

// vtotal/interlace_flag/pcnt_frame must be provided externally as isl51002 measurements are not reliable/accurate enough
int isl_get_sync_stats(isl51002_dev *dev, uint16_t vtotal, uint8_t interlace_flag, uint32_t pcnt_frame);

void isl_source_setup(isl51002_dev *dev, uint16_t h_samplerate);

uint16_t isl_get_pll_htotal(isl51002_dev *dev);

void isl_de_adj(isl51002_dev *dev);

void isl_set_sampler_phase(isl51002_dev *dev, uint8_t sampler_phase);

void isl_set_afe_bw(isl51002_dev *dev, uint32_t dotclk_hz);

uint16_t isl_get_afe_bw(isl51002_dev *dev, uint8_t afe_bw);

void isl_set_de(isl51002_dev *dev);

void isl_update_config(isl51002_dev *dev, isl51002_config *cfg);

#endif /* ISL51002_H_ */
