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
#include "menu.h"
#include "av_controller.h"
#include "avconfig.h"
#include "controls.h"
#include "firmware.h"
#include "userdata.h"
#include "us2066.h"

#define MAX_MENU_DEPTH 3

#define OPT_NOWRAP  0
#define OPT_WRAP    1

#ifdef OSDLANG_JP
#define LNG(e, j) j
#else
#define LNG(e, j) e
#endif

extern avconfig_t tc;
extern settings_t ts;
extern isl51002_dev isl_dev;
#ifndef DExx_FW
extern adv761x_dev advrx_dev;
#endif
extern volatile osd_regs *osd;
extern mode_data_t video_modes_plm[];
extern mode_data_t video_modes[];
extern mode_data_t vmode_in;
extern smp_preset_t smp_presets[], smp_presets_default[];
extern sync_timings_t hdmi_timings[];
extern uint8_t update_cur_vm;
extern oper_mode_t oper_mode;
extern const int num_video_modes_plm, num_video_modes, num_smp_presets;
extern uint8_t sl_def_iv_x, sl_def_iv_y;
extern avinput_t target_avinput;
extern uint8_t profile_sel_menu, sd_profile_sel_menu;

char menu_row1[US2066_ROW_LEN+1], menu_row2[US2066_ROW_LEN+1], func_ret_status[US2066_ROW_LEN+1];
extern char target_profile_name[USERDATA_NAME_LEN+1];

uint16_t tc_h_samplerate, tc_h_samplerate_adj, tc_h_synclen, tc_h_bporch, tc_h_active, tc_v_synclen, tc_v_bporch, tc_v_active, tc_sampler_phase, tc_h_mask, tc_v_mask, tc_v_total, tc_v_hz, tc_v_hz_frac;
uint8_t menu_active;
uint8_t vm_cur, vm_sel, vm_edit, vm_out_cur, vm_out_sel, vm_out_edit, smp_cur, smp_sel, smp_edit, dtmg_cur, dtmg_edit;

menunavi navi[MAX_MENU_DEPTH];
uint8_t navlvl;

// Pointer to custom menu display function
void (*cstm_f)(menucode_id, int);

// Listview variables
uint8_t lw_mp;
menuitem_t *lw_item;

static const char *off_on_desc[] = { LNG("Off","ｵﾌ"), LNG("On","ｵﾝ") };
static const char *ypbpr_cs_desc[] = { "Auto", "Rec. 601", "Rec. 709" };
static const char *s480p_mode_desc[] = { LNG("Auto","ｵｰﾄ"), "DTV 480p", "VESA 640x480@60" };
static const char *s400p_mode_desc[] = { "VGA 640x400@70", "VGA 720x400@70" };
static const char *syncmux_stc_desc[] = { "Off", "On (5MHz LPF)", "On (2.5MHz LPF)", "On (0.5MHz LPF)" };
static const char *l3_mode_desc[] = { LNG("Generic 16:9","ｼﾞｪﾈﾘｯｸ 16:9"), LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l2l4l5l6_mode_desc[] = { LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l5_fmt_desc[] = { "1920x1080", "1600x1200", "1920x1200" };
static const char *pm_240p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x", "Line4x", "Line5x", "Line6x" };
static const char *pm_480i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Deint + Line2x", "Line3x (laced)", "Deint + Line4x" };
static const char *pm_384p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x Generic", "Line2x 240x360", "Line3x 240x360" };
static const char *pm_480p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x" };
static const char *pm_1080i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Deint + Line2x" };
static const char *pm_ad_240p_desc[] = { "240p_CRT (Passthru)", "720x480 (Line2x)", "1280x720 (Line3x)", "1280x1024 (Line4x)", "1920x1080i (Line2x)", "1920x1080 (Line4x)", "1920x1080 (Line5x)", "1600x1200 (Line5x)", "1920x1200 (Line5x)", "1920x1440 (Line6x)", "2560x1440 (Line6x)", "2880x2160 (Line9x)" };
static const char *pm_ad_288p_desc[] = { "288p_CRT (Passthru)", "720x576 (Line2x)", "1920x1080i (Line2x)", "1920x1080 (Line4x)", "1920x1200 (Line4x)", "1920x1440 (Line5x)", "2560x1440 (Line5x)", "2880x2160 (Line7x)" };
static const char *pm_ad_384p_desc[] = { "1280x720 (Line2x)", "1024x768 (Line2x)", "1920x1080 (Line3x)", "1600x1200 (Line3x)", "1920x1200 (Line3x)", "1920x1440 (Line4x)", "2560x1440 (Line4x)" };
static const char *pm_ad_480i_desc[] = { "720x480i (Passthru)", "240p_CRT (240p rest.)", "720x480 (Dint+L2x)", "1280x1024 (Dint+L4x)", "1080i (240p rest+L2x)", "1920x1080 (Dint+L4x)", "1920x1440 (Dint+L6x)", "2560x1440 (Dint+L6x)" };
static const char *pm_ad_576i_desc[] = { "720x576i (Passthru)", "288p_CRT (288p rest.)", "720x576 (Dint+L2x)", "1080i (288p rest+L2x)", "1920x1080 (Dint+L4x)", "1920x1200 (Dint+L4x)" };
static const char *pm_ad_480p_desc[] = { "720x480 (Passthru)", "720x480i (Line drop)", "240p_CRT (Line drop)", "1280x1024 (Line2x)", "1920x1080i (Line1x)", "1920x1080 (Line2x)", "1920x1440 (Line3x)", "2560x1440 (Line3x)" };
static const char *pm_ad_576p_desc[] = { "720x576 (Passthru)", "720x576i (Line drop)", "288p_CRT (Line drop)", "1920x1080i (Line1x)", "1920x1080 (Line2x)", "1920x1200 (Line2x)" };
static const char *pm_ad_720p_desc[] = { "1280x720 (Passthru)", "2560x1440 (Line2x)" };
static const char *pm_ad_1080i_desc[] = { "1920x1080i (Passthru)", "1920x1080 (Dint+L2x)" };
static const char *pm_ad_1080p_desc[] = { "1920x1080 (Passthru)", "1920x1080i (Line drop)" };
static const char *sm_ad_240p_288p_desc[] = { "Generic 4:3", "SNES 256col", "SNES 512col", "MD 256col", "MD 320col", "PSX 256col", "PSX 320col", "PSX 384col", "PSX 512col", "PSX 640col", "SAT 320col", "SAT 352col", "SAT 640col", "SAT 704col", "N64 320col", "N64 640col", "Neo Geo 320col", "X68k 512col", "C64 4XXcol" };
static const char *sm_ad_384p_desc[] = { "Generic 4:3", "VGA 640x350", "VGA 720x350", "VGA 640x400", "VGA 720x400", "GBI 240x360", "PC98 640x400" };
static const char *sm_ad_480i_576i_desc[] = { "Generic 4:3", "Generic 16:9", "DTV 480i/576i 4:3", "DTV 480i/576i 16:9", "SNES 512col", "MD 320col", "PSX 512col", "PSX 640col", "SAT 640col", "SAT 704col", "N64 640col", "DC/PS2/GC 640col" };
static const char *sm_ad_480p_desc[] = { "Generic 4:3", "Generic 16:9", "DTV 480p 4:3", "DTV 480p 16:9", "VESA 640x480", "DC/PS2/GC 640col", "PS2-GSM 512col", "X68k 512col", "X68k 768col" };
static const char *sm_ad_576p_desc[] = { "Generic 4:3", "Generic 16:9", "DTV 576p 4:3", "DTV 576p 16:9" };
static const char *lm_deint_mode_desc[] = { "Bob", "Noninterlace restore" };
static const char *ar_256col_desc[] = { "Pseudo 4:3 DAR", "1:1 PAR" };
static const char *tx_mode_desc[] = { "HDMI (RGB Full)", "HDMI (RGB Limited)", "HDMI (YCbCr444)", "DVI" };
static const char *sl_mode_desc[] = { LNG("Off","ｵﾌ"), LNG("Auto","ｵｰﾄ"), LNG("On","ｵﾝ") };
static const char *sl_method_desc[] = { LNG("Multiplication","Multiplication"), LNG("Subtraction","Subtraction") };
static const char *sl_type_desc[] = { LNG("Horizontal","ﾖｺ"), LNG("Vertical","ﾀﾃ"), "Horiz. + Vert.", "Custom" };
static const char *sl_id_desc[] = { LNG("Top","ｳｴ"), LNG("Bottom","ｼﾀ") };
#ifdef DExx_FW
const char *avinput_str[] = { "Test pattern", "AV1_RGBS", "AV1_RGsB", "AV1_YPbPr", "AV1_RGBHV", "AV1_RGBCS" };
static const char *audio_fmt_desc[] = { "24bit/48kHz", "24bit/96kHz" };
static const char *audio_src_desc[] = { "AV1 (analog)", "SPDIF" };
#else
const char *avinput_str[] = { "Test pattern", "AV1_RGBS", "AV1_RGsB", "AV1_YPbPr", "AV1_RGBHV", "AV1_RGBCS", "AV2_YPbPr", "AV2_RGsB", "AV3_RGBHV", "AV3_RGBCS", "AV3_RGBS", "AV3_RGsB", "AV3_YPbPr", "AV4" };
static const char *audio_fmt_desc[] = { "24bit/48kHz", "24bit/96kHz", "24bit/192kHz" };
static const char *audio_src_desc[] = { "AV1 (analog)", "AV2 (analog)", "AV3 (analog)", "SPDIF", "AV4 (digital)" };
#endif
static const char *audio_sr_desc[] = { "Off", "On (4.0)", "On (5.1)", "On (7.1)" };
static const char *audmux_sel_desc[] = { "AV3 input", "AV1 output" };
static const char *osd_enable_desc[] = { "Off", "Full", "Simple" };
static const char *osd_status_desc[] = { "2s", "5s", "10s", "Off" };
static const char *rgsb_ypbpr_desc[] = { "RGsB", "YPbPr" };
static const char *auto_input_desc[] = { "Off", "Current input", "All inputs" };
static const char *mask_color_desc[] = { "Black", "Blue", "Green", "Cyan", "Red", "Magenta", "Yellow", "White" };
static const char *shmask_mode_desc[] = { "Off", "A-Grille", "TV", "PVM", "PVM-2530", "XC-3315C", "C-1084", "JVC", "VGA", "Custom shmask1.txt", "Custom shmask2.txt" };
static const char *adv761x_rgb_range_desc[] = { "Limited", "Full" };
static const char *oper_mode_desc[] = { "Line multiplier", "Scaler" };
static const char *lm_mode_desc[] = { "Pure", "Adaptive" };
static const char *scl_out_mode_desc[] = { "720x480 (60Hz)", "720x480 WS (60Hz)", "720x576 (50Hz)", "720x576 WS (50Hz)", "1280x720 (50-120Hz)", "1280x1024 (60Hz)", "1920x1080i (50-60Hz)", "1920x1080 (50-120Hz)", "1600x1200 (60Hz)", "1920x1200 (50-60Hz)", "1920x1440 (50-60Hz)", "2560x1440 (50-60Hz)", "2880x2160 (50-60Hz)" };
static const char *scl_crt_out_mode_desc[] = { "240p (60-120Hz)", "240p WS (60-120Hz)", "288p (50-100Hz)", "288p WS (50-100Hz)", "480i (60Hz)", "480i WS (60Hz)", "576i (50Hz)", "576i WS (50Hz)", "480p (60-120Hz)", "540p (50-60Hz)", "1024x768 (60-120Hz)", "1280x960 (60-120Hz)" };
static const char *scl_out_type_desc[] = { "DFP", "CRT" };
static const char *scl_framelock_desc[] = { "On", "On (2x source Hz)", "Off (source Hz)", "Off (50Hz)", "Off (60Hz)", "Off (100Hz)", "Off (120Hz)" };
static const char *scl_aspect_desc[] = { "Auto", "4:3", "16:9", "8:7", "1:1 source PAR", "Full" };
static const char *scl_alg_desc[] = { "Auto", "Integer (underscan)", "Integer (overscan)", "Nearest", "Lanczos3", "Lanczos3_sharp", "Lanczos3&3_sharp", "Lanczos4", "GS sharp", "Custom scaler1.txt", "Custom scaler2.txt" };
static const char *scl_gen_sr_desc[] = { "Auto", "Lowest", "Highest" };
#ifndef VIP_DIL_B
#ifdef DEBUG
static const char *scl_dil_alg_desc[] = { "Bob", "Weave", "Motion adaptive", "Visualize motion" };
#else
static const char *scl_dil_alg_desc[] = { "Bob", "Weave", "Motion adaptive" };
#endif
#endif
static const char *sm_scl_240p_288p_desc[] = { "Generic", "SNES 256col", "SNES 512col", "MD 256col", "MD 320col", "PSX 256col", "PSX 320col", "PSX 384col", "PSX 512col", "PSX 640col", "SAT 320col", "SAT 352col", "SAT 640col", "SAT 704col", "N64 320col", "N64 640col", "Neo Geo 320col", "X68k 512col", "C64 4XXcol" };
static const char *sm_scl_384p_desc[] = { "Generic", "VGA 640x350", "VGA 720x350", "VGA 640x400", "VGA 720x400", "GBI 240x360", "PC98 640x400" };
static const char *sm_scl_480i_576i_desc[] = { "Generic", "DTV 480i/576i", "SNES 512col", "MD 320col", "PSX 512col", "PSX 640col", "SAT 640col", "SAT 704col", "N64 640col", "DC/PS2/GC 640col" };
static const char *sm_scl_480p_desc[] = { "Generic", "DTV 480p", "VESA 640x480", "DC/PS2/GC 640col", "PS2-GSM 512col", "X68k 512col", "X68k 768col" };
static const char *sm_scl_576p_desc[] = { "Generic", "DTV 576p", "GC 640col" };
static const char *timing_1080p120_desc[] = { "CVT-RB", "Min. blank", "CEA-861", "CEA-861 PR2x" };
static const char *timing_2160p60_desc[] = { "CVT-RB PR2x", "Min. blank PR2x" };
static const char *exp_sel_desc[] = { "Auto", "Off", "Extra AV out", "Legacy AV in", "UVC bridge" };
static const char *extra_av_out_mode_desc[] = { "Off", "RGBHV", "RGBCS/RGBS", "RGsB" };
static const char *hdmi_timings_groups[] = { "HDMI other", "HDMI 240p", "HDMI 288p", "HDMI 384p", "HDMI 480i", "HDMI 576i", "HDMI 480p", "HDMI 576p", "HDMI 720p", "HDMI 1080i", "HDMI 1080p" };

static void afe_bw_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s%uMHz%s", (v==0 ? "Auto (" : ""), isl_get_afe_bw(&isl_dev, v), (v==0 ? ")" : "")); }
static void sog_vth_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u mV", (v*20)); }
static void hsync_vth_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u mV", ((v+2)*280)); }
static void sync_gf_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u ns", ((((v+14)%16)+1)*37)); }
static void sl_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", ((v+1)*625)/100); }
static void sl_cust_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", ((v)*625)/100); }
static void sl_cust_iv_x_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s%u%s", (v ? "" : "Auto ("), (v ? v : sl_def_iv_x)+1, (v ? "" : ")")); }
static void sl_cust_iv_y_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s%u%s", (v ? "" : "Auto ("), (v ? v : sl_def_iv_y)+1, (v ? "" : ")")); }
static void sl_hybr_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", (v*625)/100); }
static void lines_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u lines","%u ﾗｲﾝ"), v); }
static void pixels_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u pixels","%u ﾄﾞｯﾄ"), v); }
static void value_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", v); }
static void value16_disp(uint16_t *v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", *v); }
static void signed_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%d", (int8_t)(v-SIGNED_NUMVAL_ZERO)); }
static void pct_x10_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u.%u%%", (v/10), (v%10) ); }
#ifdef INC_PCM186X
static void aud_db_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%d dB", ((int8_t)v-PCM_GAIN_0DB)); }
#endif
static void audio_src_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s", audio_src_desc[v]); }
static void vm_plm_display_name (uint8_t v) { strlcpy(menu_row2, video_modes_plm[v].name, US2066_ROW_LEN+1); }
static void vm_display_name (uint8_t v) { strlcpy(menu_row2, video_modes[v].name, US2066_ROW_LEN+1); }
static void pwm_disp (uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", (v*10)); }
//static void link_av_desc (avinput_t v) { strlcpy(menu_row2, v == AV_LAST ? "No link" : avinput_str[v], US2066_ROW_LEN+1); }
static void profile_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u: %s", v, (read_userdata(v, 1) != 0) ? "<empty>" : target_profile_name); }
static void sd_profile_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u: %s", v, (read_userdata_sd(v, 1) != 0) ? "<empty>" : target_profile_name); }
static void alc_v_filter_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u lines","%u ﾗｲﾝ"), (1<<(v+5))); }
static void alc_h_filter_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u pixels","%u ﾄﾞｯﾄ"), (1<<(v+4))); }

static void smp_display_name(uint8_t v) {
#ifndef DExx_FW
    if (advrx_dev.powered_on && advrx_dev.sync_active)
        strlcpy(menu_row2, hdmi_timings_groups[v % NUM_VIDEO_GROUPS], US2066_ROW_LEN+1);
    else
#endif
        strlcpy(menu_row2, smp_presets[v].name, US2066_ROW_LEN+1);
}

static void sampler_phase_disp(char *dst, int max_len, uint8_t v, int active_mode) {
    if (v) {
        sniprintf(dst, max_len, "%d deg", ((v-1)*5625)/1000);
    } else {
        if (active_mode) {
            if (!update_cur_vm)
                sniprintf(dst, max_len, "Auto (%u deg)", get_sampler_phase());
            else
                sniprintf(dst, max_len, "Auto (refresh)");
        } else {
            sniprintf(dst, max_len, "Auto");
        }
    }
}

#ifndef DExx_FW
static void pixelderep_mode_disp(uint8_t v) {
    if (v)
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%ux", v);
    else
        sniprintf(menu_row2, US2066_ROW_LEN+1, "Auto (%ux)", advrx_dev.pixelderep_ifr+1);
}
#endif

static arg_info_t vm_arg_info = {&vm_sel, 0, vm_plm_display_name};
static arg_info_t vm_out_arg_info = {&vm_out_sel, 0, vm_display_name};
static arg_info_t smp_arg_info = {&smp_sel, 0, smp_display_name};
static const arg_info_t profile_arg_info = {&profile_sel_menu, MAX_PROFILE, profile_disp};
static const arg_info_t sd_profile_arg_info = {&sd_profile_sel_menu, MAX_SD_PROFILE, sd_profile_disp};


MENU(menu_advtiming_plm, P99_PROTECT({
    { LNG("H. samplerate","H. ｻﾝﾌﾟﾙﾚｰﾄ"),      OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate, H_TOTAL_MIN, H_TOTAL_MAX, vm_tweak } } },
    { "H. s.rate frac",                       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate_adj,       0, H_TOTAL_ADJ_MAX, vm_tweak } } },
    { LNG("H. synclen","H. ﾄﾞｳｷﾅｶﾞｻ"),         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_synclen,  H_SYNCLEN_MIN, H_SYNCLEN_MAX, vm_tweak } } },
    { LNG("H. backporch","H. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_bporch,    H_BPORCH_MIN, H_BPORCH_MAX, vm_tweak } } },
    { LNG("H. active","H. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_active,    H_ACTIVE_MIN, H_ACTIVE_MAX, vm_tweak } } },
    { LNG("V. synclen","V. ﾄﾞｳｷﾅｶﾞｻ"),         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_synclen,  V_SYNCLEN_MIN, V_SYNCLEN_MAX, vm_tweak } } },
    { LNG("V. backporch","V. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_bporch,    V_BPORCH_MIN, V_BPORCH_MAX, vm_tweak } } },
    { LNG("V. active","V. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_active,    V_ACTIVE_MIN, V_ACTIVE_MAX, vm_tweak } } },
    { "H. border",                            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_mask,    0, H_MASK_MAX, vm_tweak } } },
    { "V. border",                            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_mask,    0, V_MASK_MAX, vm_tweak } } },
    { LNG("Sampling phase","ｻﾝﾌﾟﾘﾝｸﾞﾌｪｰｽﾞ"),    OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_sampler_phase,          0, SAMPLER_PHASE_MAX, vm_tweak } } },
}))

