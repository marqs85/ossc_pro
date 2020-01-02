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

#ifndef ADV7611_H_
#define ADV7611_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "adv7611_regs.h"

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint16_t h_total;
    uint16_t v_total;
    uint8_t h_backporch;
    uint8_t v_backporch;
    uint8_t h_synclen;
    uint8_t v_synclen;
    uint8_t interlace_flag;
    uint8_t h_polarity;
    uint8_t v_polarity;
} adv7611_sync_status;

typedef struct {
    uint8_t io_base;
    uint8_t cec_base;
    uint8_t infoframe_base;
    uint8_t dpll_base;
    uint8_t ksv_base;
    uint8_t edid_base;
    uint8_t hdmi_base;
    uint8_t cp_base;
    uint32_t xtal_freq;
    uint8_t *edid;
    uint16_t edid_len;
    uint8_t sync_active;
    adv7611_sync_status ss;
    uint32_t pclk_hz;
    uint32_t pixelrep;
} adv7611_dev;

void adv7611_init(adv7611_dev *dev);

int adv7611_check_activity(adv7611_dev *dev);

int adv7611_get_sync_stats(adv7611_dev *dev);

#endif /* ADV7611_H_ */
