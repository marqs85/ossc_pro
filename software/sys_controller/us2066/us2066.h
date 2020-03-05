//
// Copyright (C) 2019-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef US2066_H_
#define US2066_H_

#include "system.h"
#include "sysconfig.h"
#include <stdio.h>

#define US2066_ROW_LEN 20

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
} us2066_dev;

void us2066_init(us2066_dev *dev);

void us2066_write(us2066_dev *dev, char *row1, char *row2);

#endif /* US2066_H_ */