MENU(menu_advtiming_out, P99_PROTECT({
    { "H. total",                             OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate, H_TOTAL_MIN, H_TOTAL_MAX, vm_out_tweak } } },
    { LNG("H. synclen","H. ﾄﾞｳｷﾅｶﾞｻ"),         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_synclen,  H_SYNCLEN_MIN, H_SYNCLEN_MAX, vm_out_tweak } } },
    { LNG("H. backporch","H. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_bporch,    H_BPORCH_MIN, H_BPORCH_MAX, vm_out_tweak } } },
    { LNG("H. active","H. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_active,    H_ACTIVE_MIN, H_ACTIVE_MAX, vm_out_tweak } } },
    { "V. total",                             OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_total,     V_TOTAL_MIN, V_TOTAL_MAX, vm_out_tweak } } },
    { LNG("V. synclen","V. ﾄﾞｳｷﾅｶﾞｻ"),         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_synclen,  V_SYNCLEN_MIN, V_SYNCLEN_MAX, vm_out_tweak } } },
    { LNG("V. backporch","V. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_bporch,    V_BPORCH_MIN, V_BPORCH_MAX, vm_out_tweak } } },
    { LNG("V. active","V. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_active,    V_ACTIVE_MIN, V_ACTIVE_MAX, vm_out_tweak } } },
    { "TP rfr rate",                          OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_hz,        40, 144, vm_out_tweak } } },
    { "TP rfr rate frac",                     OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_hz_frac,   0, 99, vm_out_tweak } } },
    { "Display AR H",                         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_mask,    1, 255, vm_out_tweak } } },
    { "Display AR V",                         OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_mask,    1, 255, vm_out_tweak } } },
}))

MENU(menu_advtiming, P99_PROTECT({
    { "Clock & Phase",                        OPT_CUSTOMMENU,         { .cstm = { &cstm_clock_phase } } },
    { "Size",                                 OPT_CUSTOMMENU,         { .cstm = { &cstm_size } } },
    { "Position",                             OPT_CUSTOMMENU,         { .cstm = { &cstm_position } } },
    { "Reset preset",                         OPT_FUNC_CALL,          { .fun =  { smp_reset, NULL } } },
}))

