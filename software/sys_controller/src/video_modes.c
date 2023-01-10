//
// Copyright (C) 2015-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "system.h"
#include "video_modes.h"
#include "avconfig.h"

#define LINECNT_MAX_TOLERANCE   30

#define VM_OUT_YMULT        (vm_conf->y_rpt+1)
#define VM_OUT_XMULT        (vm_conf->x_rpt+1)
#define VM_OUT_PCLKMULT     (((vm_conf->x_rpt+1)*(vm_conf->y_rpt+1))/(vm_conf->h_skip+1))

#include "video_modes_list.c"

extern uint8_t vm_cur, vm_sel, smp_cur, smp_sel, dtmg_cur;

const int num_video_modes_plm = sizeof(video_modes_plm_default)/sizeof(mode_data_t);
const int num_video_modes = sizeof(video_modes_default)/sizeof(mode_data_t);
const int num_smp_presets = sizeof(smp_presets_default)/sizeof(smp_preset_t);

mode_data_t video_modes_plm[sizeof(video_modes_plm_default)/sizeof(mode_data_t)];
mode_data_t video_modes[sizeof(video_modes_default)/sizeof(mode_data_t)];
smp_preset_t smp_presets[sizeof(smp_presets_default)/sizeof(smp_preset_t)];
sync_timings_t hdmi_timings[NUM_VIDEO_GROUPS]; // one for each video_group to hold HDMI video timings

void set_default_vm_table() {
    memcpy(video_modes_plm, video_modes_plm_default, sizeof(video_modes_plm_default));
    memcpy(video_modes, video_modes_default, sizeof(video_modes_default));
    memcpy(smp_presets, smp_presets_default, sizeof(smp_presets_default));
    memset(hdmi_timings, 0, sizeof(hdmi_timings));
}

void vmode_hv_mult(mode_data_t *vmode, uint8_t h_mult, uint8_t v_mult) {
    uint32_t val, bp_extra;

    val = vmode->timings.h_synclen * h_mult;
    if (val > H_SYNCLEN_MAX) {
        vmode->timings.h_synclen = H_SYNCLEN_MAX;
        bp_extra = val - vmode->timings.h_synclen;
    } else {
        vmode->timings.h_synclen = val;
        bp_extra = 0;
    }

    val = (vmode->timings.h_backporch * h_mult) + bp_extra;
    if (val > H_BPORCH_MAX)
        vmode->timings.h_backporch = H_BPORCH_MAX;
    else
        vmode->timings.h_backporch = val;

    val = vmode->timings.h_active * h_mult;
    if (val > H_ACTIVE_MAX)
        vmode->timings.h_active = H_ACTIVE_MAX;
    else
        vmode->timings.h_active = val;

    vmode->timings.h_total = h_mult * vmode->timings.h_total + ((h_mult * vmode->timings.h_total_adj * 5 + 50) / 100);

    val = vmode->timings.v_synclen * v_mult;
    if (val > V_SYNCLEN_MAX) {
        vmode->timings.v_synclen = V_SYNCLEN_MAX;
        bp_extra = val - vmode->timings.v_synclen;
    } else {
        vmode->timings.v_synclen = val;
        bp_extra = 0;
    }

    val = (vmode->timings.v_backporch * v_mult) + bp_extra;
    if (val > V_BPORCH_MAX)
        vmode->timings.v_backporch = V_BPORCH_MAX;
    else
        vmode->timings.v_backporch = val;

    val = vmode->timings.v_active * v_mult;
    if (val > V_ACTIVE_MAX)
        vmode->timings.v_active = V_ACTIVE_MAX;
    else
        vmode->timings.v_active = val;

    if (vmode->timings.interlaced && ((v_mult % 2) == 0)) {
        vmode->timings.interlaced = 0;
        vmode->timings.v_total *= (v_mult / 2);
    } else {
        vmode->timings.v_total *= v_mult;
    }
}

void adjust_gen_width_diff(gen_width_mode_t gen_width_mode, int *diff_width) {
    if (gen_width_mode == GEN_WIDTH_CLOSEST_PREFER_UNDER) {
        if (*diff_width < 0)
            *diff_width = (3*abs(*diff_width))/2;
    } else if (gen_width_mode == GEN_WIDTH_CLOSEST_PREFER_OVER) {
        if (*diff_width > 0)
            *diff_width = (3*(*diff_width))/2;
        else
            *diff_width = abs(*diff_width);
    }
}

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz) {
    if ((vm_in->type & VIDEO_SDTV) ||
        (vm_in->type & VIDEO_EDTV))
    {
        return h_hz * 858;
    } else {
        return vm_in->timings.h_total * h_hz;
    }
}

uint32_t calculate_pclk(uint32_t src_clk_hz, mode_data_t *vm_out, vm_proc_config_t *vm_conf) {
    uint32_t pclk_hz;

    if (vm_conf->si_pclk_mult > 0) {
        pclk_hz = vm_conf->si_pclk_mult*src_clk_hz;
    } else if (vm_conf->si_pclk_mult < 0) {
        pclk_hz = src_clk_hz/((-1)*vm_conf->si_pclk_mult+1);
    } else {
        // Round to kHz but maximize accuracy without using 64-bit division
        pclk_hz = (((uint32_t)((((uint64_t)vm_out->timings.h_total*vm_out->timings.v_total*vm_out->timings.v_hz_x100)>>vm_out->timings.interlaced)/8)+6250)/12500)*1000;

        // Switch to integer mult if possible
        if (!vm_conf->framelock) {
            if ((pclk_hz >= src_clk_hz) && (pclk_hz % src_clk_hz == 0))
                vm_conf->si_pclk_mult = (pclk_hz/src_clk_hz);
            else if ((pclk_hz < src_clk_hz) && (src_clk_hz % pclk_hz == 0))
                vm_conf->si_pclk_mult = (-1)*((src_clk_hz/pclk_hz)-1);
        }
    }

    return pclk_hz;
}

oper_mode_t get_operating_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf) {
    oper_mode_t mode;

    if (cc->oper_mode == 1) {
#ifdef VIP
        mode = (get_scaler_mode(cc, vm_in, vm_out, vm_conf) == 0) ? OPERMODE_SCALER : OPERMODE_INVALID;
#else
        mode = OPERMODE_INVALID;
#endif
    } else if (cc->lm_mode == 0) {
        mode = (get_pure_lm_mode(cc, vm_in, vm_out, vm_conf) == 0) ? OPERMODE_PURE_LM : OPERMODE_INVALID;
    } else {
        mode = (get_adaptive_lm_mode(cc, vm_in, vm_out, vm_conf) == 0) ? OPERMODE_ADAPT_LM : OPERMODE_INVALID;
        if (mode == OPERMODE_INVALID)
            mode = (get_pure_lm_mode(cc, vm_in, vm_out, vm_conf) == 0) ? OPERMODE_PURE_LM : OPERMODE_INVALID;
    }

    return mode;
}

