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
    .pm_ad_480p = 1,
    .sl_altern = 1,
    .adapt_lm = 1,
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
    pcm186x_get_default_cfg(&tc.pcm_cfg);

    if (update_cc)
        memcpy(&cc, &tc, sizeof(avconfig_t));

    set_default_vm_table();

    return 0;
}

status_t update_avconfig() {
    status_t status = NO_CHANGE;

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
        (tc.pm_ad_480p != cc.pm_ad_480p) ||
        (tc.adapt_lm != cc.adapt_lm) ||
        (tc.upsample2x != cc.upsample2x) ||
        (tc.default_vic != cc.default_vic))
        status = (status < MODE_CHANGE) ? MODE_CHANGE : status;

    if (tc.audmux_sel != cc.audmux_sel)
        switch_audmux(tc.audmux_sel);

    memcpy(&cc, &tc, sizeof(avconfig_t));

    cc.adv7513_cfg.i2s_fs = cc.pcm_cfg.fs;

    return status;
}