MENU(menu_cust_sl, P99_PROTECT({
    { "H interval",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_iv_x,          OPT_NOWRAP, 0, 9, sl_cust_iv_x_disp } } },
    { "V interval",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_iv_y,          OPT_NOWRAP, 0, 5, sl_cust_iv_y_disp } } },
    { "Sub-line 1 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 2 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 3 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 4 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 5 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 6 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[5],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 1 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 2 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 3 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 4 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 5 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 6 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[5],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 7 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[6],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 8 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[7],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 9 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[8],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 10 str",                    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[9],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
}))


MENU(menu_isl_video_opt, P99_PROTECT({
    { LNG("Video LPF","ﾋﾞﾃﾞｵ LPF"),             OPT_AVCONFIG_NUMVALUE, { .num = { &tc.isl_cfg.afe_bw,     OPT_WRAP, 0, 16,  afe_bw_disp } } },
    { LNG("Reverse LPF","ｷﾞｬｸLPF"),             OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.reverse_lpf, OPT_NOWRAP, 0, REVERSE_LPF_MAX, value_disp } } },
    { LNG("YPbPr in ColSpa","ｲﾛｸｳｶﾝﾆYPbPr"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.ypbpr_cs,          OPT_WRAP,   SETTING_ITEM(ypbpr_cs_desc) } } },
    { LNG("R/Pr offset","R/Pr ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.r_offs, 0, 0x3FF, value16_disp } } },
    { LNG("G/Y offset","G/Y ｵﾌｾｯﾄ"),            OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.g_offs, 0, 0x3FF, value16_disp } } },
    { LNG("B/Pb offset","B/Pb ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.b_offs, 0, 0x3FF, value16_disp } } },
    { LNG("R/Pr gain","R/Pr ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.r_gain, 0, 0x3FF, value16_disp } } },
    { LNG("G/Y gain","G/Y ｹﾞｲﾝ"),               OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.g_gain, 0, 0x3FF, value16_disp } } },
    { LNG("B/Pb gain","B/Pb ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.b_gain, 0, 0x3FF, value16_disp } } },
    { "Clamp strength",                         OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.clamp_str,      OPT_NOWRAP, 0, CLAMP_STR_MAX, value_disp } } },
    { "Clamp/ALC position",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.clamp_alc_start_pct_x10,  OPT_NOWRAP, CLAMP_POS_MIN, CLAMP_POS_MAX, pct_x10_disp } } },
    { "Clamp width",                            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.clamp_alc_width_pct_x10,  OPT_NOWRAP, CLAMP_WIDTH_MIN, CLAMP_WIDTH_MAX, pct_x10_disp } } },
    { "Clamp/ALC on coast",                     OPT_AVCONFIG_SELECTION, { .sel = { &tc.isl_cfg.coast_clamp,    OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { "Auto level ctl (ALC)",                   OPT_AVCONFIG_SELECTION, { .sel = { &tc.isl_cfg.alc_enable,    OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { "ALC V filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.alc_v_filter,  OPT_NOWRAP, 0, ALC_V_FILTER_MAX, alc_v_filter_disp } } },
    { "ALC H filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.alc_h_filter,  OPT_NOWRAP, 0, ALC_H_FILTER_MAX, alc_h_filter_disp } } },
}))

MENU(menu_isl_sync_opt, P99_PROTECT({
    { LNG("Analog sync Vth","ｱﾅﾛｸﾞﾄﾞｳｷ Vth"),    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.sog_vth,     OPT_NOWRAP, 0, SYNC_VTH_MAX, sog_vth_disp } } },
    { "TTL Hsync Vth",                          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.hsync_vth,    OPT_NOWRAP, 0, SYNC_VTH_MAX, hsync_vth_disp } } },
#ifdef INC_THS7353
    { "Analog sync pre-STC",                    OPT_AVCONFIG_SELECTION, { .sel = { &tc.syncmux_stc,         OPT_WRAP,   SETTING_ITEM_LIST(syncmux_stc_desc) } } },
#endif
    { "Sync glitch filt len",                   OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.sync_gf,     OPT_WRAP, 0, 15, sync_gf_disp } } },
    { "H-PLL Pre-Coast",                        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.pre_coast,   OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
    { "H-PLL Post-Coast",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.post_coast,  OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
    { "H-PLL Loop Gain",                        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.pll_loop_gain, OPT_NOWRAP, 0, PLL_LOOP_GAIN_MAX, value_disp } } },
}))

#ifdef INC_ADV761X
MENU(menu_adv_video_opt, P99_PROTECT({
    { "Default RGB range",                      OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmirx_cfg.default_rgb_range,    OPT_WRAP,   SETTING_ITEM(adv761x_rgb_range_desc) } } },
    { "Pixel de-repetition",                    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.hdmirx_cfg.pixelderep_mode,      OPT_NOWRAP, 0, 10, pixelderep_mode_disp } } },
}))
#endif

MENU(menu_pure_lm, P99_PROTECT({
    { LNG("240p/288p proc","240p/288pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_240p,         OPT_WRAP, SETTING_ITEM_LIST(pm_240p_desc) } } },
    { LNG("384p/400p proc","384p/400pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_384p,         OPT_WRAP, SETTING_ITEM_LIST(pm_384p_desc) } } },
    { LNG("480i/576i proc","480i/576iｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480i,         OPT_WRAP, SETTING_ITEM_LIST(pm_480i_desc) } } },
    { LNG("480p/576p proc","480p/576pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480p,         OPT_WRAP, SETTING_ITEM_LIST(pm_480p_desc) } } },
    { LNG("960i/1080i proc","960i/1080iｼｮﾘ"),   OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_1080i,        OPT_WRAP, SETTING_ITEM_LIST(pm_1080i_desc) } } },
    { LNG("Line2x mode","Line2xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l2_mode,         OPT_WRAP, SETTING_ITEM_LIST(l2l4l5l6_mode_desc) } } },
    { LNG("Line3x mode","Line3xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l3_mode,         OPT_WRAP, SETTING_ITEM_LIST(l3_mode_desc) } } },
    { LNG("Line4x mode","Line4xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l4_mode,         OPT_WRAP, SETTING_ITEM_LIST(l2l4l5l6_mode_desc) } } },
    { LNG("Line5x mode","Line5xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_mode,         OPT_WRAP, SETTING_ITEM_LIST(l2l4l5l6_mode_desc) } } },
    { LNG("Line6x mode","Line6xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l6_mode,         OPT_WRAP, SETTING_ITEM_LIST(l2l4l5l6_mode_desc) } } },
    { LNG("Line5x format","Line5xｹｲｼｷ"),        OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_fmt,          OPT_WRAP, SETTING_ITEM_LIST(l5_fmt_desc) } } },
    { "480p in preset",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.s480p_mode,     OPT_WRAP, SETTING_ITEM_LIST(s480p_mode_desc) } } },
    { "400p in preset",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.s400p_mode,     OPT_WRAP, SETTING_ITEM_LIST(s400p_mode_desc) } } },
    { LNG("Allow upsample2x","ｱｯﾌﾟｻﾝﾌﾟﾙ2xｷｮﾖｳ"), OPT_AVCONFIG_SELECTION, { .sel = { &tc.upsample2x,      OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
}))

MENU(menu_adap_lm, P99_PROTECT({
    { LNG("240p proc","240pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_240p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_240p_desc) } } },
    { LNG("288p proc","288pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_288p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_288p_desc) } } },
    { LNG("350-400p proc","350-400pｼｮﾘ"),       OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_384p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_384p_desc) } } },
    { LNG("480i proc","480iｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_480i,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_480i_desc) } } },
    { LNG("576i proc","576iｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_576i,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_576i_desc) } } },
    { LNG("480p proc","480pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_480p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_480p_desc) } } },
    { LNG("576p proc","576pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_576p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_576p_desc) } } },
    { LNG("720p proc","720pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_720p,      OPT_WRAP, SETTING_ITEM_LIST(pm_ad_720p_desc) } } },
    { LNG("1080i proc","1080iｼｮﾘ"),             OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_1080i,     OPT_WRAP, SETTING_ITEM_LIST(pm_ad_1080i_desc) } } },
    { LNG("1080p proc","1080pｼｮﾘ"),             OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_1080p,     OPT_WRAP, SETTING_ITEM_LIST(pm_ad_1080p_desc) } } },
    { LNG("240p/288p mode","240p/288pﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_240p_288p, OPT_WRAP, SETTING_ITEM_LIST(sm_ad_240p_288p_desc) } } },
    { LNG("350-400p mode","350-400pﾓｰﾄﾞ"),      OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_384p,      OPT_WRAP, SETTING_ITEM_LIST(sm_ad_384p_desc) } } },
    { LNG("480i/576i mode","480i/576iﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_480i_576i, OPT_WRAP, SETTING_ITEM_LIST(sm_ad_480i_576i_desc) } } },
    { LNG("480-512p mode","480-512pﾓｰﾄﾞ"),      OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_480p,      OPT_WRAP, SETTING_ITEM_LIST(sm_ad_480p_desc) } } },
    { LNG("576p mode","576pﾓｰﾄﾞ"),              OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_576p,      OPT_WRAP, SETTING_ITEM_LIST(sm_ad_576p_desc) } } },
}))

MENU(menu_lm, P99_PROTECT({
    { "Line multiplier mode",                   OPT_AVCONFIG_SELECTION, { .sel = { &tc.lm_mode,         OPT_WRAP, SETTING_ITEM(lm_mode_desc) } } },
    { "Deinterlace mode",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.lm_deint_mode,   OPT_WRAP, SETTING_ITEM(lm_deint_mode_desc) } } },
    { "NI restore Y offset",                    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.nir_even_offset, OPT_NOWRAP, 0, 1, value_disp } } },
    { LNG("256x240 aspect","256x240ｱｽﾍﾟｸﾄ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.ar_256col,       OPT_WRAP, SETTING_ITEM(ar_256col_desc) } } },
    { "Pure LM opt.",                           OPT_SUBMENU,            { .sub = { &menu_pure_lm, NULL, NULL } } },
    { "Adaptive LM opt.",                       OPT_SUBMENU,            { .sub = { &menu_adap_lm, NULL, NULL } } },
    { "P-LM Adv. timing",                       OPT_SUBMENU,            { .sub = { &menu_advtiming_plm, &vm_arg_info, vm_select } } },
    { "A-LM Adv. timing",                       OPT_SUBMENU,            { .sub = { &menu_advtiming,     &smp_arg_info, smp_select } } },
}))

#ifdef VIP
MENU(menu_scaler, P99_PROTECT({
    { "DFP output mode",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_out_mode,    OPT_WRAP, SETTING_ITEM_LIST(scl_out_mode_desc) } } },
    { "CRT output mode",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_crt_out_mode,OPT_WRAP, SETTING_ITEM_LIST(scl_crt_out_mode_desc) } } },
    { "Output type",                            OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_out_type,    OPT_WRAP, SETTING_ITEM_LIST(scl_out_type_desc) } } },
    { "Framelock",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_framelock,   OPT_WRAP, SETTING_ITEM_LIST(scl_framelock_desc) } } },
    { "Aspect ratio",                           OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_aspect,      OPT_WRAP, SETTING_ITEM_LIST(scl_aspect_desc) } } },
    { "Scaling algorithm",                      OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_alg,         OPT_WRAP, SETTING_ITEM_LIST(scl_alg_desc) } } },
    { "Edge threshold",                         OPT_AVCONFIG_NUMVALUE, { .num = { &tc.scl_edge_thold,  OPT_NOWRAP, 0, 255, value_disp } } },
#ifndef VIP_DIL_B
    { "Deinterlace mode",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_dil_alg,     OPT_WRAP, SETTING_ITEM_LIST(scl_dil_alg_desc) } } },
#endif
    { "Motion shift",                           OPT_AVCONFIG_NUMVALUE, { .num = { &tc.scl_dil_motion_shift,     OPT_NOWRAP, 0, 7, value_disp } } },
#ifdef VIP_DIL_B
    { "Motion scale",                           OPT_AVCONFIG_NUMVALUE, { .num = { &tc.scl_dil_motion_scale,     OPT_NOWRAP, 0, 255, value_disp } } },
    { "Cadence detection",                      OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_dil_cadence_detect_enable,     OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
#ifdef DEBUG
    { "Visualize motion",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_dil_visualize_motion,     OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
#endif
#endif
    { "Gen. samplerate",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.scl_gen_sr,       OPT_WRAP, SETTING_ITEM_LIST(scl_gen_sr_desc) } } },
    { LNG("240p/288p mode","240p/288pﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_scl_240p_288p, OPT_WRAP, SETTING_ITEM_LIST(sm_scl_240p_288p_desc) } } },
    { LNG("350-400p mode","350-400pﾓｰﾄﾞ"),      OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_scl_384p,      OPT_WRAP, SETTING_ITEM_LIST(sm_scl_384p_desc) } } },
    { LNG("480i/576i mode","480i/576iﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_scl_480i_576i, OPT_WRAP, SETTING_ITEM_LIST(sm_scl_480i_576i_desc) } } },
    { LNG("480-512p mode","480-512pﾓｰﾄﾞ"),      OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_scl_480p,      OPT_WRAP, SETTING_ITEM_LIST(sm_scl_480p_desc) } } },
    { LNG("576p mode","576pﾓｰﾄﾞ"),              OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_scl_576p,      OPT_WRAP, SETTING_ITEM_LIST(sm_scl_576p_desc) } } },
    { "Adv. timing",                            OPT_SUBMENU,            { .sub = { &menu_advtiming,     &smp_arg_info, smp_select } } },
}))
#endif