int get_sampling_preset(mode_data_t *vm_in, ad_mode_t ad_mode_list[], smp_mode_t target_sm_list[], int gen_width_target, gen_width_mode_t gen_width_mode, vm_proc_config_t *vm_conf)
{
    int i, diff_lines, diff_width, mindiff_id=0, mindiff_lines=1000, mindiff_width, v_active_ref;
    mode_data_t *mode_preset;
    smp_preset_t *smp_preset;
    aspect_ratio_t gen_ar_target = {4, 3};

    // Force fixed preset for digital sources
    if (vm_in->timings.h_total) {
        gen_width_mode = GEN_WIDTH_SMALLEST;

        for (i=1; i<=7; i++)
            target_sm_list[i] = SM_GEN_4_3;
    }

    // Go through sampling presets and find closest one
    for (i=0; i<sizeof(smp_presets)/sizeof(smp_preset_t); i++) {
        smp_preset = &smp_presets[i];

        // Calculate generic width target for A-LM modes
        if (!vm_in->timings.h_total && ad_mode_list && (smp_preset->group != GROUP_NONE) && (smp_preset->sm <= SM_GEN_16_9)) {
            mode_preset = (mode_data_t*)&video_modes[ad_mode_list[smp_preset->group].stdmode_id];
            v_active_ref = (ad_mode_list[smp_preset->group].y_rpt < 0) ? ((smp_preset->timings_i.v_active*(mode_preset->timings.interlaced+1)) / (-1*ad_mode_list[smp_preset->group].y_rpt+1)) :
                                                                         smp_preset->timings_i.v_active*(mode_preset->timings.interlaced+1) * (ad_mode_list[smp_preset->group].y_rpt+1);
            gen_ar_target.h = (target_sm_list[smp_preset->group] == SM_GEN_16_9) ? 16 : 4;
            gen_ar_target.v = (target_sm_list[smp_preset->group] == SM_GEN_16_9) ? 9 : 3;

            // BRAM linebuffer is only <= 2048 samples, thus halve sampling rate and pixel-repeat if necessary
            if (mode_preset->flags & MODE_CRT) {
                gen_width_target = (mode_preset->timings.h_active > 2048) ? mode_preset->timings.h_active/2 : mode_preset->timings.h_active;
            } else {
                gen_width_target = ((gen_ar_target.h*v_active_ref)/gen_ar_target.v)+1;

                if (gen_width_target >= 2100)
                    gen_width_target /= 2;
            }
        }

        if ((vm_in->timings.interlaced == smp_preset->timings_i.interlaced) &&
            (!smp_preset->timings_i.v_hz_x100 || (vm_in->timings.v_hz_x100 <= smp_preset->timings_i.v_hz_x100)) &&
            ((target_sm_list[smp_preset->group] == smp_preset->sm) || ((target_sm_list[smp_preset->group] <= SM_GEN_16_9) && (smp_preset->sm <= SM_GEN_16_9))))
        {
            diff_lines = abs(vm_in->timings.v_total - smp_preset->timings_i.v_total);

            if (diff_lines < mindiff_lines) {
                mindiff_id = i;
                mindiff_lines = diff_lines;
                if (smp_preset->sm <= SM_GEN_16_9) {
                    mindiff_width = gen_width_target - smp_preset->timings_i.h_active;
                    adjust_gen_width_diff(gen_width_mode, &mindiff_width);
                }
            } else if ((diff_lines == mindiff_lines) && (gen_width_mode != GEN_WIDTH_SMALLEST)) {
                // Find closest matching generic sampling width
                if (smp_preset->sm <= SM_GEN_16_9) {
                    diff_width = gen_width_target - smp_preset->timings_i.h_active;
                    adjust_gen_width_diff(gen_width_mode, &diff_width);

                    if ((gen_width_mode == GEN_WIDTH_LARGEST) || (diff_width < mindiff_width)) {
                        mindiff_id = i;
                        mindiff_width = diff_width;
                    }
                }
            } else if ((mindiff_lines < 10) && (diff_lines > mindiff_lines)) {
                // Break out if suitable mode already found
                break;
            }
        }
    }

    if (mindiff_lines >= 110)
        return -1;

    smp_preset = &smp_presets[mindiff_id];
    smp_cur = mindiff_id;
    smp_sel = mindiff_id;

    vm_in->group = smp_preset->group;
    if (smp_preset->sm <= SM_GEN_16_9)
        vm_in->ar = gen_ar_target;
    else
        vm_in->ar = smp_preset->ar;

    // write vm_in timings for digital sources
    if (vm_in->timings.h_total) {
        if ((hdmi_timings[vm_in->group].h_total != vm_in->timings.h_total) || (hdmi_timings[vm_in->group].v_total != vm_in->timings.v_total))
            memcpy(&hdmi_timings[vm_in->group], &vm_in->timings, sizeof(sync_timings_t));
        else
            memcpy(&vm_in->timings, &hdmi_timings[vm_in->group], sizeof(sync_timings_t));

        dtmg_cur = vm_in->group;
        smp_sel = vm_in->group;

        return 0;
    }

    vm_in->timings.h_active = smp_preset->timings_i.h_active;
    vm_in->timings.v_active = smp_preset->timings_i.v_active;
    vm_in->timings.h_synclen = smp_preset->timings_i.h_synclen;
    vm_in->timings.v_synclen = smp_preset->timings_i.v_synclen;
    vm_in->timings.h_backporch = smp_preset->timings_i.h_backporch;
    vm_in->timings.v_backporch = smp_preset->timings_i.v_backporch;
    vm_in->timings.h_total = smp_preset->timings_i.h_total;
    vm_in->timings.h_total_adj = smp_preset->timings_i.h_total_adj;
    vm_in->sampler_phase = smp_preset->sampler_phase;
    vm_in->type = smp_preset->type;
    strncpy(vm_in->name, smp_preset->name, 16);

    // adjust sampling preset by reducing active height if needed
    diff_lines = vm_in->timings.v_total-(vm_in->timings.v_synclen+vm_in->timings.v_backporch+vm_in->timings.v_active);
    if (diff_lines < 0)
        vm_in->timings.v_active += diff_lines;

    // repeat for width
    diff_width = vm_in->timings.h_total-(vm_in->timings.h_synclen+vm_in->timings.h_backporch+vm_in->timings.h_active);
    if (diff_width < 0)
        vm_in->timings.h_active += diff_width;

    vm_conf->h_skip = smp_preset->h_skip;

    return 0;
}

