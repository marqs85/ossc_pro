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

#include <string.h>
#include "system.h"
#include "avconfig.h"
#include "av_controller.h"
#include "video_modes.h"
#include "userdata.h"

#define DEFAULT_ON              1

// Current and target configuration
avconfig_t cc, tc;

uint8_t update_cur_vm;

uint8_t profile_sel, profile_sel_menu, sd_profile_sel_menu;

// Default configuration
const avconfig_t tc_default = {
    .l3_mode = 1,
    .pm_240p = 1,
    .pm_384p = 1,
    .pm_480i = 1,
    .pm_1080i = 1,
    .pm_ad_240p = 2,
    .pm_ad_288p = 3,
    .pm_ad_480i = 5,
    .pm_ad_576i = 4,
    .pm_ad_480p = 4,
    .pm_ad_576p = 0,
    .pm_ad_1080i = 1,
    .sl_altern = 1,
    .lm_mode = 1,
    .tp_mode = STDMODE_480p,
    .mask_br = 8,
    .bfi_str = 15,
#ifdef VIP
    .scl_out_mode = 4,
    .scl_edge_thold = 7,
    .scl_dil_motion_shift = 3,
#ifndef VIP_DIL_B
    .scl_dil_alg = 2,
#else
    .scl_dil_motion_scale = 125,
    .scl_dil_cadence_detect_enable = 0,
    .scl_dil_visualize_motion = 0,
#endif
    .sm_scl_480p = 1,
#endif
#ifndef DExx_FW
    .audio_src_map = {AUD_AV1_ANALOG, AUD_AV2_ANALOG, AUD_AV3_ANALOG, AUD_AV4_DIGITAL},
#else
    .audio_src_map = {AUD_AV1_ANALOG, 0, 0, 0},
#endif
};

const HDMI_i2s_fs_t audio_fmt_iec_map[] = {IEC60958_FS_48KHZ, IEC60958_FS_96KHZ, IEC60958_FS_192KHZ};

avconfig_t* get_current_avconfig() {
    return &cc;
}

status_t update_avconfig() {
    status_t status = 0;

    if ((tc.mask_br != cc.mask_br) ||
        (tc.mask_color != cc.mask_color) ||
        (tc.reverse_lpf != cc.reverse_lpf) ||
        (tc.lm_deint_mode != cc.lm_deint_mode) ||
        (tc.nir_even_offset != cc.nir_even_offset) ||
        (tc.ypbpr_cs != cc.ypbpr_cs) ||
        (tc.bfi_enable != cc.bfi_enable) ||
        (tc.bfi_str != cc.bfi_str) ||
        (tc.sl_mode != cc.sl_mode) ||
        (tc.sl_type != cc.sl_type) ||
        (tc.sl_method != cc.sl_method) ||
        (tc.sl_str != cc.sl_str) ||
        (tc.sl_id != cc.sl_id) ||
        (tc.sl_altern != cc.sl_altern) ||
        (tc.sl_cust_iv_x != cc.sl_cust_iv_x) ||
        (tc.sl_cust_iv_y != cc.sl_cust_iv_y) ||
        (memcmp(tc.sl_cust_l_str, cc.sl_cust_l_str, 6*sizeof(uint8_t))) ||
        (memcmp(tc.sl_cust_c_str, cc.sl_cust_c_str, 10*sizeof(uint8_t)))
#ifdef VIP
        || (tc.scl_edge_thold != cc.scl_edge_thold) ||
        (tc.scl_dil_motion_shift != cc.scl_dil_motion_shift)
#ifndef VIP_DIL_B
        || (tc.scl_dil_alg != cc.scl_dil_alg)
#else
        || (tc.scl_dil_motion_scale != cc.scl_dil_motion_scale) ||
        (tc.scl_dil_cadence_detect_enable != cc.scl_dil_cadence_detect_enable) ||
        (tc.scl_dil_visualize_motion != cc.scl_dil_visualize_motion)
#endif
#endif
       )
        status |= SC_CONFIG_CHANGE;

    if (tc.tp_mode != cc.tp_mode)
        status |= TP_MODE_CHANGE;

    if ((tc.pm_240p != cc.pm_240p) ||
        (tc.pm_384p != cc.pm_384p) ||
        (tc.pm_480i != cc.pm_480i) ||
        (tc.pm_480p != cc.pm_480p) ||
        (tc.pm_1080i != cc.pm_1080i) ||
        (tc.l2_mode != cc.l2_mode) ||
        (tc.l3_mode != cc.l3_mode) ||
        (tc.l4_mode != cc.l4_mode) ||
        (tc.l5_mode != cc.l5_mode) ||
        (tc.l5_fmt != cc.l5_fmt) ||
        (tc.pm_ad_240p != cc.pm_ad_240p) ||
        (tc.pm_ad_288p != cc.pm_ad_288p) ||
        (tc.pm_ad_384p != cc.pm_ad_384p) ||
        (tc.pm_ad_480i != cc.pm_ad_480i) ||
        (tc.pm_ad_576i != cc.pm_ad_576i) ||
        (tc.pm_ad_480p != cc.pm_ad_480p) ||
        (tc.pm_ad_576p != cc.pm_ad_576p) ||
        (tc.pm_ad_720p != cc.pm_ad_720p) ||
        (tc.pm_ad_1080i != cc.pm_ad_1080i) ||
        (tc.sm_ad_240p_288p != cc.sm_ad_240p_288p) ||
        (tc.sm_ad_384p != cc.sm_ad_384p) ||
        (tc.sm_ad_480i_576i != cc.sm_ad_480i_576i) ||
        (tc.sm_ad_480p != cc.sm_ad_480p) ||
        (tc.sm_ad_576p != cc.sm_ad_576p) ||
        (tc.lm_mode != cc.lm_mode) ||
        (tc.oper_mode != cc.oper_mode) ||
        (tc.upsample2x != cc.upsample2x) ||
        update_cur_vm
#ifdef VIP
        || (tc.scl_alg != cc.scl_alg) ||
        (tc.scl_out_mode != cc.scl_out_mode) ||
        (tc.scl_framelock != cc.scl_framelock) ||
        (tc.scl_aspect != cc.scl_aspect) ||
        (tc.sm_scl_240p_288p != cc.sm_scl_240p_288p) ||
        (tc.sm_scl_384p != cc.sm_scl_384p) ||
        (tc.sm_scl_480i_576i != cc.sm_scl_480i_576i) ||
        (tc.sm_scl_480p != cc.sm_scl_480p) ||
        (tc.sm_scl_576p != cc.sm_scl_576p)
#endif
        )
        status |= MODE_CHANGE;

#ifndef DExx_FW
    if (tc.audmux_sel != cc.audmux_sel)
        switch_audmux(tc.audmux_sel);
#endif
#ifdef INC_THS7353
    if (tc.syncmux_stc != cc.syncmux_stc)
        set_syncmux_biasmode(tc.syncmux_stc);
#endif
    if (memcmp(tc.audio_src_map, cc.audio_src_map, 4*sizeof(audinput_t)))
        switch_audsrc(tc.audio_src_map, &tc.hdmitx_cfg.audio_fmt);

    memcpy(&cc, &tc, sizeof(avconfig_t));
    update_cur_vm = 0;

#ifdef INC_PCM186X
    cc.pcm_cfg.fs = cc.audio_fmt;
#endif
    cc.hdmitx_cfg.i2s_fs = audio_fmt_iec_map[cc.audio_fmt];
    cc.hdmitx_cfg.audio_cc_val = CC_2CH;
    cc.hdmitx_cfg.audio_ca_val = CA_2p0;

    return status;
}

