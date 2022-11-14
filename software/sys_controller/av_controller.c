//
// Copyright (C) 2019-2021  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include <unistd.h>
#include "system.h"
#include "string.h"
#include "altera_avalon_pio_regs.h"
#include "sysconfig.h"
#include "utils.h"
#include "sys/alt_timestamp.h"
#include "i2c_opencores.h"
#include "av_controller.h"
#include "avconfig.h"
#include "isl51002.h"
#include "ths7353.h"
#include "pcm186x.h"
#include "us2066.h"
#include "controls.h"
#include "menu.h"
#include "mmc.h"
#include "file.h"
#include "si5351.h"
#include "adv7513.h"
#include "adv761x.h"
#include "sc_config_regs.h"
#include "video_modes.h"
#include "flash.h"
#include "firmware.h"
#include "userdata.h"

#define FW_VER_MAJOR 0
#define FW_VER_MINOR 62

//fix PD and cec
#define ADV7513_MAIN_BASE 0x72
#define ADV7513_EDID_BASE 0x7e
#define ADV7513_PKTMEM_BASE 0x70
#define ADV7513_CEC_BASE 0x78

#define SII1136_BASE (0x76>>1)

#define ISL51002_BASE (0x9A>>1)
#define THS7353_BASE (0x58>>1)
#define SI5351_BASE (0xC0>>1)
#define PCM1863_BASE (0x94>>1)
#define US2066_BASE (0x7A>>1)

#define ADV7610_IO_BASE 0x98
#define ADV7610_CEC_BASE 0x80
#define ADV7610_INFOFRAME_BASE 0x8c
#define ADV7610_DPLL_BASE 0x4c
#define ADV7610_KSV_BASE 0x64
#define ADV7610_EDID_BASE 0x6c
#define ADV7610_HDMI_BASE 0x68
#define ADV7610_CP_BASE 0x44

#define BLKSIZE 512
#define BLKCNT 2

unsigned char pro_edid_bin[] = {
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x36, 0x51, 0x5c, 0x05,
  0x15, 0xcd, 0x5b, 0x07, 0x0a, 0x1d, 0x01, 0x04, 0xa2, 0x3c, 0x22, 0x78,
  0xff, 0xde, 0x51, 0xa3, 0x54, 0x4c, 0x99, 0x26, 0x0f, 0x50, 0x54, 0xff,
  0xff, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38,
  0x2d, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x55, 0x50, 0x21, 0x00, 0x00, 0x1a,
  0x00, 0x00, 0x00, 0xff, 0x00, 0x32, 0x30, 0x35, 0x34, 0x38, 0x31, 0x32,
  0x35, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x2d,
  0x90, 0x0a, 0x96, 0x14, 0x01, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x00, 0x00, 0x00, 0xfc, 0x00, 0x4f, 0x53, 0x53, 0x43, 0x20, 0x50, 0x72,
  0x6f, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x01, 0x46, 0x02, 0x03, 0x40, 0xf2,
  0x50, 0x9f, 0x90, 0x14, 0x05, 0x20, 0x13, 0x04, 0x12, 0x03, 0x11, 0x02,
  0x16, 0x07, 0x15, 0x06, 0x01, 0x29, 0x0f, 0x7f, 0x07, 0x17, 0x7f, 0xff,
  0x3f, 0x7f, 0xff, 0x83, 0x4f, 0x00, 0x00, 0x78, 0x03, 0x0c, 0x00, 0x10,
  0x00, 0xb8, 0x26, 0x2f, 0x01, 0x01, 0x01, 0x01, 0xff, 0xfc, 0x06, 0x16,
  0x08, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe3, 0x05, 0x1f, 0x01,
  0x01, 0x1d, 0x80, 0xd0, 0x72, 0x1c, 0x16, 0x20, 0x10, 0x2c, 0x25, 0x80,
  0xba, 0x88, 0x21, 0x00, 0x00, 0x9e, 0x01, 0x1d, 0x80, 0x18, 0x71, 0x1c,
  0x16, 0x20, 0x58, 0x2c, 0x25, 0x00, 0xba, 0x88, 0x21, 0x00, 0x00, 0x9e,
  0x01, 0x1d, 0x00, 0xbc, 0x52, 0xd0, 0x1e, 0x20, 0xb8, 0x28, 0x55, 0x40,
  0xba, 0x88, 0x21, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0,
  0x1e, 0x20, 0x6e, 0x0c
};

// Default settings
const settings_t ts_default = {
    .default_avinput = 0,
    .osd_enable = 1,
    .osd_status_timeout = 1,
    .fan_pwm = 0,
    .led_pwm = 5,
};

isl51002_dev isl_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                        .i2c_addr = ISL51002_BASE,
                        .xclk_out_en = 0,
                        .xtal_freq = 27000000LU};

ths7353_dev ths_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                        .i2c_addr = THS7353_BASE};

si5351_dev si_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                     .i2c_addr = SI5351_BASE,
                     .xtal_freq = 27000000LU};

#define SI_PCLK_PIN SI_CLK0

adv761x_dev advrx_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                         .io_base = ADV7610_IO_BASE,
                         .cec_base = ADV7610_CEC_BASE,
                         .infoframe_base = ADV7610_INFOFRAME_BASE,
                         .dpll_base = ADV7610_DPLL_BASE,
                         .ksv_base = ADV7610_KSV_BASE,
                         .edid_base = ADV7610_EDID_BASE,
                         .hdmi_base = ADV7610_HDMI_BASE,
                         .cp_base = ADV7610_CP_BASE,
                         .xtal_freq = 27000000LU,
                         .edid = pro_edid_bin,
                         .edid_len = sizeof(pro_edid_bin)};

#ifdef INC_ADV7513
adv7513_dev advtx_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                         .main_base = ADV7513_MAIN_BASE,
                         .edid_base = ADV7513_EDID_BASE,
                         .pktmem_base = ADV7513_PKTMEM_BASE,
                         .cec_base = ADV7513_CEC_BASE};
#endif

#ifdef INC_SII1136
sii1136_dev siitx_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                         .i2c_addr = SII1136_BASE};
#endif

pcm186x_dev pcm_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                       .i2c_addr = PCM1863_BASE};

us2066_dev chardisp_dev = {.i2cm_base = I2C_OPENCORES_0_BASE,
                           .i2c_addr = US2066_BASE};

flash_ctrl_dev flashctrl_dev = {.regs = (volatile gen_flash_if_regs*)INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,
                                .flash_size = 0x1000000};

rem_update_dev rem_reconfig_dev = {.regs = (volatile rem_update_regs*)0x00023400};

volatile sc_regs *sc = (volatile sc_regs*)SC_CONFIG_0_BASE;
volatile osd_regs *osd = (volatile osd_regs*)OSD_GENERATOR_0_BASE;

struct mmc *mmc_dev;
struct mmc * ocsdc_mmc_init(int base_addr, int clk_freq, unsigned int host_caps);

uint32_t sys_ctrl;
uint32_t sys_status;
uint8_t sys_powered_on;

uint8_t sd_det, sd_det_prev;
uint8_t sl_def_iv_x, sl_def_iv_y;

int enable_isl, enable_hdmirx, enable_tp;
oper_mode_t oper_mode;

avinput_t avinput, target_avinput;

mode_data_t vmode_in, vmode_out;
vm_proc_config_t vm_conf;

settings_t cs, ts;

char row1[US2066_ROW_LEN+1], row2[US2066_ROW_LEN+1];
extern char menu_row1[US2066_ROW_LEN+1], menu_row2[US2066_ROW_LEN+1];

extern const char *avinput_str[];

#ifdef VIP
#include "src/scl_pp_coeffs.c"

FIL file;
char char_buff[256];

const pp_coeff* scl_pp_coeff_list[][2][2] = {{{&pp_coeff_nearest, NULL}, {&pp_coeff_nearest, NULL}},
                                            {{&pp_coeff_lanczos3, NULL}, {&pp_coeff_lanczos3, NULL}},
                                            {{&pp_coeff_lanczos3_13, NULL}, {&pp_coeff_lanczos3_13, NULL}},
                                            {{&pp_coeff_lanczos3, &pp_coeff_lanczos3_13}, {&pp_coeff_lanczos3, &pp_coeff_lanczos3_13}},
                                            {{&pp_coeff_lanczos4, NULL}, {&pp_coeff_lanczos4, NULL}},
                                            {{&pp_coeff_nearest, NULL}, {&pp_coeff_sl_sharp, NULL}}};
