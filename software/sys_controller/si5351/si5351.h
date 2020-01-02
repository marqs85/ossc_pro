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

#ifndef SI5351_H_
#define SI5351_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "si5351_regs.h"

#define SI_VCO_CENTER_FREQ  750000000UL
#define SI_CLKIN_MIN_FREQ   10000000UL
#define SI_CLKIN_MAX_FREQ   40000000UL
#define SI_MAX_OUTPUT_FREQ  200000000UL

typedef enum {
    SI_XTAL = 0,
    SI_CLKIN
} si5351_clk_src;

typedef enum {
    SI_PLLA = 0,
    SI_PLLB
} si5351_pll_ch;

typedef enum {
    SI_CLK0 = 0,
    SI_CLK1,
    SI_CLK2,
    SI_CLK3,
    SI_CLK4,
    SI_CLK5,
    SI_CLK6,
    SI_CLK7,
} si5351_out_ch;

typedef struct {
    uint8_t i2c_addr;
    uint32_t xtal_freq;
} si5351_dev;

int si5351_set_integer_mult(si5351_dev *dev, si5351_pll_ch pll_ch, si5351_out_ch out_ch, si5351_clk_src clksrc, uint32_t clksrc_hz, uint8_t mult);

void si5351_init(si5351_dev *dev);

#endif /* SI5351_H_ */
