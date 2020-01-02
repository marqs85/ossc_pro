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
#include "isl51002.h"
#include "i2c_opencores.h"

#define PCNT_TOLERANCE 50
#define HPX16_TOLERANCE 10

void isl_writereg(isl51002_dev *dev, uint8_t regaddr, uint8_t data) {
    I2C_start(I2CA_BASE, dev->i2c_addr, 0);
    I2C_write(I2CA_BASE, regaddr, 0);
    I2C_write(I2CA_BASE, data, 1);
}

uint8_t isl_readreg(isl51002_dev *dev, uint8_t regaddr) {
    //Phase 1
    I2C_start(I2CA_BASE, dev->i2c_addr, 0);
    I2C_write(I2CA_BASE, regaddr, 0);

    //Phase 2
    I2C_start(I2CA_BASE, dev->i2c_addr, 1);
    return I2C_read(I2CA_BASE,1);
}

int isl_init(isl51002_dev *dev) {
    isl_enable_power(dev, 0);

    // 8-bit output
    isl_writereg(dev, ISL_OUTFORMAT1, 0x08);

    // enable XTALCLK output, set HSout active low
    isl_writereg(dev, ISL_OUTFORMAT2, 0x48);

    // CH1 SOG only
    isl_writereg(dev, ISL_SYNCPOLLCFG, 0x20);

    // lock to hsync leading edge
    isl_writereg(dev, ISL_HPLL_MISCCFG, 0x01);

    // TODO: check optimal way
    isl_writereg(dev, ISL_MEASCFG, 0x00);

    // fastest ABLC to minimize brightness fluctuation caused by flawed backporch clamp
    //isl_writereg(dev, ISL_ABLCCFG, 0x00);

    //testing
    //isl_writereg(dev, ISL_AFEBW, 0x00);
    //isl_writereg(dev, ISL_AFECTRL, 0x00);
    /*isl_writereg(dev, ISL_HPLL_PRECOAST, 4);
    isl_writereg(dev, ISL_HPLL_POSTCOAST, 4);*/
    /*isl_writereg(dev, ISL_ABLC_START_MSB, 0x03);
    isl_writereg(dev, ISL_ABLC_START_LSB, 16);
    isl_writereg(dev, ISL_CLAMPWIDTH, 16);*/

    return 0;
}

void isl_enable_power(isl51002_dev *dev, int enable) {
    isl_writereg(dev, ISL_POWERCTRL, enable ? 0x00 : 0x0f);
    dev->powered_on = enable;
}

void isl_enable_outputs(isl51002_dev *dev, int enable) {
    isl_writereg(dev, ISL_OUTPUTENA, enable ? 0x20 : 0xff);
}

void isl_source_sel(isl51002_dev *dev, isl_input_t input, video_sync syncinput, video_format fmt) {
    isl_writereg(dev, ISL_INPUTCFG, (input | ((fmt == FORMAT_YPbPr) ? 0x10 : 0x00)));

    if (syncinput == SYNC_SOG)
        isl_writereg(dev, ISL_SYNCSRC, 0x07);
    else if (syncinput == SYNC_CS)
        isl_writereg(dev, ISL_SYNCSRC, 0x05);
    else if (syncinput == SYNC_HV)
        isl_writereg(dev, ISL_SYNCSRC, 0x01);
    else
        isl_writereg(dev, ISL_SYNCSRC, 0x00); // auto
}

int isl_check_activity(isl51002_dev *dev, isl_input_t input, video_sync syncinput) {
    uint8_t sync_active, sync_activity = 0;
    video_sync act = 0;
    int activity_change = 0;

    if (input == ISL_CH0)
        sync_activity = isl_readreg(dev, ISL_CH0_CH1_STATUS) & 0x0f;
    else if (input == ISL_CH1)
        sync_activity = isl_readreg(dev, ISL_CH0_CH1_STATUS) >> 4;
    else if (input == ISL_CH2)
        sync_activity = isl_readreg(dev, ISL_CH2_STATUS) & 0x0f;

    if ((sync_activity & 0x01) == 0x01)
        act |= SYNC_CS;
    if ((sync_activity & 0x03) == 0x03)
        act |= SYNC_HV;
    if ((sync_activity & 0x0c) == 0x08)
        act |= SYNC_SOG;

    sync_active = !!(act & syncinput);

    if (sync_active != dev->sync_active) {
        activity_change = 1;
        memset(&dev->ss, 0, sizeof(isl51002_sync_status));

        printf("isl activity: 0x%lx\n", sync_activity);
    }

    dev->sync_active = sync_active;

    return activity_change;
}

