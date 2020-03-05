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
#include "sysconfig.h"
#include "si5351.h"
#include "i2c_opencores.h"

si5351c_revb_register_t const si5351c_revb_registers[] =
{
    //Sys init mask
    { 0x0002, 0x0B },
    //CLK enable
    { 0x0003, 0x54 },
    //???
    { 0x0007, 0x01 },
    //Ignore OEB pin
    { 0x0009, 0xFF },
    //CLKIN div and PLL src sel
    { 0x000F, 0x04 },
    //CLK0-7
    { 0x0010, 0x0F },
    { 0x0011, 0x8C },
    { 0x0012, 0x8F },
    { 0x0013, 0x8C },
    { 0x0014, 0xAF },
    { 0x0015, 0x8C },
    { 0x0016, 0x8F },
    { 0x0017, 0x8C },
    //MSNA
    { 0x001A, 0x00 }, // P3[15:8]
    { 0x001B, 0x01 }, // P3[7:0]
    { 0x001C, 0x00 }, // {4'bx, 2'b00, P1[17:16]}
    { 0x001D, 0x0D }, // P1[15:8]
    { 0x001E, 0x00 }, // P1[7:0]
    { 0x001F, 0x00 }, // {P3[19:16], P2[19:16]}
    { 0x0020, 0x00 }, // P2[15:8]
    { 0x0021, 0x00 }, // P2[8:0]
    //MSNB
    { 0x0022, 0x00 },
    { 0x0023, 0x01 },
    { 0x0024, 0x00 },
    { 0x0025, 0x10 },
    { 0x0026, 0x00 },
    { 0x0027, 0x00 },
    { 0x0028, 0x00 },
    { 0x0029, 0x00 },
    //MS0
    { 0x002A, 0x00 }, // P3[15:8]
    { 0x002B, 0x01 }, // P3[7:0]
    { 0x002C, 0x00 }, // {1'bx, R0_DIV[2:0], DIVBY4[1:0], P1[17:16]}
    { 0x002D, 0x05 }, // P1[15:8]
    { 0x002E, 0x80 }, // P1[7:0]
    { 0x002F, 0x00 }, // {P3[19:16], P2[19:16]}
    { 0x0030, 0x00 }, // P2[15:8]
    { 0x0031, 0x00 }, // P2[8:0]
    //MS2
    { 0x003A, 0x00 },
    { 0x003B, 0x01 },
    { 0x003C, 0x00 },
    { 0x003D, 0x03 },
    { 0x003E, 0x00 },
    { 0x003F, 0x00 },
    { 0x0040, 0x00 },
    { 0x0041, 0x00 },
    //MS4
    { 0x004A, 0x00 },
    { 0x004B, 0x01 },
    { 0x004C, 0x00 },
    { 0x004D, 0x02 },
    { 0x004E, 0x80 },
    { 0x004F, 0x00 },
    { 0x0050, 0x00 },
    { 0x0051, 0x00 },
    //MS6
    { 0x005A, 0x06 },
    { 0x005B, 0x00 },
    //Spread spectrum
    { 0x0095, 0x00 },
    { 0x0096, 0x00 },
    { 0x0097, 0x00 },
    { 0x0098, 0x00 },
    { 0x0099, 0x00 },
    { 0x009A, 0x00 },
    { 0x009B, 0x00 },
    //VCXO
    { 0x00A2, 0x00 },
    { 0x00A3, 0x00 },
    { 0x00A4, 0x00 },
    //XTAL CL
    { 0x00B7, 0x12 },
    //fanout / bypass
    { 0x00BB, 0xC2 },
};

#define SI5351C_REVB_REG_CONFIG_NUM_REGS    (sizeof(si5351c_revb_registers)/sizeof(si5351c_revb_register_t))