int scl_loaded_pp_coeff = -1;
#define PP_COEFF_SIZE  (sizeof(scl_pp_coeff_list) / sizeof((scl_pp_coeff_list)[0]))
#define PP_TAPS 4
#define PP_PHASES 64
#define SCL_ALG_COEFF_START 3

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t irq;
    uint32_t words;
    uint32_t h_active;
    uint32_t v_active_f0;
    uint32_t v_active_f1;
    uint32_t h_total;
    uint32_t v_total_f0;
    uint32_t v_total_f1;
} vip_cvi_ii_regs;

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t irq;
    uint32_t vmode_match;
    uint32_t banksel;
    uint32_t mode_ctrl;
    uint32_t h_active;
    uint32_t v_active;
    uint32_t v_active_f1;
    uint32_t h_frontporch;
    uint32_t h_synclen;
    uint32_t h_blank;
    uint32_t v_frontporch;
    uint32_t v_synclen;
    uint32_t v_blank;
    uint32_t v_frontporch_f0;
    uint32_t v_synclen_f0;
    uint32_t v_blank_f0;
    uint32_t active_start;
    uint32_t v_blank_start;
    uint32_t fid_r;
    uint32_t fid_f;
    uint32_t vid_std;
    uint32_t sof_sample;
    uint32_t sof_line;
    uint32_t vco_div;
    uint32_t anc_line;
    uint32_t anc_line_f0;
    uint32_t h_polarity;
    uint32_t v_polarity;
    uint32_t valid;
} vip_cvo_ii_regs;

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t irq;
    uint32_t width;
    uint32_t height;
    uint32_t edge_thold;
    uint32_t reserved[2];
    uint32_t h_coeff_wbank;
    uint32_t h_coeff_rbank;
    uint32_t v_coeff_wbank;
    uint32_t v_coeff_rbank;
    uint32_t h_phase;
    uint32_t v_phase;
    int32_t coeff_data[PP_TAPS];
} vip_scl_ii_regs;

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t rsv;
    uint32_t unused[11];
    uint32_t motion_shift;
    uint32_t unused2;
    uint32_t mode;
} vip_dil_ii_regs;

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t irq;
    uint32_t config;
} vip_il_ii_regs;

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t irq;
    uint32_t frame_cnt;
    uint32_t drop_rpt_cnt;
    uint32_t unused[3];
    uint32_t misc;
    uint32_t locked;
    uint32_t input_rate;
    uint32_t output_rate;
} vip_vfb_ii_regs;

volatile vip_cvi_ii_regs *vip_cvi = (volatile vip_cvi_ii_regs*)ALT_VIP_CL_CVI_0_BASE;
volatile vip_dil_ii_regs *vip_dil = (volatile vip_dil_ii_regs*)ALT_VIP_CL_DIL_0_BASE;
volatile vip_vfb_ii_regs *vip_fb = (volatile vip_vfb_ii_regs*)ALT_VIP_CL_VFB_0_BASE;
volatile vip_scl_ii_regs *vip_scl_pp = (volatile vip_scl_ii_regs*)ALT_VIP_CL_SCL_0_BASE;
volatile vip_il_ii_regs *vip_il = (volatile vip_il_ii_regs*)ALT_VIP_CL_IL_0_BASE;
volatile vip_cvo_ii_regs *vip_cvo = (volatile vip_cvo_ii_regs*)ALT_VIP_CL_CVO_0_BASE;
#endif


