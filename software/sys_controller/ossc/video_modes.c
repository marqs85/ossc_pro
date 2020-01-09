//
// Copyright (C) 2015-2017  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "system.h"
#include "video_modes.h"

#define LINECNT_MAX_TOLERANCE   30

#define VM_OUT_YMULT        (vm_conf->y_rpt+1)
#define VM_OUT_XMULT        (vm_conf->x_rpt+1)
#define VM_OUT_PCLKMULT     ((vm_conf->x_rpt+1)*(vm_conf->y_rpt+1))

const mode_data_t video_modes_default[] = VIDEO_MODES_DEF;
mode_data_t video_modes[VIDEO_MODES_CNT];

const ad_mode_data_t adaptive_modes_default[] = ADAPTIVE_MODES_DEF;
ad_mode_data_t adaptive_modes[ADAPTIVE_MODES_CNT];

void set_default_vm_table() {
    memcpy(video_modes, video_modes_default, VIDEO_MODES_SIZE);
    memcpy(adaptive_modes, adaptive_modes_default, ADAPTIVE_MODES_SIZE);
}

void vmode_hv_mult(mode_data_t *vmode, uint8_t h_mult, uint8_t v_mult) {
    // TODO: check limits
    vmode->h_synclen *= h_mult;
    vmode->h_backporch *= h_mult;
    vmode->h_active *= h_mult;
    vmode->h_total = h_mult * vmode->h_total + ((h_mult * vmode->h_total_adj * 5 + 50) / 100);

    vmode->v_synclen *= v_mult;
    vmode->v_backporch *= v_mult;
    vmode->v_active *= v_mult;
    if ((vmode->flags & MODE_INTERLACED) && ((v_mult % 2) == 0)) {
        vmode->flags &= ~MODE_INTERLACED;
        vmode->v_total *= (v_mult / 2);
    } else {
        vmode->v_total *= v_mult;
    }
}

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz) {
    if ((vm_in->type & VIDEO_LDTV) ||
        (vm_in->type & VIDEO_SDTV) ||
        (vm_in->type & VIDEO_EDTV))
    {
        return h_hz * 858;
    } else {
        return vm_in->h_total * h_hz;
    }
}

int get_adaptive_mode(uint16_t totlines, uint8_t progressive, uint16_t hz_x100, vm_mult_config_t *vm_conf, uint8_t ymult, mode_data_t *vm_in, mode_data_t *vm_out, si5351_ms_config_t *si_ms_conf)
{
    int i;
    unsigned num_modes = sizeof(adaptive_modes)/sizeof(ad_mode_data_t);
    memset(vm_in, 0, sizeof(ad_mode_data_t));
    memset(vm_out, 0, sizeof(ad_mode_data_t));

    for (i=0; i<num_modes; i++) {
        if ((totlines == (adaptive_modes[i].v_total_i)) && (ymult == (adaptive_modes[i].y_rpt+1))) {
            vm_conf->x_rpt = 0;
            vm_conf->y_rpt = 0;
            vm_conf->x_skip = 0;
            vm_conf->x_start = 0;
            vm_conf->pclk_mult = 1;
            vm_conf->tx_pixelrep = TX_PIXELREP_DISABLE;
            vm_conf->hdmitx_pixr_ifr = TX_PIXELREP_DISABLE;

            vm_in->h_active = adaptive_modes[i].h_active_i;
            vm_in->v_active = adaptive_modes[i].v_active_i;
            vm_in->h_total = adaptive_modes[i].h_total_i;
            vm_in->h_total_adj = adaptive_modes[i].h_total_adj_i;
            vm_in->v_total = adaptive_modes[i].v_total_i;
            vm_in->h_backporch = adaptive_modes[i].h_backporch_i;
            vm_in->v_backporch = adaptive_modes[i].v_backporch_i;
            vm_in->h_synclen = adaptive_modes[i].h_synclen_i;
            vm_in->v_synclen = adaptive_modes[i].v_synclen_i;
            vm_in->type = adaptive_modes[i].type;

            strncpy(vm_out->name, adaptive_modes[i].name, 10);
            vm_out->vic = adaptive_modes[i].vic;
            vm_out->h_active = adaptive_modes[i].h_active_o;
            vm_out->v_active = adaptive_modes[i].v_active_o;
            vm_out->h_total = adaptive_modes[i].h_total_o;
            vm_out->h_total_adj = adaptive_modes[i].h_total_adj_o;
            vm_out->v_total = adaptive_modes[i].v_total_o;
            vm_out->h_backporch = adaptive_modes[i].h_backporch_o;
            vm_out->v_backporch = adaptive_modes[i].v_backporch_o;
            vm_out->h_synclen = adaptive_modes[i].h_synclen_o;
            vm_out->v_synclen = adaptive_modes[i].v_synclen_o;
            vm_conf->y_rpt = adaptive_modes[i].y_rpt;
            vm_conf->x_start = (vm_out->h_active-vm_in->h_active)/2;

            memcpy(si_ms_conf, &adaptive_modes[i].si_ms_conf, sizeof(si5351_ms_config_t));

            return i;
        }
    }

    return -1;
}

