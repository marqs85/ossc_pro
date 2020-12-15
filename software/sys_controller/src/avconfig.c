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
    .adapt_lm = 1,
    .audio_src_map = {AUD_AV1_ANALOG, AUD_AV2_ANALOG, AUD_AV3_ANALOG, AUD_AV4_DIGITAL},
};

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
    adv7513_get_default_cfg(&tc.adv7513_cfg);
#ifndef DExx_FW
    pcm186x_get_default_cfg(&tc.pcm_cfg);
#else
    tc.adv7513_cfg.i2s_fs = ADV_96KHZ;
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
        (tc.ypbpr_cs != cc.ypbpr_cs))
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
        (tc.adapt_lm != cc.adapt_lm) ||
        (tc.upsample2x != cc.upsample2x) ||
        (tc.default_vic != cc.default_vic))
        status = (status < MODE_CHANGE) ? MODE_CHANGE : status;

#ifndef DExx_FW
    if (tc.audmux_sel != cc.audmux_sel)
        switch_audmux(tc.audmux_sel);
    if (memcmp(tc.audio_src_map, cc.audio_src_map, 4*sizeof(audinput_t)))
        switch_audsrc(tc.audio_src_map, &tc.adv7513_cfg.audio_fmt);
#endif

    memcpy(&cc, &tc, sizeof(avconfig_t));

#ifndef DExx_FW
    cc.adv7513_cfg.i2s_fs = cc.pcm_cfg.fs;
#endif

    return status;
}
