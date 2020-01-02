//
// Copyright (C) 2017  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "i2c_opencores.h"
#include "pcm1862.h"

inline alt_u32 pcm1862_readreg(alt_u8 regaddr)
{
    //Phase 1
    I2C_start(I2CA_BASE, PCM1862_BASE, 0);
    I2C_write(I2CA_BASE, regaddr, 0);

    //Phase 2
    I2C_start(I2CA_BASE, PCM1862_BASE, 1);
    return I2C_read(I2CA_BASE,1);
}

inline void pcm1862_writereg(alt_u8 regaddr, alt_u8 data)
{
    I2C_start(I2CA_BASE, PCM1862_BASE, 0);
    I2C_write(I2CA_BASE, regaddr, 0);
    I2C_write(I2CA_BASE, data, 1);
}

void pcm_source_sel(pcm_input_t input) {
    alt_u8 adc_ch = 1<<input;

    pcm1862_writereg(PCM1862_ADC1L, adc_ch);
    pcm1862_writereg(PCM1862_ADC1R, adc_ch);
}

void pcm_set_gain(alt_8 db_gain) {
    alt_8 gain_val = 2*db_gain;

    pcm1862_writereg(PCM1862_PGA1L, gain_val);
    pcm1862_writereg(PCM1862_PGA1R, gain_val);
}

int pcm1862_init()
{
    if (pcm1862_readreg(0x05) != 0x86)
        return -1;

    //pcm1862_writereg(0x00, 0xff);
    pcm1862_writereg(PCM1862_CLKCONFIG, 0x7e);

    pcm1862_writereg(PCM1862_PLL_SCK_DIV, 0x07);
    pcm1862_writereg(PCM1862_SCK_BCK_DIV, 0x03);
    pcm1862_writereg(PCM1862_SCK_LRCK_DIV, 0x3f);
    pcm1862_writereg(PCM1862_PLLCONFIG, 0x01);
    pcm1862_writereg(PCM1862_PLL_P, 0x01);
    pcm1862_writereg(PCM1862_PLL_R, 0x00);
    pcm1862_writereg(PCM1862_PLL_J, 0x07);
    pcm1862_writereg(PCM1862_PLL_D_LSB, 0x02);
    pcm1862_writereg(PCM1862_PLL_D_MSB, 0x0b);
    //while (!(pcm1862_readreg(PCM1862_PLLCONFIG) & (1<<4))) {}

    pcm1862_writereg(PCM1862_DSP1_CLKDIV, 0x01);
    pcm1862_writereg(PCM1862_DSP2_CLKDIV, 0x01);
    pcm1862_writereg(PCM1862_ADC_CLKDIV,  0x0f);
    pcm1862_writereg(PCM1862_DSP_CTRL, 0x30);

    return 0;
}
