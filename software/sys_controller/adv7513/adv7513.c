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

#include <string.h>
#include "adv7513.h"
#include "i2c_opencores.h"

const adv7513_config adv7513_cfg_default = {
    .tx_mode = TX_HDMI_RGB_FULL,
    .audio_fmt = ADV_AUDIO_I2S,
    .i2s_fs = ADV_48KHZ,
    .i2s_chcfg = ADV_2CH_STEREO
};

void adv7513_writereg(adv7513_dev *dev, uint8_t regaddr, uint8_t data)
{
    I2C_start(dev->i2cm_base, (dev->main_base>>1), 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t adv7513_readreg(adv7513_dev *dev, uint8_t regaddr)
{
    //Phase 1
    I2C_start(dev->i2cm_base, (dev->main_base>>1), 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, (dev->main_base>>1), 1);
    return I2C_read(dev->i2cm_base,1);
}

void adv7513_init(adv7513_dev *dev, uint8_t load_default_cfg) {
    if (load_default_cfg)
        memcpy(&dev->cfg, &adv7513_cfg_default, sizeof(adv7513_config));

    //Set IO mapping
    adv7513_writereg(dev, 0x43, dev->edid_base);
    adv7513_writereg(dev, 0x45, dev->pktmem_base);
    adv7513_writereg(dev, 0xE1, dev->cec_base);

    // Return if no display is detected
    if ((adv7513_readreg(dev, 0x42) & 0x60) != 0x60)
        return;

    // Power up TX
    adv7513_writereg(dev, 0x41, 0x10);
    //adv7513_writereg(0xd6, 0xc0);
    dev->powered_on = 1;

    // Setup fixed registers
    adv7513_writereg(dev, 0x98, 0x03);
    adv7513_writereg(dev, 0x9A, 0xE0);
    adv7513_writereg(dev, 0x9C, 0x30);
    adv7513_writereg(dev, 0x9D, 0x01);
    adv7513_writereg(dev, 0xA2, 0xA4);
    adv7513_writereg(dev, 0xA3, 0xA4);
    adv7513_writereg(dev, 0xE0, 0xD0);
    adv7513_writereg(dev, 0xF9, 0x00);

    // Setup audio format
    adv7513_writereg(dev, 0x12, 0x20); // disable copyright protection
    adv7513_writereg(dev, 0x13, 0x20); // set category code
    adv7513_writereg(dev, 0x14, 0x0B); // 24-bit audio
    adv7513_writereg(dev, 0x15, 0x20); // 24-bit RGB/YCbCr

    // Input video format
    adv7513_writereg(dev, 0x16, 0x30); // RGB 8bpc
    adv7513_writereg(dev, 0x17, 0x02); // 16:9 aspect

    // HDMI/DVI output selection
    adv7513_set_tx_mode(dev, dev->cfg.tx_mode);

    // No clock delay (?)
    adv7513_writereg(dev, 0xBA, 0x60);

    // I2S0 input for I2S audio
    adv7513_writereg(dev, 0x0C, 0x04);

    // Quad stereo
    adv7513_writereg(dev, 0x10, 0x01);

    adv7513_set_audio(dev, dev->cfg.audio_fmt, dev->cfg.i2s_fs, dev->cfg.i2s_chcfg);

    // Setup manual pixel repetition
    adv7513_writereg(dev, 0x3B, (0xC0 | (dev->pixelrep << 3) | (dev->pixelrep_infoframe << 1)));

    // Set VIC
    adv7513_writereg(dev, 0x3C, dev->vic);
}

void adv7513_get_default_cfg(adv7513_config *cfg) {
    memcpy(cfg, &adv7513_cfg_default, sizeof(adv7513_config));
}

void adv7513_set_audio(adv7513_dev *dev, adv7513_audio_fmt_t fmt, adv7513_i2s_fs_t i2s_fs, adv7513_i2s_chcfg_t i2s_chcfg) {
    uint32_t N=6144;
    uint8_t fs_val=0, ca_val=0, cc_val=0;
    uint8_t val;

    if (fmt == ADV_AUDIO_SPDIF) {
        N = 6144;
        adv7513_writereg(dev, 0x0A, 0x10);
        adv7513_writereg(dev, 0x0B, 0x8e);
    } else {
        adv7513_writereg(dev, 0x0A, 0x00);
        adv7513_writereg(dev, 0x0B, 0x0e);

        switch (i2s_fs) {
            case ADV_48KHZ:
                N = 6144;
                fs_val = 0x2;
                break;
            case ADV_96KHZ:
                N = 12288;
                fs_val = 0xa;
                break;
            case ADV_192KHZ:
                N = 24576;
                fs_val = 0xe;
                break;
            default:
                break;
        }

        val = adv7513_readreg(dev, 0x15) & 0x0f;
        adv7513_writereg(dev, 0x15, val | (fs_val<<4));

        switch (i2s_chcfg) {
            case ADV_2CH_STEREO:
                ca_val = 0x00;
                cc_val = 0x1;
                break;
            case ADV_4CH_STEREO_4p0:
                ca_val = 0x08;
                cc_val = 0x3;
                break;
            case ADV_4CH_STEREO_5p1:
                ca_val = 0x0b;
                cc_val = 0x5;
                break;
            case ADV_4CH_STEREO_7p1:
                ca_val = 0x1f;
                cc_val = 0x7;
                break;
            default:
                break;
        }
    }

    // Audio regeneration settings
    adv7513_writereg(dev, 0x01, (N>>16));
    adv7513_writereg(dev, 0x02, ((N>>8) & 0xff));
    adv7513_writereg(dev, 0x03, (N & 0xff));

    adv7513_writereg(dev, 0x4A, 0xa0); // Enable Audio InfoFrame modify
    adv7513_writereg(dev, 0x73, cc_val);
    adv7513_writereg(dev, 0x76, ca_val);
    adv7513_writereg(dev, 0x4A, 0x80); // Disable Audio InfoFrame modify
}

void adv7513_set_tx_mode(adv7513_dev *dev, tx_mode_t mode) {
    if (mode == TX_HDMI_YCBCR444)
        adv7513_set_csc_mode(dev, 1, CSC_RGB_FULL, CSC_YCBCR_709);
    else if (mode == TX_HDMI_RGB_LIM)
        adv7513_set_csc_mode(dev, 1, CSC_RGB_FULL, CSC_RGB_LIMITED);
    else
        adv7513_set_csc_mode(dev, 0, CSC_RGB_FULL, CSC_RGB_FULL);

    adv7513_writereg(dev, 0xAF, ((1<<2) | ((mode != TX_DVI) << 1)));

    // Setup InfoFrame
    adv7513_writereg(dev, 0x4A, 0xC0); // Enable AVI InfoFrame modify
    adv7513_writereg(dev, 0x55, 0x02 | ((mode==TX_HDMI_YCBCR444)<<6));  // RGB/YCbCr444, no overscan
    adv7513_writereg(dev, 0x57, (mode==TX_HDMI_RGB_LIM) ? 0x04 : 0x08); // Full/limited range RGB
    adv7513_writereg(dev, 0x4A, 0x80); // Disable AVI InfoFrame modify
}

void adv7513_set_csc_mode(adv7513_dev *dev, uint8_t enable, csc_colorspace_t src, csc_colorspace_t dst) {
    uint8_t val;
    uint8_t coeffs_rgbf_ycbcr709[] = {0x86, 0xff, 0x19, 0xa6, 0x1f, 0x5b, 0x08, 0x00,
                                      0x02, 0xe9, 0x09, 0xcb, 0x00, 0xfd, 0x01, 0x00,
                                      0x1e, 0x66, 0x1a, 0x9b, 0x06, 0xff, 0x08, 0x00};
    uint8_t coeffs_rgbf_rgbl[] =     {0x8d, 0xbc, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00,
                                      0x00, 0x00, 0x0d, 0xbc, 0x00, 0x00, 0x01, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x0d, 0xbc, 0x01, 0x00};
    int i;

    if (!enable) {
        val = adv7513_readreg(dev, 0x18) & ~(1<<7);
        adv7513_writereg(dev, 0x18, val);
    } else {
        if ((src==CSC_RGB_FULL) && (dst==CSC_YCBCR_709)) {
            for (i=0; i<sizeof(coeffs_rgbf_ycbcr709)/sizeof(uint8_t); i++)
                adv7513_writereg(dev, 0x18+i, coeffs_rgbf_ycbcr709[i]);
        } else if ((src==CSC_RGB_FULL) && (dst==CSC_RGB_LIMITED)) {
            for (i=0; i<sizeof(coeffs_rgbf_rgbl)/sizeof(uint8_t); i++)
                adv7513_writereg(dev, 0x18+i, coeffs_rgbf_rgbl[i]);
        }
    }

    val = adv7513_readreg(dev, 0x16) & ~(1<<0);
    adv7513_writereg(dev, 0x16, val | (dst>CSC_RGB_LIMITED));
}

void adv7513_set_pixelrep_vic(adv7513_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_Video_Type vic) {
    adv7513_writereg(dev, 0x3B, (0xC0 | (pixelrep << 3) | (pixelrep_infoframe << 1)));
    adv7513_writereg(dev, 0x3C, vic);

    dev->pixelrep = pixelrep;
    dev->pixelrep_infoframe = pixelrep_infoframe;
    dev->vic = vic;
}

int adv7513_check_hpd_power(adv7513_dev *dev) {
    int activity_change = 0;

    uint8_t powered_on = ((adv7513_readreg(dev, 0x42) & 0x60) == 0x60);

    if (powered_on) {
        if (!dev->powered_on) {
            activity_change = 1;

            printf("Power-on ADV7513\n\n");
            adv7513_init(dev, 0);
        }
    } else {
        dev->powered_on = 0;
    }

    return activity_change;
}

void adv7513_update_config(adv7513_dev *dev, adv7513_config *cfg) {
    if (dev->powered_on) {
        if (cfg->tx_mode != dev->cfg.tx_mode)
            adv7513_set_tx_mode(dev, cfg->tx_mode);
        if ((cfg->audio_fmt != dev->cfg.audio_fmt) ||
            (cfg->i2s_fs != dev->cfg.i2s_fs) ||
            (cfg->i2s_chcfg != dev->cfg.i2s_chcfg))
            adv7513_set_audio(dev, cfg->audio_fmt, cfg->i2s_fs, cfg->i2s_chcfg);

        memcpy(&dev->cfg, cfg, sizeof(adv7513_config));
    }
}