void ui_disp_menu(uint8_t osd_mode)
{
    uint8_t menu_page;

    if ((osd_mode == 1) || (ts.osd_enable == 2)) {
        strncpy((char*)osd->osd_array.data[0][0], menu_row1, OSD_CHAR_COLS);
        strncpy((char*)osd->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
        osd->osd_row_color.mask = 0;
        osd->osd_sec_enable[0].mask = 3;
        osd->osd_sec_enable[1].mask = 0;
    } else if (osd_mode == 2) {
        menu_page = get_current_menunavi()->mp;
        strncpy((char*)osd->osd_array.data[menu_page][1], menu_row2, OSD_CHAR_COLS);
        osd->osd_sec_enable[1].mask |= (1<<menu_page);
    }

    us2066_write(&chardisp_dev, (char*)&menu_row1, (char*)&menu_row2);
}

void ui_disp_status(uint8_t refresh_osd_timer) {
    if (!is_menu_active()) {
        if (refresh_osd_timer)
            osd->osd_config.status_refresh = 1;

        strncpy((char*)osd->osd_array.data[0][0], row1, OSD_CHAR_COLS);
        strncpy((char*)osd->osd_array.data[1][0], row2, OSD_CHAR_COLS);
        osd->osd_row_color.mask = 0;
        osd->osd_sec_enable[0].mask = 3;
        osd->osd_sec_enable[1].mask = 0;

        us2066_write(&chardisp_dev, (char*)&row1, (char*)&row2);
    }
}

void update_sc_config(mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf, avconfig_t *avconfig)
{
    int vip_enable, scl_target_pp_coeff, scl_ea, i, p, t, n;
    int v0,v1,v2,v3;
    char coeff_filename[16];

    hv_config_reg hv_in_config = {.data=0x00000000};
    hv_config2_reg hv_in_config2 = {.data=0x00000000};
    hv_config3_reg hv_in_config3 = {.data=0x00000000};
    hv_config_reg hv_out_config = {.data=0x00000000};
    hv_config2_reg hv_out_config2 = {.data=0x00000000};
    hv_config3_reg hv_out_config3 = {.data=0x00000000};
    xy_config_reg xy_out_config = {.data=0x00000000};
    xy_config2_reg xy_out_config2 = {.data=0x00000000};
    misc_config_reg misc_config = {.data=0x00000000};
    sl_config_reg sl_config = {.data=0x00000000};
    sl_config2_reg sl_config2 = {.data=0x00000000};
    sl_config3_reg sl_config3 = {.data=0x00000000};

    vip_enable = !enable_tp && (avconfig->oper_mode == 1);
    uint32_t h_blank, v_blank, h_frontporch, v_frontporch;

    // Set input params
    hv_in_config.h_total = vm_in->timings.h_total;
    hv_in_config.h_active = vm_in->timings.h_active;
    hv_in_config.h_synclen = vm_in->timings.h_synclen;
    hv_in_config2.h_backporch = vm_in->timings.h_backporch;
    hv_in_config2.v_active = vm_in->timings.v_active;
    hv_in_config3.v_synclen = vm_in->timings.v_synclen;
    hv_in_config3.v_backporch = vm_in->timings.v_backporch;
    hv_in_config2.interlaced = vm_in->timings.interlaced;
    hv_in_config3.v_startline = vm_in->timings.v_synclen+vm_in->timings.v_backporch+12;
    hv_in_config3.h_skip = vm_conf->h_skip;
    hv_in_config3.h_sample_sel = vm_conf->h_sample_sel;

    // Set output params
    hv_out_config.h_total = vm_out->timings.h_total;
    hv_out_config.h_active = vm_out->timings.h_active;
    hv_out_config.h_synclen = vm_out->timings.h_synclen;
    hv_out_config2.h_backporch = vm_out->timings.h_backporch;
    hv_out_config2.v_total = vm_out->timings.v_total;
    hv_out_config2.v_active = vm_out->timings.v_active;
    hv_out_config3.v_synclen = vm_out->timings.v_synclen;
    hv_out_config3.v_backporch = vm_out->timings.v_backporch;
    hv_out_config2.interlaced = vm_out->timings.interlaced;
    hv_out_config3.v_startline = vm_conf->framesync_line;

    xy_out_config.x_size = vm_conf->x_size;
    xy_out_config.y_size = vm_conf->y_size;
    xy_out_config.y_offset = vm_conf->y_offset;
    xy_out_config2.x_offset = vm_conf->x_offset;
    xy_out_config2.x_start_lb = vm_conf->x_start_lb;
    xy_out_config2.y_start_lb = vm_conf->y_start_lb;
    xy_out_config2.x_rpt = vm_conf->x_rpt;
    xy_out_config2.y_rpt = vm_conf->y_rpt;

    misc_config.mask_br = avconfig->mask_br;
    misc_config.mask_color = avconfig->mask_color;
    misc_config.reverse_lpf = avconfig->reverse_lpf;
    misc_config.lm_deint_mode = avconfig->lm_deint_mode;
    misc_config.nir_even_offset = avconfig->nir_even_offset;
    misc_config.ypbpr_cs = (avconfig->ypbpr_cs == 0) ? ((vm_in->type & VIDEO_HDTV) ? 1 : 0) : avconfig->ypbpr_cs-1;
    misc_config.vip_enable = vip_enable;
    misc_config.bfi_enable = avconfig->bfi_enable & ((uint32_t)vm_out->timings.v_hz_x100*5 >= (uint32_t)vm_in->timings.v_hz_x100*9);
    misc_config.bfi_str = avconfig->bfi_str;

    // set default/custom scanline interval
    sl_def_iv_y = (vm_conf->y_rpt > 0) ? vm_conf->y_rpt : 1;
    sl_def_iv_x = (vm_conf->x_rpt > 0) ? vm_conf->x_rpt : sl_def_iv_y;
    sl_config3.sl_iv_x = ((avconfig->sl_type == 3) && (avconfig->sl_cust_iv_x)) ? avconfig->sl_cust_iv_x : sl_def_iv_x;
    sl_config3.sl_iv_y = ((avconfig->sl_type == 3) && (avconfig->sl_cust_iv_y)) ? avconfig->sl_cust_iv_y : sl_def_iv_y;

    // construct custom/default scanline overlay
    for (i=0; i<6; i++) {
        if (avconfig->sl_type == 3) {
            sl_config.sl_l_str_arr |= ((avconfig->sl_cust_l_str[i]-1)&0xf)<<(4*i);
            sl_config.sl_l_overlay |= (avconfig->sl_cust_l_str[i]!=0)<<i;
        } else {
            sl_config.sl_l_str_arr |= avconfig->sl_str<<(4*i);

            if ((i==5) && ((avconfig->sl_type == 0) || (avconfig->sl_type == 2))) {
                sl_config.sl_l_overlay = (1<<((sl_config3.sl_iv_y+1)/2))-1;
                if (avconfig->sl_id)
                    sl_config.sl_l_overlay <<= (sl_config3.sl_iv_y+2)/2;
            }
        }
    }
    for (i=0; i<10; i++) {
        if (avconfig->sl_type == 3) {
            if (i<8)
                sl_config2.sl_c_str_arr_l |= ((avconfig->sl_cust_c_str[i]-1)&0xf)<<(4*i);
            else
                sl_config3.sl_c_str_arr_h |= ((avconfig->sl_cust_c_str[i]-1)&0xf)<<(4*(i-8));
            sl_config3.sl_c_overlay |= (avconfig->sl_cust_c_str[i]!=0)<<i;
        } else {
            if (i<8)
                sl_config2.sl_c_str_arr_l |= avconfig->sl_str<<(4*i);
            else
                sl_config3.sl_c_str_arr_h |= avconfig->sl_str<<(4*(i-8));

            if ((i==9) && ((avconfig->sl_type == 1) || (avconfig->sl_type == 2)))
                sl_config3.sl_c_overlay = (1<<((sl_config3.sl_iv_x+1)/2))-1;
        }
    }
    sl_config.sl_method = avconfig->sl_method;
    sl_config.sl_altern = avconfig->sl_altern;

    // disable scanlines if configured so
    if (((avconfig->sl_mode == 1) && (!vm_conf->y_rpt)) || (avconfig->sl_mode == 0)) {
        sl_config.sl_l_overlay = 0;
        sl_config3.sl_c_overlay = 0;
    }

    sc->hv_in_config = hv_in_config;
    sc->hv_in_config2 = hv_in_config2;
    sc->hv_in_config3 = hv_in_config3;
    sc->hv_out_config = hv_out_config;
    sc->hv_out_config2 = hv_out_config2;
    sc->hv_out_config3 = hv_out_config3;
    sc->xy_out_config = xy_out_config;
    sc->xy_out_config2 = xy_out_config2;
    sc->misc_config = misc_config;
    sc->sl_config = sl_config;
    sc->sl_config2 = sl_config2;
    sc->sl_config3 = sl_config3;

#ifdef VIP
    vip_cvi->ctrl = vip_enable;
    vip_dil->ctrl = vip_enable;
    vip_il->ctrl = vip_enable;

    if (avconfig->scl_alg == 0)
        scl_target_pp_coeff = ((vm_in->group >= GROUP_240P) && (vm_in->group <= GROUP_384P)) ? 0 : 2; // Nearest or Lanchos3_sharp
    else if (avconfig->scl_alg < SCL_ALG_COEFF_START)
        scl_target_pp_coeff = 0; // Nearest for integer scale
    else
        scl_target_pp_coeff = avconfig->scl_alg-SCL_ALG_COEFF_START;
    scl_ea = (scl_target_pp_coeff >= PP_COEFF_SIZE) ? 0 : !!scl_pp_coeff_list[scl_target_pp_coeff][0][1];

    vip_scl_pp->ctrl = vip_enable ? (scl_ea<<1)|1 : 0;
    vip_fb->ctrl = vip_enable;
    vip_cvo->ctrl = vip_enable ? (1 | (1<<3)) : 0;

    if (!vip_enable) {
        scl_loaded_pp_coeff = -1;
        return;
    }

    if (avconfig->scl_dil_alg == 0) {
        vip_dil->mode = (1<<1);
    } else if (avconfig->scl_dil_alg == 1) {
        vip_dil->mode = (1<<2);
    } else if (avconfig->scl_dil_alg == 3) {
        vip_dil->mode = (1<<0);
    } else {
        vip_dil->mode = 0;
    }

    vip_dil->motion_shift = avconfig->scl_dil_motion_shift;
    
    vip_il->config = !vm_out->timings.interlaced;

    if (scl_target_pp_coeff != scl_loaded_pp_coeff) {
        if (scl_target_pp_coeff >= PP_COEFF_SIZE) { // Custom
            snprintf(coeff_filename, sizeof(coeff_filename), "scaler%d.txt", (scl_target_pp_coeff + 1 - PP_COEFF_SIZE) );
            if (!file_open(&file, coeff_filename)) {
                p = 0;
                while (file_get_string(&file, char_buff, sizeof(char_buff))) {
                    n = sscanf(char_buff, "%d,%d,%d,%d", &v0, &v1, &v2, &v3);
                    if (n == PP_TAPS) {
                        vip_scl_pp->coeff_data[0] = v0;
                        vip_scl_pp->coeff_data[1] = v1;
                        vip_scl_pp->coeff_data[2] = v2;
                        vip_scl_pp->coeff_data[3] = v3;

                        vip_scl_pp->h_phase = p;
                        vip_scl_pp->v_phase = p;

                        if (++p == PP_PHASES)
                            break;
                    }
                }
                file_close(&file);
            }
        } else {
            for (p=0; p<PP_PHASES; p++) {
                for (t=0; t<PP_TAPS; t++)
                    vip_scl_pp->coeff_data[t] = scl_pp_coeff_list[scl_target_pp_coeff][0][0]->v[p][t];

                vip_scl_pp->h_phase = p;

                for (t=0; t<PP_TAPS; t++)
                    vip_scl_pp->coeff_data[t] = scl_pp_coeff_list[scl_target_pp_coeff][1][0]->v[p][t];

                vip_scl_pp->v_phase = p;

                if (scl_ea) {
                    for (t=0; t<PP_TAPS; t++)
                        vip_scl_pp->coeff_data[t] = scl_pp_coeff_list[scl_target_pp_coeff][0][1]->v[p][t];

                    vip_scl_pp->h_phase = p+(1<<15);

                    for (t=0; t<PP_TAPS; t++)
                        vip_scl_pp->coeff_data[t] = scl_pp_coeff_list[scl_target_pp_coeff][1][1]->v[p][t];

                    vip_scl_pp->v_phase = p+(1<<15);
                }
            }
        }

        scl_loaded_pp_coeff = scl_target_pp_coeff;
    }

    vip_scl_pp->edge_thold = avconfig->scl_edge_thold;

    vip_scl_pp->width = vm_conf->x_size;
    vip_scl_pp->height = vm_conf->y_size<<vm_out->timings.interlaced;

    vip_fb->input_rate = vm_in->timings.v_hz_x100;
    vip_fb->output_rate = vm_out->timings.v_hz_x100;
    //vip_fb->locked = vm_conf->framelock;  // causes cvo fifo underflows
    vip_fb->locked = 0;

    h_blank = vm_out->timings.h_total-vm_conf->x_size;
    v_blank = (vm_out->timings.v_total>>vm_out->timings.interlaced)-vm_conf->y_size;
    h_frontporch = h_blank-vm_conf->x_offset-vm_out->timings.h_backporch-vm_out->timings.h_synclen;
    v_frontporch = v_blank-vm_conf->y_offset-vm_out->timings.v_backporch-vm_out->timings.v_synclen;

    if ((vip_cvo->h_active != vm_conf->x_size) ||
        (vip_cvo->v_active != vm_conf->y_size) ||
        (vip_cvo->h_synclen != vm_out->timings.h_synclen) ||
        (vip_cvo->v_synclen != vm_out->timings.v_synclen) ||
        (vip_cvo->h_frontporch != h_frontporch) ||
        (vip_cvo->v_frontporch != v_frontporch) ||
        (vip_cvo->h_blank != h_blank) ||
        (vip_cvo->v_blank != v_blank) ||
        (vip_cvo->mode_ctrl != vm_out->timings.interlaced))
    {
        vip_cvo->banksel = 0;
        vip_cvo->valid = 0;
        vip_cvo->mode_ctrl = vm_out->timings.interlaced;
        vip_cvo->h_active = vm_conf->x_size;
        vip_cvo->v_active = vm_conf->y_size;
        vip_cvo->v_active_f1 = vm_conf->y_size;
        vip_cvo->h_synclen = vm_out->timings.h_synclen;
        vip_cvo->v_synclen = vm_out->timings.v_synclen;
        vip_cvo->v_synclen_f0 = vm_out->timings.v_synclen;
        vip_cvo->h_frontporch = h_frontporch;
        vip_cvo->v_frontporch = v_frontporch;
        vip_cvo->v_frontporch_f0 = v_frontporch+1;
        vip_cvo->h_blank = h_blank;
        vip_cvo->v_blank = v_blank;
        vip_cvo->v_blank_f0 = v_blank+1;
        vip_cvo->active_start = 0;
        vip_cvo->v_blank_start = vm_conf->y_size;
        vip_cvo->fid_r = vm_conf->y_size + v_frontporch;
        vip_cvo->fid_f = 2*vm_conf->y_size + vip_cvo->v_blank_f0 + vip_cvo->v_frontporch_f0;
        vip_cvo->h_polarity = 0;
        vip_cvo->v_polarity = 0;
        vip_cvo->valid = 1;
    }

    if (vm_conf->framelock)
        vip_cvo->ctrl |= (1<<4);
#endif
}

int init_emif()
{
    alt_timestamp_type start_ts;

    sys_ctrl |= SCTRL_EMIF_HWRESET_N;
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
    start_ts = alt_timestamp();
    while (1) {
        sys_status = IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);
        if (sys_status & (1<<SSTAT_EMIF_PLL_LOCKED))
            break;
        else if (alt_timestamp() >= start_ts + 100000*(TIMER_0_FREQ/1000000))
            return -1;
    }

    sys_ctrl |= SCTRL_EMIF_SWRESET_N;
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
    start_ts = alt_timestamp();
    while (1) {
        sys_status = IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);
        if (sys_status & (1<<SSTAT_EMIF_STAT_INIT_DONE_BIT))
            break;
        else if (alt_timestamp() >= start_ts + 100000*(TIMER_0_FREQ/1000000))
            return -2;
    }
    if (((sys_status & SSTAT_EMIF_STAT_MASK) >> SSTAT_EMIF_STAT_OFFS) != 0x3) {
        printf("Mem calib fail: 0x%x\n", ((sys_status & SSTAT_EMIF_STAT_MASK) >> SSTAT_EMIF_STAT_OFFS));
        return -3;
    }

    // Place LPDDR2 into deep powerdown mode
    sys_ctrl |= (SCTRL_EMIF_POWERDN_REQ);
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
    start_ts = alt_timestamp();
    while (1) {
        sys_status = IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);
        if (sys_status & (1<<SSTAT_EMIF_POWERDN_ACK_BIT))
            break;
        else if (alt_timestamp() >= start_ts + 100000*(TIMER_0_FREQ/1000000))
            return -4;
    }

    return 0;
}

