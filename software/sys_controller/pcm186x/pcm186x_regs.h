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

#ifndef PCM186X_REGS_H_
#define PCM186X_REGS_H_

#define PCM186X_PAGESEL         0x00
#define PCM186X_PGA1L           0x01
#define PCM186X_PGA1R           0x02
#define PCM186X_PGA2L           0x03
#define PCM186X_PGA2R           0x04
#define PCM186X_PGACONFIG       0x05
#define PCM186X_ADC1L           0x06
#define PCM186X_ADC1R           0x07
#define PCM186X_ADC2L           0x08
#define PCM186X_ADC2R           0x09
#define PCM186X_ADC2CONFIG      0x0A
#define PCM186X_PCMFMT          0x0B
#define PCM186X_TDMFMT          0x0C
#define PCM186X_TXTDM_OFFSET    0x0D
#define PCM186X_RXTDM_OFFSET    0x0E
#define PCM186X_DPGA1_GAIN_L    0x0F

#define PCM186X_DPGA1_GAIN_R    0x16
#define PCM186X_DPGA2_GAIN_L    0x17
#define PCM186X_DPGA2_GAIN_R    0x18
#define PCM186X_DPGA_CONFIG     0x19
#define PCM186X_DMIC_CONFIG     0x1A
#define PCM186X_DIN_RESAMPLE    0x1B

#define PCM186X_CLKCONFIG       0x20
#define PCM186X_DSP1_CLKDIV     0x21
#define PCM186X_DSP2_CLKDIV     0x22
#define PCM186X_ADC_CLKDIV      0x23

#define PCM186X_PLL_SCK_DIV     0x25
#define PCM186X_SCK_BCK_DIV     0x26
#define PCM186X_BCK_LRCK_DIV    0x27
#define PCM186X_PLLCONFIG       0x28
#define PCM186X_PLL_P           0x29
#define PCM186X_PLL_R           0x2A
#define PCM186X_PLL_J           0x2B
#define PCM186X_PLL_D_LSB       0x2C
#define PCM186X_PLL_D_MSB       0x2D

#define PCM186X_DSP_CTRL        0x71

#endif /* PCM186X_REGS_H_ */