MENU(menu_output, P99_PROTECT({
    { "Operating mode",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.oper_mode,       OPT_WRAP, SETTING_ITEM(oper_mode_desc) } } },
    { "Test pattern mode",                      OPT_AVCONFIG_NUMVALUE, { .num = { &tc.tp_mode,          OPT_WRAP, 0, (NUM_VIDEO_MODES-1), vm_display_name} } },
    { LNG("TX mode","TXﾓｰﾄﾞ"),                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmitx_cfg.tx_mode,  OPT_WRAP, SETTING_ITEM_LIST(tx_mode_desc) } } },
    { "HDMI HDR flag",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmitx_cfg.hdr,      OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    //{ "HDMI ITC",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmi_itc,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
#ifndef DExx_FW
    { "1080p120 preset",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.timing_1080p120,  OPT_WRAP, SETTING_ITEM_LIST(timing_1080p120_desc) } } },
    { "2160p60 preset",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.timing_2160p60,   OPT_WRAP, SETTING_ITEM_LIST(timing_2160p60_desc) } } },
#endif
    { "Adv. disp timing",                      OPT_SUBMENU,            { .sub = { &menu_advtiming_out, &vm_out_arg_info, vm_out_select } } },
}))

MENU(menu_scanlines, P99_PROTECT({
    { LNG("Scanlines","ｽｷｬﾝﾗｲﾝ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_mode,     OPT_WRAP,   SETTING_ITEM_LIST(sl_mode_desc) } } },
    { LNG("Sl. strength","ｽｷｬﾝﾗｲﾝﾂﾖｻ"),         OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_str,      OPT_NOWRAP, 0, SCANLINESTR_MAX, sl_str_disp } } },
    { "Sl. hybrid str.",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_hybr_str, OPT_NOWRAP, 0, SL_HYBRIDSTR_MAX, sl_hybr_str_disp } } },
    { "Sl. method",                             OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_method,   OPT_WRAP,   SETTING_ITEM(sl_method_desc) } } },
    { "Sl. LM Bob altern.",                     OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_altern,   OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { LNG("Sl. alignment","ｽｷｬﾝﾗｲﾝﾎﾟｼﾞｼｮﾝ"),      OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_id,       OPT_WRAP,   SETTING_ITEM(sl_id_desc) } } },
    { LNG("Sl. type","ｽｷｬﾝﾗｲﾝﾙｲ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_type,     OPT_WRAP,   SETTING_ITEM_LIST(sl_type_desc) } } },
    { "Custom Sl.",                             OPT_SUBMENU,            { .sub = { &menu_cust_sl, NULL, NULL } } },
}))

MENU(menu_postproc, P99_PROTECT({
    { "Border color",                            OPT_AVCONFIG_SELECTION, { .sel = { &tc.mask_color,  OPT_NOWRAP,   SETTING_ITEM_LIST(mask_color_desc) } } },
    { LNG("Border brightness","ﾏｽｸｱｶﾙｻ"),         OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.mask_br,     OPT_NOWRAP, 0, HV_MASK_MAX_BR, value_disp } } },
    { "Shadow mask",                             OPT_AVCONFIG_SELECTION, { .sel = { &tc.shmask_mode, OPT_WRAP,   SETTING_ITEM_LIST(shmask_mode_desc) } } },
    { "BFI for 2x Hz",                           OPT_AVCONFIG_SELECTION, { .sel = { &tc.bfi_enable,  OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { "BFI strength",                            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.bfi_str,     OPT_NOWRAP, 0, SCANLINESTR_MAX, sl_str_disp } } },
    //{ LNG("DIY lat. test","DIYﾁｴﾝﾃｽﾄ"),         OPT_FUNC_CALL,          { .fun = { latency_test, &lt_arg_info } } },
}))

MENU(menu_audio, P99_PROTECT({
    { "Sampling format",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.audio_fmt,  OPT_WRAP, SETTING_ITEM_LIST(audio_fmt_desc) } } },
    { "Quad stereo",                            OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmitx_cfg.i2s_stereo_cfg, OPT_WRAP, SETTING_ITEM_LIST(audio_sr_desc) } } },
#ifdef INC_PCM186X
    { "Pre-ADC gain",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.pcm_cfg.gain,    OPT_NOWRAP, PCM_GAIN_M12DB, PCM_GAIN_12DB, aud_db_disp } } },
