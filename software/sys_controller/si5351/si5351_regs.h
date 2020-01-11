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

#ifndef SI5351_REGS_H_
#define SI5351_REGS_H_

/* copied from export file */
typedef struct
{
    unsigned int address; /* 16-bit register address */
    unsigned char value; /* 8-bit register data */

} si5351c_revb_register_t;


#define SI5351_DEV_STATUS           0
#define SI5351_IRQ_STATUS           1

#define SI5351_IRQ_MASK             2
#define SI5351_OEN_CTRL             3
#define SI5351_OEN_CTRL_MASK        9

#define SI5351_PLL_SRC              15

#define SI5351_CLK0_CTRL            16
#define SI5351_CLK1_CTRL            17
#define SI5351_CLK2_CTRL            18
#define SI5351_CLK3_CTRL            19
#define SI5351_CLK4_CTRL            20
#define SI5351_CLK5_CTRL            21
#define SI5351_CLK6_CTRL            22
#define SI5351_CLK7_CTRL            23

#define SI5351_CLK3_0_DIS_STATE     24
#define SI5351_CLK7_4_DIS_STATE     25

#define SI5351_MSNA_BASE            26
#define SI5351_MSNB_BASE            34

#define SI5351_MS0_BASE             42
#define SI5351_MS1_BASE             50
#define SI5351_MS2_BASE             58
#define SI5351_MS3_BASE             66
#define SI5351_MS4_BASE             74
#define SI5351_MS5_BASE             82
#define SI5351_MS6                  90
#define SI5351_MS7                  91

#define SI5351_CLK6_7_OUTDIV        92

#define SI5351_SS_CFG_BASE          149

#define SI5351_VCXO_CFG_BASE        162

#define SI5351_CLK0_PHASEOFFS       165
#define SI5351_CLK1_PHASEOFFS       166
#define SI5351_CLK2_PHASEOFFS       167
#define SI5351_CLK3_PHASEOFFS       168
#define SI5351_CLK4_PHASEOFFS       169
#define SI5351_CLK5_PHASEOFFS       170

#define SI5351_PLL_RESET            177

#define SI5351_XTAL_CL              183

#define SI5351_FANOUT_CFG           187


#endif /* SI5351_REGS_H_ */