#ifdef VIP
int get_scaler_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    int i, mode_hz_index, gen_width_target=0, allow_x_inc, allow_y_inc, int_scl_x=1, int_scl_y=1, src_crop;
    int32_t error_cur, error_x_inc, error_y_inc;
    mode_data_t *mode_preset = NULL;
    gen_width_mode_t gen_width_mode = (cc->scl_alg == 2) ? GEN_WIDTH_CLOSEST_PREFER_OVER : GEN_WIDTH_CLOSEST_PREFER_UNDER;
    memset(vm_out, 0, sizeof(mode_data_t));
    memset(vm_conf, 0, sizeof(vm_proc_config_t));

    // {50Hz, 60Hz, 100Hz, 120Hz} id array for each output resolution
    const stdmode_t pm_scl_map_dfp[][4] = {{-1, STDMODE_480p, -1, -1},
                                         {-1, STDMODE_480p_WS, -1, -1},
                                         {STDMODE_576p, -1, -1, -1},
                                         {STDMODE_576p_WS, -1, -1, -1},
                                         {STDMODE_720p_50, STDMODE_720p_60, STDMODE_720p_100, STDMODE_720p_120},
                                         {-1, STDMODE_1280x1024_60, -1, -1},
                                         {STDMODE_1080i_50, STDMODE_1080i_60, -1, -1},
                                         {STDMODE_1080p_50, STDMODE_1080p_60, STDMODE_1080p_100, STDMODE_1080p_120},
                                         {-1, STDMODE_1600x1200_60, -1, -1},
                                         {STDMODE_1920x1200_50, STDMODE_1920x1200_60, -1, -1},
                                         {STDMODE_1920x1440_50, STDMODE_1920x1440_60, -1, -1},
                                         {STDMODE_2560x1440_50, STDMODE_2560x1440_60, -1, -1}};

    const stdmode_t pm_scl_map_crt[][4] = {{-1, STDMODE_240p_CRT, STDMODE_240p_CRT, STDMODE_240p_CRT},
                                         {STDMODE_288p_CRT, STDMODE_288p_CRT, STDMODE_288p_CRT, -1},
                                         {-1, STDMODE_480i_CRT, -1, -1},
                                         {-1, STDMODE_480i_WS_CRT, -1, -1},
                                         {STDMODE_576i_CRT, -1, -1, -1},
                                         {STDMODE_576i_WS_CRT, -1, -1, -1},
                                         {-1, STDMODE_480p_60_CRT, STDMODE_480p_100_CRT, STDMODE_480p_120_CRT},
                                         {STDMODE_540p_50_CRT, STDMODE_540p_60_CRT, -1, -1}};

    const stdmode_t *pm_scl_map = cc->scl_out_type ? pm_scl_map_crt[cc->scl_crt_out_mode] : pm_scl_map_dfp[cc->scl_out_mode];

    const smp_mode_t sm_240p_288p_map[] = {SM_GEN_4_3,
                                          SM_OPT_SNES_256COL, SM_OPT_SNES_512COL,
                                          SM_OPT_MD_256COL, SM_OPT_MD_320COL,
                                          SM_OPT_PSX_256COL, SM_OPT_PSX_320COL, SM_OPT_PSX_384COL, SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                          SM_OPT_SAT_320COL, SM_OPT_SAT_352COL, SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                          SM_OPT_N64_320COL, SM_OPT_N64_640COL,
                                          SM_OPT_NG_320COL};
    const smp_mode_t sm_384p_map[] = {SM_GEN_4_3, SM_OPT_VGA_640x350, SM_OPT_VGA_720x350, SM_OPT_VGA_640x400, SM_OPT_VGA_720x400, SM_OPT_GBI_240COL, SM_OPT_PC98_640COL};
    const smp_mode_t sm_480i_map[] = {SM_GEN_4_3, SM_OPT_DTV480I,
                                     SM_OPT_SNES_512COL,
                                     SM_OPT_MD_320COL,
                                     SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                     SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                     SM_OPT_N64_640COL,
                                     SM_OPT_DC_640COL};
    const smp_mode_t sm_576i_map[] = {SM_GEN_4_3, SM_OPT_DTV576I,
                                     SM_OPT_SNES_512COL,
                                     SM_OPT_MD_320COL,
                                     SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                     SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                     SM_OPT_N64_640COL,
                                     SM_OPT_DC_640COL};
    const smp_mode_t sm_480p_map[] = {SM_GEN_4_3, SM_OPT_DTV480P, SM_OPT_VESA_640x480, SM_OPT_DC_640COL, SM_OPT_PS2_512COL};
    const smp_mode_t sm_576p_map[] = {SM_GEN_4_3, SM_OPT_DTV576P};

    unsigned aspect_map[][2] = {{4, 3},
                                {4, 3},
                                {16, 9},
                                {8, 7},
                                {0, 0},
                                {0, 0}};

    if (vm_in->timings.v_hz_x100 < 5500)
        mode_hz_index = 0;
    else if (vm_in->timings.v_hz_x100 < 7500)
        mode_hz_index = 1;
    else if (vm_in->timings.v_hz_x100 < 11000)
        mode_hz_index = 2;
    else
        mode_hz_index = 3;

    if ((cc->scl_framelock == SCL_FL_ON_2X) && (mode_hz_index < 2) && (pm_scl_map[mode_hz_index+2] != (stdmode_t)-1)) {
        mode_hz_index += 2;
        vm_conf->framelock = 2;
    } else if ((cc->scl_framelock <= SCL_FL_ON_2X) && (pm_scl_map[mode_hz_index] != (stdmode_t)-1)) {
        vm_conf->framelock = 1;
    } else if ((cc->scl_framelock >= SCL_FL_OFF_50HZ) && (pm_scl_map[cc->scl_framelock-SCL_FL_OFF_50HZ] != (stdmode_t)-1)) {
        mode_hz_index = cc->scl_framelock-SCL_FL_OFF_50HZ;
    }


    smp_mode_t target_sm_list[] = { SM_OPT_PC_HDTV,                         // GROUP_NONE
                                    sm_240p_288p_map[cc->sm_scl_240p_288p], // GROUP_240P
                                    sm_240p_288p_map[cc->sm_scl_240p_288p], // GROUP_288P
                                    sm_384p_map[cc->sm_scl_384p],           // GROUP_384P
                                    sm_480i_map[cc->sm_scl_480i_576i],      // GROUP_480I
                                    sm_576i_map[cc->sm_scl_480i_576i],      // GROUP_576I
                                    sm_480p_map[cc->sm_scl_480p],           // GROUP_480P
                                    sm_576p_map[cc->sm_scl_576p],           // GROUP_576P
                                    SM_OPT_PC_HDTV,                         // GROUP_720P
                                    SM_OPT_PC_HDTV};                        // GROUP_1080I


    // Use lowest available vertical frequency if preferred is not available
    if (pm_scl_map[mode_hz_index] == (stdmode_t)-1) {
        for (i=0; i<4; i++) {
            if (pm_scl_map[i] != (stdmode_t)-1) {
                mode_hz_index = i;
                break;
            }
        }
    }

    mode_preset = (mode_data_t*)&video_modes[pm_scl_map[mode_hz_index]];

    // Force largest generic width for CRT output modes
    if (mode_preset->flags & MODE_CRT)
        gen_width_mode = GEN_WIDTH_LARGEST;

    // Calculate target active width for generic modes
    if (cc->scl_aspect < 4)
        gen_width_target = (aspect_map[cc->scl_aspect][0]*mode_preset->timings.v_active<<mode_preset->timings.interlaced)/aspect_map[cc->scl_aspect][1];
    else if (cc->scl_aspect == 5)
        gen_width_target = mode_preset->timings.h_active;
    else
        gen_width_target = 0;

    // Get sampling preset for analog sources and group for digital sources
    if (get_sampling_preset(vm_in, NULL, target_sm_list, gen_width_target, gen_width_mode, vm_conf) != 0)
        return -1;

    memcpy(vm_out, mode_preset, sizeof(mode_data_t));

    if (vm_conf->framelock) {
        vm_out->timings.v_hz_x100 = vm_conf->framelock*vm_in->timings.v_hz_x100;

        // Integer mult / div if input and output H/V total match
        if ((vm_in->timings.v_total == vm_out->timings.v_total) && (vm_in->timings.h_total == vm_out->timings.h_total)) {
            if (vm_in->timings.interlaced && !vm_out->timings.interlaced) {
                vm_conf->si_pclk_mult = 2*vm_conf->framelock;
            } else if (!vm_in->timings.interlaced && vm_out->timings.interlaced) {
                vm_conf->si_pclk_mult = -1;
            } else {
                vm_conf->si_pclk_mult = vm_conf->framelock;
            }
        }
    } else {
        if (cc->scl_framelock == SCL_FL_OFF_CLOSEST)
            vm_out->timings.v_hz_x100 = vm_in->timings.v_hz_x100;
        else if (mode_hz_index == 0)
            vm_out->timings.v_hz_x100 = 5000;
        else if (mode_hz_index == 1)
            vm_out->timings.v_hz_x100 = 6000;
        else if (mode_hz_index == 2)
            vm_out->timings.v_hz_x100 = 10000;
        else
            vm_out->timings.v_hz_x100 = 12000;
    }

    // Fill in source AR values for Auto and 1:1 PAR
    aspect_map[0][0] = vm_in->ar.h;
    aspect_map[0][1] = vm_in->ar.v;
    aspect_map[4][0] = vm_in->timings.h_active;
    aspect_map[4][1] = vm_in->timings.v_active<<vm_in->timings.interlaced;
    aspect_map[5][0] = vm_out->ar.h;
    aspect_map[5][1] = vm_out->ar.v;

    // Return error if input H/V active is zero for whatever reason
    if (!vm_in->timings.h_active || !vm_in->timings.v_active)
        return -1;

    // Calculate size and position based on aspect ratio
    if ((cc->scl_alg == 1) || (cc->scl_alg == 2)) { // Integer
        // Integer scale incrementally in horizontal or vertical direction depending which results to more correct aspect ratio
        while (1) {
            error_cur = ((100*int_scl_x*vm_in->timings.h_active*vm_out->timings.v_active)/(aspect_map[cc->scl_aspect][0]*vm_out->ar.v)) -
                        ((100*int_scl_y*(vm_in->timings.v_active<<vm_in->timings.interlaced)*vm_out->timings.h_active)/(aspect_map[cc->scl_aspect][1]*vm_out->ar.h));
            error_x_inc = ((100*(int_scl_x+1)*vm_in->timings.h_active*vm_out->timings.v_active)/(aspect_map[cc->scl_aspect][0]*vm_out->ar.v)) -
                          ((100*int_scl_y*(vm_in->timings.v_active<<vm_in->timings.interlaced)*vm_out->timings.h_active)/(aspect_map[cc->scl_aspect][1]*vm_out->ar.h));
            error_y_inc = ((100*(int_scl_y+1)*(vm_in->timings.v_active<<vm_in->timings.interlaced)*vm_out->timings.h_active)/(aspect_map[cc->scl_aspect][1]*vm_out->ar.h)) -
                          ((100*int_scl_x*vm_in->timings.h_active*vm_out->timings.v_active)/(aspect_map[cc->scl_aspect][0]*vm_out->ar.v));

            // Up to 1/8 horizontally or vertically allowed to be cropped when overscanning enabled
            allow_x_inc = (cc->scl_alg == 2) ? (7*(int_scl_x+1)*vm_in->timings.h_active <= 8*vm_out->timings.h_active) : ((int_scl_x+1)*vm_in->timings.h_active <= vm_out->timings.h_active);
            allow_y_inc = (cc->scl_alg == 2) ? (7*(int_scl_y+1)*(vm_in->timings.v_active<<vm_in->timings.interlaced) <= 8*(vm_out->timings.v_active<<vm_out->timings.interlaced))
                                             : ((int_scl_y+1)*(vm_in->timings.v_active<<vm_in->timings.interlaced) <= vm_out->timings.v_active<<vm_out->timings.interlaced);

            if (!allow_x_inc && !allow_y_inc) {
                break;
            } else if (allow_x_inc && allow_y_inc) {
                if (abs(error_x_inc) < abs(error_y_inc))
                    int_scl_x++;
                else
                    int_scl_y++;
            } else if (allow_x_inc) {
                if (abs(error_x_inc) < abs(error_cur))
                    int_scl_x++;
                else
                    break;
            } else { // allow_y_inc
                if (abs(error_y_inc) < abs(error_cur))
                    int_scl_y++;
                else
                    break;
            }
        }

        vm_conf->x_size = int_scl_x*vm_in->timings.h_active;
        vm_conf->y_size = int_scl_y*(vm_in->timings.v_active<<vm_in->timings.interlaced);

        if (vm_conf->x_size > vm_out->timings.h_active) {
            error_cur = vm_conf->x_size - vm_out->timings.h_active;
            src_crop = (error_cur % int_scl_x) ? ((error_cur/int_scl_x)+1) : (error_cur/int_scl_x);
            vm_in->timings.h_active -= src_crop;
            vm_in->timings.h_backporch += src_crop/2;
        }
        if (vm_conf->y_size > vm_out->timings.v_active<<vm_out->timings.interlaced) {
            error_cur = vm_conf->y_size - (vm_out->timings.v_active<<vm_out->timings.interlaced);
            src_crop = (error_cur % int_scl_y) ? ((error_cur/int_scl_y)+1)>>vm_in->timings.interlaced : (error_cur/int_scl_y)>>vm_in->timings.interlaced;
            vm_in->timings.v_active -= src_crop;
            vm_in->timings.v_backporch += src_crop/2;
        }

        vm_conf->x_size = int_scl_x*vm_in->timings.h_active;
        vm_conf->y_size = (int_scl_y*(vm_in->timings.v_active<<vm_in->timings.interlaced))>>vm_out->timings.interlaced;

        vm_conf->x_offset = (vm_out->timings.h_active - vm_conf->x_size)/2;
        vm_conf->y_offset = (vm_out->timings.v_active - vm_conf->y_size)/2;

        vm_conf->y_rpt = int_scl_y - 1;
        vm_conf->x_rpt = int_scl_x - 1;

        printf("\nint_scl_x: %d int_scl_y: %d", int_scl_x, int_scl_y);
    } else { // Scale to aspect
        if (vm_out->ar.v*aspect_map[cc->scl_aspect][0] <= vm_out->ar.h*aspect_map[cc->scl_aspect][1]) {
            // Pillarbox
            vm_conf->y_size = vm_out->timings.v_active;
            vm_conf->x_size = (vm_out->timings.h_active*aspect_map[cc->scl_aspect][0]*vm_out->ar.v)/(vm_out->ar.h*aspect_map[cc->scl_aspect][1]);
            vm_conf->x_offset = (vm_out->timings.h_active - vm_conf->x_size)/2;
        } else {
            // Letterbox
            vm_conf->x_size = vm_out->timings.h_active;
            vm_conf->y_size = (vm_out->timings.v_active*vm_out->ar.h*aspect_map[cc->scl_aspect][1])/(aspect_map[cc->scl_aspect][0]*vm_out->ar.v);
            vm_conf->y_offset = (vm_out->timings.v_active - vm_conf->y_size)/2;
        }

        vm_conf->y_rpt = (vm_conf->y_size < vm_in->timings.v_active) ? 0 : (vm_conf->y_size / vm_in->timings.v_active) - 1;
        vm_conf->x_rpt = (vm_conf->x_size < vm_in->timings.h_active) ? 0 : (vm_conf->x_size / vm_in->timings.h_active) - 1;
    }

    printf("\nx_size: %u, y_size: %u\nx_offset: %d, y_offset: %d\n", vm_conf->x_size, vm_conf->y_size, vm_conf->x_offset, vm_conf->y_offset);

    return 0;
}
#endif

