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

#include <unistd.h>
#include <string.h>
#include "firmware.h"
#include "file.h"
#include "flash.h"
#include "utils.h"
#include "menu.h"
#include "us2066.h"
#include "av_controller.h"

#define DRAM_TMP_ADDR (0x10000000 + 0x05000000)

#define PAD_TO_ALIGN4(x) (4*((x+3)/4))

extern rem_update_dev rem_reconfig_dev;
extern flash_ctrl_dev flashctrl_dev;
extern uint8_t sd_det;
extern char menu_row1[US2066_ROW_LEN+1], menu_row2[US2066_ROW_LEN+1];

int fw_update(char *dirname, char *filename) {
    FIL fw_file;
    char dirname_root[10];
    fw_header hdr;
    int i, retval = 0;
    unsigned bytes_read, bytes_to_copy=0;
    uint32_t crcval;
    unsigned char *dram_tmp = (unsigned char*)DRAM_TMP_ADDR;

    if (!sd_det) {
        printf("SD card not detected\n");
        return -1;
    }

    sniprintf(dirname_root, sizeof(dirname_root), "/%s", dirname);
    f_chdir(dirname_root);

    if (!file_open(&fw_file, filename)) {
        strlcpy(menu_row1, "Checking FW", US2066_ROW_LEN+1);
        strlcpy(menu_row2, "Please wait...", US2066_ROW_LEN+1);
        ui_disp_menu(1);

        if (f_read(&fw_file, &hdr, sizeof(hdr), &bytes_read) != F_OK) {
            printf("FW hdr read error\n");
            retval = -3;
            goto close_file;
        }

        if (strncmp(hdr.key, "OSSCPRO", 8) || (hdr.num_images == 0) || (hdr.num_images > 8)) {
            printf("Invalid FW header\n");
            retval = -4;
            goto close_file;
        }

        crcval = crc32((unsigned char *)&hdr, sizeof(hdr)-4, 1);
        if (crcval != hdr.hdr_crc) {
            printf("Invalid FW header CRC (0x%.8x instead of 0x%.8x)\n", crcval, hdr.hdr_crc);
            retval = -5;
            goto close_file;
        }

        printf("Firmware %u.%u%s%s - %u images:\n", hdr.fw_version_major, hdr.fw_version_minor, hdr.fw_suffix[0] ? "-" : "", hdr.fw_suffix, hdr.num_images);
        for (i=0; i<hdr.num_images; i++) {
            bytes_to_copy += PAD_TO_ALIGN4(hdr.image_info[i].size);
            printf("    Image %u: %u bytes, offset 0x%.8x, crc 0x%.8x\n", i, hdr.image_info[i].size, hdr.image_info[i].target_offset, hdr.image_info[i].crc);
            if ((hdr.image_info[i].target_offset % FLASH_SECTOR_SIZE) != 0) {
                printf("Target offset not aligned to flash sector\n");
                retval = -6;
                goto close_file;
            } else if (hdr.image_info[i].target_offset + hdr.image_info[i].size >= flashctrl_dev.flash_size) {
                printf("Image exceeds flash boundary\n");
                retval = -7;
                goto close_file;
            }
        }

        sniprintf(menu_row1, US2066_ROW_LEN+1, "v%u.%u%s%s (%lu parts)", hdr.fw_version_major, hdr.fw_version_minor, hdr.fw_suffix[0] ? "-" : "", hdr.fw_suffix, hdr.num_images);
        sniprintf(menu_row2, US2066_ROW_LEN+1, "Update? N:< Y:>");
        ui_disp_menu(1);

        if (prompt_yesno(RC_RIGHT, JOY_RIGHT, RC_LEFT, JOY_LEFT) != 1) {
            set_func_ret_msg("Cancelled");
            retval = 1;
            goto close_file;
        }

        strlcpy(menu_row2, "Please wait...", US2066_ROW_LEN+1);
        ui_disp_menu(1);

        printf("Copying images to DRAM...\n");
        set_dram_refresh(1);
        if ((f_read(&fw_file, dram_tmp, bytes_to_copy, &bytes_read) != F_OK) || (bytes_read != bytes_to_copy)) {
            printf("FW data read error\n");
            retval = -8;
            goto close_file;
        }

        printf("Checking copied data...\n");
        bytes_read = 0;
        for (i=0; i<hdr.num_images; i++) {
            crcval = crc32(dram_tmp, hdr.image_info[i].size, 1);
            dram_tmp += PAD_TO_ALIGN4(hdr.image_info[i].size);
            if (crcval != hdr.image_info[i].crc) {
                printf("Image %u: Invalid CRC (0x%.8x)\n", i, crcval);
                retval = -9;
                goto close_file;
            }
        }

        file_close(&fw_file);

        printf("Starting update procedure...\n");
        strlcpy(menu_row1, "Updating", US2066_ROW_LEN+1);
        ui_disp_menu(1);

        // No return from here
        fw_update_commit(&hdr);
    } else {
        printf("FW file not found\n");
        f_chdir("/");
        return -2;
    }

close_file:
    file_close(&fw_file);
    f_chdir("/");
    set_dram_refresh(0);
    return retval;
}

// commit FW update. Do not call functions located in flash during update
void __attribute__((noinline, __section__(".text_bram"))) fw_update_commit(fw_header *hdr) {
    int i, j, sectors;
    uint32_t addr;
    uint32_t *data_from, *data_to, *data_end;

    flash_write_protect(&flashctrl_dev, 0);

    // Erase sectors
    for (i=0; i<hdr->num_images; i++) {
        sectors = (hdr->image_info[i].size/FLASH_SECTOR_SIZE) + ((hdr->image_info[i].size % FLASH_SECTOR_SIZE) != 0);
        addr = hdr->image_info[i].target_offset;

        for (j=0; j<sectors; j++) {
            flash_sector_erase(&flashctrl_dev, addr);
            addr += FLASH_SECTOR_SIZE;
        }
    }

    // Write data
    data_from = (uint32_t*)DRAM_TMP_ADDR;
    for (i=0; i<hdr->num_images; i++) {
        data_to = (uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + hdr->image_info[i].target_offset);
        data_end = (uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE + hdr->image_info[i].target_offset + hdr->image_info[i].size);

        while (data_to < data_end) {
            *data_to++ = *data_from++;
        }
    }

    // flush command FIFO before FPGA reconfiguration start
    *(volatile uint32_t*)(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE);

    rem_reconfig_dev.regs->reconfig_start = 1;

    while (1) {}
}
