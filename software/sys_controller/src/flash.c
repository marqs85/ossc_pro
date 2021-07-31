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
#include "flash.h"

void __attribute__((noinline, __section__(".text_bram"))) flash_write_protect(flash_ctrl_dev *dev, int enable) {
    // Write enable
    dev->regs->flash_cmd_cfg = 0x00000006;
    dev->regs->flash_cmd_ctrl = 1;

    // Write status register
    dev->regs->flash_cmd_cfg = 0x00001001;
    dev->regs->flash_cmd_wrdata[0] = enable ? 0x0000005c : 0x00000000;
    dev->regs->flash_cmd_ctrl = 1;

    // Poll status register until write has completed
    while (1) {
        dev->regs->flash_cmd_cfg = 0x00001805;
        dev->regs->flash_cmd_ctrl = 1;
        if (!(dev->regs->flash_cmd_rddata[0] & (1<<0)))
            break;
    }

    // Write disable
    dev->regs->flash_cmd_cfg = 0x00000004;
    dev->regs->flash_cmd_ctrl = 1;
}
