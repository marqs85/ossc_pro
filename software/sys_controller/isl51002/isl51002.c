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

uint16_t afe_bw_arr[] = {9, 10, 11, 12, 14, 17, 21, 24, 30, 38, 50, 75, 83, 105, 149, 450};

const isl51002_config isl_cfg_default = {
    .col = {0x154, 0x154, 0x154, 0x200, 0x200, 0x200},
    .pre_coast = 4,
    .post_coast = 4,
    .clamp_alc_start = 2,
    .clamp_alc_width = 16,
    .coast_clamp = 16,
    .alc_enable = 1,
    .alc_h_filter = 0,
    .alc_v_filter = 4,
    .afe_bw = 0,
    .hsync_vth = 4,
    .sog_vth = 6,
    .sync_gf = 4
};

void isl_writereg(isl51002_dev *dev, uint8_t regaddr, uint8_t data) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t isl_readreg(isl51002_dev *dev, uint8_t regaddr) {
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
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

    memcpy(&dev->cfg, &isl_cfg_default, sizeof(isl51002_config));

    return 0;
}

void isl_get_default_cfg(isl51002_config *cfg) {
    memcpy(cfg, &isl_cfg_default, sizeof(isl51002_config));
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

    if (((vtotal > 0) && (pcnt_frame > 0)) &&
        ((vtotal != dev->ss.v_total) ||
        (interlace_flag != dev->ss.interlace_flag) ||
#ifdef ISL_MEAS_HZ
        (h_period_x16 < (dev->sm.h_period_x16 - HPX16_TOLERANCE)) ||
        (h_period_x16 > (dev->sm.h_period_x16 + HPX16_TOLERANCE)) ||
#else
        (pcnt_frame < (dev->ss.pcnt_frame - PCNT_TOLERANCE)) ||
        (pcnt_frame > (dev->ss.pcnt_frame + PCNT_TOLERANCE)) ||
#endif
        ((sync_params & 0x2c) != (dev->ss.sync_params & 0x2c))))
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

void isl_set_sampler_phase(isl51002_dev *dev, uint8_t sampler_phase) {
    if (!sampler_phase)
        isl_writereg(dev, ISL_PHASEADJCMD, 0x03);
    else
        isl_writereg(dev, ISL_HPLL_PHASE, sampler_phase-1);
}

uint16_t isl_get_afe_bw_value(uint8_t index) {
    if (index >= sizeof(afe_bw_arr)/sizeof(uint16_t))
        return 0;
    else
        return afe_bw_arr[index];
}

void isl_set_afe_bw(isl51002_dev *dev, uint32_t dotclk_hz) {
    uint8_t bw_sel;
    uint32_t target_bw_hz;

    // Set AFE -3dB bandwidth to (2/3) of input pixel clock
    target_bw_hz = (2*dotclk_hz)/3;

    if (target_bw_hz <= 9000000)
        dev->auto_bw_sel = 0x00;
    else if (target_bw_hz <= 10000000)
        dev->auto_bw_sel = 0x01;
    else if (target_bw_hz <= 11000000)
        dev->auto_bw_sel = 0x02;
    else if (target_bw_hz <= 12000000)
        dev->auto_bw_sel = 0x03;
    else if (target_bw_hz <= 14000000)
        dev->auto_bw_sel = 0x04;
    else if (target_bw_hz <= 17000000)
        dev->auto_bw_sel = 0x05;
    else if (target_bw_hz <= 21000000)
        dev->auto_bw_sel = 0x06;
    else if (target_bw_hz <= 24000000)
        dev->auto_bw_sel = 0x07;
    else if (target_bw_hz <= 30000000)
        dev->auto_bw_sel = 0x08;
    else if (target_bw_hz <= 38000000)
        dev->auto_bw_sel = 0x09;
    else if (target_bw_hz <= 50000000)
        dev->auto_bw_sel = 0x0a;
    else if (target_bw_hz <= 75000000)
        dev->auto_bw_sel = 0x0b;
    else if (target_bw_hz <= 83000000)
        dev->auto_bw_sel = 0x0c;
    else if (target_bw_hz <= 105000000)
        dev->auto_bw_sel = 0x0d;
    else if (target_bw_hz <= 149000000)
        dev->auto_bw_sel = 0x0e;
    else
        dev->auto_bw_sel = 0x0f;

    if (!dev->cfg.afe_bw) {
        isl_writereg(dev, ISL_AFEBW, dev->auto_bw_sel);
        printf("AFE BW auto-set to %uMHz\n\n", afe_bw_arr[dev->auto_bw_sel]);
    }
}

uint16_t isl_get_afe_bw(isl51002_dev *dev, uint8_t afe_bw) {
    if (!afe_bw)
        return afe_bw_arr[dev->auto_bw_sel];
    else
        return afe_bw_arr[afe_bw-1];
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

void isl_update_config(isl51002_dev *dev, isl51002_config *cfg) {
    uint8_t val;

    if (memcmp(&cfg->col, &dev->cfg.col, sizeof(color_setup_t))) {
        isl_writereg(dev, ISL_R_GAIN_MSB, (cfg->col.r_gain >> 2));
        isl_writereg(dev, ISL_R_GAIN_LSB, (cfg->col.r_gain << 6));
        isl_writereg(dev, ISL_G_GAIN_MSB, (cfg->col.g_gain >> 2));
        isl_writereg(dev, ISL_G_GAIN_LSB, (cfg->col.g_gain << 6));
        isl_writereg(dev, ISL_B_GAIN_MSB, (cfg->col.b_gain >> 2));
        isl_writereg(dev, ISL_B_GAIN_LSB, (cfg->col.b_gain << 6));
        isl_writereg(dev, ISL_R_OFFSET_MSB, (cfg->col.r_offs >> 2));
        isl_writereg(dev, ISL_R_OFFSET_LSB, (cfg->col.r_offs << 6));
        isl_writereg(dev, ISL_G_OFFSET_MSB, (cfg->col.g_offs >> 2));
        isl_writereg(dev, ISL_G_OFFSET_LSB, (cfg->col.g_offs << 6));
        isl_writereg(dev, ISL_B_OFFSET_MSB, (cfg->col.b_offs >> 2));
        isl_writereg(dev, ISL_B_OFFSET_LSB, (cfg->col.b_offs << 6));
    }

    if (cfg->pre_coast != dev->cfg.pre_coast)
        isl_writereg(dev, ISL_HPLL_PRECOAST, cfg->pre_coast);
    if (cfg->post_coast != dev->cfg.post_coast)
        isl_writereg(dev, ISL_HPLL_POSTCOAST, cfg->post_coast);
    if (cfg->clamp_alc_start != dev->cfg.clamp_alc_start) {
        isl_writereg(dev, ISL_ABLC_START_MSB, (cfg->clamp_alc_start >> 8));
        isl_writereg(dev, ISL_ABLC_START_LSB, (cfg->clamp_alc_start & 0xff));
    }
    if (cfg->clamp_alc_width != dev->cfg.clamp_alc_width)
        isl_writereg(dev, ISL_CLAMPWIDTH, cfg->clamp_alc_width);
    if (cfg->coast_clamp != dev->cfg.coast_clamp) {
        val = isl_readreg(dev, ISL_AFECTRL) & ~(1<<2);
        isl_writereg(dev, ISL_AFECTRL, val | (cfg->coast_clamp<<2));
    }
    if (cfg->alc_enable != dev->cfg.alc_enable) {
        val = isl_readreg(dev, ISL_ABLCCFG) & ~(1<<0);
        isl_writereg(dev, ISL_ABLCCFG, val | !cfg->alc_enable);
    }
    if (cfg->alc_h_filter != dev->cfg.alc_h_filter) {
        val = isl_readreg(dev, ISL_ABLCCFG) & ~(3<<2);
        isl_writereg(dev, ISL_ABLCCFG, val | (cfg->alc_h_filter<<2));
    }
    if (cfg->alc_v_filter != dev->cfg.alc_v_filter) {
        val = isl_readreg(dev, ISL_ABLCCFG) & ~(7<<4);
        isl_writereg(dev, ISL_ABLCCFG, val | (cfg->alc_v_filter<<4));
    }
    if (cfg->hsync_vth != dev->cfg.hsync_vth)
        isl_writereg(dev, ISL_HSYNC_VTH, (cfg->hsync_vth<<4) | cfg->hsync_vth);
    if (cfg->sog_vth != dev->cfg.sog_vth)
        isl_writereg(dev, ISL_SOG_VTH, cfg->sog_vth);
    if (cfg->sync_gf != dev->cfg.sync_gf) {
        val = isl_readreg(dev, ISL_SYNCCFG) & ~(0x0f<<0);
        isl_writereg(dev, ISL_SYNCCFG, val | cfg->sync_gf);
    }

    if (cfg->afe_bw != dev->cfg.afe_bw) {
        if (!cfg->afe_bw) {
            isl_writereg(dev, ISL_AFEBW, dev->auto_bw_sel);
            printf("AFE BW auto-set to %uMHz\n\n", afe_bw_arr[dev->auto_bw_sel]);
        } else {
            isl_writereg(dev, ISL_AFEBW, cfg->afe_bw-1);
            printf("AFE BW manually set to %uMHz\n\n", afe_bw_arr[cfg->afe_bw-1]);
        }
    }

    memcpy(&dev->cfg, cfg, sizeof(isl51002_config));}
