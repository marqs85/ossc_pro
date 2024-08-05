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
    .pm_ad_480p = 5,
    .pm_ad_576p = 4,
    .pm_ad_1080i = 1,
    .sl_altern = 1,
    .lm_mode = 1,
    .tp_mode = STDMODE_480p,
    .mask_br = 8,
    .bfi_str = 15,
    .shmask_str = 15,
#ifdef VIP
    .scl_out_mode = 7,
    .scl_edge_thold = 7,
    .scl_dil_motion_shift = 3,
#ifndef VIP_DIL_B
    .scl_dil_alg = 2,
#else
    .scl_dil_motion_scale = 125,
    .scl_dil_cadence_detect_enable = 0,
    .scl_dil_visualize_motion = 0,
#endif
#endif
#ifdef INC_THS7353
    .syncmux_stc = 1,
#endif
#ifndef DExx_FW
    .audio_src_map = {AUD_AV1_ANALOG, AUD_AV2_ANALOG, AUD_AV3_ANALOG, AUD_AV4_DIGITAL},
#else
    .audio_src_map = {AUD_AV1_ANALOG, 0, 0, 0},
#endif
    .extra_av_out_mode = 1,
};

const HDMI_i2s_fs_t audio_fmt_iec_map[] = {IEC60958_FS_48KHZ, IEC60958_FS_96KHZ, IEC60958_FS_192KHZ};

avconfig_t* get_current_avconfig() {
    return &cc;
}

avconfig_t* get_target_avconfig() {
    return &tc;
}

status_t update_avconfig() {
    status_t status = 0;

    if (memcmp(&tc, &cc, offsetof(avconfig_t, sl_mode)) || (update_cur_vm == 1))
        status |= MODE_CHANGE;

    if (memcmp(&tc.sl_mode, &cc.sl_mode, offsetof(avconfig_t, tp_mode) - offsetof(avconfig_t, sl_mode)))
        status |= SC_CONFIG_CHANGE;

    if ((tc.tp_mode != cc.tp_mode) || (update_cur_vm == 1))
        status |= TP_MODE_CHANGE;

#ifndef DExx_FW
    if (tc.audmux_sel != cc.audmux_sel)
        switch_audmux(tc.audmux_sel);
    if ((tc.exp_sel != cc.exp_sel) || (tc.extra_av_out_mode != cc.extra_av_out_mode))
        switch_expansion(tc.exp_sel, tc.extra_av_out_mode);
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
#else
    adv7280a_get_default_cfg(&tc.sdp_cfg);
#endif

    if (update_cc)
        memcpy(&cc, &tc, sizeof(avconfig_t));

    set_default_c_pp_coeffs();
    set_default_c_shmask();
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