int set_default_profile(int update_cc)
{
    memcpy(&tc, &tc_default, sizeof(avconfig_t));
    isl_get_default_cfg(&tc.isl_cfg);
#ifdef INC_ADV7513
    adv7513_get_default_cfg(&tc.hdmitx_cfg);
#endif
#ifdef INC_SII1136
    sii1136_get_default_cfg(&tc.hdmitx_cfg);
#endif
#ifdef INC_PCM186X
    pcm186x_get_default_cfg(&tc.pcm_cfg);
#endif

#ifdef DExx_FW
    tc.hdmitx_cfg.i2s_fs = IEC60958_FS_96KHZ;
#endif

    if (update_cc)
        memcpy(&cc, &tc, sizeof(avconfig_t));

    set_default_vm_table();

    return 0;
}

int reset_profile() {
    set_default_profile(0);

    return 0;
}

int load_profile() {
    int retval;

    retval = read_userdata(profile_sel_menu, 0);
    if (retval == 0) {
        profile_sel = profile_sel_menu;

        // Change the input if the new profile demands it.
        /*if (tc.link_av != AV_LAST)
            target_input = tc.link_av;

        // Update profile link (also prevents the change of input from inducing a profile load).
        input_profiles[profile_link ? target_input : AV_TESTPAT] = profile_sel;
        write_userdata(INIT_CONFIG_SLOT);*/
    }

    return retval;
}

int save_profile() {
    int retval;

    retval = write_userdata(profile_sel_menu);
    if (retval == 0) {
        profile_sel = profile_sel_menu;

        //input_profiles[profile_link ? cm.avinput : AV_TESTPAT] = profile_sel;
        write_userdata(INIT_CONFIG_SLOT);
    }

    return retval;
}

int load_profile_sd() {
    return read_userdata_sd(sd_profile_sel_menu, 0);
}

int save_profile_sd() {
    int retval;

    retval = write_userdata_sd(sd_profile_sel_menu);
    if (retval == 0)
        write_userdata_sd(SD_INIT_CONFIG_SLOT);

    return retval;
}
