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

#define DEFAULT_ON              1

// Current and target configuration
avconfig_t cc, tc;

// Default configuration
const avconfig_t tc_default = {
    .l3_mode = 1,
    .pm_240p = 1,
    .pm_384p = 1,
    .pm_480i = 1,
    .pm_1080i = 1,
    .pm_ad_240p = 2,
    .pm_ad_288p = 3,
    .pm_ad_480i = 4,
    .pm_ad_576i = 3,
    .pm_ad_480p = 4,
    .pm_ad_576p = 0,
    .sl_altern = 1,
    .oper_mode = 1,
    .scl_out_mode = 4,
    .scl_alg = 1,
    .scl_dil_alg = 2,
#ifdef VIP
    .scl_dil_motion_shift = 3,
#endif
    .sm_scl_480p = 1,
#ifndef DExx_FW
    .audio_src_map = {AUD_AV1_ANALOG, AUD_AV2_ANALOG, AUD_AV3_ANALOG, AUD_AV4_DIGITAL},
#else
    .audio_src_map = {AUD_AV1_ANALOG, 0, 0, 0},
#endif
};

const HDMI_i2s_fs_t audio_fmt_iec_map[] = {IEC60958_FS_48KHZ, IEC60958_FS_96KHZ, IEC60958_FS_192KHZ};

int reset_target_avconfig() {
    set_default_avconfig(0);

    return 0;
}

avconfig_t* get_current_avconfig() {
    return &cc;
}

int set_default_avconfig(int update_cc)
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

status_t update_avconfig() {
    status_t status = NO_CHANGE;

    if ((tc.mask_br != cc.mask_br) ||
        (tc.mask_color != cc.mask_color) ||
        (tc.reverse_lpf != cc.reverse_lpf) ||
        (tc.lm_deint_mode != cc.lm_deint_mode) ||
        (tc.nir_even_offset != cc.nir_even_offset) ||
        (tc.ypbpr_cs != cc.ypbpr_cs) ||
        (tc.scl_alg != cc.scl_alg) ||
        (tc.scl_dil_alg != cc.scl_dil_alg)
#ifdef VIP
        || (tc.scl_dil_motion_shift != cc.scl_dil_motion_shift)
#endif
       )
        status = (status < SC_CONFIG_CHANGE) ? SC_CONFIG_CHANGE : status;

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
        (tc.pm_ad_480i != cc.pm_ad_480i) ||
        (tc.pm_ad_576i != cc.pm_ad_576i) ||
        (tc.pm_ad_480p != cc.pm_ad_480p) ||
        (tc.pm_ad_576p != cc.pm_ad_576p) ||
        (tc.sm_ad_240p_288p != cc.sm_ad_240p_288p) ||
        (tc.sm_ad_480i_576i != cc.sm_ad_480i_576i) ||
        (tc.sm_ad_480p != cc.sm_ad_480p) ||
        (tc.sm_ad_576p != cc.sm_ad_576p) ||
        (tc.oper_mode != cc.oper_mode) ||
        (tc.upsample2x != cc.upsample2x) ||
        (tc.default_vic != cc.default_vic) ||
        (tc.scl_out_mode != cc.scl_out_mode) ||
        (tc.scl_framelock != cc.scl_framelock) ||
        (tc.scl_aspect != cc.scl_aspect) ||
        (tc.sm_scl_240p_288p != cc.sm_scl_240p_288p) ||
        (tc.sm_scl_480i_576i != cc.sm_scl_480i_576i) ||
        (tc.sm_scl_480p != cc.sm_scl_480p) ||
        (tc.sm_scl_576p != cc.sm_scl_576p))
        status = (status < MODE_CHANGE) ? MODE_CHANGE : status;

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

#ifdef INC_PCM186X
    cc.pcm_cfg.fs = cc.audio_fmt;
#endif
    cc.hdmitx_cfg.i2s_fs = audio_fmt_iec_map[cc.audio_fmt];
    cc.hdmitx_cfg.audio_cc_val = CC_2CH;
    cc.hdmitx_cfg.audio_ca_val = CA_2p0;

    return status;
}
