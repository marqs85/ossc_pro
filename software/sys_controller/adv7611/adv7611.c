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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sysconfig.h"
#include "adv7611.h"
#include "i2c_opencores.h"

#define PCLK_HZ_TOLERANCE 1000000UL

void adv7611_writereg(uint8_t map, uint8_t regaddr, uint8_t data)
{
    I2C_start(I2CA_BASE, (map>>1), 0);
    I2C_write(I2CA_BASE, regaddr, 0);
    I2C_write(I2CA_BASE, data, 1);
}

uint8_t adv7611_readreg(uint8_t map, uint8_t regaddr)
{
    //Phase 1
    I2C_start(I2CA_BASE, (map>>1), 0);
    I2C_write(I2CA_BASE, regaddr, 0);

    //Phase 2
    I2C_start(I2CA_BASE, (map>>1), 1);
    return I2C_read(I2CA_BASE,1);
}

void adv7611_init(adv7611_dev *dev) {
    int i;

    // Set I2C mapping
    adv7611_writereg(dev->io_base, 0xf4, dev->cec_base);
    adv7611_writereg(dev->io_base, 0xf5, dev->infoframe_base);
    adv7611_writereg(dev->io_base, 0xf8, dev->dpll_base);
    adv7611_writereg(dev->io_base, 0xf9, dev->ksv_base);
    adv7611_writereg(dev->io_base, 0xfa, dev->edid_base);
    adv7611_writereg(dev->io_base, 0xfb, dev->hdmi_base);
    adv7611_writereg(dev->io_base, 0xfd, dev->cp_base);

    // ???
    //adv7611_writereg(dev->cp_base, 0x6c, 00);
    //adv7611_writereg(dev->hdmi_base, 0x9b, 03);

    // power up
    adv7611_writereg(dev->io_base, 0x0c, 0x00);

    // activate pads
    adv7611_writereg(dev->io_base, 0x15, 0x00);

    // Component order and XTAL freq select
    adv7611_writereg(dev->io_base, 0x04, 0x60);

    // pixel output port config
    adv7611_writereg(dev->io_base, 0x03, 0x40);

    // output color mode
    adv7611_writereg(dev->io_base, 0x02, 0xf2);

    // set free run mode 1080p
    adv7611_writereg(dev->io_base, 0x01, 0x06);
    adv7611_writereg(dev->io_base, 0x00, 0x19);

    // set HPA mode
    /*adv7611_writereg(dev->io_base, 0x20, 0x70);
    adv7611_writereg(dev->hdmi_base, 0x6c, 0x23);
    usleep(1000000);*/
    adv7611_writereg(dev->hdmi_base, 0x6c, 0x22);

    //program and enabled EDID
    for (i=0; i<dev->edid_len; i++)
        adv7611_writereg(dev->edid_base, i, dev->edid[i]);
    adv7611_writereg(dev->ksv_base, 0x74, 0x01);

    // enable TMDS termination
    adv7611_writereg(dev->hdmi_base, 0x83, 0x00);
}

int adv7611_check_activity(adv7611_dev *dev) {
    uint8_t sync_activity, sync_active;
    int activity_change = 0;

    /*retval = adv7611_readreg(ADV7611_IO_BASE, 0x6f);
    printf("+5V det: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_IO_BASE, 0x6a);
    printf("TMDS det: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_IO_BASE, 0x21);
    printf("HPA stat: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_HDMI_BASE, 0x05);
    printf("HDMI stat: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_HDMI_BASE, 0x07);
    printf("VF stat: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_HDMI_BASE, 0x09);
    printf("Height: %d\n", ((retval&0x1f)<<8) | adv7611_readreg(ADV7611_HDMI_BASE, 0x0a));

    retval = adv7611_readreg(ADV7611_HDMI_BASE, 0x83);
    printf("term stat: 0x%lx\n", retval);

    retval = adv7611_readreg(ADV7611_KSV_BASE, 0x76);
    printf("edid enable: 0x%lx\n", retval);*/

    // check V_LOCKED_RAW
    sync_activity = adv7611_readreg(dev->io_base, 0x6a);
    sync_active = !!(sync_activity & 0x2);

    if (sync_active != dev->sync_active) {
        activity_change = 1;
        memset(&dev->ss, 0, sizeof(adv7611_sync_status));
        dev->pclk_hz = 0;
        dev->pixelrep = 0;

        printf("advrx activity: 0x%lx\n", sync_activity);
    }

    dev->sync_active = sync_active;

    return activity_change;
}

