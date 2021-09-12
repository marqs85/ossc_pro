//
// Copyright (C) 2021  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef FIRMWARE_H_
#define FIRMWARE_H_

#include "sysconfig.h"

#define FW_KEY_SIZE 8
#define FW_SUFFIX_MAX_SIZE 16

typedef struct {
    uint32_t size;
    uint32_t target_offset;
    uint32_t crc;
} fw_image_info;

/* numbers in little-endian */
typedef struct {
    char key[FW_KEY_SIZE];
    uint8_t fw_version_major;
    uint8_t fw_version_minor;
    char fw_suffix[FW_SUFFIX_MAX_SIZE];
    uint32_t num_images;
    fw_image_info image_info[8]; // max 8 images
    uint32_t hdr_crc;
} fw_header;

typedef struct {
    uint32_t reg_trig_cnd;
    uint32_t wdog_timeout;
    uint32_t wdog_enable;
    uint32_t image_addr;
    uint32_t cfg_mode;
    uint32_t reset_timer;
    uint32_t reconfig_start;
} rem_update_regs;

typedef struct {
    volatile rem_update_regs *regs;
} rem_update_dev;

int fw_update();

void fw_update_commit(fw_header *hdr);

#endif /* FIRMWARE_H_ */
