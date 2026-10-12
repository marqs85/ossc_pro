//
// Copyright (C) 2026  Luan Freitas (ludufre)
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

#ifndef USB_CDC_HOST_H_
#define USB_CDC_HOST_H_

#define USB_CDC_LINE_SIZE 15

void usb_cdc_host_init();

void usb_cdc_host_poll();

char* usb_cdc_host_get_line();

void usb_cdc_host_reply(const char *reply);

#endif /* USB_CDC_HOST_H_ */