#endif
#ifndef DExx_FW
    { "AV1 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[0], OPT_NOWRAP, 0, 3, audio_src_disp } } },
    { "AV2 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[1], OPT_NOWRAP, 1, 3, audio_src_disp } } },
    { "AV3 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[2], OPT_NOWRAP, 1, 3, audio_src_disp } } },
    { "AV4 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[3], OPT_NOWRAP, 1, 4, audio_src_disp } } },
    { "3.5mm jack assign",                      OPT_AVCONFIG_SELECTION,  { .sel = { &tc.audmux_sel,     OPT_WRAP, SETTING_ITEM(audmux_sel_desc) } } },
#else
    { "AV1 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[0], OPT_NOWRAP, 0, 1, audio_src_disp } } },
#endif
}))

#ifndef DExx_FW
MENU(menu_exp, P99_PROTECT({
    { "Expansion select",                       OPT_AVCONFIG_SELECTION, { .sel = { &tc.exp_sel,  OPT_NOWRAP, SETTING_ITEM_LIST(exp_sel_desc) } } },
    { "Extra AV out mode",                      OPT_AVCONFIG_SELECTION, { .sel = { &tc.extra_av_out_mode,  OPT_NOWRAP, SETTING_ITEM_LIST(extra_av_out_mode_desc) } } },
}))
#endif


MENU(menu_settings, P99_PROTECT({
    { LNG("Initial input","ｼｮｷﾆｭｳﾘｮｸ"),          OPT_AVCONFIG_SELECTION, { .sel = { &ts.default_avinput,       OPT_WRAP, SETTING_ITEM_LIST(avinput_str) } } },
    //{ LNG("Link prof->input","Link prof->input"), OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.link_av,  OPT_WRAP, AV1_RGBs, AV_LAST, link_av_desc } } },
    //{ LNG("Link input->prof","Link input->prof"),   OPT_AVCONFIG_SELECTION, { .sel = { &profile_link,  OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    //{ "Autodetect input",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_input,     OPT_WRAP, SETTING_ITEM(auto_input_desc) } } },
    //{ "Auto AV1 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av1_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    //{ "Auto AV2 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av2_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    //{ "Auto AV3 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av3_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    { "OSD",                                    OPT_AVCONFIG_SELECTION, { .sel = { &ts.osd_enable,   OPT_WRAP,   SETTING_ITEM_LIST(osd_enable_desc) } } },
    { "OSD status disp.",                       OPT_AVCONFIG_SELECTION, { .sel = { &ts.osd_status_timeout,   OPT_WRAP,   SETTING_ITEM_LIST(osd_status_desc) } } },
#ifndef DExx_FW
    { "Fan PWM",                                OPT_AVCONFIG_NUMVALUE,  { .num = { &ts.fan_pwm,   OPT_NOWRAP, 0, 10,  pwm_disp } } },
    { "Led PWM",                                OPT_AVCONFIG_NUMVALUE,  { .num = { &ts.led_pwm,   OPT_NOWRAP, 1, 10,  pwm_disp } } },
#endif
    { "Bind IR remote",                         OPT_FUNC_CALL,          { .fun = { setup_rc, NULL } } },
#ifndef DE10N
    { LNG("Load profile","ﾌﾟﾛﾌｧｲﾙﾛｰﾄﾞ"),        OPT_FUNC_CALL,          { .fun = { load_profile, &profile_arg_info } } },
    { LNG("Save profile","ﾌﾟﾛﾌｧｲﾙｾｰﾌﾞ"),        OPT_FUNC_CALL,          { .fun = { save_profile, &profile_arg_info } } },
#endif
    { "SD Load profile" ,                      OPT_FUNC_CALL,          { .fun = { load_profile_sd, &sd_profile_arg_info } } },
    { "SD Save profile" ,                      OPT_FUNC_CALL,          { .fun = { save_profile_sd, &sd_profile_arg_info } } },
    { LNG("Reset profile","ｾｯﾃｲｵｼｮｷｶ"),          OPT_FUNC_CALL,          { .fun = { reset_profile, NULL } } },
#ifdef OSSC_PRO_FINAL_CFG
    { LNG("Fw. update","ﾌｧｰﾑｳｪｱｱｯﾌﾟﾃﾞｰﾄ"),       OPT_FUNC_CALL,          { .fun = { fw_update, NULL } } },
#endif
}))

MENU(menu_main, P99_PROTECT({
    { "Input select",                          OPT_AVCONFIG_SELECTION, { .sel = { &target_avinput,   OPT_WRAP,   SETTING_ONLY_LIST(avinput_str) } } },
    { "AV1-3 video in opt.",                    OPT_SUBMENU,            { .sub = { &menu_isl_video_opt, NULL, NULL } } },
    { "AV1-3 sync opt.",                        OPT_SUBMENU,            { .sub = { &menu_isl_sync_opt, NULL, NULL } } },
#ifdef INC_ADV761X
    { "AV4 video in opt.",                      OPT_SUBMENU,            { .sub = { &menu_adv_video_opt, NULL, NULL } } },
#endif
    { "Line multiplier opt",                    OPT_SUBMENU,            { .sub = { &menu_lm, NULL, NULL } } },
#ifdef VIP
    { "Scaler opt.",                            OPT_SUBMENU,            { .sub = { &menu_scaler, NULL, NULL } } },
#endif
    { LNG("Output opt.","ｼｭﾂﾘｮｸｵﾌﾟｼｮﾝ"),        OPT_SUBMENU,            { .sub = { &menu_output, NULL, NULL } } },
    { LNG("Audio opt.","ｵｰﾃﾞｨｵｵﾌﾟｼｮﾝ"),         OPT_SUBMENU,             { .sub = { &menu_audio, NULL, NULL } } },
    { LNG("Scanline opt.","ｽｷｬﾝﾗｲﾝｵﾌﾟｼｮﾝ"),     OPT_SUBMENU,            { .sub = { &menu_scanlines, NULL, NULL } } },
    { LNG("Post-proc.","ｱﾄｼｮﾘ"),                OPT_SUBMENU,            { .sub = { &menu_postproc, NULL, NULL } } },
#ifndef DExx_FW
    { "Expansion opt.",                         OPT_SUBMENU,            { .sub = { &menu_exp, NULL, NULL } } },
#endif
    { "Settings",                               OPT_SUBMENU,             { .sub = { &menu_settings, NULL, NULL } } },
}))

menuitem_t menu_profile_load = {"Profile load", OPT_CUSTOMMENU,         { .cstm = { &cstm_profile_load } } };


int is_menu_active() {
    return !!menu_active;
}

void init_menu() {
    menu_active = 0;
    memset(navi, 0, sizeof(navi));
    navi[0].m = &menu_main;
    navlvl = 0;
    cstm_f = NULL;

    // Set max ids for adv timing
    vm_arg_info.max = num_video_modes_plm-1;
    vm_out_arg_info.max = num_video_modes-1;
    smp_arg_info.max = num_smp_presets-1;

    // Setup OSD
    osd->osd_config.x_size = 0;
    osd->osd_config.y_size = 0;
    osd->osd_config.x_offset = 3;
    osd->osd_config.y_offset = 3;
    osd->osd_config.border_color = 1;
}

menunavi* get_current_menunavi() {
    return &navi[navlvl];
}

void write_option_name(menuitem_t *item)
{
    int i, offset;

    if ((item->type == OPT_FUNC_CALL) || ((item->type == OPT_SUBMENU) && item->sub.arg_info)) {
        offset = (US2066_ROW_LEN-strlen(item->name))/2;
        if (offset < 2)
            offset = 2;
        menu_row1[0] = '<';
        for (i=1; i<US2066_ROW_LEN-1; i++) {
            if ((i < offset) || (i >= offset+strlen(item->name)))
                menu_row1[i] = ' ';
            else
                menu_row1[i] = item->name[i-offset];
        }
        menu_row1[US2066_ROW_LEN-1] = '>';
        menu_row1[US2066_ROW_LEN] = 0;
    } else if ((item->type == OPT_SUBMENU) || (item->type == OPT_CUSTOMMENU) || ((item->type == OPT_AVCONFIG_SELECTION) && item->sel.list_view)) {
        for (i=0; i<US2066_ROW_LEN-1; i++) {
            if (i >= strlen(item->name))
                menu_row1[i] = ' ';
            else
                menu_row1[i] = item->name[i];
        }
        menu_row1[US2066_ROW_LEN-1] = '>';
        menu_row1[US2066_ROW_LEN] = 0;
    } else {
        strlcpy(menu_row1, item->name, US2066_ROW_LEN+1);
    }
}

void write_option_value(menuitem_t *item, int func_called, int retval)
{
    switch (item->type) {
        case OPT_AVCONFIG_SELECTION:
            if (item->sel.list_view < 2)
                strlcpy(menu_row2, item->sel.setting_str[*(item->sel.data)], US2066_ROW_LEN+1);
            else
                menu_row2[0] = 0;
            break;
        case OPT_AVCONFIG_NUMVALUE:
            item->num.df(*(item->num.data));
            break;
        case OPT_AVCONFIG_NUMVAL_U16:
            item->num_u16.df(item->num_u16.data);
            break;
        case OPT_SUBMENU:
            if (item->sub.arg_info)
                item->sub.arg_info->df(*item->sub.arg_info->data);
            else
                menu_row2[0] = 0;
            break;
        case OPT_CUSTOMMENU:
            menu_row2[0] = 0;
            break;
        case OPT_FUNC_CALL:
            if (func_called) {
                if (retval == 0)
                    strlcpy(menu_row2, "Done", US2066_ROW_LEN+1);
                else if (retval < 0)
                    sniprintf(menu_row2, US2066_ROW_LEN+1, "Failed (%d)", retval);
                else
                    strlcpy(menu_row2, func_ret_status, US2066_ROW_LEN+1);
            } else if (item->fun.arg_info) {
                item->fun.arg_info->df(*item->fun.arg_info->data);
            } else {
                menu_row2[0] = 0;
            }
            break;
        default:
            break;
    }
}

void render_osd_menu() {
    int i;
    menuitem_t *item;
    uint32_t row_mask[2] = {0, 0};

    if (!menu_active || (ts.osd_enable != 1))
        return;

    for (i=0; i < navi[navlvl].m->num_items; i++) {
        item = &navi[navlvl].m->items[i];
        write_option_name(item);
        strncpy((char*)osd->osd_array.data[i][0], menu_row1, OSD_CHAR_COLS);
        row_mask[0] |= (1<<i);

        if ((item->type != OPT_SUBMENU) && (item->type != OPT_CUSTOMMENU) && (item->type != OPT_FUNC_CALL) && !((item->type == OPT_AVCONFIG_SELECTION) && (item->sel.list_view == 2))) {
            write_option_value(item, 0, 0);
            strncpy((char*)osd->osd_array.data[i][1], menu_row2, OSD_CHAR_COLS);
            row_mask[1] |= (1<<i);
        }
    }

    osd->osd_sec_enable[0].mask = row_mask[0];
    osd->osd_sec_enable[1].mask = row_mask[1];
}

void cstm_clock_phase(menucode_id code, int setup_disp) {
    uint32_t row_mask[2] = {0x3ff, 0x3f0};
    smp_preset_t *smp = &smp_presets[smp_edit];
    char clock_disp[US2066_ROW_LEN+1], phase_disp[US2066_ROW_LEN+1];
    int i;
    int active_mode = smp_is_active();
    static int sr_step;
    static const char *sr_step_str[] = {"1", "10", "0.05"};

#ifndef DExx_FW
    if (advrx_dev.powered_on && advrx_dev.sync_active) {
        sniprintf(menu_row1, US2066_ROW_LEN+1, "Not applicable");
        sniprintf(menu_row2, US2066_ROW_LEN+1, "for HDMI");
        ui_disp_menu(1);

        return;
    }
#endif

    // Samplerate step can be only integer for non-oversampled modes
    if ((sr_step == 2) && !smp->h_skip)
        sr_step = 0;

    if (setup_disp) {
        memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));

        sniprintf((char*)osd->osd_array.data[0][0], OSD_CHAR_COLS, "       Phase+");
        sniprintf((char*)osd->osd_array.data[1][0], OSD_CHAR_COLS, "         ^");
        sniprintf((char*)osd->osd_array.data[2][0], OSD_CHAR_COLS, "Clock- < \x85 > Clock+");
        sniprintf((char*)osd->osd_array.data[3][0], OSD_CHAR_COLS, "         v");
        sniprintf((char*)osd->osd_array.data[4][0], OSD_CHAR_COLS, "       Phase-");
        sniprintf((char*)osd->osd_array.data[4][1], OSD_CHAR_COLS, "%c%s", (active_mode ? '*' : ' '), smp->name);
        sniprintf((char*)osd->osd_array.data[5][1], OSD_CHAR_COLS, "--------------------");

        sniprintf((char*)osd->osd_array.data[6][0], OSD_CHAR_COLS, "H. samplerate");
        sniprintf((char*)osd->osd_array.data[7][0], OSD_CHAR_COLS, "Sampling phase");

        sniprintf((char*)osd->osd_array.data[9][0], OSD_CHAR_COLS, "\x85 H. sr. step");

        osd->osd_sec_enable[0].mask = row_mask[0];
        osd->osd_sec_enable[1].mask = row_mask[1];
        osd->osd_row_color.mask = 0;
    }

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
        smp->sampler_phase = (smp->sampler_phase == SAMPLER_PHASE_MAX) ? 0 : smp->sampler_phase+1;
        if (active_mode)
            set_sampler_phase(smp->sampler_phase, 1, 1);
        break;
    case NEXT_PAGE:
        smp->sampler_phase = (smp->sampler_phase == 0) ? SAMPLER_PHASE_MAX : smp->sampler_phase-1;
        if (active_mode)
            set_sampler_phase(smp->sampler_phase, 1, 1);
        break;
    case VAL_MINUS:
        if (sr_step == 2) {
            if (smp->timings_i.h_total_adj == 0) {
                smp->timings_i.h_total_adj = H_TOTAL_ADJ_MAX;
                smp->timings_i.h_total--;
            } else {
                smp->timings_i.h_total_adj--;
            }
        } else {
            smp->timings_i.h_total -= (sr_step ? 10 : 1);
        }
        if (smp->timings_i.h_total < H_TOTAL_MIN) {
            smp->timings_i.h_total = H_TOTAL_MIN;
            smp->timings_i.h_total_adj = 0;
        }
        if (active_mode)
            update_cur_vm = 1;
        break;
    case VAL_PLUS:
        if (sr_step == 2) {
            if (smp->timings_i.h_total_adj == H_TOTAL_ADJ_MAX) {
                smp->timings_i.h_total_adj = 0;
                smp->timings_i.h_total++;
            } else {
                smp->timings_i.h_total_adj++;
            }
        } else {
            smp->timings_i.h_total += (sr_step ? 10 : 1);
        }
        if ((smp->timings_i.h_total > H_TOTAL_MAX) || ((smp->timings_i.h_total == H_TOTAL_MAX) && (smp->timings_i.h_total_adj != 0))) {
            smp->timings_i.h_total = H_TOTAL_MAX;
            smp->timings_i.h_total_adj = 0;
        }
        if (active_mode)
            update_cur_vm = 1;
        break;
    case OPT_SELECT:
        sr_step = (sr_step + 1) % (smp->h_skip ? 3 : 2);
        break;
    default:
        break;
    }

    if (smp->h_skip)
        sniprintf(clock_disp, US2066_ROW_LEN+1, "%u.%.2u (x%u = %u)", smp->timings_i.h_total, smp->timings_i.h_total_adj*5,
                                                                       smp->h_skip+1,
                                                                       (smp->h_skip+1) * smp->timings_i.h_total + (((smp->h_skip+1) * smp->timings_i.h_total_adj * 5 + 50) / 100));
    else
        sniprintf(clock_disp, US2066_ROW_LEN+1, "%u", smp->timings_i.h_total);
    sampler_phase_disp(phase_disp, US2066_ROW_LEN+1, smp->sampler_phase, active_mode);

    strncpy((char*)osd->osd_array.data[6][1], clock_disp, OSD_CHAR_COLS);
    strncpy((char*)osd->osd_array.data[7][1], phase_disp, OSD_CHAR_COLS);
    strncpy((char*)osd->osd_array.data[9][1], sr_step_str[sr_step], OSD_CHAR_COLS);
    sniprintf(menu_row1, US2066_ROW_LEN+1, "Clock: %s", clock_disp);
    sniprintf(menu_row2, US2066_ROW_LEN+1, "Phase: %s", phase_disp);

    ui_disp_menu(0);
}