int get_adaptive_lm_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    int32_t v_linediff;
    int16_t x_offset_i=0, y_offset_i=0;
    uint32_t in_interlace_mult, out_interlace_mult, vtotal_ref;
    memset(vm_out, 0, sizeof(mode_data_t));
    memset(vm_conf, 0, sizeof(vm_proc_config_t));

    const ad_mode_t pm_ad_240p_map[] = {{STDMODE_240p_CRT, 0}, {STDMODE_480p, 1}, {STDMODE_720p_60, 2}, {STDMODE_1280x1024_60, 3}, {STDMODE_1080i_60, 1}, {STDMODE_1080p_60, 3}, {STDMODE_1080p_60, 4},
                                        {STDMODE_1600x1200_60, 4}, {STDMODE_1920x1200_60, 4}, {STDMODE_1920x1440_60, 5}, {STDMODE_2560x1440_60, 5}};
    const ad_mode_t pm_ad_288p_map[] = {{STDMODE_288p_CRT, 0}, {STDMODE_576p, 1}, {STDMODE_1080i_50, 1}, {STDMODE_1080p_50, 3},
                                        {STDMODE_1920x1200_50, 3}, {STDMODE_1920x1440_50, 4}, {STDMODE_2560x1440_50, 4}};
    const ad_mode_t pm_ad_384p_map[] = {{STDMODE_720p_60, 1}, {STDMODE_1024x768_60, 1}, {STDMODE_1080p_60, 2},
                                        {STDMODE_1600x1200_60, 2}, {STDMODE_1920x1200_60, 2}, {STDMODE_1920x1440_60, 3}, {STDMODE_2560x1440_60, 3}};
    const ad_mode_t pm_ad_480i_map[] = {{STDMODE_480i, 0}, {STDMODE_240p_CRT, 0}, {STDMODE_480p, 1}, {STDMODE_1280x1024_60, 3}, {STDMODE_1080i_60, 1}, {STDMODE_1080p_60, 3},
                                        {STDMODE_1920x1440_60, 5}, {STDMODE_2560x1440_60, 5}};
    const ad_mode_t pm_ad_576i_map[] = {{STDMODE_576i, 0}, {STDMODE_288p_CRT, 0}, {STDMODE_576p, 1}, {STDMODE_1080i_50, 1}, {STDMODE_1080p_50, 3}, {STDMODE_1920x1200_50, 3}};
    const ad_mode_t pm_ad_480p_map[] = {{STDMODE_480p, 0}, {STDMODE_240p_CRT, -1}, {STDMODE_1280x1024_60, 1}, {STDMODE_1080i_60, 0}, {STDMODE_1080p_60, 1},
                                        {STDMODE_1920x1440_60, 2}, {STDMODE_2560x1440_60, 2}};
    const ad_mode_t pm_ad_576p_map[] = {{STDMODE_576p, 0}, {STDMODE_288p_CRT, -1}, {STDMODE_1080i_50, 0}, {STDMODE_1080p_50, 1}, {STDMODE_1920x1200_50, 1}};
    const ad_mode_t pm_ad_720p_map[] = {{STDMODE_720p_50, 0}, {STDMODE_2560x1440_50, 1}};
    const ad_mode_t pm_ad_1080i_map[] = {{STDMODE_1080i_50, 0}, {STDMODE_1080p_50, 1}};


    const smp_mode_t sm_240p_288p_map[] = {SM_GEN_4_3,
                                          SM_OPT_SNES_256COL, SM_OPT_SNES_512COL,
                                          SM_OPT_MD_256COL, SM_OPT_MD_320COL,
                                          SM_OPT_PSX_256COL, SM_OPT_PSX_320COL, SM_OPT_PSX_384COL, SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                          SM_OPT_SAT_320COL, SM_OPT_SAT_352COL, SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                          SM_OPT_N64_320COL, SM_OPT_N64_640COL,
                                          SM_OPT_NG_320COL};
    const smp_mode_t sm_384p_map[] = {SM_GEN_4_3, SM_OPT_VGA_640x350, SM_OPT_VGA_720x350, SM_OPT_VGA_640x400, SM_OPT_VGA_720x400, SM_OPT_GBI_240COL, SM_OPT_PC98_640COL};
    const smp_mode_t sm_480i_map[] = {SM_GEN_4_3, SM_GEN_16_9, SM_OPT_DTV480I, SM_OPT_DTV480I_WS,
                                     SM_OPT_SNES_512COL,
                                     SM_OPT_MD_320COL,
                                     SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                     SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                     SM_OPT_N64_640COL,
                                     SM_OPT_DC_640COL};
    const smp_mode_t sm_576i_map[] = {SM_GEN_4_3, SM_GEN_16_9, SM_OPT_DTV576I, SM_OPT_DTV576I_WS,
                                     SM_OPT_SNES_512COL,
                                     SM_OPT_MD_320COL,
                                     SM_OPT_PSX_512COL, SM_OPT_PSX_640COL,
                                     SM_OPT_SAT_640COL, SM_OPT_SAT_704COL,
                                     SM_OPT_N64_640COL,
                                     SM_OPT_DC_640COL};
    const smp_mode_t sm_480p_map[] = {SM_GEN_4_3, SM_GEN_16_9, SM_OPT_DTV480P, SM_OPT_DTV480P_WS, SM_OPT_VESA_640x480, SM_OPT_DC_640COL, SM_OPT_PS2_512COL};
    const smp_mode_t sm_576p_map[] = {SM_GEN_4_3, SM_GEN_16_9, SM_OPT_DTV576P, SM_OPT_DTV576P_WS};

    ad_mode_t ad_mode_list[] = { {-1, 0},                                   // GROUP_NONE
                                 pm_ad_240p_map[cc->pm_ad_240p],            // GROUP_240P
                                 pm_ad_288p_map[cc->pm_ad_288p],            // GROUP_288P
                                 pm_ad_384p_map[cc->pm_ad_384p],            // GROUP_384P
                                 pm_ad_480i_map[cc->pm_ad_480i],            // GROUP_480I
                                 pm_ad_576i_map[cc->pm_ad_576i],            // GROUP_576I
                                 pm_ad_480p_map[cc->pm_ad_480p],            // GROUP_480P
                                 pm_ad_576p_map[cc->pm_ad_576p],            // GROUP_576P
                                 pm_ad_720p_map[cc->pm_ad_720p],            // GROUP_720P
                                 pm_ad_1080i_map[cc->pm_ad_1080i]};         // GROUP_1080I

    smp_mode_t target_sm_list[] = { SM_OPT_PC_HDTV,                         // GROUP_NONE
                                    sm_240p_288p_map[cc->sm_ad_240p_288p],  // GROUP_240P
                                    sm_240p_288p_map[cc->sm_ad_240p_288p],  // GROUP_288P
                                    sm_384p_map[cc->sm_ad_384p],            // GROUP_384P
                                    sm_480i_map[cc->sm_ad_480i_576i],       // GROUP_480I
                                    sm_576i_map[cc->sm_ad_480i_576i],       // GROUP_576I
                                    sm_480p_map[cc->sm_ad_480p],            // GROUP_480P
                                    sm_576p_map[cc->sm_ad_576p],            // GROUP_576P
                                    SM_OPT_PC_HDTV,                         // GROUP_720P
                                    SM_OPT_PC_HDTV};                        // GROUP_1080I


    // Get sampling preset for analog sources and group for digital sources
    if (get_sampling_preset(vm_in, ad_mode_list, target_sm_list, 0, GEN_WIDTH_CLOSEST_PREFER_UNDER, vm_conf) != 0)
        return -1;

    // Switch to 60Hz output preset if needed
    if ((vm_in->group >= GROUP_720P) && (vm_in->timings.v_hz_x100 > 5500))
        ad_mode_list[vm_in->group].stdmode_id++;

    // Copy default sampling preset timings to output mode if no group found
    if (vm_in->group == GROUP_NONE) {
        strncpy(vm_out->name, smp_presets_default[smp_cur].name, 16);
        memcpy(&vm_out->timings, &smp_presets_default[smp_cur].timings_i, sizeof(sync_timings_t));
        vm_out->ar = smp_presets_default[smp_cur].ar;
        vm_out->type = smp_presets_default[smp_cur].type;
        vm_out->group = GROUP_NONE;
    } else {
        memcpy(vm_out, &video_modes[ad_mode_list[vm_in->group].stdmode_id], sizeof(mode_data_t));
    }

    vm_out->timings.v_hz_x100 = vm_in->timings.v_hz_x100;

    // Integer mult / div if input and output H/V total match
    if ((vm_in->timings.v_total == vm_out->timings.v_total) && (vm_in->timings.h_total == vm_out->timings.h_total)) {
        if (vm_in->timings.interlaced && !vm_out->timings.interlaced) {
            vm_conf->si_pclk_mult = 2;
        } else if (!vm_in->timings.interlaced && vm_out->timings.interlaced) {
            vm_conf->si_pclk_mult = -1;
        } else {
            vm_conf->si_pclk_mult = 1;
        }
    }

    in_interlace_mult = vm_in->timings.interlaced ? 2 : 1;
    out_interlace_mult = vm_out->timings.interlaced ? 2 : 1;

    vm_conf->x_rpt = 0;
    vm_conf->y_rpt = ad_mode_list[vm_in->group].y_rpt;

    // Calculate x_rpt for optimal modes based on output mode, sampling preset and y_rpt
    switch (ad_mode_list[vm_in->group].stdmode_id) {
    case STDMODE_240p_CRT:
    case STDMODE_288p_CRT:
        vm_conf->x_rpt = (vm_out->timings.h_active / vm_in->timings.h_active) - 1;
        break;
    case STDMODE_480p:
    case STDMODE_576p:
        if (vm_in->timings.h_active <= 284)
            vm_conf->x_rpt = 2 - cc->ar_256col;
        else if (vm_in->timings.h_active <= 400)
            vm_conf->x_rpt = 1;
        break;
    case STDMODE_720p_60:
    case STDMODE_1024x768_60:
        if (vm_in->timings.h_active <= 300)
            vm_conf->x_rpt = 3 - cc->ar_256col;
        else if (vm_in->timings.h_active <= 400)
            vm_conf->x_rpt = 2;
        else if (vm_in->timings.h_active <= 720)
            vm_conf->x_rpt = 1;
        break;
    case STDMODE_1280x1024_60:
        if (vm_in->timings.h_active <= 284)
            vm_conf->x_rpt = 4 - cc->ar_256col;
        else if (vm_in->timings.h_active <= 366)
            vm_conf->x_rpt = 3;
        else if (vm_in->timings.h_active <= 460)
            vm_conf->x_rpt = 2;
        else if (vm_in->timings.h_active <= 720)
            vm_conf->x_rpt = 1;
        break;
    case STDMODE_1080i_50:
    case STDMODE_1080p_50:
    case STDMODE_1080i_60:
    case STDMODE_1080p_60:
    case STDMODE_1600x1200_60:
    case STDMODE_1920x1200_50:
    case STDMODE_1920x1200_60:
        if (vm_in->timings.v_active*(vm_conf->y_rpt+1) <= 1080) {
            if (vm_in->timings.h_active <= 284)
                vm_conf->x_rpt = 4 - cc->ar_256col;
            else if (vm_in->timings.h_active <= 366)
                vm_conf->x_rpt = 3;
            else if (vm_in->timings.h_active <= 510)
                vm_conf->x_rpt = 2;
            else if (vm_in->timings.h_active <= 850)
                vm_conf->x_rpt = 1;
        } else {
            if (vm_in->timings.h_active <= 290)
                vm_conf->x_rpt = 5 - cc->ar_256col;
            else if (vm_in->timings.h_active <= 355)
                vm_conf->x_rpt = 4;
            else if (vm_in->timings.h_active <= 460)
                vm_conf->x_rpt = 3;
            else if (vm_in->timings.h_active <= 640)
                vm_conf->x_rpt = 2;
            else if (vm_in->timings.h_active <= 1060)
                vm_conf->x_rpt = 1;
        }

        if ((target_sm_list[vm_in->group] == SM_OPT_DTV480I_WS) || (target_sm_list[vm_in->group] == SM_OPT_DTV480P_WS) || (target_sm_list[vm_in->group] == SM_OPT_DTV576I_WS) || (target_sm_list[vm_in->group] == SM_OPT_DTV576P_WS))
            vm_conf->x_rpt++;
        else if (target_sm_list[vm_in->group] == SM_OPT_GBI_240COL)
            vm_conf->x_rpt = 5;
        break;
    case STDMODE_1920x1440_50:
    case STDMODE_1920x1440_60:
    case STDMODE_2560x1440_50:
    case STDMODE_2560x1440_60:
        if (vm_in->timings.h_active <= 295)
            vm_conf->x_rpt = 6 - cc->ar_256col;
        else if (vm_in->timings.h_active <= 350)
            vm_conf->x_rpt = 5;
        else if (vm_in->timings.h_active <= 426)
            vm_conf->x_rpt = 4;
        else if (vm_in->timings.h_active <= 548)
            vm_conf->x_rpt = 3;
        else if (vm_in->timings.h_active <= 768)
            vm_conf->x_rpt = 2;
        else if (vm_in->timings.h_active <= 1280)
            vm_conf->x_rpt = 1;

        if ((target_sm_list[vm_in->group] == SM_OPT_DTV480I_WS) || (target_sm_list[vm_in->group] == SM_OPT_DTV480P_WS))
            vm_conf->x_rpt++;
        else if (target_sm_list[vm_in->group] == SM_OPT_GBI_240COL)
            vm_conf->x_rpt = 7;
        break;
    default:
        break;
    }

    vm_conf->x_offset = ((vm_out->timings.h_active-vm_in->timings.h_active*(vm_conf->x_rpt+1))/2) + x_offset_i;
    vm_conf->x_start_lb = (vm_conf->x_offset >= 0) ? 0 : (-vm_conf->x_offset / (vm_conf->x_rpt+1));
    vm_conf->x_size = vm_in->timings.h_active*(vm_conf->x_rpt+1);
    if (vm_conf->x_size >= 4096)
        vm_conf->x_size = 4095;
    if (vm_conf->y_rpt == -1) {
        vm_conf->y_start_lb = ((vm_in->timings.v_active - (vm_out->timings.v_active*2))/2) + y_offset_i*2;
        vm_conf->y_offset = -vm_conf->y_start_lb/2;
        vm_conf->y_size = vm_in->timings.v_active/2;
    } else {
        vm_conf->y_start_lb = ((vm_in->timings.v_active - (vm_out->timings.v_active/(vm_conf->y_rpt+1)))/2) + y_offset_i;
        vm_conf->y_offset = -(vm_conf->y_rpt+1)*vm_conf->y_start_lb;
        vm_conf->y_size = vm_in->timings.v_active*(vm_conf->y_rpt+1);
    }

    // calculate the time (in output lines, rounded up) from source frame start to the point where first to-be-visible line has been written into linebuf
    v_linediff = (((vm_in->timings.v_synclen + vm_in->timings.v_backporch + ((vm_conf->y_start_lb < 0) ? 0 : vm_conf->y_start_lb) + 1) * vm_out->timings.v_total * in_interlace_mult) / (vm_in->timings.v_total * out_interlace_mult)) + 1;

    // subtract the previous value from the total number of output blanking/empty lines. Resulting value indicates how many lines output framestart must be offset
    v_linediff = (vm_out->timings.v_synclen + vm_out->timings.v_backporch + ((vm_conf->y_offset < 0) ? 0 : vm_conf->y_offset)) - v_linediff;

    // if linebuf is read faster than written, output framestart must be delayed accordingly to avoid read pointer catching write pointer
    vtotal_ref = (vm_conf->y_rpt == -1) ? ((vm_in->timings.v_total*out_interlace_mult)/2) : (vm_in->timings.v_total*out_interlace_mult*(vm_conf->y_rpt+1));
    if (vm_out->timings.v_total * in_interlace_mult > vtotal_ref)
        v_linediff -= (((vm_in->timings.v_active * vm_out->timings.v_total * in_interlace_mult) / (vm_in->timings.v_total * out_interlace_mult)) - vm_conf->y_size);