int init_sdcard() {
    int err = mmc_init(mmc_dev);

    if (err != 0 || mmc_dev->has_init == 0) {
        printf("mmc_init failed: %d\n\n", err);
        return -1;
    } else {
        printf("mmc_init success\n\n");

        printf("Device: %s\n", mmc_dev->name);
        printf("Manufacturer ID: %x\n", mmc_dev->cid[0] >> 24);
        printf("OEM: %x\n", (mmc_dev->cid[0] >> 8) & 0xffff);
        printf("Name: %c%c%c%c%c \n", mmc_dev->cid[0] & 0xff,
                (mmc_dev->cid[1] >> 24), (mmc_dev->cid[1] >> 16) & 0xff,
                (mmc_dev->cid[1] >> 8) & 0xff, mmc_dev->cid[1] & 0xff);

        printf("Tran Speed: %d\n", mmc_dev->tran_speed);
        printf("Rd Block Len: %d\n", mmc_dev->read_bl_len);

        printf("%s version %d.%d\n", IS_SD(mmc_dev) ? "SD" : "MMC",
                (mmc_dev->version >> 4) & 0xf, mmc_dev->version & 0xf);

        printf("High Capacity: %s\n", mmc_dev->high_capacity ? "Yes" : "No");
        printf("Capacity: %lu MB\n", mmc_dev->capacity>>20);

        printf("Bus Width: %d-bit\n\n", mmc_dev->bus_width);

        /*printf("attempting to read 1 block\n\r");
        if (mmc_bread(mmc_dev, 0, BLKCNT, buf) == 0) {
            printf("mmc_bread failed\n\r");
            return -1;
        }
        printf("mmc_bread success\n\r");
        for (i=0; i<8; i++)
            printf("0x%.2x\n", buf[i]);*/

        /*char buff[256];


        res = f_mount(&fs, "", 1);
        if (res == FR_OK) {
            strcpy(buff, "/");
            res = scan_files(buff);
        }*/

        file_mount();
    }

    return 0;
}

int check_sdcard() {
    int ret = 0;

    sys_status = IORD_ALTERA_AVALON_PIO_DATA(PIO_2_BASE);
    sd_det = !!(sys_status & (1<<SSTAT_SD_DETECT_BIT));

    //init sdcard if detected
    if (sd_det != sd_det_prev) {
        if (sd_det) {
            printf("SD card inserted\n");
            ret = init_sdcard();
        } else {
            printf("SD card ejected\n");
            mmc_dev->has_init = 0;
        }
    }

    sd_det_prev = sd_det;

    return ret;
}

int init_hw()
{
    int ret;

    // reset hw
    sys_ctrl = 0x00;
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
    usleep(400000);
    sys_ctrl |= SCTRL_EMIF_MPFE_RESET_N|SCTRL_ISL_RESET_N|SCTRL_HDMI_RESET_N;
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

    I2C_init(I2CA_BASE,ALT_CPU_FREQ, 400000);

    // Init character OLED
    us2066_init(&chardisp_dev);

    // Init ISL51002
    ret = isl_init(&isl_dev);
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "ISL51002 init fail");
        return ret;
    }

    // Init THS7353
    ret = ths7353_init(&ths_dev);
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "THS7353 init fail");
        return ret;
    }

    // Init LPDDR2 interface
    ret = init_emif();
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "EMIF init fail");
        return ret;
    }

    // Enable test pattern generation
    sys_ctrl |= SCTRL_VGTP_ENABLE;
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

    // Init Si5351C
    si5351_init(&si_dev);

    //init ocsdc driver
    mmc_dev = ocsdc_mmc_init(SDC_CONTROLLER_QSYS_0_BASE, 108000000U, MMC_MODE_HS|MMC_MODE_HS_52MHz);
    mmc_dev->has_init = 0;
    sd_det = sd_det_prev = 0;

    // Init PCM1863
    ret = pcm186x_init(&pcm_dev);
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "PCM1863 init fail");
        return ret;
    }
    //pcm_source_sel(PCM_INPUT1);

    // Init ADV7610
    adv761x_init(&advrx_dev);

    // Init HDMI TX
#ifdef INC_ADV7513
    ret = adv7513_init(&advtx_dev);
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "ADV7513 init fail");
        return ret;
    }
#endif
#ifdef INC_SII1136
    ret = sii1136_init(&siitx_dev);
    if (ret != 0) {
        sniprintf(row1, US2066_ROW_LEN+1, "SII1136 init fail");
        return ret;
    }
#endif

    set_default_profile(1);
    set_default_settings();
    init_menu();

    // Load initconfig and profile
    read_userdata(INIT_CONFIG_SLOT, 0);
    read_userdata(0, 0);

    update_settings(1);

    return 0;
}

