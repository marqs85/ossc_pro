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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "i2c_opencores.h"
#include "pcm186x.h"

const pcm186x_config pcm_cfg_default = {
    .fs = PCM_48KHZ,
    .gain = PCM_GAIN_0DB
};

static uint32_t pcm186x_readreg(pcm186x_dev *dev, uint8_t regaddr)
{
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

static void pcm186x_writereg(pcm186x_dev *dev, uint8_t regaddr, uint8_t data)
{
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

static void pcm186x_reset(pcm186x_dev *dev) {
    pcm186x_writereg(dev, PCM186X_PAGESEL, 0xfe);
}

void pcm186x_source_sel(pcm186x_dev *dev, pcm_input_t input) {
    uint8_t adc_ch = 1<<input;

    pcm186x_writereg(dev, PCM186X_ADC1L, adc_ch);
    pcm186x_writereg(dev, PCM186X_ADC1R, adc_ch);
}

void pcm186x_set_gain(pcm186x_dev *dev, int8_t db_gain) {
    int8_t gain_val = 2*db_gain;

    pcm186x_writereg(dev, PCM186X_PGA1L, gain_val);
    pcm186x_writereg(dev, PCM186X_PGA1R, gain_val);
}

void pcm186x_set_samplerate(pcm186x_dev *dev, pcm_samplerate_t fs) {
    uint8_t sck_div, dsp1_div, dsp2_div, adc_div;

    switch (fs) {
        case PCM_192KHZ:
            sck_div = 2;
            adc_div = 16;
            break;
        case PCM_96KHZ:
            sck_div = 4;
            adc_div = 16;
            break;
        default: // 48KHz
            sck_div = 8;
            adc_div = 16;
            break;
    }

    dsp1_div = sck_div;
    dsp2_div = sck_div;

    pcm186x_writereg(dev, PCM186X_PLL_SCK_DIV, sck_div-1);
    pcm186x_writereg(dev, PCM186X_DSP1_CLKDIV, dsp1_div-1);
    pcm186x_writereg(dev, PCM186X_DSP2_CLKDIV, dsp2_div-1);
    pcm186x_writereg(dev, PCM186X_ADC_CLKDIV, adc_div-1);
}

int pcm186x_init(pcm186x_dev *dev)
{
    if (pcm186x_readreg(dev, 0x05) != 0x86)
        return -1;

    pcm186x_reset(dev);

    // SCKI->PLL->[BCK,ADC,DSP1,DSP2], master mode
    pcm186x_writereg(dev, PCM186X_CLKCONFIG, 0x7e);

    // BCK = SCK/4
    pcm186x_writereg(dev, PCM186X_SCK_BCK_DIV, 0x03);

    // LRCK = BCK/64
    pcm186x_writereg(dev, PCM186X_BCK_LRCK_DIV, 0x3f);

    // Configure PLL for 98.30MHz PLL output
    pcm186x_writereg(dev, PCM186X_PLLCONFIG, 0x00);
    pcm186x_writereg(dev, PCM186X_PLL_P, 0x01);
    pcm186x_writereg(dev, PCM186X_PLL_R, 0x00);
    pcm186x_writereg(dev, PCM186X_PLL_J, 0x07);
    pcm186x_writereg(dev, PCM186X_PLL_D_LSB, 0x02);
    pcm186x_writereg(dev, PCM186X_PLL_D_MSB, 0x0b);
    pcm186x_writereg(dev, PCM186X_PLLCONFIG, 0x01);
    //while (!(pcm186x_readreg(dev, PCM186X_PLLCONFIG) & (1<<4))) {}

    pcm186x_set_samplerate(dev, PCM_48KHZ);

    // Use IIR filter
    pcm186x_writereg(dev, PCM186X_DSP_CTRL, 0x30);

    memcpy(&dev->cfg, &pcm_cfg_default, sizeof(pcm186x_config));

    return 0;
}

void pcm186x_get_default_cfg(pcm186x_config *cfg) {
    memcpy(cfg, &pcm_cfg_default, sizeof(pcm186x_config));
}

void pcm186x_update_config(pcm186x_dev *dev, pcm186x_config *cfg) {
    if (cfg->fs != dev->cfg.fs)
        pcm186x_set_samplerate(dev, cfg->fs);
    if (cfg->gain != dev->cfg.gain)
        pcm186x_set_gain(dev, cfg->gain-PCM_GAIN_0DB);

    memcpy(&dev->cfg, cfg, sizeof(pcm186x_config));
}
