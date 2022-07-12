//
// Copyright (C) 2022  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include <unistd.h>
#include "userdata.h"
#include "avconfig.h"
#include "controls.h"
#include "flash.h"
#include "video_modes.h"

#define UDE_ITEM(ID, VER, ITEM) {{ID, VER, sizeof(ITEM)}, &ITEM}

extern const int num_video_modes_plm, num_video_modes, num_smp_presets;
extern flash_ctrl_dev flashctrl_dev;
extern uint16_t rc_keymap[REMOTE_MAX_KEYS];
extern avconfig_t tc;
extern settings_t ts;
extern mode_data_t video_modes_plm[];
extern mode_data_t video_modes[];
extern smp_preset_t smp_presets[];

char target_profile_name[USERDATA_NAME_LEN+1];

ude_item_map ude_initcfg_items[] = {
    UDE_ITEM(0, 58, rc_keymap),
    UDE_ITEM(1, 58, ts.default_avinput),
    UDE_ITEM(2, 58, ts.osd_enable),
    UDE_ITEM(3, 58, ts.osd_status_timeout),
#ifndef DExx_FW
    UDE_ITEM(4, 58, ts.fan_pwm),
    UDE_ITEM(5, 58, ts.led_pwm),
#endif
};

ude_item_map ude_profile_items[] = {
    {{0, 58, 0}, video_modes_plm}, //size updated during init
    {{1, 58, 0}, video_modes}, //size updated during init
    {{2, 58, 0}, smp_presets}, //size updated during init
    // avconfig_t
    UDE_ITEM(3, 58, tc.sl_mode),
    UDE_ITEM(4, 58, tc.sl_type),
    UDE_ITEM(5, 58, tc.sl_hybr_str),
    UDE_ITEM(6, 58, tc.sl_method),
    UDE_ITEM(7, 58, tc.sl_altern),
    UDE_ITEM(8, 58, tc.sl_altiv),
    UDE_ITEM(9, 58, tc.sl_str),
    UDE_ITEM(10, 58, tc.sl_id),
    UDE_ITEM(11, 58, tc.sl_cust_l_str),
    UDE_ITEM(12, 58, tc.sl_cust_c_str),
    UDE_ITEM(13, 58, tc.sl_cust_iv_x),
    UDE_ITEM(14, 58, tc.sl_cust_iv_y),
    UDE_ITEM(15, 58, tc.l2_mode),
    UDE_ITEM(16, 58, tc.l3_mode),
    UDE_ITEM(17, 58, tc.l4_mode),
    UDE_ITEM(18, 58, tc.l5_mode),
    UDE_ITEM(19, 58, tc.l5_fmt),
    UDE_ITEM(20, 58, tc.pm_240p),
    UDE_ITEM(21, 58, tc.pm_384p),
    UDE_ITEM(22, 58, tc.pm_480i),
    UDE_ITEM(23, 58, tc.pm_480p),
    UDE_ITEM(24, 58, tc.pm_1080i),
    UDE_ITEM(25, 58, tc.pm_ad_240p),
    UDE_ITEM(26, 58, tc.pm_ad_288p),
    UDE_ITEM(27, 58, tc.pm_ad_384p),
    UDE_ITEM(28, 58, tc.pm_ad_480i),
    UDE_ITEM(29, 58, tc.pm_ad_576i),
    UDE_ITEM(30, 58, tc.pm_ad_480p),
    UDE_ITEM(31, 58, tc.pm_ad_576p),
    UDE_ITEM(32, 58, tc.pm_ad_720p),
    UDE_ITEM(33, 58, tc.pm_ad_1080i),
    UDE_ITEM(34, 58, tc.sm_ad_240p_288p),
    UDE_ITEM(35, 58, tc.sm_ad_384p),
    UDE_ITEM(36, 58, tc.sm_ad_480i_576i),
    UDE_ITEM(37, 58, tc.sm_ad_480p),
    UDE_ITEM(38, 58, tc.sm_ad_576p),
    UDE_ITEM(39, 58, tc.lm_mode),
    UDE_ITEM(40, 58, tc.oper_mode),
    UDE_ITEM(41, 58, tc.tp_mode),
    UDE_ITEM(42, 58, tc.lm_deint_mode),
    UDE_ITEM(43, 58, tc.nir_even_offset),
    UDE_ITEM(44, 58, tc.ar_256col),
    UDE_ITEM(45, 58, tc.h_mask),
    UDE_ITEM(46, 58, tc.v_mask),
    UDE_ITEM(47, 58, tc.mask_br),
    UDE_ITEM(48, 58, tc.mask_color),
    UDE_ITEM(49, 58, tc.bfi_enable),
    UDE_ITEM(50, 58, tc.bfi_str),
    UDE_ITEM(51, 58, tc.s480p_mode),
    UDE_ITEM(52, 58, tc.s400p_mode),
    UDE_ITEM(53, 58, tc.upsample2x),
    UDE_ITEM(54, 58, tc.ypbpr_cs),
    UDE_ITEM(55, 58, tc.audmux_sel),
    UDE_ITEM(56, 58, tc.audio_src_map),
    UDE_ITEM(57, 58, tc.reverse_lpf),
    UDE_ITEM(58, 58, tc.audio_fmt),
#ifdef VIP
    UDE_ITEM(59, 58, tc.scl_out_mode),
    UDE_ITEM(60, 58, tc.scl_framelock),
    UDE_ITEM(61, 58, tc.scl_aspect),
    UDE_ITEM(62, 58, tc.scl_alg),
    UDE_ITEM(63, 58, tc.scl_edge_thold),
    UDE_ITEM(64, 58, tc.scl_dil_motion_shift),
#ifndef VIP_DIL_B
    UDE_ITEM(65, 58, tc.scl_dil_alg),
#else
    UDE_ITEM(66, 58, tc.scl_dil_motion_scale),
    UDE_ITEM(67, 58, tc.scl_dil_cadence_detect_enable),
    UDE_ITEM(68, 58, tc.scl_dil_visualize_motion),
#endif
    UDE_ITEM(69, 58, tc.sm_scl_240p_288p),
    UDE_ITEM(70, 58, tc.sm_scl_384p),
    UDE_ITEM(71, 58, tc.sm_scl_480i_576i),
    UDE_ITEM(72, 58, tc.sm_scl_480p),
    UDE_ITEM(73, 58, tc.sm_scl_576p),
#endif
#ifdef INC_THS7353
    UDE_ITEM(74, 58, tc.syncmux_stc),
#endif
    UDE_ITEM(75, 58, tc.isl_cfg),
#ifdef INC_ADV7513
    UDE_ITEM(76, 58, tc.hdmitx_cfg),
#endif
#ifdef INC_SII1136
    UDE_ITEM(77, 58, tc.hdmitx_cfg),
#endif
#ifdef INC_ADV761X
    UDE_ITEM(78, 58, tc.hdmirx_cfg),
#endif
#ifdef INC_PCM186X
    UDE_ITEM(79, 58, tc.pcm_cfg),
#endif
};