int isl_get_sync_stats(isl51002_dev *dev, uint16_t vtotal, uint8_t interlace_flag, uint32_t pcnt_frame) {
    uint8_t sync_params;
    uint16_t h_period_x16;
    int mode_changed = 0;

    sync_params = isl_readreg(dev, ISL_SYNCTYPE);

    /*if (dev->sync_params & (1<<7))
        isl_writereg(dev, ISL_MEASCFG, 0x00);
    else
        isl_writereg(dev, ISL_MEASCFG, 0x02);*/

    h_period_x16 = (isl_readreg(dev, ISL_HSYNCPERIOD_MSB) << 8) | isl_readreg(dev, ISL_HSYNCPERIOD_LSB);
#ifdef ISL_MEAS_HZ
    if (h_period_x16 == 0) {
        h_period_x16 = (16*pcnt_frame)/vtotal;
    }
#endif

    dev->sm.v_totlines = ((isl_readreg(dev, ISL_VSYNCPERIOD_MSB) & 0x0f) << 8) | isl_readreg(dev, ISL_VSYNCPERIOD_LSB);
    dev->sm.h_synclen_x16 = (isl_readreg(dev, ISL_HSYNCWIDTH_MSB) << 8) | isl_readreg(dev, ISL_HSYNCWIDTH_LSB);
    dev->sm.v_synclen = isl_readreg(dev, ISL_VSYNCWIDTH) & 0x7f;
    dev->sm.h_sync_backporch = (isl_readreg(dev, ISL_DESTART_MSB) << 8) | isl_readreg(dev, ISL_DESTART_LSB);
    dev->sm.h_active = (isl_readreg(dev, ISL_DEWIDTH_MSB) << 8) | isl_readreg(dev, ISL_DEWIDTH_LSB);
    dev->sm.v_sync_backporch = (isl_readreg(dev, ISL_LINESTART_MSB) << 8) | isl_readreg(dev, ISL_LINESTART_LSB);
    dev->sm.v_active = (isl_readreg(dev, ISL_LINEWIDTH_MSB) << 8) | isl_readreg(dev, ISL_LINEWIDTH_LSB);

    if ((vtotal != dev->ss.v_total) ||
        (interlace_flag != dev->ss.interlace_flag) ||
#ifdef ISL_MEAS_HZ
        (h_period_x16 < (dev->sm.h_period_x16 - HPX16_TOLERANCE)) ||
        (h_period_x16 > (dev->sm.h_period_x16 + HPX16_TOLERANCE)) ||
#else
        (pcnt_frame < (dev->ss.pcnt_frame - PCNT_TOLERANCE)) ||
        (pcnt_frame > (dev->ss.pcnt_frame + PCNT_TOLERANCE)) ||
#endif
        ((sync_params & 0x2c) != (dev->ss.sync_params & 0x2c)))
    {
        mode_changed = 1;

        printf("isl sync params: 0x%lx\n", sync_params);
        printf("isl h_period_x16: %u\n", h_period_x16);
        printf("isl h_synclen_x16: %u\n", dev->sm.h_synclen_x16);
        printf("isl totlines: %u\n", dev->sm.v_totlines);
        printf("isl v_synclen: %u\n", dev->sm.v_synclen);
        printf("totlines: %u\n", vtotal);
        printf("interlace_flag: %u\n", interlace_flag);
        printf("pcnt_frame: %lu\n", pcnt_frame);
    }

    dev->sm.h_period_x16 = h_period_x16;
    dev->ss.sync_params = sync_params;
    dev->ss.v_total = vtotal;
    dev->ss.interlace_flag = interlace_flag;
    dev->ss.pcnt_frame = pcnt_frame;

    return mode_changed;
}

void isl_source_setup(isl51002_dev *dev, uint16_t h_samplerate) {
    isl_writereg(dev, ISL_HPLL_HTOTAL_MSB, (h_samplerate >> 8));
    isl_writereg(dev, ISL_HPLL_HTOTAL_LSB, (h_samplerate & 0xff));
}

uint16_t isl_get_pll_htotal(isl51002_dev *dev) {
    return ((isl_readreg(dev, ISL_HPLL_HTOTAL_MSB) << 8) | isl_readreg(dev, ISL_HPLL_HTOTAL_LSB));
}

void isl_de_adj(isl51002_dev *dev) {
    isl_writereg(dev, ISL_PHASEADJCMD, 0x04);
}

void isl_phase_adj(isl51002_dev *dev) {
    isl_writereg(dev, ISL_PHASEADJCMD, 0x03);
}

void isl_set_de(isl51002_dev *dev) {
    isl_writereg(dev, ISL_DESTART_MSB, dev->sm.h_sync_backporch >> 8);
    isl_writereg(dev, ISL_DESTART_LSB, dev->sm.h_sync_backporch & 0xff);
    isl_writereg(dev, ISL_DEWIDTH_MSB, dev->sm.h_active >> 8);
    isl_writereg(dev, ISL_DEWIDTH_LSB, dev->sm.h_active & 0xff);

    isl_writereg(dev, ISL_LINESTART_MSB, dev->sm.v_sync_backporch >> 8);
    isl_writereg(dev, ISL_LINESTART_LSB, dev->sm.v_sync_backporch & 0xff);
    isl_writereg(dev, ISL_LINEWIDTH_MSB, dev->sm.v_active >> 8);
    isl_writereg(dev, ISL_LINEWIDTH_LSB, dev->sm.v_active & 0xff);
}
