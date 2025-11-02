//
// Copyright (C) 2015-2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef AV_CONTROLLER_H_
#define AV_CONTROLLER_H_

#include "sysconfig.h"
#include "controls.h"
#include "hdmi.h"
#include "osd_generator_regs.h"

// sys_ctrl
#define SCTRL_POWER_ON          (1<<0)
#define SCTRL_ISL_RESET_N       (1<<1)
#define SCTRL_HDMI_RESET_N      (1<<2)
#define SCTRL_EMIF_HWRESET_N    (1<<3)
#define SCTRL_EMIF_SWRESET_N    (1<<4)
#define SCTRL_EMIF_POWERDN_REQ  (1<<5)
#define SCTRL_EMIF_MPFE_RESET_N (1<<6)
#define SCTRL_CAPTURE_SEL_OFFS  7
#define SCTRL_CAPTURE_SEL_MASK  (0x3<<SCTRL_CAPTURE_SEL_OFFS)
#define SCTRL_ISL_HS_POL        (1<<9)
#define SCTRL_ISL_VS_POL        (1<<10)
#define SCTRL_ISL_VS_TYPE       (1<<11)
#define SCTRL_VGTP_ENABLE       (1<<12)
#define SCTRL_CSC_ENABLE        (1<<13)
#define SCTRL_FRAMELOCK         (1<<14)
#define SCTRL_HDMIRX_AUD_SEL    (1<<15)
#define SCTRL_FAN_PWM_OFFS      16
#define SCTRL_FAN_PWM_MASK      (0xf<<SCTRL_FAN_PWM_OFFS)
#define SCTRL_LED_PWM_OFFS      20
#define SCTRL_LED_PWM_MASK      (0xf<<SCTRL_LED_PWM_OFFS)
#define SCTRL_DRAM_RFR_ENA      (1<<24)
#define SCTRL_VIP_DIL_RESET_N   (1<<25)
#define SCTRL_EXTRA_AV_O_OFFS   26
#define SCTRL_EXTRA_AV_O_MASK   (0x3<<SCTRL_EXTRA_AV_O_OFFS)
#define SCTRL_EXP_SEL_OFFS      28
#define SCTRL_EXP_SEL_MASK      (0x3<<SCTRL_EXP_SEL_OFFS)
#define SCTRL_AUDMUX_SEL        (1<<30)
#define SCTRL_RF_AUD_SEL        (1<<31)

// sys_status
#define SSTAT_EMIF_STAT_MASK            0x00000007
#define SSTAT_EMIF_STAT_OFFS            0
#define SSTAT_EMIF_STAT_INIT_DONE_BIT   0
#define SSTAT_EMIF_POWERDN_ACK_BIT      3
#define SSTAT_EMIF_PLL_LOCKED           4
#define SSTAT_SD_DETECT_BIT             5
#define SSTAT_CVO_RESYNC_BIT            29

#define SCTRL_CAPTURE_SEL_ISL       0
#define SCTRL_CAPTURE_SEL_HDMIRX    1
#define SCTRL_CAPTURE_SEL_SDP       2

// SCL defs
#define PP_TAPS 4
#define PP_PHASES 64

typedef enum {
    AV_TESTPAT      = 0,
    AV1_RGBS,
    AV1_RGsB,
    AV1_YPbPr,
    AV1_RGBHV,
    AV1_RGBCS,
    AV2_YPbPr,
    AV2_RGsB,
    AV3_RGBHV,
    AV3_RGBCS,
    AV3_RGBS,
    AV3_RGsB,
    AV3_YPbPr,
    AV4,
    AV_EXP_SVIDEO,
    AV_EXP_CVBS,
    AV_EXP_RF,
} avinput_t;

#ifdef DExx_FW
typedef enum {
    AUD_AV1_ANALOG  = 0,
    AUD_SPDIF       = 1,
} audinput_t;
#else
typedef enum {
    AUD_AV1_ANALOG  = 0,
    AUD_AV2_ANALOG  = 1,
    AUD_AV3_ANALOG  = 2,
    AUD_SPDIF       = 3,
    AUD_AV4_DIGITAL = 4,
} audinput_t;
#endif

typedef struct {
    avinput_t default_avinput;
    uint8_t osd_enable;
    uint8_t osd_status_timeout;
    uint8_t osd_highlight_color;
#ifndef DExx_FW
    uint8_t power_up_state;
    uint8_t fan_pwm;
    uint8_t led_pwm;
#endif
} settings_t;

typedef struct {
    char name[13];
    uint8_t bits;
    int16_t v[PP_PHASES][PP_TAPS];
} pp_coeff;

typedef struct {
    char name[19];
    uint8_t ea;
    pp_coeff coeffs[4];
} c_pp_coeffs_t;

typedef struct {
    uint8_t iv_x;
    uint8_t iv_y;
    uint16_t v[16][16];
} shmask_data_arr;

typedef struct {
    char name[20];
    shmask_data_arr arr;
} c_shmask_t;

typedef struct {
    char name[20];
    edid_t edid;
} c_edid_t;

typedef struct {
    char name[8];
    uint32_t start_freq;
    uint32_t stop_freq;
} rf_band_t;

typedef union {
    struct {
        uint32_t c64_pal[16];
        uint32_t zx_pal[16];
        uint32_t msx_pal[16];
        uint32_t intv_pal[16];
        uint32_t nes_pal[64];
        uint32_t tia_pal[128];
        uint32_t gtia_pal[256];
    } __attribute__((packed, __may_alias__));
    uint32_t data[512];
} lc_palette_set;

typedef struct {
    char name[20];
    lc_palette_set pal;
} c_lc_palette_set_t;

typedef int (*load_func)(char*, char*);

void ui_disp_menu(uint8_t osd_mode);
void ui_disp_status(uint8_t refresh_osd_timer);

void restart_isl(uint8_t isl_ext_range);

void switch_input(rc_code_t rcode, btn_code_t bcode);

void set_syncmux_biasmode(uint8_t syncmux_stc);

void switch_audmux(uint8_t audmux_sel);

void switch_audsrc(audinput_t *audsrc_map, HDMI_audio_fmt_t *aud_tx_fmt);

void switch_expansion(uint8_t exp_sel, uint8_t extra_av_out_mode);

void set_dram_refresh(uint8_t enable);

int sys_is_powered_on();

void sys_toggle_power();

void print_vm_stats(int menu_mode);

int get_scl_pp_coeff_preset(pp_coeff *dst, char* name);

uint16_t get_sampler_phase();
int set_sampler_phase(uint8_t sampler_phase, uint8_t update_isl, uint8_t update_sc);
void set_default_settings();
void update_settings(int init_setup);
void update_sc_config();

void set_default_c_pp_coeffs();
void set_default_c_shmask();
void set_default_c_lc_palette_set();
void set_default_c_edid();
void set_custom_edid_reload();
void invalidate_loaded_arrays();

#endif