void switch_input(rc_code_t rcode, btn_code_t bcode) {
    avinput_t prev_input = (avinput == AV_TESTPAT) ? AV4 : (avinput-1);
    avinput_t next_input = (avinput == AV4) ? AV_TESTPAT : (avinput+1);

    switch (rcode) {
        case RC_BTN1: target_avinput = AV1_RGBS; break;
        case RC_BTN4: target_avinput = (avinput == AV1_RGsB) ? AV1_YPbPr : AV1_RGsB; break;
        case RC_BTN7: target_avinput = (avinput == AV1_RGBHV) ? AV1_RGBCS : AV1_RGBHV; break;
        case RC_BTN2: target_avinput = (avinput == AV2_YPbPr) ? AV2_RGsB : AV2_YPbPr; break;
        case RC_BTN3: target_avinput = (avinput == AV3_RGBHV) ? AV3_RGBCS : AV3_RGBHV; break;
        case RC_BTN6: target_avinput = AV3_RGBS; break;
        case RC_BTN9: target_avinput = (avinput == AV3_RGsB) ? AV3_YPbPr : AV3_RGsB; break;
        case RC_BTN5: target_avinput = AV4; break;
        case RC_BTN0: target_avinput = AV_TESTPAT; break;
        case RC_UP: target_avinput = prev_input; break;
        case RC_DOWN: target_avinput = next_input; break;
        default: break;
    }

    if (bcode == BC_UP)
        target_avinput = prev_input;
    else if (bcode == BC_DOWN)
        target_avinput = next_input;
}

void set_syncmux_biasmode(uint8_t syncmux_stc) {
    ths7353_set_input_biasmode(&ths_dev, syncmux_stc ? THS_BIAS_STC_MID : THS_BIAS_AC, (3-syncmux_stc), (3-syncmux_stc));
}

void switch_audmux(uint8_t audmux_sel) {
    if (audmux_sel)
        sys_ctrl |= SCTRL_AUDMUX_SEL;
    else
        sys_ctrl &= ~SCTRL_AUDMUX_SEL;

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
}

void switch_audsrc(audinput_t *audsrc_map, HDMI_audio_fmt_t *aud_tx_fmt) {
    uint8_t audsrc;

    if (avinput <= AV1_RGBCS)
        audsrc = audsrc_map[0];
    else if (avinput <= AV2_RGsB)
        audsrc = audsrc_map[1];
    else if (avinput <= AV3_YPbPr)
        audsrc = audsrc_map[2];
    else
        audsrc = audsrc_map[3];

    if (audsrc <= AUD_AV3_ANALOG)
        pcm186x_source_sel(&pcm_dev, audsrc);

    *aud_tx_fmt = (audsrc == AUD_SPDIF) ? AUDIO_SPDIF : AUDIO_I2S;
}

void set_dram_refresh(uint8_t enable) {
    if (enable)
        sys_ctrl |= SCTRL_DRAM_RFR_ENA;
    else
        sys_ctrl &= ~SCTRL_DRAM_RFR_ENA;

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
}

int sys_is_powered_on() {
    return sys_powered_on;
}

void sys_toggle_power() {
    sys_powered_on ^= 1;
}

void print_vm_stats() {
    alt_timestamp_type ts = alt_timestamp();
    int row = 0;
    memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));

    if (enable_tp || (enable_isl && isl_dev.sync_active) || (enable_hdmirx && advrx_dev.sync_active)) {
        if (!enable_tp) {
            sniprintf((char*)osd->osd_array.data[row][0], OSD_CHAR_COLS, "Input preset:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%s", vmode_in.name);
            sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "Refresh rate:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%u.%.2uHz", vmode_in.timings.v_hz_x100/100, vmode_in.timings.v_hz_x100%100);
            sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V synclen:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_in.timings.h_synclen, vmode_in.timings.v_synclen);
            sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V backporch:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_in.timings.h_backporch, vmode_in.timings.v_backporch);
            sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V active:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_in.timings.h_active, vmode_in.timings.v_active);
            sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V total:");
            sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_in.timings.h_total, vmode_in.timings.v_total);
            row++;
            row++;
        }

        sniprintf((char*)osd->osd_array.data[row][0], OSD_CHAR_COLS, "Output mode:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%s", vmode_out.name);
        sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "Refresh rate:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%u.%.2uHz (%s)", vmode_out.timings.v_hz_x100/100, vmode_out.timings.v_hz_x100%100, vm_conf.framelock ? "lock" : "unlock");
        sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V synclen:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_synclen, vmode_out.timings.v_synclen);
        sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V backporch:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_backporch, vmode_out.timings.v_backporch);
        sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V active:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_active, vmode_out.timings.v_active);
        sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "H/V total:");
        sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%-5u %-5u", vmode_out.timings.h_total, vmode_out.timings.v_total);
        row++;
    }
    sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "Firmware:");
    sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "v%u.%.2u @ " __DATE__, FW_VER_MAJOR, FW_VER_MINOR);
    sniprintf((char*)osd->osd_array.data[++row][0], OSD_CHAR_COLS, "Uptime:");
    sniprintf((char*)osd->osd_array.data[row][1], OSD_CHAR_COLS, "%luh %lumin", (uint32_t)((ts/TIMER_0_FREQ)/3600), ((uint32_t)((ts/TIMER_0_FREQ)/60) % 60));
    
    osd->osd_config.status_refresh = 1;
    osd->osd_row_color.mask = 0;
    osd->osd_sec_enable[0].mask = (1<<(row+1))-1;
    osd->osd_sec_enable[1].mask = (1<<(row+1))-1;
}

uint16_t get_sampler_phase() {
    uint32_t sample_rng_x1000;
    uint32_t isl_phase_x1000 = isl_get_sampler_phase(&isl_dev)*5625;
    
    if (vm_conf.h_skip == 0) {
        return isl_phase_x1000/1000;
    } else {
        sample_rng_x1000 = 360000 / (vm_conf.h_skip+1);
        return ((vm_conf.h_sample_sel*sample_rng_x1000)+(isl_phase_x1000/(vm_conf.h_skip+1)))/1000;
    }
}

void set_sampler_phase(uint8_t sampler_phase, uint8_t update_isl, uint8_t update_sc) {
    uint32_t sample_rng_x1000;
    uint8_t isl_phase;

    vmode_in.sampler_phase = sampler_phase;

    if ((sampler_phase == 0) || (vm_conf.h_skip == 0)) {
        vm_conf.h_sample_sel = vm_conf.h_skip / 2;
        isl_phase = sampler_phase;
    } else {
        sample_rng_x1000 = 360000 / (vm_conf.h_skip+1);
        vm_conf.h_sample_sel = ((sampler_phase-1)*5625)/sample_rng_x1000;
        isl_phase = (((((sampler_phase-1)*5625) % sample_rng_x1000)*64)/sample_rng_x1000) + 1;
    }
    
    if (vm_conf.h_skip > 0)
        printf("Sample sel: %u/%u\n", (vm_conf.h_sample_sel+1), (vm_conf.h_skip+1));

    if (update_isl)
        isl_set_sampler_phase(&isl_dev, isl_phase);

    if (update_sc)
        update_sc_config(&vmode_in, &vmode_out, &vm_conf, get_current_avconfig());
}

void set_default_settings() {
    memcpy(&ts, &ts_default, sizeof(settings_t));
    set_default_keymap();
}

void update_settings(int init_setup) {
    if (init_setup || (ts.osd_enable != cs.osd_enable) || (ts.osd_status_timeout != cs.osd_status_timeout)) {
        osd->osd_config.enable = !!ts.osd_enable;
        osd->osd_config.status_timeout = ts.osd_status_timeout;
        if (is_menu_active()) {
            render_osd_menu();
            display_menu((rc_code_t)-1, (btn_code_t)-1);
        }
    }
    if (init_setup || (ts.fan_pwm != cs.fan_pwm) || (ts.led_pwm != cs.led_pwm)) {
        sys_ctrl &= ~(SCTRL_FAN_PWM_MASK|SCTRL_LED_PWM_MASK);
        sys_ctrl |= (ts.fan_pwm << SCTRL_FAN_PWM_OFFS) | (ts.led_pwm << SCTRL_LED_PWM_OFFS);
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
    }
    if (init_setup)
        target_avinput = ts.default_avinput;

    memcpy(&cs, &ts, sizeof(settings_t));
}

