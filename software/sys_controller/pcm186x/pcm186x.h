//
// Copyright (C) 2017-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef PCM186X_H_
#define PCM186X_H_

#include <stdint.h>
#include "pcm186x_regs.h"

typedef enum {
    PCM_INPUT1 = 0,
    PCM_INPUT2 = 1,
    PCM_INPUT3 = 2,
    PCM_INPUT4 = 3
} pcm_input_t;

typedef enum {
    PCM_48KHZ = 0,
    PCM_96KHZ,
    PCM_192KHZ
} pcm_samplerate_t;

typedef enum {
    PCM_GAIN_M12DB = 0,
    PCM_GAIN_0DB = 12,
    PCM_GAIN_12DB = 24
} pcm_gain_t;

typedef struct {
    pcm_samplerate_t fs;
    uint8_t gain;
} pcm186x_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    pcm186x_config cfg;
} pcm186x_dev;

void pcm186x_source_sel(pcm186x_dev *dev, pcm_input_t input);

void pcm186x_set_gain(pcm186x_dev *dev, int8_t db_gain);

void pcm186x_set_samplerate(pcm186x_dev *dev, pcm_samplerate_t fs);

int pcm186x_init(pcm186x_dev *dev);

void pcm186x_get_default_cfg(pcm186x_config *cfg);

void pcm186x_update_config(pcm186x_dev *dev, pcm186x_config *cfg);

#endif /* PCM186X_H_ */