void init_userdata() {
    ude_profile_items[0].hdr.data_size = num_video_modes_plm*sizeof(mode_data_t);
    ude_profile_items[1].hdr.data_size = num_video_modes*sizeof(mode_data_t);
    ude_profile_items[2].hdr.data_size = num_smp_presets*sizeof(smp_preset_t);
}

int write_userdata(uint8_t entry) {
    ude_hdr hdr;
    ude_item_map *target_map;
    uint32_t flash_addr, bytes_written;
    int i;

    if (entry > MAX_USERDATA_ENTRY) {
        printf("invalid entry\n");
        return -1;
    }

    memset(&hdr, 0x00, sizeof(ude_hdr));
    strncpy(hdr.userdata_key, "USRDATA", 8);
    hdr.type = (entry > MAX_PROFILE) ? UDE_INITCFG : UDE_PROFILE;

    if (hdr.type == UDE_INITCFG) {
        target_map = ude_initcfg_items;
        hdr.num_items = sizeof(ude_initcfg_items)/sizeof(ude_item_map);

        sniprintf(hdr.name, USERDATA_NAME_LEN+1, "INITCFG");
    } else if (hdr.type == UDE_PROFILE) {
        target_map = ude_profile_items;
        hdr.num_items = sizeof(ude_profile_items)/sizeof(ude_item_map);

        if (target_profile_name[0] == 0)
            sniprintf(target_profile_name, USERDATA_NAME_LEN+1, "<used>");

        strncpy(hdr.name, target_profile_name, USERDATA_NAME_LEN+1);
    }

    flash_addr = flashctrl_dev.flash_size - (16-entry)*FLASH_SECTOR_SIZE;

    // Disable flash write protect and erase sector
#ifndef DExx_FW
    flash_write_protect(&flashctrl_dev, 0);
#endif
    flash_sector_erase(&flashctrl_dev, flash_addr);

    // Write data into erased sector
    memcpy((uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr), &hdr, sizeof(ude_hdr));
    bytes_written = sizeof(ude_hdr);
    for (i=0; i<hdr.num_items; i++) {
        memcpy((uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr + bytes_written), &target_map[i].hdr, sizeof(ude_item_hdr));
        bytes_written += sizeof(ude_item_hdr);
        memcpy((uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr + bytes_written), target_map[i].data, target_map[i].hdr.data_size);
        bytes_written += target_map[i].hdr.data_size;
    }

    // Re-enable write protection
#ifndef DExx_FW
    flash_write_protect(&flashctrl_dev, 1);
#endif

    printf("%u bytes written into userdata entry %u\n", bytes_written, entry);

    return 0;
}

