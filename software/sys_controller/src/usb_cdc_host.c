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

#include <string.h>
#include "sysconfig.h"
#include "usb_hw.h"
#include "usb_core.h"
#include "usb_defs.h"
#include "usb_cdc_host.h"

#ifdef INC_USB

#define USB_CDC_DEV_ADDR            0x04

#define USB_CDC_DEBOUNCE_MS         100
#define USB_CDC_RETRY_MS            1000
#define USB_CDC_MAX_TIMEOUTS        10

#define USB_CDC_DESC_BUF_SIZE       160
#define USB_CDC_PKT_SIZE            64

#define USB_CLASS_CDC               0x02
#define USB_CLASS_CDC_DATA          0x0A
#define CDC_REQ_SET_CONTROL_LINE    0x22
#define CDC_LINE_DTR_RTS            0x03

typedef enum {
    CDC_DETACHED = 0,
    CDC_ATTACHING,
    CDC_READY,
    CDC_FAILED
} cdc_state_t;

static struct usb_device cdc_dev;
static struct usb_endpoint cdc_ep_in, cdc_ep_out;
static cdc_state_t cdc_state;
static t_time cdc_state_ts;
static uint8_t cdc_timeouts;

static char cdc_line[USB_CDC_LINE_SIZE+1];
static uint8_t cdc_line_len, cdc_line_ready, cdc_line_overflow;

static void cdc_set_state(cdc_state_t state)
{
    cdc_state = state;
    cdc_state_ts = usbhw_timer_now();
}

static int cdc_parse_config(uint8_t *desc, int len, int *comm_if)
{
    int off, found = 0;
    uint8_t cur_if = 0, cur_class = 0;

    *comm_if = -1;

    for (off = 0; (off + 2 <= len) && (desc[off] >= 2) && (off + desc[off] <= len); off += desc[off]) {
        uint8_t *d = &desc[off];

        if (d[1] == DESC_INTERFACE) {
            cur_if = d[2];
            cur_class = d[5];
            if ((cur_class == USB_CLASS_CDC) && (*comm_if < 0))
                *comm_if = cur_if;
        } else if ((d[1] == DESC_ENDPOINT) && (cur_class == USB_CLASS_CDC_DATA) &&
                   ((d[3] & ENDPOINT_TYPE_MASK) == ENDPOINT_TYPE_BULK)) {
            struct usb_endpoint *ep = (d[2] & ENDPOINT_DIR_MASK) ? &cdc_ep_in : &cdc_ep_out;

            ep->dev = &cdc_dev;
            ep->endpoint = d[2] & ENDPOINT_ADDR_MASK;
            ep->direction = d[2] & ENDPOINT_DIR_MASK;
            ep->endpoint_type = ENDPOINT_TYPE_BULK;
            ep->max_packet_size = d[4] | (d[5] << 8);
            ep->data_toggle = 0;
            found |= (d[2] & ENDPOINT_DIR_MASK) ? 1 : 2;
        }
    }

    return (found == 3);
}

static int cdc_attach()
{
    uint8_t desc[USB_CDC_DESC_BUF_SIZE];
    int len, total, comm_if;

    usbhw_reset();
    if (!usbhw_hub_device_detected())
        return 0;
    usbhw_setup_device();
    usbhw_timer_sleep(10);

    if (!usb_configure_device(&cdc_dev, USB_CDC_DEV_ADDR))
        return 0;

    len = usb_send_control_read(&cdc_dev, 0x80, REQ_GET_DESCRIPTOR, (DESC_CONFIGURATION << 8), 0, 9, desc, 9);
    if (len < 9)
        return 0;

    total = desc[2] | (desc[3] << 8);
    if (total > USB_CDC_DESC_BUF_SIZE - USB_CDC_PKT_SIZE)
        total = USB_CDC_DESC_BUF_SIZE - USB_CDC_PKT_SIZE;

    len = usb_send_control_read(&cdc_dev, 0x80, REQ_GET_DESCRIPTOR, (DESC_CONFIGURATION << 8), 0, total, desc, total);
    if ((len <= 0) || !cdc_parse_config(desc, len, &comm_if))
        return 0;

    if (comm_if >= 0)
        usb_send_control_write(&cdc_dev, 0x21, CDC_REQ_SET_CONTROL_LINE, CDC_LINE_DTR_RTS, comm_if, 0, NULL);

    return 1;
}

static void cdc_detach()
{
    usbhw_hub_enable(1, 0);
    cdc_line_len = cdc_line_ready = cdc_line_overflow = 0;
    cdc_set_state(CDC_DETACHED);
}

static void cdc_rx_bytes(const uint8_t *p, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (cdc_line_ready)
            return;

        if ((p[i] == '\r') || (p[i] == '\n')) {
            if (cdc_line_overflow) {
                usb_cdc_host_reply("Unrecognized sequence, RX buffer cleared");
                cdc_line_overflow = 0;
                cdc_line_len = 0;
            } else if (cdc_line_len > 0) {
                cdc_line[cdc_line_len] = 0;
                cdc_line_ready = 1;
            }
        } else if (cdc_line_len < USB_CDC_LINE_SIZE) {
            cdc_line[cdc_line_len++] = p[i];
        } else {
            cdc_line_overflow = 1;
        }
    }
}

void usb_cdc_host_init()
{
    memset(&cdc_dev, 0, sizeof(cdc_dev));
    cdc_detach();
}

void usb_cdc_host_poll()
{
    uint8_t pkt[USB_CDC_PKT_SIZE];
    int det, res;
    long elapsed;

    det = usbhw_hub_device_detected();
    elapsed = usbhw_timer_diff(usbhw_timer_now(), cdc_state_ts);

    switch (cdc_state) {
    case CDC_DETACHED:
        if (det)
            cdc_set_state(CDC_ATTACHING);
        break;
    case CDC_ATTACHING:
        if (!det) {
            cdc_set_state(CDC_DETACHED);
        } else if (elapsed >= USB_CDC_DEBOUNCE_MS) {
            cdc_timeouts = 0;
            cdc_set_state(cdc_attach() ? CDC_READY : CDC_FAILED);
        }
        break;
    case CDC_FAILED:
        if (!det)
            cdc_detach();
        else if (elapsed >= USB_CDC_RETRY_MS)
            cdc_set_state(CDC_ATTACHING);
        break;
    case CDC_READY:
        if (det)
            cdc_state_ts = usbhw_timer_now();
        else if (elapsed >= USB_CDC_DEBOUNCE_MS) {
            cdc_detach();
            break;
        }

        res = usb_bulk_read(&cdc_ep_in, pkt, sizeof(pkt), 0);
        if (res > 0) {
            cdc_timeouts = 0;
            cdc_rx_bytes(pkt, res);
        } else if ((res == USB_RES_TIMEOUT) || (res == USB_RES_STALL)) {
            if (++cdc_timeouts >= USB_CDC_MAX_TIMEOUTS)
                cdc_detach();
        }
        break;
    }
}

char* usb_cdc_host_get_line()
{
    if (!cdc_line_ready)
        return NULL;

    cdc_line_ready = 0;
    cdc_line_len = 0;
    return cdc_line;
}

void usb_cdc_host_reply(const char *reply)
{
    uint8_t buf[48];
    int len = strlen(reply);

    if (cdc_state != CDC_READY)
        return;

    if (len > (int)sizeof(buf)-1)
        len = sizeof(buf)-1;
    memcpy(buf, reply, len);
    buf[len++] = '\n';

    usb_bulk_write(&cdc_ep_out, buf, len, 20);
}

#endif
