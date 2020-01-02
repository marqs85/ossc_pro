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

#ifndef ADV7513_H_
#define ADV7513_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "adv7513_regs.h"
#include "video_modes.h"

typedef enum {
    TX_HDMI_RGB         = 0,
    TX_HDMI_YCBCR444    = 1,
    TX_DVI              = 2
} tx_mode_t;

typedef struct {
    uint8_t main_base;
    uint8_t edid_base;
    uint8_t pktmem_base;
    uint8_t cec_base;
    uint8_t powered_on;
    tx_mode_t tx_mode;
    uint8_t pixelrep;
    uint8_t pixelrep_infoframe;
    HDMI_Video_Type vic;
} adv7513_dev;

void adv7513_init(adv7513_dev *dev);

void adv7513_set_pixelrep_vic(adv7513_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_Video_Type vic);

int adv7513_check_hpd_power(adv7513_dev *dev);

#endif /* ADV7513_H_ */
