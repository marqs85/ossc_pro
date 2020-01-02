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

#include "adv7513.h"
#include "i2c_opencores.h"

void adv7513_writereg(uint8_t map, uint8_t regaddr, uint8_t data)
{
    I2C_start(I2CA_BASE, (map>>1), 0);
    I2C_write(I2CA_BASE, regaddr, 0);
    I2C_write(I2CA_BASE, data, 1);
}

uint8_t adv7513_readreg(uint8_t map, uint8_t regaddr)
{
    //Phase 1
    I2C_start(I2CA_BASE, (map>>1), 0);
    I2C_write(I2CA_BASE, regaddr, 0);

    //Phase 2
    I2C_start(I2CA_BASE, (map>>1), 1);
    return I2C_read(I2CA_BASE,1);
}

void adv7513_init(adv7513_dev *dev) {
    //Set IO mapping
    adv7513_writereg(dev->main_base, 0x43, dev->edid_base);
    adv7513_writereg(dev->main_base, 0x45, dev->pktmem_base);
    adv7513_writereg(dev->main_base, 0xE1, dev->cec_base);

    // Return if no display is detected
    if ((adv7513_readreg(dev->main_base, 0x42) & 0x60) != 0x60)
        return;

    // Power up TX
    adv7513_writereg(dev->main_base, 0x41, 0x10);
    //adv7513_writereg(0xd6, 0xc0);
    dev->powered_on = 1;

    // Setup fixed registers
    adv7513_writereg(dev->main_base, 0x98, 0x03);
    adv7513_writereg(dev->main_base, 0x9A, 0xE0);
    adv7513_writereg(dev->main_base, 0x9C, 0x30);
    adv7513_writereg(dev->main_base, 0x9D, 0x01);
    adv7513_writereg(dev->main_base, 0xA2, 0xA4);
    adv7513_writereg(dev->main_base, 0xA3, 0xA4);
    adv7513_writereg(dev->main_base, 0xE0, 0xD0);
    adv7513_writereg(dev->main_base, 0xF9, 0x00);

    // Setup audio format
    adv7513_writereg(dev->main_base, 0x12, 0x20); // disable copyright protection
    adv7513_writereg(dev->main_base, 0x13, 0x20); // set category code
    adv7513_writereg(dev->main_base, 0x14, 0x0B); // 24-bit audio
    adv7513_writereg(dev->main_base, 0x15, 0x20); // 48kHz audio Fs, 24-bit RGB

    // Input video format
    adv7513_writereg(dev->main_base, 0x16, 0x30); // RGB 8bpc
    adv7513_writereg(dev->main_base, 0x17, 0x02); // 16:9 aspect

    // HDMI/DVI output selection
    adv7513_writereg(dev->main_base, 0xAF, ((1<<2) | ((dev->tx_mode != TX_DVI) << 1)));

    // No clock delay (?)
    adv7513_writereg(dev->main_base, 0xBA, 0x60);

    // Audio regeneration settings
    adv7513_writereg(dev->main_base, 0x01, 0x00);
    adv7513_writereg(dev->main_base, 0x02, 0x18);
    adv7513_writereg(dev->main_base, 0x03, 0x00); // N=6144

    if (0) {
        //i2s
        adv7513_writereg(dev->main_base, 0x0A, 0x10);
        adv7513_writereg(dev->main_base, 0x0B, 0x80);
    } else {
        //spdif
        adv7513_writereg(dev->main_base, 0x0A, 0x00);
        adv7513_writereg(dev->main_base, 0x0B, 0x00);
        adv7513_writereg(dev->main_base, 0x0C, 0x04); // I2S0 input
    }

    // Setup manual pixel repetition
    adv7513_writereg(dev->main_base, 0x3B, (0xC0 | (dev->pixelrep << 3) | (dev->pixelrep_infoframe << 1)));

    // Set VIC
    adv7513_writereg(dev->main_base, 0x3C, dev->vic);

    // Setup InfoFrame
    adv7513_writereg(dev->main_base, 0x4A, 0xC0); // Enable InfoFrame modify
    adv7513_writereg(dev->main_base, 0x55, 0x02); // No overscan
    adv7513_writereg(dev->main_base, 0x57, 0x08); // Full-range RGB
    adv7513_writereg(dev->main_base, 0x4A, 0x80); // Disable InfoFrame modify
}

void adv7513_set_pixelrep_vic(adv7513_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_Video_Type vic) {
    adv7513_writereg(dev->main_base, 0x3B, (0xC0 | (pixelrep << 3) | (pixelrep_infoframe << 1)));
    adv7513_writereg(dev->main_base, 0x3C, vic);

    dev->pixelrep = pixelrep;
    dev->pixelrep_infoframe = pixelrep_infoframe;
    dev->vic = vic;
}

int adv7513_check_hpd_power(adv7513_dev *dev) {
    int activity_change = 0;

    uint8_t powered_on = ((adv7513_readreg(dev->main_base, 0x42) & 0x60) == 0x60);

    if (powered_on) {
        if (!dev->powered_on) {
            activity_change = 1;

            printf("Power-on ADV7513\n\n");
            adv7513_init(dev);
        }
    } else {
        dev->powered_on = 0;
    }

    return activity_change;
}