/* TODO: rewrite, check hz etc. */
int get_mode_id(uint16_t totlines, uint8_t progressive, uint16_t hz_x100, video_type typemask, uint8_t s400p_mode, uint8_t s480p_mode, vm_mult_config_t *vm_conf, uint8_t ymult, mode_data_t *vm_in, mode_data_t *vm_out)
{
    int i;
    unsigned num_modes = sizeof(video_modes)/sizeof(mode_data_t);
    video_type mode_type;
    mode_flags valid_lm[] = { MODE_PT, (MODE_L2 | (MODE_L2)), (MODE_L3_GEN_4_3), (MODE_L4_GEN_4_3), (MODE_L5_GEN_4_3) };
    mode_flags target_lm;
    uint8_t pt_only = 0;
    uint8_t pm_240p = ymult-1;
    uint8_t pm_384p = 1;
    uint8_t pm_480i = 1;
    uint8_t pm_480p = 0;
    uint8_t pm_1080i = 1;
    uint8_t upsample2x = 1;

    // one for each video_group
    uint8_t* group_ptr[] = { &pt_only, &pm_240p, &pm_384p, &pm_480i, &pm_480p, &pm_1080i };

    for (i=0; i<num_modes; i++) {
        mode_type = video_modes[i].type;

        switch (video_modes[i].group) {
            case GROUP_NONE:
            case GROUP_240P:
                break;
            case GROUP_384P:
                //fixed Line2x/3x mode for 240x360p
                valid_lm[2] = MODE_L2_240x360;
                valid_lm[3] = MODE_L3_240x360;
                valid_lm[4] = MODE_L3_GEN_16_9;
                if (video_modes[i].v_total == 449) {
                    if (!strncmp(video_modes[i].name, "720x400", 7)) {
                        if (s400p_mode == 0)
                            continue;
                    } else if (!strncmp(video_modes[i].name, "640x400", 7)) {
                        if (s400p_mode == 1)
                            continue;
                    }
                }
                break;
            case GROUP_480I:
                //fixed Line3x/4x mode for 480i
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L4_GEN_4_3;
                break;
            case GROUP_480P:
                if (video_modes[i].v_total == 525) {
                    if (video_modes[i-1].group == GROUP_480I) { // hit "480p" on the list
                        if (s480p_mode == 0) // Auto
                            mode_type &= ~VIDEO_PC;
                        else if (s480p_mode == 2) // VESA 640x480@60
                            continue;
                    } else { // "640x480" on the list
                        if (s480p_mode == 0) // Auto
                            mode_type &= ~VIDEO_EDTV;
                        else if (s480p_mode == 1) // DTV 480p
                            continue;
                    }
                }
                break;
            case GROUP_1080I:
                break;
            default:
                printf("WARNING: Corrupted mode (id %d)\n", i);
                continue;
                break;
        }

        target_lm = valid_lm[*group_ptr[video_modes[i].group]];

        if ((typemask & mode_type) && (target_lm & video_modes[i].flags) && (progressive == !(video_modes[i].flags & MODE_INTERLACED)) && (totlines <= (video_modes[i].v_total+LINECNT_MAX_TOLERANCE))) {

            // defaults
            memcpy(vm_in, &video_modes[i], sizeof(mode_data_t));
            memcpy(vm_out, &video_modes[i], sizeof(mode_data_t));
            vm_in->v_total = totlines;
            vm_out->v_total = totlines;
            vm_out->vic = HDMI_Unknown;

            vm_conf->x_rpt = 0;
            vm_conf->y_rpt = 0;
            vm_conf->x_skip = 0;
            vm_conf->x_start = 0;
            vm_conf->pclk_mult = 1;
            vm_conf->tx_pixelrep = TX_PIXELREP_DISABLE;
            vm_conf->hdmitx_pixr_ifr = TX_PIXELREP_DISABLE;

            target_lm &= video_modes[i].flags;    //ensure L2 mode uniqueness

            switch (target_lm) {
                case MODE_PT:
                    vm_out->vic = vm_in->vic;
                    // Upsample / pixel-repeat horizontal resolution of 240p/480i modes to fulfill min. 25MHz TMDS clock requirement
                    if ((video_modes[i].group == GROUP_240P) || (video_modes[i].group == GROUP_480I)) {
                        if (upsample2x) {
                            vmode_hv_mult(vm_in, 2, 1);
                            vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                        } else {
                            vm_conf->tx_pixelrep = TX_PIXELREP_2X;
                        }
                        vm_conf->hdmitx_pixr_ifr = TX_PIXELREP_2X;
                    }
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2:
                    vm_conf->y_rpt = 1;

                    // Upsample / pixel-repeat horizontal resolution of 384p/480p/960i modes
                    if ((video_modes[i].group == GROUP_384P) || (video_modes[i].group == GROUP_480P) || ((video_modes[i].group == GROUP_1080I) && (video_modes[i].h_total < 1200))) {
                        if (upsample2x) {
                            vmode_hv_mult(vm_in, 2, 1);
                            vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                            vm_conf->tx_pixelrep = TX_PIXELREP_2X;
                        }
                    } else {
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    }
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_512_COL:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 1;
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_256_COL:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 5;
                    vm_conf->x_skip = 5;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_320_COL:
                case MODE_L2_384_COL:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_240x360:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_GEN_16_9:
                    vm_conf->y_rpt = 2;

                    // Upsample / pixel-repeat horizontal resolution of 480i mode
                    if (video_modes[i].group == GROUP_480I) {
                        if (upsample2x) {
                            vmode_hv_mult(vm_in, 2, 1);
                            vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                            vm_conf->tx_pixelrep = TX_PIXELREP_2X;
                        }
                    } else {
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    }
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_GEN_4_3:
                    vm_conf->y_rpt = 2;
                    vm_out->h_synclen /= 3;
                    vm_out->h_backporch /= 3;
                    vm_out->h_active /= 3;
                    vm_conf->x_start = vm_out->h_active/2;
                    vm_out->h_total /= 3;
                    vm_out->h_total_adj = 0;
                    vmode_hv_mult(vm_out, 4, VM_OUT_YMULT);
                    vm_conf->pclk_mult = 4;
                    break;
                case MODE_L3_512_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 1;
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_384_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_320_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_256_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_240x360:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 6;
                    vm_conf->x_skip = 6;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 13;
                    break;
                case MODE_L4_GEN_4_3:
                    vm_conf->y_rpt = 3;

                    // Upsample / pixel-repeat horizontal resolution of 480i mode
                    if (video_modes[i].group == GROUP_480I) {
                        if (upsample2x) {
                            vmode_hv_mult(vm_in, 2, 1);
                            vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                            vm_conf->tx_pixelrep = TX_PIXELREP_2X;
                        }
                    } else {
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    }
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_512_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 1;
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_384_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_320_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_256_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L5_GEN_4_3:
                    vm_conf->y_rpt = 4;
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    if (1) {
                        vm_out->h_synclen = (vm_out->h_total - 1920)/4;
                        vm_out->h_backporch = (vm_out->h_total - 1920)/2;
                        vm_out->h_active = 1920;
                        vm_conf->x_start = (1920-vm_in->h_active)/2;

                        vm_out->v_backporch += (vm_out->v_active-1080)/2;
                        vm_out->v_active = 1080;
                    }
                    break;
                case MODE_L5_512_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 40;
                    break;
                case MODE_L5_384_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 30;
                    break;
                case MODE_L5_320_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 24;
                    break;
                case MODE_L5_256_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 5;
                    vm_conf->x_skip = 5;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_conf->pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 20;
                    break;
                default:
                    printf("WARNING: invalid target_lm\n");
                    continue;
                    break;
            }

            /*if (cm.hdmitx_vic == HDMI_Unknown)
                cm.hdmitx_vic = cm.cc.default_vic;*/

            return i;
        }
    }

    return -1;
}