void cstm_size(menucode_id code, int setup_disp) {
    uint32_t row_mask[2] = {0xfff, 0xff0};
    sync_timings_t *st;
    const char *mode_name;
    int i, adj=0;
    static int size_step_idx;
    static uint8_t size_step_arr[] = {1, 4, 10};
    int active_mode = smp_is_active();

#ifndef DExx_FW
    if (advrx_dev.powered_on && advrx_dev.sync_active) {
        st = active_mode ? &vmode_in.timings : &hdmi_timings[dtmg_edit];
        mode_name = hdmi_timings_groups[dtmg_edit % NUM_VIDEO_GROUPS];
    } else
#endif
    {
        st = &smp_presets[smp_edit].timings_i;
        mode_name = smp_presets[smp_edit].name;
        if (active_mode)
            st->v_total = vmode_in.timings.v_total;
    }

    if (setup_disp) {
        memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));

        sniprintf((char*)osd->osd_array.data[0][0], OSD_CHAR_COLS, "       ZoomY+");
        sniprintf((char*)osd->osd_array.data[1][0], OSD_CHAR_COLS, "         ^");
        sniprintf((char*)osd->osd_array.data[2][0], OSD_CHAR_COLS, "ZoomX- < \x85 > ZoomX+");
        sniprintf((char*)osd->osd_array.data[3][0], OSD_CHAR_COLS, "         v");
        sniprintf((char*)osd->osd_array.data[4][0], OSD_CHAR_COLS, "       ZoomY-");
        sniprintf((char*)osd->osd_array.data[4][1], OSD_CHAR_COLS, "%c%s", (active_mode ? '*' : ' '), mode_name);
        sniprintf((char*)osd->osd_array.data[5][1], OSD_CHAR_COLS, "--------------------");

        sniprintf((char*)osd->osd_array.data[6][0], OSD_CHAR_COLS, "H. active");
        sniprintf((char*)osd->osd_array.data[7][0], OSD_CHAR_COLS, "V. active");

        sniprintf((char*)osd->osd_array.data[9][0], OSD_CHAR_COLS, "\x85 Stepsize");
        sniprintf((char*)osd->osd_array.data[10][0], OSD_CHAR_COLS, "(H. backporch)");
        sniprintf((char*)osd->osd_array.data[11][0], OSD_CHAR_COLS, "(V. backporch)");

        osd->osd_sec_enable[0].mask = row_mask[0];
        osd->osd_sec_enable[1].mask = row_mask[1];
        osd->osd_row_color.mask = 0;
    }

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
    case NEXT_PAGE:
        adj = (code == PREV_PAGE) ? -size_step_arr[size_step_idx] : size_step_arr[size_step_idx];
        if ((int)st->v_active+adj < V_ACTIVE_MIN)
            adj = V_ACTIVE_MIN - st->v_active;
        else if ((int)st->v_active+adj > V_ACTIVE_MAX)
            adj = V_ACTIVE_MAX - st->v_active;

        if ((int)st->v_backporch+(adj/2)+st->v_synclen+st->v_active > st->v_total)
            adj = 2*(st->v_total-(int)st->v_backporch-st->v_synclen-st->v_active);

        if ((adj == -1) || (adj == 1)) {
            if (((int)st->v_backporch - adj >= V_BPORCH_MIN) && ((int)st->v_backporch - adj <= V_BPORCH_MAX))
                st->v_backporch -= !(st->v_active % 2) ? adj : 0;
            else
                adj = 0;
        } else {
            if (((int)st->v_backporch - (adj/2) >= V_BPORCH_MIN) && ((int)st->v_backporch - (adj/2) <= V_BPORCH_MAX))
                st->v_backporch -= adj/2;
            else
                adj = 0;
        }

        st->v_active += adj;
        break;
    case VAL_MINUS:
    case VAL_PLUS:
        adj = (code == VAL_PLUS) ? -size_step_arr[size_step_idx] : size_step_arr[size_step_idx];
        if ((int)st->h_active+adj < H_ACTIVE_MIN)
            adj = H_ACTIVE_MIN - st->h_active;
        else if ((int)st->h_active+adj > H_ACTIVE_SMP_MAX)
            adj = H_ACTIVE_SMP_MAX - st->h_active;

        if ((int)st->h_backporch+adj+st->h_synclen+st->h_active > st->h_total)
            adj = 0;

        if ((adj == -1) || (adj == 1)) {
            if (((int)st->h_backporch - adj >= H_BPORCH_MIN) && ((int)st->h_backporch - adj <= H_BPORCH_MAX))
                st->h_backporch -= !(st->h_active % 2) ? adj : 0;
            else
                adj = 0;
        } else {
            if (((int)st->h_backporch - (adj/2) >= H_BPORCH_MIN) && ((int)st->h_backporch - (adj/2) <= H_BPORCH_MAX))
                st->h_backporch -= adj/2;
            else
                adj = 0;
        }

        st->h_active += adj;
        break;
    case OPT_SELECT:
        size_step_idx = (size_step_idx + 1) % (sizeof(size_step_arr)/sizeof(uint8_t));
        break;
    default:
        break;
    }

    if (active_mode && (adj != 0)) {
        update_cur_vm = 1;
#ifndef DExx_FW
        if (advrx_dev.powered_on && advrx_dev.sync_active)
            memcpy(&hdmi_timings[dtmg_edit], &vmode_in.timings, sizeof(sync_timings_t));
#endif
    }

    // clear rows
    for (i=6; i<=11; i++)
        strncpy((char*)osd->osd_array.data[i][1], "", OSD_CHAR_COLS);

    sniprintf((char*)osd->osd_array.data[6][1], OSD_CHAR_COLS, "%u", st->h_active);
    sniprintf((char*)osd->osd_array.data[7][1], OSD_CHAR_COLS, "%u", st->v_active);
    sniprintf((char*)osd->osd_array.data[9][1], OSD_CHAR_COLS, "%u", size_step_arr[size_step_idx]);
    sniprintf((char*)osd->osd_array.data[10][1], OSD_CHAR_COLS, "%u", st->h_backporch);
    sniprintf((char*)osd->osd_array.data[11][1], OSD_CHAR_COLS, "%u", st->v_backporch);
    sniprintf(menu_row1, US2066_ROW_LEN+1, "H.bp: %u  V.bp: %u", st->h_backporch, st->v_backporch);
    sniprintf(menu_row2, US2066_ROW_LEN+1, "Active: %ux%u", st->h_active, st->v_active);

    ui_disp_menu(0);
}

void cstm_position(menucode_id code, int setup_disp) {
    uint32_t row_mask[2] = {0x3ff, 0x3f0};
    sync_timings_t *st;
    const char *mode_name;
    int i, adj=0;
    static int pos_step_idx;
    static uint8_t pos_step_arr[] = {1, 4, 10};
    int active_mode = smp_is_active();

#ifndef DExx_FW
    if (advrx_dev.powered_on && advrx_dev.sync_active) {
        st = active_mode ? &vmode_in.timings : &hdmi_timings[dtmg_edit];
        mode_name = hdmi_timings_groups[dtmg_edit % NUM_VIDEO_GROUPS];
    } else
#endif
    {
        st = &smp_presets[smp_edit].timings_i;
        mode_name = smp_presets[smp_edit].name;
        if (active_mode)
            st->v_total = vmode_in.timings.v_total;
    }

    if (setup_disp) {
        memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));

        sniprintf((char*)osd->osd_array.data[0][0], OSD_CHAR_COLS, "       PosY-");
        sniprintf((char*)osd->osd_array.data[1][0], OSD_CHAR_COLS, "         ^");
        sniprintf((char*)osd->osd_array.data[2][0], OSD_CHAR_COLS, " PosX- < \x85 > PosX+");
        sniprintf((char*)osd->osd_array.data[3][0], OSD_CHAR_COLS, "         v");
        sniprintf((char*)osd->osd_array.data[4][0], OSD_CHAR_COLS, "       PosY+");
        sniprintf((char*)osd->osd_array.data[4][1], OSD_CHAR_COLS, "%c%s", (active_mode ? '*' : ' '), mode_name);
        sniprintf((char*)osd->osd_array.data[5][1], OSD_CHAR_COLS, "--------------------");

        sniprintf((char*)osd->osd_array.data[6][0], OSD_CHAR_COLS, "H. backporch");
        sniprintf((char*)osd->osd_array.data[7][0], OSD_CHAR_COLS, "V. backporch");

        sniprintf((char*)osd->osd_array.data[9][0], OSD_CHAR_COLS, "\x85 Stepsize");

        osd->osd_sec_enable[0].mask = row_mask[0];
        osd->osd_sec_enable[1].mask = row_mask[1];
        osd->osd_row_color.mask = 0;
    }

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
    case NEXT_PAGE:
        adj = (code == NEXT_PAGE) ? -pos_step_arr[pos_step_idx] : pos_step_arr[pos_step_idx];

        if ((int)st->v_backporch+adj < V_BPORCH_MIN)
            adj = V_BPORCH_MIN - st->v_backporch;
        else if ((int)st->v_backporch+adj > V_BPORCH_MAX)
            adj = V_BPORCH_MAX - st->v_backporch;

        if ((int)st->v_backporch+adj+st->v_synclen+st->v_active > st->v_total)
            adj = st->v_total - st->v_backporch - st->v_synclen - st->v_active;

        st->v_backporch += adj;
        break;
    case VAL_MINUS:
    case VAL_PLUS:
        adj = (code == VAL_PLUS) ? -pos_step_arr[pos_step_idx] : pos_step_arr[pos_step_idx];

        if ((int)st->h_backporch+adj < H_BPORCH_MIN)
            adj = H_BPORCH_MIN - st->h_backporch;
        else if ((int)st->h_backporch+adj > H_BPORCH_MAX)
            adj = H_BPORCH_MAX - st->h_backporch;

        if ((int)st->h_backporch+adj+st->h_synclen+st->h_active > st->h_total)
            adj = st->h_total - st->h_backporch - st->h_synclen - st->h_active;

        st->h_backporch += adj;
        break;
    case OPT_SELECT:
        pos_step_idx = (pos_step_idx + 1) % (sizeof(pos_step_arr)/sizeof(uint8_t));
        break;
    default:
        break;
    }

    if (active_mode && (adj != 0)) {
        update_cur_vm = 1;
#ifndef DExx_FW
        if (advrx_dev.powered_on && advrx_dev.sync_active)
            memcpy(&hdmi_timings[dtmg_edit], &vmode_in.timings, sizeof(sync_timings_t));
#endif
    }

    // clear rows
    for (i=6; i<=9; i++)
        strncpy((char*)osd->osd_array.data[i][1], "", OSD_CHAR_COLS);

    sniprintf((char*)osd->osd_array.data[6][1], OSD_CHAR_COLS, "%u", st->h_backporch);
    sniprintf((char*)osd->osd_array.data[7][1], OSD_CHAR_COLS, "%u", st->v_backporch);
    sniprintf((char*)osd->osd_array.data[9][1], OSD_CHAR_COLS, "%u", pos_step_arr[pos_step_idx]);
    sniprintf(menu_row1, US2066_ROW_LEN+1, "H. backporch: %u", st->h_backporch);
    sniprintf(menu_row2, US2066_ROW_LEN+1, "V. backporch: %u", st->v_backporch);

    ui_disp_menu(0);
}