int adv7611_get_sync_stats(adv7611_dev *dev) {
    int mode_changed = 0;
    adv7611_sync_status ss = {0};
    uint32_t pclk_hz;
    uint8_t pixelrep;
    uint8_t regval;

    ss.interlace_flag = !!(adv7611_readreg(dev->hdmi_base, 0x0b) & (1<<5));

    if (!ss.interlace_flag) {
        ss.v_total = (((adv7611_readreg(dev->hdmi_base, 0x26) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x27))/2;
        ss.v_synclen = (((adv7611_readreg(dev->hdmi_base, 0x2e) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x2f))/2;
        ss.v_backporch = (((adv7611_readreg(dev->hdmi_base, 0x32) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x33))/2;
        ss.v_active = (((adv7611_readreg(dev->hdmi_base, 0x09) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x0a));
    } else {
        ss.v_total = ((((adv7611_readreg(dev->hdmi_base, 0x26) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x27)) + (((adv7611_readreg(dev->hdmi_base, 0x28) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x29)))/2;
        ss.v_synclen = ((((adv7611_readreg(dev->hdmi_base, 0x2e) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x2f)) + (((adv7611_readreg(dev->hdmi_base, 0x30) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x31)))/2;
        ss.v_backporch = ((((adv7611_readreg(dev->hdmi_base, 0x32) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x33)) + (((adv7611_readreg(dev->hdmi_base, 0x34) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x35)))/2;
        ss.v_active = (((adv7611_readreg(dev->hdmi_base, 0x09) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x0a)) + (((adv7611_readreg(dev->hdmi_base, 0x0b) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x0c));
    }

    ss.h_total = ((adv7611_readreg(dev->hdmi_base, 0x1e) & 0x3f) << 8) | adv7611_readreg(dev->hdmi_base, 0x1f);
    ss.h_synclen = ((adv7611_readreg(dev->hdmi_base, 0x22) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x23);
    ss.h_backporch = ((adv7611_readreg(dev->hdmi_base, 0x24) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x25);
    ss.h_active = ((adv7611_readreg(dev->hdmi_base, 0x07) & 0x1f) << 8) | adv7611_readreg(dev->hdmi_base, 0x08);

    regval = adv7611_readreg(dev->hdmi_base, 0x05);
    ss.h_polarity = !!(regval & (1<<5));
    ss.v_polarity = !!(regval & (1<<4));
    pixelrep = regval & 0xf;

    regval = adv7611_readreg(dev->hdmi_base, 0x52);
    pclk_hz = ((adv7611_readreg(dev->hdmi_base, 0x51) << 1) | (regval >> 7))*1000000 + ((1000000*(regval & 0x7f)) / 128);

    if (memcmp(&ss, &dev->ss, sizeof(adv7611_sync_status)) ||
        (pclk_hz < (dev->pclk_hz - PCLK_HZ_TOLERANCE)) ||
        (pclk_hz > (dev->pclk_hz + PCLK_HZ_TOLERANCE)) ||
        (pixelrep != dev->pixelrep))
    {
        mode_changed = 1;

        printf("advrx h_total: %u\n", ss.h_total);
        printf("advrx h_synclen: %u\n", ss.h_synclen);
        printf("advrx h_backporch: %u\n", ss.h_backporch);
        printf("advrx h_active: %u\n", ss.h_active);
        printf("advrx v_total: %u\n", ss.v_total);
        printf("advrx v_synclen: %u\n", ss.v_synclen);
        printf("advrx v_backporch: %u\n", ss.v_backporch);
        printf("advrx v_active: %u\n", ss.v_active);
        printf("advrx sync polarities: H(%c) V(%c)\n", (ss.h_polarity ? '+' : '-'), (ss.v_polarity ? '+' : '-'));
        printf("advrx interlace_flag: %u\n", ss.interlace_flag);
        printf("advrx pclk: %luHz\n", pclk_hz);
        printf("advrx pixelrep: %u\n", pixelrep);
    }

    memcpy(&dev->ss, &ss, sizeof(adv7611_sync_status));
    dev->pclk_hz = pclk_hz;
    dev->pixelrep = pixelrep;

    return mode_changed;
}