#ifdef LM_EMIF_EXTRA_DELAY
    v_linediff -= (vm_conf->y_rpt+1);
#endif

    vm_conf->framesync_line = (v_linediff < 0) ? (vm_out->timings.v_total/out_interlace_mult)+v_linediff : v_linediff;
    vm_conf->framelock = 1;

    printf("framesync_line = %u\nx_start_lb: %d, x_offset: %d, x_size: %u\ny_start_lb: %d, y_offset: %d, y_size: %u\n", vm_conf->framesync_line, vm_conf->x_start_lb, vm_conf->x_offset, vm_conf->x_size, vm_conf->y_start_lb, vm_conf->y_offset, vm_conf->y_size);

    return 0;
}

int get_pure_lm_mode(avconfig_t *cc, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    int i;
    unsigned num_modes = sizeof(video_modes_plm)/sizeof(mode_data_t);
    mode_flags valid_lm[] = { MODE_PT, (MODE_L2 | (MODE_L2<<cc->l2_mode)), (MODE_L3_GEN_16_9<<cc->l3_mode), (MODE_L4_GEN_4_3<<cc->l4_mode), (MODE_L5_GEN_4_3<<cc->l5_mode) };
    mode_flags target_lm;
    uint8_t pt_only = 0;
    uint8_t nonsampled_h_mult = 0, nonsampled_v_mult = 0;
    uint8_t upsample2x = vm_in->timings.h_total ? 0 : cc->upsample2x;

    // one for each video_group
    uint8_t* group_ptr[] = { &pt_only, &cc->pm_240p, &cc->pm_240p, &cc->pm_384p, &cc->pm_480i, &cc->pm_480i, &cc->pm_480p, &cc->pm_480p, &pt_only, &cc->pm_1080i };

    for (i=0; i<num_modes; i++) {
        switch (video_modes_plm[i].group) {
            case GROUP_384P:
                //fixed Line2x/3x mode for 240x360p
                valid_lm[2] = MODE_L2_240x360;
                valid_lm[3] = MODE_L3_240x360;
                valid_lm[4] = MODE_L3_GEN_16_9;
                if ((!vm_in->timings.h_total) && (video_modes_plm[i].timings.v_total == 449)) {
                    if (!strncmp(video_modes_plm[i].name, "720x400_70", 10)) {
                        if (cc->s400p_mode == 0)
                            continue;
                    } else if (!strncmp(video_modes_plm[i].name, "640x400_70", 10)) {
                        if (cc->s400p_mode == 1)
                            continue;
                    }
                }
                break;
            case GROUP_480I:
            case GROUP_576I:
                //fixed Line3x/4x mode for 480i
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L4_GEN_4_3;
                break;
            case GROUP_480P:
                 if (video_modes_plm[i].vic == HDMI_480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                            if (vm_in->timings.h_synclen > 82)
                                continue;
                            break;
                        case 1: // DTV 480p
                            break;
                        default:
                            continue;
                    }
                } else if (video_modes_plm[i].vic == HDMI_640x480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                        case 2: // VESA 640x480@60
                            break;
                        default:
                            continue;
                    }
                }
                break;
            default:
                break;
        }

        // allow up to +10% difference in refresh rate
        if (vm_in->timings.v_hz_x100 > video_modes_plm[i].timings.v_hz_x100 + video_modes_plm[i].timings.v_hz_x100/10)
            continue;

        target_lm = valid_lm[*group_ptr[video_modes_plm[i].group]];

        // HDMI input modes
        if (vm_in->timings.h_total) {
            if (target_lm >= MODE_L5_GEN_4_3)
                nonsampled_v_mult = 5;
            else if (target_lm >= MODE_L4_GEN_4_3)
                nonsampled_v_mult = 4;
            else if (target_lm >= MODE_L3_GEN_16_9)
                nonsampled_v_mult = 3;
            else if (target_lm >= MODE_L2)
                nonsampled_v_mult = 2;
            else
                nonsampled_v_mult = 1;

            target_lm = MODE_PT;
        }

        if ((target_lm & video_modes_plm[i].flags) &&
            (vm_in->timings.interlaced == video_modes_plm[i].timings.interlaced) &&
            (vm_in->timings.v_total <= (video_modes_plm[i].timings.v_total+LINECNT_MAX_TOLERANCE)))
        {
            if (!vm_in->timings.h_active)
                vm_in->timings.h_active = video_modes_plm[i].timings.h_active;
            if (!vm_in->timings.v_active)
                vm_in->timings.v_active = video_modes_plm[i].timings.v_active;
            if ((!vm_in->timings.h_synclen) || (!vm_in->timings.h_backporch))
                vm_in->timings.h_synclen = video_modes_plm[i].timings.h_synclen;
            if (!vm_in->timings.v_synclen)
                vm_in->timings.v_synclen = video_modes_plm[i].timings.v_synclen;
            if (!vm_in->timings.h_backporch)
                vm_in->timings.h_backporch = video_modes_plm[i].timings.h_backporch;
            if (!vm_in->timings.v_backporch)
                vm_in->timings.v_backporch = video_modes_plm[i].timings.v_backporch;
            if (!vm_in->timings.h_total)
                vm_in->timings.h_total = video_modes_plm[i].timings.h_total;
            vm_in->timings.h_total_adj = video_modes_plm[i].timings.h_total_adj;
            vm_in->sampler_phase = video_modes_plm[i].sampler_phase;
            vm_in->type = video_modes_plm[i].type;
            vm_in->group = video_modes_plm[i].group;
            if (!vm_in->vic)
                vm_in->vic = video_modes_plm[i].vic;
            if (vm_in->name[0] == 0)
                strncpy(vm_in->name, video_modes_plm[i].name, 16);

            memcpy(vm_out, vm_in, sizeof(mode_data_t));
            vm_out->vic = HDMI_Unknown;
            vm_out->tx_pixelrep = TX_1X;
            vm_out->hdmitx_pixr_ifr = TX_1X;

            memset(vm_conf, 0, sizeof(vm_proc_config_t));
            vm_conf->si_pclk_mult = 1;

            target_lm &= video_modes_plm[i].flags;    //ensure L2 mode uniqueness

            if (nonsampled_v_mult) {
                if (nonsampled_v_mult > 1)
                    nonsampled_h_mult = (((((uint32_t)vm_in->timings.v_active*nonsampled_v_mult*40)/3)/vm_in->timings.h_active)+5)/10;

                vm_conf->x_rpt = (nonsampled_h_mult == 0) ? 0 : (nonsampled_h_mult-1);
                vm_conf->y_rpt = nonsampled_v_mult-1;

                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);

                if (((vm_out->timings.v_hz_x100/100)*vm_out->timings.v_total*vm_out->timings.h_total)>>vm_out->timings.interlaced < 25000000UL) {
                    vm_out->tx_pixelrep = TX_2X;
                    vm_out->hdmitx_pixr_ifr = TX_2X;
                }

                vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
            } else {
                switch (target_lm) {
                    case MODE_PT:
                        vm_out->vic = vm_in->vic;
                        // Upsample / pixel-repeat horizontal resolution if necessary to fulfill min. 25MHz TMDS clock requirement
                        if (((vm_out->timings.v_hz_x100/100)*vm_out->timings.v_total*vm_out->timings.h_total)>>vm_out->timings.interlaced < 25000000UL) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vm_out->tx_pixelrep = TX_2X;
                            }
                            vm_out->hdmitx_pixr_ifr = TX_2X;
                        }
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2:
                        vm_conf->y_rpt = 1;

                        // Upsample / pixel-repeat horizontal resolution of 384p/480p/960i modes
                        if ((video_modes_plm[i].group == GROUP_384P) || (video_modes_plm[i].group == GROUP_480P) || (video_modes_plm[i].group == GROUP_576P) || ((video_modes_plm[i].group == GROUP_1080I) && (video_modes_plm[i].timings.h_total < 1200))) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_512_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_384_COL:
                    case MODE_L2_320_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_256_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        vm_conf->x_rpt -= cc->ar_256col;
                        break;
                    case MODE_L2_240x360:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_GEN_16_9:
                        vm_conf->y_rpt = 2;

                        // Upsample / pixel-repeat horizontal resolution of 480i mode
                        if ((video_modes_plm[i].group == GROUP_480I) || (video_modes_plm[i].group == GROUP_576I)) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_GEN_4_3:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_size = vm_out->timings.h_active;
                        vm_out->timings.h_synclen /= 3;
                        vm_out->timings.h_backporch /= 3;
                        vm_out->timings.h_active /= 3;
                        vm_conf->x_offset = vm_out->timings.h_active/2;
                        vm_out->timings.h_total /= 3;
                        vm_out->timings.h_total_adj = 0;
                        vmode_hv_mult(vm_out, 4, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = 4;
                        break;
                    case MODE_L3_512_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_384_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_320_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        vm_conf->x_rpt = 2;
                        break;
                    case MODE_L3_256_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        vm_conf->x_rpt = cc->ar_256col ? 2 : 3;
                        break;
                    case MODE_L3_240x360:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = vm_conf->h_skip = 6;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 13;
                        break;
                    case MODE_L4_GEN_4_3:
                        vm_conf->y_rpt = 3;

                        // Upsample / pixel-repeat horizontal resolution of 480i mode
                        if ((video_modes_plm[i].group == GROUP_480I) || (video_modes_plm[i].group == GROUP_576I)) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_512_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_384_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_320_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_256_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        vm_conf->x_rpt -= cc->ar_256col;
                        break;
                    case MODE_L5_GEN_4_3:
                        vm_conf->y_rpt = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L5_512_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 40;
                        break;
                    case MODE_L5_384_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 30;
                        break;
                    case MODE_L5_320_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 24;
                        break;
                    case MODE_L5_256_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = vm_conf->h_skip = 5;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_conf->si_pclk_mult = VM_OUT_PCLKMULT;
                        vm_conf->x_rpt -= cc->ar_256col;
                        //cm.hsync_cut = 20;
                        break;
                    default:
                        printf("WARNING: invalid target_lm\n");
                        continue;
                        break;
                }
            }

            sniprintf(vm_out->name, 16, "%s x%u", vm_in->name, vm_conf->y_rpt+1);

            if (vm_conf->x_size == 0)
                vm_conf->x_size = vm_in->timings.h_active*(vm_conf->x_rpt+1);
            if (vm_conf->y_size == 0)
                vm_conf->y_size = vm_out->timings.v_active;

            vm_conf->x_offset = ((vm_out->timings.h_active-vm_conf->x_size)/2);
            vm_conf->x_start_lb = (vm_conf->x_offset >= 0) ? 0 : (-vm_conf->x_offset / (vm_conf->x_rpt+1));

            // Line5x format
            if (vm_conf->y_rpt == 4) {
                // adjust output width to 1920
                if ((cc->l5_fmt != 1) && (nonsampled_h_mult == 0)) {
                    vm_conf->x_offset = (1920-vm_conf->x_size)/2;
                    vm_out->timings.h_synclen = (vm_out->timings.h_total - 1920)/4;
                    vm_out->timings.h_backporch = (vm_out->timings.h_total - 1920)/2;
                    vm_out->timings.h_active = 1920;
                }

                // adjust output height to 1080
                if (cc->l5_fmt == 0) {
                    vm_conf->y_start_lb = (vm_out->timings.v_active-1080)/10;
                    vm_out->timings.v_backporch += 5*vm_conf->y_start_lb;
                    vm_out->timings.v_active = 1080;
                }
            }

#ifdef LM_EMIF_EXTRA_DELAY
            vm_conf->framesync_line = ((vm_out->timings.v_total>>vm_out->timings.interlaced)-(1+vm_out->timings.interlaced)*(vm_conf->y_rpt+1));
#else
            vm_conf->framesync_line = vm_in->timings.interlaced ? ((vm_out->timings.v_total>>vm_out->timings.interlaced)-(vm_conf->y_rpt+1)) : 0;
#endif
            vm_conf->framelock = 1;

            vm_cur = i;
            vm_sel = i;

            return 0;
        }
    }

    return -1;
}

int get_standard_mode(stdmode_t stdmode_id, mode_data_t *vm_in, mode_data_t *vm_out, vm_proc_config_t *vm_conf)
{
    memset(vm_conf, 0, sizeof(vm_proc_config_t));
    memset(vm_in, 0, sizeof(mode_data_t));
    memcpy(vm_out, &video_modes[stdmode_id], sizeof(mode_data_t));

    return 0;
}