void cstm_profile_load(menucode_id code, int setup_disp) {
    uint32_t row_mask[2] = {0x03, 0x00};
    int i, retval, items_curpage;
    static int nav = 0;
#ifdef DE10N
    static int page = 1;
#else
    static int page = 0;
#endif

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
        nav--;
        break;
    case NEXT_PAGE:
        nav++;
        break;
    case VAL_MINUS:
#ifdef DE10N
        page = (page == 1) ? 5 : (page - 1);
#else
        page = (page == 0) ? 5 : (page - 1);
#endif
        setup_disp = 1;
        break;
    case VAL_PLUS:
#ifdef DE10N
        page = (page == 5) ? 1 : (page + 1);
#else
        page = (page + 1) % 6;
#endif
        setup_disp = 1;
        break;
    case OPT_SELECT:
        if (page == 0)
            retval = read_userdata(nav, 0);
        else
            retval = read_userdata_sd((page-1)*20+nav, 0);
        if (retval < 0)
            sniprintf((char*)osd->osd_array.data[nav+2][1], OSD_CHAR_COLS, "Failed (%d)", retval);
        else
            sniprintf((char*)osd->osd_array.data[nav+2][1], OSD_CHAR_COLS, (retval > 0) ? func_ret_status : "OK");
        row_mask[1] = (1<<(nav+2));
        osd->osd_sec_enable[1].mask = row_mask[1];
        break;
    default:
        break;
    }

    items_curpage = (page == 0) ? MAX_PROFILE+1 : (MAX_SD_PROFILE+1 - (page-1)*20 >= 20 ? 20 : (MAX_SD_PROFILE % 20));

    if (nav < 0)
        nav = items_curpage-1;
    else if (nav >= items_curpage)
        nav = 0;

    if (setup_disp) {
        memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));

        sniprintf((char*)osd->osd_array.data[0][0], OSD_CHAR_COLS, "%s (%s)", menu_profile_load.name, ((page == 0) ? "int" : "SD"));
        sniprintf(menu_row1, US2066_ROW_LEN+1, "%s (%s)", menu_profile_load.name, ((page == 0) ? "int" : "SD"));
        for (i=0; i<OSD_CHAR_COLS; i++)
            osd->osd_array.data[1][0][i] = '-';

        for (i=0; i<items_curpage; i++) {
            if (page == 0)
                retval = read_userdata(i, 1);
            else
                retval = read_userdata_sd((page-1)*20+i, 1);
            sniprintf((char*)osd->osd_array.data[i+2][0], OSD_CHAR_COLS, "%u: %s", (page == 0) ? i : (page-1)*20+i, (retval != 0) ? "<empty>" : target_profile_name);
            row_mask[0] |= (1<<(i+2));
        }

        sniprintf((char*)osd->osd_array.data[i+3][0], OSD_CHAR_COLS, "< Prev       Next >");
        row_mask[0] |= (3<<(i+2));

        osd->osd_sec_enable[0].mask = row_mask[0];
        osd->osd_sec_enable[1].mask = row_mask[1];
    }

    osd->osd_row_color.mask = (1<<(nav+2));

    if (page == 0)
        retval = read_userdata(nav, 1);
    else
        retval = read_userdata_sd((page-1)*20+nav, 1);

    sniprintf(menu_row2, US2066_ROW_LEN+1, "%u: %s", (page == 0) ? nav : (page-1)*20+nav, (retval != 0) ? "<empty>" : target_profile_name);

    ui_disp_menu(0);
}

void cstm_listview(menucode_id code, int setup_disp) {
    uint32_t row_mask[2] = {0x03, 0x00};
    int i;

    if (setup_disp) {
        memset((void*)osd->osd_array.data, 0, sizeof(osd_char_array));
        lw_mp = *lw_item->sel.data;

        sniprintf((char*)osd->osd_array.data[0][0], OSD_CHAR_COLS, "%s", lw_item->name);
        strlcpy(menu_row1, lw_item->name, US2066_ROW_LEN+1);
        for (i=0; i<OSD_CHAR_COLS; i++)
            osd->osd_array.data[1][0][i] = '-';

        for (i=0; i<=lw_item->sel.max; i++) {
            sniprintf((char*)osd->osd_array.data[i+2][0], OSD_CHAR_COLS, "%c%s", (i==lw_mp? '*' : ' '), lw_item->sel.setting_str[i]);
            row_mask[0] |= (1<<(i+2));
        }
        osd->osd_sec_enable[0].mask = row_mask[0];
        osd->osd_sec_enable[1].mask = row_mask[1];
        osd->osd_row_color.mask = (1<<(lw_mp+2));
    }

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
        lw_mp = (lw_mp > 0) ? (lw_mp - 1) : lw_item->sel.max;
        osd->osd_row_color.mask = (1<<(lw_mp+2));
        break;
    case NEXT_PAGE:
        lw_mp = (lw_mp < lw_item->sel.max) ? (lw_mp + 1) : 0;
        osd->osd_row_color.mask = (1<<(lw_mp+2));
        break;
    case OPT_SELECT:
        osd->osd_array.data[*lw_item->sel.data+2][0][0] = ' ';
        *lw_item->sel.data = lw_mp;
        osd->osd_array.data[lw_mp+2][0][0] = '*';
        break;
    default:
        break;
    }

    sniprintf(menu_row2, US2066_ROW_LEN+1, "%c%s", (*lw_item->sel.data==lw_mp? '*' : ' '), lw_item->sel.setting_str[lw_mp]);

    ui_disp_menu(0);
}

void enter_cstm(menuitem_t *item, int detached_mode) {
    if (detached_mode) {
        navlvl = 0;
        menu_active = 1;
        osd->osd_config.menu_active = menu_active;
    }
    if (item->type == OPT_AVCONFIG_SELECTION) {
        lw_item = item;
        cstm_f = cstm_listview;
    } else {
        cstm_f = item->cstm.cstm_f;
    }
    cstm_f(NO_ACTION, 1);
}