void mainloop()
{
    int i, man_input_change;
    char op_status[4];
    uint32_t pclk_i_hz, pclk_o_hz, dotclk_hz, h_hz, pll_h_total, pll_h_total_prev=0;
    uint8_t h_skip_prev, sampler_phase_prev;
    ths_channel_t target_ths_ch;
    ths_input_t target_ths_input;
    isl_input_t target_isl_input=0;
    video_sync target_isl_sync=0;
    video_format target_format=0;
    status_t status;
    avconfig_t *cur_avconfig;
    si5351_clk_src si_clk_src;
    alt_timestamp_type start_ts;

    cur_avconfig = get_current_avconfig();

    while (1) {
        start_ts = alt_timestamp();

        read_controls();
        parse_control();

        if (!sys_powered_on)
            break;

        if (target_avinput != avinput) {

            // defaults
            enable_isl = 1;
            enable_hdmirx = 0;
            enable_tp = 0;
            target_ths_ch = THS_CH_NONE;
            target_ths_input = THS_INPUT_A;
            target_isl_sync = SYNC_SOG;

            switch (target_avinput) {
            case AV_TESTPAT:
                enable_isl = 0;
                enable_tp = 1;
                cur_avconfig->tp_mode = -1;
                break;
            case AV1_RGBS:
                target_isl_input = ISL_CH0;
                target_format = FORMAT_RGBS;
                target_ths_ch = THS_CH_1;
                target_ths_input = THS_INPUT_A;
                break;
            case AV1_RGsB:
                target_isl_input = ISL_CH0;
                target_format = FORMAT_RGsB;
                target_ths_ch = THS_CH_1;
                target_ths_input = THS_INPUT_B;
                break;
            case AV1_YPbPr:
                target_isl_input = ISL_CH0;
                target_format = FORMAT_YPbPr;
                target_ths_ch = THS_CH_1;
                target_ths_input = THS_INPUT_B;
                break;
            case AV1_RGBHV:
                target_isl_input = ISL_CH0;
                target_isl_sync = SYNC_HV;
                target_format = FORMAT_RGBHV;
                break;
            case AV1_RGBCS:
                target_isl_input = ISL_CH0;
                target_isl_sync = SYNC_CS;
                target_format = FORMAT_RGBS;
                break;
            case AV2_YPbPr:
                target_isl_input = ISL_CH1;
                target_format = FORMAT_YPbPr;
                break;
            case AV2_RGsB:
                target_isl_input = ISL_CH1;
                target_format = FORMAT_RGsB;
                break;
            case AV3_RGBHV:
                target_isl_input = ISL_CH2;
                target_format = FORMAT_RGBHV;
                target_isl_sync = SYNC_HV;
                break;
            case AV3_RGBCS:
                target_isl_input = ISL_CH2;
                target_isl_sync = SYNC_CS;
                target_format = FORMAT_RGBS;
                break;
            case AV3_RGBS:
                target_isl_input = ISL_CH2;
                target_format = FORMAT_RGBS;
                target_ths_ch = THS_CH_3;
                target_ths_input = THS_INPUT_A;
                break;
            case AV3_RGsB:
                target_isl_input = ISL_CH2;
                target_format = FORMAT_RGsB;
                target_ths_ch = THS_CH_3;
                target_ths_input = THS_INPUT_B;
                break;
            case AV3_YPbPr:
                target_isl_input = ISL_CH2;
                target_format = FORMAT_YPbPr;
                target_ths_ch = THS_CH_3;
                target_ths_input = THS_INPUT_B;
                break;
            case AV4:
                enable_isl = 0;
                enable_hdmirx = 1;
                target_format = FORMAT_YPbPr;
                break;
            default:
                enable_isl = 0;
                break;
            }

            printf("### SWITCH MODE TO %s ###\n", avinput_str[target_avinput]);

            avinput = target_avinput;
            isl_enable_power(&isl_dev, 0);
            isl_enable_outputs(&isl_dev, 0);

            sys_ctrl &= ~(SCTRL_CAPTURE_SEL|SCTRL_ISL_VS_POL|SCTRL_ISL_VS_TYPE|SCTRL_VGTP_ENABLE|SCTRL_CSC_ENABLE|SCTRL_HDMIRX_SPDIF);

            ths7353_singlech_source_sel(&ths_dev, target_ths_ch, target_ths_input, cur_avconfig->syncmux_stc ? THS_BIAS_STC_MID : THS_BIAS_AC, (3-cur_avconfig->syncmux_stc), (3-cur_avconfig->syncmux_stc));

            if (enable_isl) {
                isl_source_sel(&isl_dev, target_isl_input, target_isl_sync, target_format);
                isl_dev.sync_active = 0;
                pll_h_total_prev = 0;
                vmode_in.sampler_phase = (uint8_t)-1;
                vm_conf.h_skip = (uint8_t)-1;

                // send current PLL h_total to isl_frontend for mode detection
                sc->hv_in_config.h_total = isl_get_pll_htotal(&isl_dev);

                // set some defaults
                if (target_isl_sync == SYNC_HV)
                    sys_ctrl |= SCTRL_ISL_VS_TYPE;
                if (target_format == FORMAT_YPbPr)
                    sys_ctrl |= SCTRL_CSC_ENABLE;
            } else if (enable_hdmirx) {
                advrx_dev.sync_active = 0;
                sys_ctrl |= SCTRL_CAPTURE_SEL;
            } else if (enable_tp) {
                sys_ctrl |= SCTRL_VGTP_ENABLE;
            }

            adv761x_enable_power(&advrx_dev, enable_hdmirx);

            switch_audsrc(cur_avconfig->audio_src_map, &cur_avconfig->hdmitx_cfg.audio_fmt);

            IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

            strncpy(row1, avinput_str[avinput], US2066_ROW_LEN+1);
            strncpy(row2, "    NO SYNC", US2066_ROW_LEN+1);
            ui_disp_status(1);
        }

        update_settings(0);
        status = update_avconfig();

        if (enable_tp) {
            if (status & TP_MODE_CHANGE) {
                get_standard_mode(cur_avconfig->tp_mode, &vmode_in, &vmode_out, &vm_conf);

                pclk_o_hz = calculate_pclk(si_dev.xtal_freq, &vmode_out, &vm_conf);
                printf("PCLK_OUT: %luHz\n", pclk_o_hz);

                if (vm_conf.si_pclk_mult == 0)
                    si5351_set_frac_mult(&si_dev, SI_PLLA, SI_PCLK_PIN, SI_XTAL, 0, pclk_o_hz/1000, si_dev.xtal_freq/1000, NULL);
                else
                    si5351_set_integer_mult(&si_dev, SI_PLLA, SI_PCLK_PIN, SI_XTAL, 0, (vm_conf.si_pclk_mult > 0) ? vm_conf.si_pclk_mult : 1, (vm_conf.si_pclk_mult < 0) ? (-1)*vm_conf.si_pclk_mult : 0);

                update_osd_size(&vmode_out);
                update_sc_config(&vmode_in, &vmode_out, &vm_conf, cur_avconfig);
#ifdef INC_ADV7513
                adv7513_set_pixelrep_vic(&advtx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic);
#endif
#ifdef INC_SII1136
                sii1136_init_mode(&siitx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic, pclk_o_hz);
#endif
                sniprintf(row2, US2066_ROW_LEN+1, "%ux%u%c %u.%.2uHz", vmode_out.timings.h_active, vmode_out.timings.v_active<<vmode_out.timings.interlaced, vmode_out.timings.interlaced ? 'i' : ' ', (vmode_out.timings.v_hz_x100/100), (vmode_out.timings.v_hz_x100%100));
                ui_disp_status(1);
            }
        } else if (enable_isl) {
            if (isl_check_activity(&isl_dev, target_isl_input, target_isl_sync)) {
                if (isl_dev.sync_active) {
                    isl_enable_power(&isl_dev, 1);
                    isl_enable_outputs(&isl_dev, 1);
                    printf("ISL51002 sync up\n");
                } else {
                    isl_enable_power(&isl_dev, 0);
                    isl_enable_outputs(&isl_dev, 0);
                    strncpy(row1, avinput_str[avinput], US2066_ROW_LEN+1);
                    strncpy(row2, "    NO SYNC", US2066_ROW_LEN+1);
                    ui_disp_status(1);
                    printf("ISL51002 sync lost\n");
                }
            }

            if (isl_dev.sync_active) {
                if (isl_get_sync_stats(&isl_dev, sc->fe_status.vtotal, sc->fe_status.interlace_flag, sc->fe_status2.pcnt_frame) || (status & MODE_CHANGE)) {
                    h_skip_prev = vm_conf.h_skip;
                    sampler_phase_prev = vmode_in.sampler_phase;

                    memset(&vmode_in, 0, sizeof(mode_data_t));

#ifdef ISL_MEAS_HZ
                    if (isl_dev.sm.h_period_x16 > 0)
                        h_hz = (16*isl_dev.xtal_freq)/isl_dev.sm.h_period_x16;
                    else
                        h_hz = 0;
                    if ((isl_dev.sm.h_period_x16 > 0) && (isl_dev.ss.v_total > 0))
                        vmode_in.timings.v_hz_x100 = (16*5*isl_dev.xtal_freq)/((isl_dev.sm.h_period_x16 * isl_dev.ss.v_total) / (100/5));
                    else
                        vmode_in.timings.v_hz_x100 = 0;
#else
                    vmode_in.timings.v_hz_x100 = (100*27000000UL)/isl_dev.ss.pcnt_frame;
                    h_hz = (100*27000000UL)/((100*isl_dev.ss.pcnt_frame*(1+isl_dev.ss.interlace_flag))/isl_dev.ss.v_total);
#endif

                    vmode_in.timings.h_synclen = isl_dev.sm.h_synclen_x16 / 16;
                    vmode_in.timings.v_total = isl_dev.ss.v_total;
                    vmode_in.timings.interlaced = isl_dev.ss.interlace_flag;

                    oper_mode = get_operating_mode(cur_avconfig, &vmode_in, &vmode_out, &vm_conf);

                    if (oper_mode == OPERMODE_PURE_LM)
                        sniprintf(op_status, 4, "x%u", vm_conf.y_rpt+1);
                    else if (oper_mode == OPERMODE_ADAPT_LM)
                        sniprintf(op_status, 4, "%c%ua", (vm_conf.y_rpt < 0) ? '/' : 'x', (vm_conf.y_rpt < 0) ? (-1*vm_conf.y_rpt+1) : vm_conf.y_rpt+1);
                    else if (oper_mode == OPERMODE_SCALER)
                        sniprintf(op_status, 4, "SCL");

                    if (oper_mode != OPERMODE_INVALID) {
                        printf("\nInput: %s -> Output: %s (opermode %d)\n", vmode_in.name, vmode_out.name, oper_mode);

                        sniprintf(row1, US2066_ROW_LEN+1, "%-9s %4u-%c %s", avinput_str[avinput], isl_dev.ss.v_total, isl_dev.ss.interlace_flag ? 'i' : 'p', op_status);
                        sniprintf(row2, US2066_ROW_LEN+1, "%lu.%.2lukHz %u.%.2uHz %c%c", (h_hz+5)/1000, ((h_hz+5)%1000)/10,
                                                                                            (vmode_in.timings.v_hz_x100/100),
                                                                                            (vmode_in.timings.v_hz_x100%100),
                                                                                            isl_dev.ss.h_polarity ? '-' : '+',
                                                                                            (target_isl_sync == SYNC_HV) ? (isl_dev.ss.v_polarity ? '-' : '+') : (isl_dev.ss.sog_trilevel ? '3' : ' '));
                        ui_disp_status(1);

                        pll_h_total = (vm_conf.h_skip+1) * vmode_in.timings.h_total + (((vm_conf.h_skip+1) * vmode_in.timings.h_total_adj * 5 + 50) / 100);

                        si_clk_src = vm_conf.framelock ? SI_CLKIN : SI_XTAL;
                        pclk_i_hz = h_hz * pll_h_total;
                        dotclk_hz = estimate_dotclk(&vmode_in, h_hz);
                        pclk_o_hz = calculate_pclk((si_clk_src == SI_CLKIN) ? pclk_i_hz : si_dev.xtal_freq, &vmode_out, &vm_conf);
                        printf("H: %lu.%.2lukHz V: %u.%.2uHz\n", (h_hz+5)/1000, ((h_hz+5)%1000)/10, (vmode_in.timings.v_hz_x100/100), (vmode_in.timings.v_hz_x100%100));
                        printf("Estimated source dot clock: %lu.%.2luMHz\n", (dotclk_hz+5000)/1000000, ((dotclk_hz+5000)%1000000)/10000);
                        printf("PCLK_IN: %luHz PCLK_OUT: %luHz\n", pclk_i_hz, pclk_o_hz);

                        // CEA-770.3 HDTV modes use tri-level syncs which have twice the width of bi-level syncs of corresponding CEA-861 modes
                        if ((vmode_in.type & VIDEO_HDTV) && (target_isl_sync == SYNC_SOG)) {
                            vmode_in.timings.h_synclen *= 2;
                            isl_dev.sync_trilevel = 1;
                        } else {
                            isl_dev.sync_trilevel = 0;
                        }

                        isl_source_setup(&isl_dev, pll_h_total);

                        isl_set_afe_bw(&isl_dev, dotclk_hz);

                        if ((pll_h_total != pll_h_total_prev) || (vm_conf.h_skip != h_skip_prev))
                            set_sampler_phase(vmode_in.sampler_phase, !((pll_h_total == pll_h_total_prev) && !sampler_phase_prev && !vmode_in.sampler_phase), 0);

                        pll_h_total_prev = pll_h_total;

                        // Setup Si5351
                        if (vm_conf.si_pclk_mult == 0)
                            si5351_set_frac_mult(&si_dev,
                                                 SI_PLLA,
                                                 SI_PCLK_PIN,
                                                 si_clk_src,
                                                 pclk_i_hz,
                                                 vm_conf.framelock ? vmode_out.timings.h_total*vmode_out.timings.v_total*(vmode_in.timings.interlaced+1)*vm_conf.framelock : pclk_o_hz/1000,
                                                 vm_conf.framelock ? pll_h_total*vmode_in.timings.v_total*(vmode_out.timings.interlaced+1) : si_dev.xtal_freq/1000,
                                                 NULL);
                        else
                            si5351_set_integer_mult(&si_dev, SI_PLLA, SI_PCLK_PIN, si_clk_src, pclk_i_hz, (vm_conf.si_pclk_mult > 0) ? vm_conf.si_pclk_mult : 1, (vm_conf.si_pclk_mult < 0) ? (-1)*vm_conf.si_pclk_mult : 0);

                        if (vm_conf.framelock)
                            sys_ctrl |= SCTRL_FRAMELOCK;
                        else
                            sys_ctrl &= ~SCTRL_FRAMELOCK;

                        // TODO: dont read polarity from ISL51002
                        sys_ctrl &= ~(SCTRL_ISL_HS_POL|SCTRL_ISL_VS_POL);
                        if ((target_isl_sync != SYNC_HV) || isl_dev.ss.h_polarity)
                            sys_ctrl |= SCTRL_ISL_HS_POL;
                        if ((target_isl_sync == SYNC_HV) && isl_dev.ss.v_polarity)
                            sys_ctrl |= SCTRL_ISL_VS_POL;
                        IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

                        update_osd_size(&vmode_out);
                        update_sc_config(&vmode_in, &vmode_out, &vm_conf, cur_avconfig);

                        // Setup VIC and pixel repetition
#ifdef INC_ADV7513
                        adv7513_set_pixelrep_vic(&advtx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic);
#endif
#ifdef INC_SII1136
                        sii1136_init_mode(&siitx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic, pclk_o_hz);
#endif
                    }
                } else if (status & SC_CONFIG_CHANGE) {
                    update_sc_config(&vmode_in, &vmode_out, &vm_conf, cur_avconfig);
                }
            } else {

            }

            isl_update_config(&isl_dev, &cur_avconfig->isl_cfg, 0);
        } else if (enable_hdmirx) {
            if (adv761x_check_activity(&advrx_dev)) {
                if (advrx_dev.sync_active) {
                    printf("adv sync up\n");
                } else {
                    strncpy(row1, avinput_str[avinput], US2066_ROW_LEN+1);
                    strncpy(row2, "    free-run", US2066_ROW_LEN+1);
                    ui_disp_status(1);
                    printf("adv sync lost\n");
                }
            }

            if (advrx_dev.sync_active) {
                if (adv761x_get_sync_stats(&advrx_dev) || (status & MODE_CHANGE)) {
                    memset(&vmode_in, 0, sizeof(mode_data_t));

                    h_hz = advrx_dev.pclk_hz/advrx_dev.ss.h_total;
                    vmode_in.timings.v_hz_x100 = (((h_hz*10000)/advrx_dev.ss.v_total)+50)/100;

                    if (advrx_dev.ss.interlace_flag)
                        vmode_in.timings.v_hz_x100 *= 2;

                    sniprintf(vmode_in.name, 16, "%ux%u%c", advrx_dev.ss.h_active, (advrx_dev.ss.v_active<<advrx_dev.ss.interlace_flag), advrx_dev.ss.interlace_flag ? 'i' : '\0');
                    vmode_in.timings.h_active = advrx_dev.ss.h_active;
                    vmode_in.timings.v_active = advrx_dev.ss.v_active;
                    vmode_in.timings.h_total = advrx_dev.ss.h_total;
                    vmode_in.timings.v_total = advrx_dev.ss.v_total;
                    vmode_in.timings.h_backporch = advrx_dev.ss.h_backporch;
                    vmode_in.timings.v_backporch = advrx_dev.ss.v_backporch;
                    vmode_in.timings.h_synclen = advrx_dev.ss.h_synclen;
                    vmode_in.timings.v_synclen = advrx_dev.ss.v_synclen;
                    vmode_in.timings.interlaced = advrx_dev.ss.interlace_flag;
                    //TODO: VIC+pixelrep

                    oper_mode = get_operating_mode(cur_avconfig, &vmode_in, &vmode_out, &vm_conf);

                    if (oper_mode == OPERMODE_PURE_LM)
                        sniprintf(op_status, 4, "x%u", vm_conf.y_rpt+1);
                    else if (oper_mode == OPERMODE_ADAPT_LM)
                        sniprintf(op_status, 4, "%c%ua", (vm_conf.y_rpt < 0) ? '/' : 'x', (vm_conf.y_rpt < 0) ? (-1*vm_conf.y_rpt+1) : vm_conf.y_rpt+1);
                    else if (oper_mode == OPERMODE_SCALER)
                        sniprintf(op_status, 4, "SCL");

                    if (oper_mode != OPERMODE_INVALID) {
                        printf("\nInput: %s -> Output: %s (opermode %d)\n", vmode_in.name, vmode_out.name, oper_mode);

                        sniprintf(row1, US2066_ROW_LEN+1, "%s %s %s", avinput_str[avinput], vmode_in.name, op_status);
                        sniprintf(row2, US2066_ROW_LEN+1, "%lu.%.2lukHz %u.%.2uHz %c%c", (h_hz+5)/1000, ((h_hz+5)%1000)/10,
                                                                                            (vmode_in.timings.v_hz_x100/100),
                                                                                            (vmode_in.timings.v_hz_x100%100),
                                                                                            advrx_dev.ss.h_polarity ? '+' : '-',
                                                                                            advrx_dev.ss.v_polarity ? '+' : '-');
                        ui_disp_status(1);

                        si_clk_src = vm_conf.framelock ? SI_CLKIN : SI_XTAL;
                        pclk_i_hz = h_hz * advrx_dev.ss.h_total;
                        pclk_o_hz = calculate_pclk((si_clk_src == SI_CLKIN) ? pclk_i_hz : si_dev.xtal_freq, &vmode_out, &vm_conf);
                        printf("H: %u.%.2ukHz V: %u.%.2uHz", h_hz/1000, (((h_hz%1000)+5)/10), (vmode_in.timings.v_hz_x100/100), (vmode_in.timings.v_hz_x100%100));
                        printf("PCLK_IN: %luHz PCLK_OUT: %luHz\n", pclk_i_hz, pclk_o_hz);

                        // Setup Si5351
                        if (vm_conf.si_pclk_mult == 0)
                            si5351_set_frac_mult(&si_dev,
                                                 SI_PLLA,
                                                 SI_PCLK_PIN,
                                                 si_clk_src,
                                                 pclk_i_hz,
                                                 vm_conf.framelock ? vmode_out.timings.h_total*vmode_out.timings.v_total*(vmode_in.timings.interlaced+1)*vm_conf.framelock : pclk_o_hz/1000,
                                                 vm_conf.framelock ? vmode_in.timings.h_total*vmode_in.timings.v_total*(vmode_out.timings.interlaced+1) : si_dev.xtal_freq/1000,
                                                 NULL);
                        else
                            si5351_set_integer_mult(&si_dev, SI_PLLA, SI_PCLK_PIN, si_clk_src, pclk_i_hz, (vm_conf.si_pclk_mult > 0) ? vm_conf.si_pclk_mult : 1, (vm_conf.si_pclk_mult < 0) ? (-1)*vm_conf.si_pclk_mult : 0);

                        if (vm_conf.framelock)
                            sys_ctrl |= SCTRL_FRAMELOCK;
                        else
                            sys_ctrl &= ~SCTRL_FRAMELOCK;

                        IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

                        update_osd_size(&vmode_out);
                        update_sc_config(&vmode_in, &vmode_out, &vm_conf, cur_avconfig);

                        // Setup RX input color space
                        adv761x_set_input_cs(&advrx_dev);

                        // Setup VIC and pixel repetition
#ifdef INC_ADV7513
                        adv7513_set_pixelrep_vic(&advtx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic);
#endif
#ifdef INC_SII1136
                        sii1136_init_mode(&siitx_dev, vmode_out.tx_pixelrep, vmode_out.hdmitx_pixr_ifr, vmode_out.vic, pclk_o_hz);
#endif
                    }
                } else if (status & SC_CONFIG_CHANGE) {
                    update_sc_config(&vmode_in, &vmode_out, &vm_conf, cur_avconfig);
                }
            } else {
                advrx_dev.pclk_hz = 0;
            }

            // Set TX config to match RX status
            if (cur_avconfig->audio_src_map[3] == AUD_AV4_DIGITAL) {
                cur_avconfig->hdmitx_cfg.audio_fmt = (adv761x_get_audio_sample_type(&advrx_dev) == IEC60958_SAMPLE_LPCM) ? AUDIO_I2S : AUDIO_SPDIF;
                cur_avconfig->hdmitx_cfg.i2s_fs = adv761x_get_i2s_fs(&advrx_dev);
                cur_avconfig->hdmitx_cfg.audio_cc_val = adv761x_get_audio_cc(&advrx_dev);
                cur_avconfig->hdmitx_cfg.audio_ca_val = adv761x_get_audio_ca(&advrx_dev);
                if (!!(sys_ctrl & SCTRL_HDMIRX_SPDIF) != cur_avconfig->hdmitx_cfg.audio_fmt) {
                    sys_ctrl &= ~SCTRL_HDMIRX_SPDIF;
                    if (cur_avconfig->hdmitx_cfg.audio_fmt == AUDIO_SPDIF)
                        sys_ctrl |= SCTRL_HDMIRX_SPDIF;
                    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);
                }
            }
            adv761x_update_config(&advrx_dev, &cur_avconfig->hdmirx_cfg);
        }

#ifdef INC_ADV7513
        adv7513_check_hpd_power(&advtx_dev);
        adv7513_update_config(&advtx_dev, &cur_avconfig->hdmitx_cfg);
#endif
#ifdef INC_SII1136
        sii1136_update_config(&siitx_dev, &cur_avconfig->hdmitx_cfg);
#endif

        pcm186x_update_config(&pcm_dev, &cur_avconfig->pcm_cfg);

        check_sdcard();

        while (alt_timestamp() < start_ts + MAINLOOP_INTERVAL_US*(TIMER_0_FREQ/1000000)) {}
    }
}

