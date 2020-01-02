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

#ifndef ISL51002_REGS_H_
#define ISL51002_REGS_H_

// Status and interrupt registers
#define ISL_SYNCTYPE            0x01
#define ISL_CH0_CH1_STATUS      0x02
#define ISL_CH2_STATUS          0x03
#define ISL_IRQSTATUS           0x04
#define ISL_IRQMASK             0x05

// Configuration registers
#define ISL_INPUTCFG            0x10
#define ISL_SYNCSRC             0x11
#define ISL_R_GAIN_MSB          0x12
#define ISL_R_GAIN_LSB          0x13
#define ISL_G_GAIN_MSB          0x14
#define ISL_G_GAIN_LSB          0x15
#define ISL_B_GAIN_MSB          0x16
#define ISL_B_GAIN_LSB          0x17
#define ISL_R_OFFSET_MSB        0x18
#define ISL_R_OFFSET_LSB        0x19
#define ISL_G_OFFSET_MSB        0x1A
#define ISL_G_OFFSET_LSB        0x1B
#define ISL_B_OFFSET_MSB        0x1C
#define ISL_B_OFFSET_LSB        0x1D
#define ISL_HPLL_HTOTAL_MSB     0x1E
#define ISL_HPLL_HTOTAL_LSB     0x1F
#define ISL_HPLL_PHASE          0x20
#define ISL_HPLL_PRECOAST       0x21
#define ISL_HPLL_POSTCOAST      0x22
#define ISL_HPLL_MISCCFG        0x23
#define ISL_ABLC_START_MSB      0x24
#define ISL_ABLC_START_LSB      0x25
#define ISL_CLAMPWIDTH          0x26
#define ISL_ABLCCFG             0x27
#define ISL_OUTFORMAT1          0x28
#define ISL_OUTFORMAT2          0x29
#define ISL_HSOUTWIDTH          0x2A
#define ISL_OUTPUTENA           0x2B
#define ISL_POWERCTRL           0x2C
#define ISL_XTALFREQ            0x2D
#define ISL_AFEBW               0x2E
#define ISL_HSYNC_VTH           0x2F
#define ISL_SOG_VTH             0x30
#define ISL_SYNCCFG             0x31
#define ISL_SYNCPOLLCFG         0x32

// Measurement registers
#define ISL_HSYNCPERIOD_MSB     0x40
#define ISL_HSYNCPERIOD_LSB     0x41
#define ISL_HSYNCWIDTH_MSB      0x42
#define ISL_HSYNCWIDTH_LSB      0x43
#define ISL_VSYNCPERIOD_MSB     0x44
#define ISL_VSYNCPERIOD_LSB     0x45
#define ISL_VSYNCWIDTH          0x46
#define ISL_DESTART_MSB         0x47
#define ISL_DESTART_LSB         0x48
#define ISL_DEWIDTH_MSB         0x49
#define ISL_DEWIDTH_LSB         0x4A
#define ISL_LINESTART_MSB       0x4B
#define ISL_LINESTART_LSB       0x4C
#define ISL_LINEWIDTH_MSB       0x4D
#define ISL_LINEWIDTH_LSB       0x4E
#define ISL_MEASCFG             0x4F

// Auto-adjust registers
#define ISL_PHASEADJCMD         0x50
#define ISL_PHASEADJSTATUS      0x51
#define ISL_PHASEADJVMASK       0x52
#define ISL_PHASEADJHMASK1      0x53
#define ISL_PHASEADJHMASK2      0x54
#define ISL_PHASEADJCMDOPT      0x55
#define ISL_PHASEADJTHOLD       0x56
#define ISL_PHASEADJDATA3       0x57
#define ISL_PHASEADJDATA2       0x58
#define ISL_PHASEADJDATA1       0x59
#define ISL_PHASEADJDATA0       0x5A
#define ISL_AFECTRL             0x60
#define ISL_ADCCTRL             0x61

#endif /* ISL51002_REGS_H_ */