void quick_adjust(menuitem_t *item, int adj, int is_relative) {
    int adj_data = is_relative ? ((int)(*item->num.data) + adj) : adj;

    if (adj_data < (int)item->num.min)
        *item->num.data = item->num.wrap_cfg ? item->num.max : item->num.min;
    else if (adj_data > (int)item->num.max)
        *item->num.data = item->num.wrap_cfg ? item->num.min : item->num.max;
    else
        *item->num.data = (uint8_t)adj_data;

    strncpy((char*)osd->osd_array.data[0][0], item->name, OSD_CHAR_COLS);
    if (item->type == OPT_AVCONFIG_SELECTION) {
        strncpy((char*)osd->osd_array.data[1][0], item->sel.setting_str[*item->sel.data], OSD_CHAR_COLS);
    } else {
        item->num.df(*item->num.data);
        strncpy((char*)osd->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
    }
    osd->osd_config.status_refresh = 1;
    osd->osd_row_color.mask = 0;
    osd->osd_sec_enable[0].mask = 3;
    osd->osd_sec_enable[1].mask = 0;
}

void quick_adjust_phase(uint8_t dir) {
    uint8_t *sampler_phase;

    sampler_phase = (oper_mode == OPERMODE_PURE_LM) ? &video_modes_plm[vm_cur].sampler_phase : &smp_presets[smp_cur].sampler_phase;

    if (isl_dev.powered_on && isl_dev.sync_active) {
        if (dir)
            *sampler_phase = (*sampler_phase == SAMPLER_PHASE_MAX) ? 0 : *sampler_phase+1;
        else
            *sampler_phase = (*sampler_phase == 0) ? SAMPLER_PHASE_MAX : *sampler_phase-1;

        set_sampler_phase(*sampler_phase, 1, 1);

        strncpy((char*)osd->osd_array.data[0][0], menu_advtiming_plm_items[10].name, OSD_CHAR_COLS);
        sampler_phase_disp(menu_row2, US2066_ROW_LEN+1, *sampler_phase, 1);
        strncpy((char*)osd->osd_array.data[1][0], menu_row2, OSD_CHAR_COLS);
        osd->osd_config.status_refresh = 1;
        osd->osd_row_color.mask = 0;
        osd->osd_sec_enable[0].mask = 3;
        osd->osd_sec_enable[1].mask = 0;
    }
}

void display_menu(rc_code_t rcode, btn_code_t bcode)
{
    menucode_id code = NO_ACTION;
    menuitem_t *item;
    uint8_t *val, val_wrap, val_min, val_max;
    uint16_t *val_u16, val_u16_min, val_u16_max;
    int i, func_called = 0, retval = 0, forcedisp=0;

    if ((rcode == RC_MENU) || (!menu_active && (bcode == BC_PRESS))) {
        menu_active ^= 1;
        osd->osd_config.menu_active = menu_active;
        if (menu_active)
            render_osd_menu();
    } else if ((rcode >= RC_OK) && (rcode < RC_INFO)) {
        code = OPT_SELECT+(rcode-RC_OK);
    } else if (bcode != (btn_code_t)-1) {
        code = OPT_SELECT+(bcode-BC_OK);
    }

    if (!menu_active) {
        cstm_f = NULL;
        ui_disp_status(0);
        return;
    }

    // Custom menu function
    if ((cstm_f != NULL) && (code != PREV_MENU)) {
        cstm_f(code, 0);

        return;
    }

    item = &navi[navlvl].m->items[navi[navlvl].mp];

    // Parse menu control
    switch (code) {
    case PREV_PAGE:
    case NEXT_PAGE:
        if ((item->type == OPT_FUNC_CALL) || (item->type == OPT_SUBMENU))
            osd->osd_sec_enable[1].mask &= ~(1<<navi[navlvl].mp);

        if (code == PREV_PAGE)
            navi[navlvl].mp = (navi[navlvl].mp == 0) ? navi[navlvl].m->num_items-1 : (navi[navlvl].mp-1);
        else
            navi[navlvl].mp = (navi[navlvl].mp+1) % navi[navlvl].m->num_items;
        break;
    case PREV_MENU:
        if (navlvl > 0) {
            if (cstm_f != NULL)
                cstm_f = NULL;
            else
                navlvl--;
            render_osd_menu();
        } else {
            menu_active = 0;
            osd->osd_config.menu_active = 0;
            cstm_f = NULL;
            ui_disp_status(0);
            return;
        }
        break;
    case OPT_SELECT:
        switch (item->type) {
            case OPT_SUBMENU:
                if (item->sub.arg_f)
                    item->sub.arg_f();

                if (navi[navlvl+1].m != item->sub.menu)
                    navi[navlvl+1].mp = 0;
                navi[navlvl+1].m = item->sub.menu;
                navlvl++;
                render_osd_menu();

                break;
            case OPT_CUSTOMMENU:
                enter_cstm(item, 0);
                return;
                break;
            case OPT_AVCONFIG_SELECTION:
                if (item->sel.list_view) {
                    enter_cstm(item, 0);
                    return;
                }
                break;
            case OPT_FUNC_CALL:
                retval = item->fun.f();
                func_called = 1;
                break;
            default:
                break;
        }
        break;
    case VAL_MINUS:
    case VAL_PLUS:
        switch (item->type) {
            case OPT_AVCONFIG_SELECTION:
            case OPT_AVCONFIG_NUMVALUE:
                val = item->sel.data;
                val_wrap = item->sel.wrap_cfg;
                val_min = item->sel.min;
                val_max = item->sel.max;

                if ((item->type != OPT_AVCONFIG_SELECTION) || (item->sel.list_view < 2)) {
                    if (code == VAL_MINUS)
                        *val = (*val > val_min) ? (*val-1) : (val_wrap ? val_max : val_min);
                    else
                        *val = (*val < val_max) ? (*val+1) : (val_wrap ? val_min : val_max);
                }
                break;
            case OPT_AVCONFIG_NUMVAL_U16:
                val_u16 = item->num_u16.data;
                val_u16_min = item->num_u16.min;
                val_u16_max = item->num_u16.max;
                val_wrap = (val_u16_min == 0);
                if (code == VAL_MINUS)
                    *val_u16 = (*val_u16 > val_u16_min) ? (*val_u16-1) : (val_wrap ? val_u16_max : val_u16_min);
                else
                    *val_u16 = (*val_u16 < val_u16_max) ? (*val_u16+1) : (val_wrap ? val_u16_min : val_u16_max);
                break;
            case OPT_SUBMENU:
                val = item->sub.arg_info->data;
                val_max = item->sub.arg_info->max;

                if (item->sub.arg_info) {
                    if (code == VAL_MINUS)
                        *val = (*val > 0) ? (*val-1) : 0;
                    else
                        *val = (*val < val_max) ? (*val+1) : val_max;
                }
                break;
            case OPT_FUNC_CALL:
                val = item->fun.arg_info->data;
                val_max = item->fun.arg_info->max;

                if (item->fun.arg_info) {
                    if (code == VAL_MINUS)
                        *val = (*val > 0) ? (*val-1) : 0;
                    else
                        *val = (*val < val_max) ? (*val+1) : val_max;
                }
                break;
            default:
                break;
        }
        break;
    default:
        break;
    }

    // Generate menu text
    if (func_called)
        render_osd_menu();
    item = &navi[navlvl].m->items[navi[navlvl].mp];
    write_option_name(item);
    write_option_value(item, func_called, retval);
    strncpy((char*)osd->osd_array.data[navi[navlvl].mp][1], menu_row2, OSD_CHAR_COLS);
    osd->osd_row_color.mask = (1<<navi[navlvl].mp);
    if (func_called || ((item->type == OPT_FUNC_CALL) && item->fun.arg_info != NULL) || ((item->type == OPT_SUBMENU) && item->sub.arg_info != NULL))
        osd->osd_sec_enable[1].mask |= (1<<navi[navlvl].mp);

    ui_disp_menu(0);
}

void set_func_ret_msg(char *msg) {
    strlcpy(func_ret_status, msg, US2066_ROW_LEN+1);
}

void update_osd_size(mode_data_t *vm_out) {
    uint8_t osd_size = vm_out->timings.v_active / 700;
    uint8_t par_x4 = (((400*vm_out->timings.h_active*vm_out->ar.v)/((vm_out->timings.v_active<<vm_out->timings.interlaced)*vm_out->ar.h))+50)/100;
    int8_t xadj_log2 = -2;

    while (par_x4 > 1) {
        par_x4 >>= 1;
        xadj_log2++;
    }

    osd->osd_config.x_size = ((osd_size + vm_out->timings.interlaced + xadj_log2) >= 0) ? (osd_size + vm_out->timings.interlaced + xadj_log2) : 0;
    osd->osd_config.y_size = osd_size;
}

static void vm_select() {
    vm_edit = vm_sel;
    tc_h_samplerate = video_modes_plm[vm_edit].timings.h_total;
    tc_h_samplerate_adj = (uint16_t)video_modes_plm[vm_edit].timings.h_total_adj;
    tc_h_synclen = (uint16_t)video_modes_plm[vm_edit].timings.h_synclen;
    tc_h_bporch = video_modes_plm[vm_edit].timings.h_backporch;
    tc_h_active = video_modes_plm[vm_edit].timings.h_active;
    tc_v_synclen = (uint16_t)video_modes_plm[vm_edit].timings.v_synclen;
    tc_v_bporch = video_modes_plm[vm_edit].timings.v_backporch;
    tc_v_active = video_modes_plm[vm_edit].timings.v_active;
    tc_h_mask = (uint16_t)video_modes_plm[vm_edit].mask.h;
    tc_v_mask = (uint16_t)video_modes_plm[vm_edit].mask.v;
    tc_sampler_phase = video_modes_plm[vm_edit].sampler_phase;
}

static void vm_out_select() {
    vm_out_edit = vm_out_sel;
    tc_h_samplerate = video_modes[vm_out_edit].timings.h_total;
    tc_h_synclen = (uint16_t)video_modes[vm_out_edit].timings.h_synclen;
    tc_h_bporch = video_modes[vm_out_edit].timings.h_backporch;
    tc_h_active = video_modes[vm_out_edit].timings.h_active;
    tc_v_synclen = (uint16_t)video_modes[vm_out_edit].timings.v_synclen;
    tc_v_bporch = video_modes[vm_out_edit].timings.v_backporch;
    tc_v_active = video_modes[vm_out_edit].timings.v_active;
    tc_v_total = video_modes[vm_out_edit].timings.v_total;
    tc_v_hz = video_modes[vm_out_edit].timings.v_hz_x100 / 100;
    tc_v_hz_frac = video_modes[vm_out_edit].timings.v_hz_x100 % 100;
    // Reuse variables with different names
    tc_h_mask = (uint16_t)video_modes[vm_out_edit].ar.h;
    tc_v_mask = (uint16_t)video_modes[vm_out_edit].ar.v;
}

static void vm_tweak(uint16_t *v) {
    int active_mode = (isl_dev.powered_on && isl_dev.sync_active && (vm_cur == vm_edit) && (oper_mode == OPERMODE_PURE_LM));

    if (active_mode) {
        if ((video_modes_plm[vm_cur].timings.h_total != tc_h_samplerate) ||
            (video_modes_plm[vm_cur].timings.h_total_adj != (uint8_t)tc_h_samplerate_adj) ||
            (video_modes_plm[vm_cur].timings.h_synclen != (uint8_t)tc_h_synclen) ||
            (video_modes_plm[vm_cur].timings.h_backporch != tc_h_bporch) ||
            (video_modes_plm[vm_cur].timings.h_active != tc_h_active) ||
            (video_modes_plm[vm_cur].timings.v_synclen != (uint8_t)tc_v_synclen) ||
            (video_modes_plm[vm_cur].timings.v_backporch != tc_v_bporch) ||
            (video_modes_plm[vm_cur].timings.v_active != tc_v_active) ||
            (video_modes_plm[vm_cur].mask.h != tc_h_mask) ||
            (video_modes_plm[vm_cur].mask.v != tc_v_mask))
            update_cur_vm = 1;
        if (video_modes_plm[vm_cur].sampler_phase != tc_sampler_phase)
            set_sampler_phase(tc_sampler_phase, 1, 1);
    }
    video_modes_plm[vm_edit].timings.h_total = tc_h_samplerate;
    video_modes_plm[vm_edit].timings.h_total_adj = (uint8_t)tc_h_samplerate_adj;
    video_modes_plm[vm_edit].timings.h_synclen = (uint8_t)tc_h_synclen;
    video_modes_plm[vm_edit].timings.h_backporch = tc_h_bporch;
    video_modes_plm[vm_edit].timings.h_active = tc_h_active;
    video_modes_plm[vm_edit].timings.v_synclen = (uint8_t)tc_v_synclen;
    video_modes_plm[vm_edit].timings.v_backporch = tc_v_bporch;
    video_modes_plm[vm_edit].timings.v_active = tc_v_active;
    video_modes_plm[vm_edit].mask.h = tc_h_mask;
    video_modes_plm[vm_edit].mask.v = tc_v_mask;
    video_modes_plm[vm_edit].sampler_phase = tc_sampler_phase;

    if (v == &tc_sampler_phase)
        sampler_phase_disp(menu_row2, US2066_ROW_LEN+1, *v, active_mode);
    else if ((v == &tc_h_samplerate) || (v == &tc_h_samplerate_adj))
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u.%.2u", video_modes_plm[vm_edit].timings.h_total, video_modes_plm[vm_edit].timings.h_total_adj*5);
    else
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", *v);
}

static void vm_out_tweak(uint16_t *v) {
    int active_mode = (vm_out_cur == vm_out_edit);

    if (active_mode) {
        if ((video_modes[vm_out_cur].timings.h_total != tc_h_samplerate) ||
            (video_modes[vm_out_cur].timings.h_synclen != (uint8_t)tc_h_synclen) ||
            (video_modes[vm_out_cur].timings.h_backporch != tc_h_bporch) ||
            (video_modes[vm_out_cur].timings.h_active != tc_h_active) ||
            (video_modes[vm_out_cur].timings.v_synclen != (uint8_t)tc_v_synclen) ||
            (video_modes[vm_out_cur].timings.v_backporch != tc_v_bporch) ||
            (video_modes[vm_out_cur].timings.v_active != tc_v_active) ||
            (video_modes[vm_out_cur].timings.v_total != tc_v_total) ||
            (video_modes[vm_out_cur].timings.v_hz_x100 != (tc_v_hz*100 + tc_v_hz_frac)) ||
            (video_modes[vm_out_cur].ar.h != tc_h_mask) ||
            (video_modes[vm_out_cur].ar.v != tc_v_mask))
            update_cur_vm = 1;
    }
    video_modes[vm_out_edit].timings.h_total = tc_h_samplerate;
    video_modes[vm_out_edit].timings.h_synclen = (uint8_t)tc_h_synclen;
    video_modes[vm_out_edit].timings.h_backporch = tc_h_bporch;
    video_modes[vm_out_edit].timings.h_active = tc_h_active;
    video_modes[vm_out_edit].timings.v_synclen = (uint8_t)tc_v_synclen;
    video_modes[vm_out_edit].timings.v_backporch = tc_v_bporch;
    video_modes[vm_out_edit].timings.v_active = tc_v_active;
    video_modes[vm_out_edit].timings.v_total = tc_v_total;
    video_modes[vm_out_edit].timings.v_hz_x100 = (tc_v_hz*100 + tc_v_hz_frac);
    video_modes[vm_out_edit].ar.h = tc_h_mask;
    video_modes[vm_out_edit].ar.v = tc_v_mask;

    if ((v == &tc_v_hz) || (v == &tc_v_hz_frac))
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u.%.2u", (video_modes[vm_out_edit].timings.v_hz_x100/100), (video_modes[vm_out_edit].timings.v_hz_x100%100));
    else
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", *v);
}

static void smp_select() {
    smp_edit = smp_sel;
    dtmg_edit = smp_sel % NUM_VIDEO_GROUPS;
}

static int smp_is_active() {
    if (!((oper_mode == OPERMODE_ADAPT_LM) || (oper_mode == OPERMODE_SCALER)))
        return 0;
    else if (isl_dev.powered_on && isl_dev.sync_active && (smp_cur == smp_edit))
        return 1;
#ifndef DExx_FW
    else if (advrx_dev.powered_on && advrx_dev.sync_active && (dtmg_cur == dtmg_edit))
        return 1;
#endif
    else
        return 0;
}

static int smp_reset() {
#ifndef DExx_FW
    if (advrx_dev.powered_on && advrx_dev.sync_active)
        memset(&hdmi_timings[dtmg_edit], 0, sizeof(sync_timings_t));
    else
#endif
        memcpy(&smp_presets[smp_edit], &smp_presets_default[smp_edit], sizeof(smp_preset_t));

    if (smp_is_active())
        update_cur_vm = 1;

    return 0;
}
