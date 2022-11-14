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

#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include "sysconfig.h"
#include "controls.h"
#include "video_modes.h"

typedef enum {
    NO_ACTION    = 0,
    OPT_SELECT,
    PREV_MENU,
    PREV_PAGE,
    NEXT_PAGE,
    VAL_MINUS,
    VAL_PLUS
} menucode_id;

typedef enum {
    OPT_AVCONFIG_SELECTION,
    OPT_AVCONFIG_NUMVALUE,
    OPT_AVCONFIG_NUMVAL_U16,
    OPT_SUBMENU,
    OPT_CUSTOMMENU,
    OPT_FUNC_CALL,
} menuitem_type;

typedef int (*func_call)(void);
typedef void (*arg_func)(void);
typedef void (*disp_func)(uint8_t);
typedef void (*disp_func_u16)(uint16_t*);
typedef void (*cstm_disp_func)(menucode_id, int);

typedef struct {
    uint8_t *data;
    uint8_t max;
    disp_func df;
} arg_info_t;

typedef struct {
    uint8_t *data;
    uint8_t wrap_cfg;
    uint8_t min;
    uint8_t max;
    uint8_t list_view;
    const char **setting_str;
} opt_avconfig_selection;

typedef struct {
    uint8_t *data;
    uint8_t wrap_cfg;
    uint8_t min;
    uint8_t max;
    disp_func df;
} opt_avconfig_numvalue;

typedef struct {
    uint16_t *data;
    uint16_t min;
    uint16_t max;
    disp_func_u16 df;
} opt_avconfig_numvalue_u16;

typedef struct {
    func_call f;
    const arg_info_t *arg_info;
} opt_func_call;

typedef struct menustruct menu_t;

typedef struct {
    const menu_t *menu;
    const arg_info_t *arg_info;
    arg_func arg_f;
} opt_submenu;

typedef struct {
    cstm_disp_func cstm_f;
} opt_custommenu;

typedef struct {
    const char *name;
    menuitem_type type;
    union {
        opt_avconfig_selection sel;
        opt_avconfig_numvalue num;
        opt_avconfig_numvalue_u16 num_u16;
        opt_submenu sub;
        opt_custommenu cstm;
        opt_func_call fun;
    };
} menuitem_t;

struct menustruct {
    uint8_t num_items;
    menuitem_t *items;
};

#define SETTING_ITEM(x) 0, sizeof(x)/sizeof(char*)-1, 0, x
#define SETTING_ITEM_LIST(x) 0, sizeof(x)/sizeof(char*)-1, 1, x
#define SETTING_ONLY_LIST(x) 0, sizeof(x)/sizeof(char*)-1, 2, x
#define MENU(X, Y) menuitem_t X##_items[] = Y; const menu_t X = { sizeof(X##_items)/sizeof(menuitem_t), X##_items };
#define P99_PROTECT(...) __VA_ARGS__

typedef struct {
    const menu_t *m;
    uint8_t mp;
} menunavi;

int is_menu_active();
void init_menu();
menunavi* get_current_menunavi();
void render_osd_menu();
void cstm_clock_phase(menucode_id code, int setup_disp);
void cstm_size(menucode_id code, int setup_disp);
void cstm_position(menucode_id code, int setup_disp);
void cstm_listview(menucode_id code, int setup_disp);
void enter_cstm(menuitem_t *item, int detached_mode);
void quick_adjust(menuitem_t *item, int adj);
void quick_adjust_phase(uint8_t dir);
void display_menu(rc_code_t rcode, btn_code_t bcode);
void set_func_ret_msg(char *msg);
void update_osd_size(mode_data_t *vm_out);
static void vm_select();
static void vm_tweak(uint16_t *v);
static void smp_select();
static int smp_is_active();
static int smp_reset();

#endif