int read_userdata(uint8_t entry, int dry_run) {
    ude_hdr hdr;
    ude_item_hdr item_hdr;
    ude_item_map *target_map;
    uint32_t flash_addr, bytes_read;
    int i, j, target_map_items;

    if (entry > MAX_USERDATA_ENTRY) {
        printf("invalid entry\n");
        return -1;
    }

    target_profile_name[0] = 0;

    flash_addr = flashctrl_dev.flash_size - (16-entry)*FLASH_SECTOR_SIZE;
    memcpy(&hdr, (uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr), sizeof(ude_hdr));
    bytes_read = sizeof(ude_hdr);

    if (strncmp(hdr.userdata_key, "USRDATA", 8)) {
        printf("No userdata found on entry %u\n", entry);
        set_func_ret_msg("Not loaded");
        return 1;
    }

    strncpy(target_profile_name, hdr.name, USERDATA_NAME_LEN+1);
    if (dry_run)
        return 0;

    target_map = (hdr.type == UDE_INITCFG) ? ude_initcfg_items : ude_profile_items;
    target_map_items = (hdr.type == UDE_INITCFG) ? sizeof(ude_initcfg_items)/sizeof(ude_item_map) : sizeof(ude_profile_items)/sizeof(ude_item_map);

    for (i=0; i<hdr.num_items; i++) {
        memcpy(&item_hdr, (uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr + bytes_read), sizeof(ude_item_hdr));
        bytes_read += sizeof(ude_item_hdr);
        for (j=0; j<target_map_items; j++) {
            if (!memcmp(&item_hdr, &target_map[i].hdr, sizeof(ude_item_hdr))) {
                memcpy(target_map[i].data, (uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + flash_addr + bytes_read), item_hdr.data_size);
                break;
            }
        }
        bytes_read += item_hdr.data_size;
    }

    printf("%u bytes read from userdata entry %u\n", bytes_read, entry);

    return 0;
}