void wait_powerup() {
    while (1) {
        read_controls();
        parse_control();

        if (sys_powered_on)
            break;

        usleep(20000);
    }
}

int main()
{
    int ret;

    // Start system clock
    alt_timestamp_start();

    // Write-protect flash
    flash_write_protect(&flashctrl_dev, 1);

    while (1) {
        ret = init_hw();
        if (ret != 0) {
            sniprintf(row2, US2066_ROW_LEN+1, "Error code: %d", ret);
            printf("%s\n%s\n", row1, row2);
            us2066_display_on(&chardisp_dev);
            ui_disp_status(1);
            while (1) {}
        }

        printf("### OSSC PRO INIT OK ###\n\n");

        sys_powered_on = 0;
        wait_powerup();

        // Restart system clock
        alt_timestamp_start();

        // ADVRX powerup
        // pcm powerup
        sys_ctrl |= SCTRL_POWER_ON;
        sys_ctrl &= ~SCTRL_EMIF_POWERDN_REQ;
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, sys_ctrl);

        // Invalidate input
        avinput = (avinput_t)-1;

        pcm186x_enable_power(&pcm_dev, 1);

        us2066_display_on(&chardisp_dev);

#ifdef INC_SII1136
        sii1136_enable_power(&siitx_dev, 1);
#endif

        mainloop();

#ifdef VIP
        // Disable VIP on powerdown
        vip_cvi->ctrl = 0;
        vip_dil->ctrl = 0;
        vip_il->ctrl = 0;
        vip_scl_pp->ctrl = 0;
        vip_fb->ctrl = 0;
        vip_cvo->ctrl = 0;
        scl_loaded_pp_coeff = -1;
#endif
    }
}