static uint8_t si5351_readreg(si5351_dev *dev, uint8_t regaddr)
{
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

static void si5351_writereg(si5351_dev *dev, uint8_t regaddr, uint8_t data)
{
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

static void si5351_set_pll_fb_multisynth(si5351_dev *dev, si5351_pll_ch pll_ch, uint32_t p1, uint32_t p2, uint32_t p3) {
    uint8_t fb_int_reg;
    uint8_t msn_base = SI5351_MSNA_BASE + pll_ch*8;

    si5351_writereg(dev, msn_base+0, (p3 >> 8) & 0xff);
    si5351_writereg(dev, msn_base+1, (p3 & 0xff));
    si5351_writereg(dev, msn_base+2, (p1 >> 16) & 0x3);
    si5351_writereg(dev, msn_base+3, (p1 >> 8) & 0xff);
    si5351_writereg(dev, msn_base+4, (p1 & 0xff));
    si5351_writereg(dev, msn_base+5, (((p3 >> 16) & 0xf) << 4) | ((p2 >> 16) & 0xf));
    si5351_writereg(dev, msn_base+6, (p2 >> 8) & 0xff);
    si5351_writereg(dev, msn_base+7, (p2 & 0xff));

    fb_int_reg = si5351_readreg(dev, SI5351_CLK6_CTRL+pll_ch);
    fb_int_reg &= ~(1<<6);
    if ((p1 % 256) == 0) {
        fb_int_reg |= (1<<6);
        printf("Si5351 set PLL FB multisynth to integer mode\n");
    }
    si5351_writereg(dev, SI5351_CLK6_CTRL+pll_ch, fb_int_reg);
}

static void si5351_set_output_multisynth(si5351_dev *dev, si5351_out_ch out_ch, uint32_t p1, uint32_t p2, uint32_t p3, uint8_t outdiv, uint8_t divby4) {
    uint8_t ms_int_reg;
    uint8_t ms_base = SI5351_MS0_BASE + out_ch*8;

    si5351_writereg(dev, ms_base+0, (p3 >> 8) & 0xff);
    si5351_writereg(dev, ms_base+1, (p3 & 0xff));
    si5351_writereg(dev, ms_base+2, (outdiv<<4) | (divby4<<2) | ((p1 >> 16) & 0x3));
    si5351_writereg(dev, ms_base+3, (p1 >> 8) & 0xff);
    si5351_writereg(dev, ms_base+4, (p1 & 0xff));
    si5351_writereg(dev, ms_base+5, (((p3 >> 16) & 0xf) << 4) | ((p2 >> 16) & 0xf));
    si5351_writereg(dev, ms_base+6, (p2 >> 8) & 0xff);
    si5351_writereg(dev, ms_base+7, (p2 & 0xff));

    ms_int_reg = si5351_readreg(dev, SI5351_CLK0_CTRL+out_ch);
    ms_int_reg &= ~(1<<6);
    if ((p1 % 256) == 0) {
        ms_int_reg |= (1<<6);
        printf("Si5351 set Output multisynth to integer mode\n");
    }
    si5351_writereg(dev, SI5351_CLK0_CTRL+out_ch, ms_int_reg);
}

static void si5351_configure_pll(si5351_dev *dev, si5351_pll_ch pll_ch, si5351_clk_src clksrc, uint8_t clkin_div_regval) {
    uint8_t acc_reg;

    acc_reg = si5351_readreg(dev, SI5351_PLL_SRC);
    acc_reg &= ~(1<<(2+pll_ch));
    acc_reg |= (clksrc<<(2+pll_ch));
    acc_reg &= ~(3<<6);
    acc_reg |= (clkin_div_regval<<6);
    si5351_writereg(dev, SI5351_PLL_SRC, acc_reg);
}

static void si5351_configure_clk(si5351_dev *dev, si5351_pll_ch pll_ch, si5351_out_ch out_ch, si5351_clk_src clksrc, uint8_t bypass) {
    uint8_t acc_reg;
    uint8_t outsrc = bypass ? clksrc : 3;

    acc_reg = si5351_readreg(dev, SI5351_CLK0_CTRL+out_ch);
    acc_reg &= ~((1<<7)|(1<<5)|(3<<2));
    acc_reg |= (pll_ch<<5)|(outsrc<<2);
    si5351_writereg(dev, SI5351_CLK0_CTRL+out_ch, acc_reg);
}

static void si5351_enable_output(si5351_dev *dev, si5351_out_ch out_ch) {
    uint8_t acc_reg;

    acc_reg = si5351_readreg(dev, SI5351_OEN_CTRL);
    acc_reg &= ~(1<<out_ch);
    si5351_writereg(dev, SI5351_OEN_CTRL, acc_reg);
}

static inline void si5351_pll_reset(si5351_dev *dev, si5351_pll_ch pll_ch) {
    si5351_writereg(dev, SI5351_PLL_RESET, (1<<(5+2*pll_ch)));
}


void si5351_set_frac_mult(si5351_dev *dev, si5351_pll_ch pll_ch, si5351_out_ch out_ch, si5351_clk_src clksrc, si5351_ms_config_t *ms_conf) {

    //set PLL source and clockdiv
    si5351_configure_pll(dev, pll_ch, clksrc, ms_conf->clkin_div_regval);

    // set PLL and output multisynth
    si5351_set_pll_fb_multisynth(dev, pll_ch, ms_conf->msn_p1, ms_conf->msn_p2, ms_conf->msn_p3);
    si5351_set_output_multisynth(dev, out_ch, ms_conf->ms_p1, ms_conf->ms_p2, ms_conf->ms_p3, ms_conf->outdiv, ms_conf->divby4);

    // Set MS & output source clocks and power up output clock driver
    si5351_configure_clk(dev, pll_ch, out_ch, clksrc, 0);

    // Reset PLL to prevent occasional lockup
    si5351_pll_reset(dev, pll_ch);

    // Enable clock output
    si5351_enable_output(dev, out_ch);

    return;
}

int si5351_set_integer_mult(si5351_dev *dev, si5351_pll_ch pll_ch, si5351_out_ch out_ch, si5351_clk_src clksrc, uint32_t clksrc_hz, uint8_t mult) {
    uint32_t fbdiv_x100, msn_a, msn_p1, ms_a, ms_p1;
    uint8_t clkin_div, clkin_div_regval;
    uint8_t outdiv=0, divby4=0;
    uint8_t optim_ratio;

    if ((mult == 0) || (clksrc_hz < SI_CLKIN_MIN_FREQ) || (clksrc_hz*mult > SI_MAX_OUTPUT_FREQ)) {
        printf("ERROR: Si5351 max. output freq exceeded\n\n");
        return -1;
    }

    if (mult == 1) {
        si5351_configure_clk(dev, pll_ch, out_ch, clksrc, 1);
        si5351_configure_pll(dev, pll_ch, SI_XTAL, 0);
        printf("Si5351 Clock source bypass\n\n");
    } else {
        clkin_div = (clksrc_hz / SI_CLKIN_MAX_FREQ) + 1;
        if (clkin_div > 4) {
            clkin_div = 8;
            clkin_div_regval = 3;
        } else if (clkin_div > 2) {
            clkin_div = 4;
            clkin_div_regval = 2;
        } else {
            clkin_div_regval = clkin_div-1;
        }

        //set PLL source and clockdiv
        si5351_configure_pll(dev, pll_ch, clksrc, clkin_div_regval);

        //use even fbdiv for lowest jitter
        optim_ratio = 2*clkin_div*mult;
        fbdiv_x100 = (SI_VCO_CENTER_FREQ / ((clksrc_hz/clkin_div) / 100));
        msn_a = optim_ratio*((fbdiv_x100+((optim_ratio/2)*100)) / (optim_ratio*100));
        if ((msn_a < 16) || (msn_a > 90))
            printf("ERROR: Si5351 invalid msn_a of %u\n\n", msn_a);
        msn_p1 = 128*msn_a - 512;
        si5351_set_pll_fb_multisynth(dev, pll_ch, msn_p1, 0, 1);

        ms_a = msn_a / (mult*clkin_div);
        // 6 and 8 seem to be ok in integer mode despite what AN619 4.1.2 implies
        if (ms_a == 4) {
            ms_p1 = 0;
            divby4 = 3;
            printf("Si5351 enable DIVBY4\n");
        } else {
            ms_p1 = 128*ms_a - 512;
        }
        si5351_set_output_multisynth(dev, out_ch, ms_p1, 0, 1, outdiv, divby4);
        printf("Si5351 VCO freq: %uMHz (srcdiv=%u) (msn_a=%u) (ms_a=%u)\n\n", (msn_a*(clksrc_hz/clkin_div))/1000000, clkin_div, msn_a, ms_a);

        // Set MS & output source clocks and power up output clock driver
        si5351_configure_clk(dev, pll_ch, out_ch, clksrc, 0);

        // Reset PLL to prevent occasional lockup
        si5351_pll_reset(dev, pll_ch);
    }

    // Enable clock output
    si5351_enable_output(dev, out_ch);

    return 0;
}

void si5351_init(si5351_dev *dev) {
    int i;
    uint8_t ret;

    // Wait until Si5351 initialization is complete
    while ((si5351_readreg(dev, 0x00) & 0x80) == 0x80) ;

    for (i=0; i<SI5351C_REVB_REG_CONFIG_NUM_REGS; i++)
        si5351_writereg(dev, si5351c_revb_registers[i].address, si5351c_revb_registers[i].value);

    /*printf("Waiting PLL lock\n");
    while (((ret = si5351_readreg(0x00)) & (7<<4)) != 0x00) {
        printf("ret: 0x%.2x\n", ret);
        usleep(1000000);
    }*/
}
