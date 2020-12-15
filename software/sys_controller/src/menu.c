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
extern isl51002_dev isl_dev;
extern volatile osd_regs *osd;

char menu_row1[US2066_ROW_LEN+1], menu_row2[US2066_ROW_LEN+1];

uint16_t tc_h_samplerate, tc_h_samplerate_adj, tc_h_synclen, tc_h_bporch, tc_h_active, tc_v_synclen, tc_v_bporch, tc_v_active, tc_sampler_phase;
uint8_t menu_active;
uint8_t vm_sel, vm_edit;

menunavi navi[MAX_MENU_DEPTH];
uint8_t navlvl;

uint8_t osd_enable, osd_enable_pre=1, osd_status_timeout, osd_status_timeout_pre=1;

static const char *off_on_desc[] = { LNG("Off","ｵﾌ"), LNG("On","ｵﾝ") };
static const char *video_lpf_desc[] = { LNG("Auto","ｵｰﾄ"), LNG("Off","ｵﾌ"), "95MHz (HDTV II)", "35MHz (HDTV I)", "16MHz (EDTV)", "9MHz (SDTV)" };
static const char *ypbpr_cs_desc[] = { "Rec. 601", "Rec. 709" };
static const char *s480p_mode_desc[] = { LNG("Auto","ｵｰﾄ"), "DTV 480p", "VESA 640x480@60" };
static const char *s400p_mode_desc[] = { "VGA 640x400@70", "VGA 720x400@70" };
static const char *sync_lpf_desc[] = { LNG("2.5MHz (max)","2.5MHz (ｻｲﾀﾞｲ)"), LNG("10MHz (med)","10MHz (ﾁｭｳｲ)"), LNG("33MHz (min)","33MHz (ｻｲｼｮｳ)"), LNG("Off","ｵﾌ") };
static const char *stc_lpf_desc[] = { "4.8MHz (HDTV/PC)", "0.5MHz (SDTV)", "1.7MHz (EDTV)" };
static const char *l3_mode_desc[] = { LNG("Generic 16:9","ｼﾞｪﾈﾘｯｸ 16:9"), LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l2l4l5_mode_desc[] = { LNG("Generic 4:3","ｼﾞｪﾈﾘｯｸ 4:3"), LNG("512x240 optim.","512x240 ｻｲﾃｷｶ."), LNG("384x240 optim.","384x240 ｻｲﾃｷｶ."), LNG("320x240 optim.","320x240 ｻｲﾃｷｶ."), LNG("256x240 optim.","256x240 ｻｲﾃｷｶ.") };
static const char *l5_fmt_desc[] = { "1920x1080", "1600x1200", "1920x1200" };
static const char *pm_240p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line3x", "Line4x", "Line5x" };
static const char *pm_480i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Deint + Line2x", "Line3x (laced)", "Deint + Line4x" };
static const char *pm_384p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x", "Line2x 240x360", "Line3x 240x360", "Line3x Generic" };
static const char *pm_480p_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Line2x" };
static const char *pm_1080i_desc[] = { LNG("Passthru","ﾊﾟｽｽﾙｰ"), "Deint + Line2x" };
static const char *pm_ad_240p_desc[] = { "Skip", "720x480 (Line2x)", "1280x720 (Line3x)", "1280x1024 (Line4x)", "1920x1080i (Line2x)", "1920x1080 (Line4x)", "1920x1080 (Line5x)", "1600x1200 (Line5x)", "1920x1200 (Line5x)", "1920x1440 (Line6x)" };
static const char *pm_ad_288p_desc[] = { "Skip", "720x576 (Line2x)", "1920x1080i (Line2x)", "1920x1080 (Line4x)", "1920x1200 (Line4x)", "1920x1440 (Line5x)" };
static const char *pm_ad_480i_desc[] = { "Skip", "720x240 (240p rest.)", "1280x1024 (Dint+L4x)", "1080i (240p rest+L2x)", "1920x1080 (Dint+L4x)", "1920x1440 (Dint+L6x)" };
static const char *pm_ad_576i_desc[] = { "Skip", "720x288 (288p rest.)", "1080i (288p rest+L2x)", "1920x1080 (Dint+L4x)" };
static const char *pm_ad_480p_desc[] = { "Skip", "720x240 (Line drop)", "1280x1024 (Line2x)", "1920x1080i (Line1x)", "1920x1080 (Line2x)", "1920x1440 (Line3x)" };
static const char *pm_ad_576p_desc[] = { "Skip", "720x288 (Line drop)", "1920x1200 (Line2x)" };
static const char *sm_ad_240p_288p_desc[] = { "Generic 4:3", "SNES 256col", "SNES 512col", "MD 256col", "MD 320col", "PSX 256col", "PSX 320col", "PSX 384col", "PSX 512col", "PSX 640col", "N64 320col", "N64 640col" };
static const char *sm_ad_480i_576i_desc[] = { "Generic 4:3", "Generic 16:9" };
static const char *sm_ad_480p_desc[] = { "Generic 4:3", "Generic 16:9", "DTV 480p 4:3", "DTV 480p 16:9", "VESA 640x480@60" };
static const char *sm_ad_576p_desc[] = { "Generic 4:3" };
static const char *lm_deint_mode_desc[] = { "Bob", "Noninterlace restore" };
static const char *ar_256col_desc[] = { "4:3", "8:7" };
static const char *tx_mode_desc[] = { "HDMI (RGB Full)", "HDMI (RGB Limited)", "HDMI (YCbCr444)", "DVI" };
static const char *sl_mode_desc[] = { LNG("Off","ｵﾌ"), LNG("Auto","ｵｰﾄ"), LNG("On","ｵﾝ") };
static const char *sl_method_desc[] = { LNG("Multiplication","Multiplication"), LNG("Subtraction","Subtraction") };
static const char *sl_type_desc[] = { LNG("Horizontal","ﾖｺ"), LNG("Vertical","ﾀﾃ"), "Horiz. + Vert.", "Custom" };
static const char *sl_id_desc[] = { LNG("Top","ｳｴ"), LNG("Bottom","ｼﾀ") };
#ifdef DExx_FW
static const char *audio_fs_desc[] = { "24bit/48kHz", "24bit/96kHz" };
#else
static const char *audio_fs_desc[] = { "24bit/48kHz", "24bit/96kHz", "24bit/192kHz" };
#endif
static const char *audio_src_desc[] = { "AV1 (analog)", "AV2 (analog)", "AV3 (analog)", "SPDIF", "AV4 (digital)" };
static const char *audio_sr_desc[] = { "Off", "On (4.0)", "On (5.1)", "On (7.1)" };
static const char *audmux_sel_desc[] = { "AV3 input", "AV1 output" };
static const char *lt_desc[] = { "Top-left", "Center", "Bottom-right" };
static const char *lcd_bl_timeout_desc[] = { "Off", "3s", "10s", "30s" };
static const char *osd_enable_desc[] = { "Off", "Full", "Simple" };
static const char *osd_status_desc[] = { "2s", "5s", "10s", "Off" };
static const char *rgsb_ypbpr_desc[] = { "RGsB", "YPbPr" };
static const char *auto_input_desc[] = { "Off", "Current input", "All inputs" };
static const char *mask_color_desc[] = { "Black", "Blue", "Green", "Cyan", "Red", "Magenta", "Yellow", "White" };
static const char *av3_alt_rgb_desc[] = { "Off", "AV1", "AV2" };
static const char *adv761x_rgb_range_desc[] = { "Limited", "Full" };

static void afe_bw_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s%uMHz%s", (v==0 ? "Auto (" : ""), isl_get_afe_bw(&isl_dev, v), (v==0 ? ")" : "")); }
static void sog_vth_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u mV", (v*20)); }
static void hsync_vth_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u mV", 400+(v*187)); }
static void sync_gf_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u ns", ((((v+14)%16)+1)*37)); }
static void sl_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", ((v+1)*625)/100); }
static void sl_cust_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", ((v)*625)/100); }
static void sl_hybr_str_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u%%", (v*625)/100); }
static void lines_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u lines","%u ﾗｲﾝ"), v); }
static void pixels_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u pixels","%u ﾄﾞｯﾄ"), v); }
static void value_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", v); }
static void value16_disp(uint16_t *v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", *v); }
static void signed_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%d", (int8_t)(v-SIGNED_NUMVAL_ZERO)); }
static void lt_disp(uint8_t v) { strncpy(menu_row2, lt_desc[v], US2066_ROW_LEN+1); }
#ifndef DExx_FW
static void aud_db_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%d dB", ((int8_t)v-PCM_GAIN_0DB)); }
static void audio_src_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%s", audio_src_desc[v]); }
#endif
//static void vm_display_name (uint8_t v) { strncpy(menu_row2, video_modes[v].name, US2066_ROW_LEN+1); }
//static void link_av_desc (avinput_t v) { strncpy(menu_row2, v == AV_LAST ? "No link" : avinput_str[v], US2066_ROW_LEN+1); }
//static void profile_disp(uint8_t v) { read_userdata(v, 1); sniprintf(menu_row2, US2066_ROW_LEN+1, "%u: %s", v, (target_profile_name[0] == 0) ? "<empty>" : target_profile_name); }
static void alc_v_filter_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u lines","%u ﾗｲﾝ"), (1<<(v+5))); }
static void alc_h_filter_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%u pixels","%u ﾄﾞｯﾄ"), (1<<(v+4))); }
//static void coarse_gain_disp(uint8_t v) { sniprintf(menu_row2, US2066_ROW_LEN+1, "%u.%u", ((v*10)+50)/100, (((v*10)+50)%100)/10); }

/*static const arg_info_t vm_arg_info = {&vm_sel, VIDEO_MODES_CNT-1, vm_display_name};
static const arg_info_t profile_arg_info = {&profile_sel_menu, MAX_PROFILE, profile_disp};
static const arg_info_t lt_arg_info = {&lt_sel, (sizeof(lt_desc)/sizeof(char*))-1, lt_disp};*/


/*MENU(menu_advtiming, P99_PROTECT({
    { LNG("H. samplerate","H. ｻﾝﾌﾟﾙﾚｰﾄ"),        OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate, H_TOTAL_MIN,   H_TOTAL_MAX, vm_tweak } } },
    { "H. s.rate frac",                           OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_samplerate_adj, 0,  H_TOTAL_ADJ_MAX, vm_tweak } } },
    { LNG("H. synclen","H. ﾄﾞｳｷﾅｶﾞｻ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_synclen,    H_SYNCLEN_MIN, H_SYNCLEN_MAX, vm_tweak } } },
    { LNG("H. backporch","H. ﾊﾞｯｸﾎﾟｰﾁ"),        OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_bporch,     H_BPORCH_MIN,  H_BPORCH_MAX, vm_tweak } } },
    { LNG("H. active","H. ｱｸﾃｨﾌﾞ"),             OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_h_active,     H_ACTIVE_MIN,  H_ACTIVE_MAX, vm_tweak } } },
    { LNG("V. synclen","V. ﾄﾞｳｷﾅｶﾞｻ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_synclen,    V_SYNCLEN_MIN, V_SYNCLEN_MAX, vm_tweak } } },
    { LNG("V. backporch","V. ﾊﾞｯｸﾎﾟｰﾁ"),       OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_bporch,     V_BPORCH_MIN,  V_BPORCH_MAX, vm_tweak } } },
    { LNG("V. active","V. ｱｸﾃｨﾌﾞ"),            OPT_AVCONFIG_NUMVAL_U16,{ .num_u16 = { &tc_v_active,     V_ACTIVE_MIN,  V_ACTIVE_MAX, vm_tweak } } },
    { LNG("Sampling phase","ｻﾝﾌﾟﾘﾝｸﾞﾌｪｰｽﾞ"),     OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc_sampler_phase, 0, SAMPLER_PHASE_MAX, vm_tweak } } },
}))*/

MENU(menu_cust_sl, P99_PROTECT({
    { "Sub-line 1 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 2 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 3 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 4 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-line 5 str",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_l_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 1 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[0],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 2 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[1],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 3 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[2],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 4 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[3],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 5 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[4],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
    { "Sub-column 6 str",                     OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_cust_c_str[5],      OPT_NOWRAP, 0, SCANLINESTR_MAX+1, sl_cust_str_disp } } },
}))


MENU(menu_isl_video_opt, P99_PROTECT({
    { LNG("Video LPF","ﾋﾞﾃﾞｵ LPF"),             OPT_AVCONFIG_NUMVALUE, { .num = { &tc.isl_cfg.afe_bw,     OPT_WRAP, 0, 16,  afe_bw_disp } } },
    { LNG("YPbPr in ColSpa","ｲﾛｸｳｶﾝﾆYPbPr"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.ypbpr_cs,          OPT_WRAP,   SETTING_ITEM(ypbpr_cs_desc) } } },
    { LNG("R/Pr offset","R/Pr ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.r_offs, 0, 0x3FF, value16_disp } } },
    { LNG("G/Y offset","G/Y ｵﾌｾｯﾄ"),            OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.g_offs, 0, 0x3FF, value16_disp } } },
    { LNG("B/Pb offset","B/Pb ｵﾌｾｯﾄ"),          OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.b_offs, 0, 0x3FF, value16_disp } } },
    { LNG("R/Pr gain","R/Pr ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.r_gain, 0, 0x3FF, value16_disp } } },
    { LNG("G/Y gain","G/Y ｹﾞｲﾝ"),               OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.g_gain, 0, 0x3FF, value16_disp } } },
    { LNG("B/Pb gain","B/Pb ｹﾞｲﾝ"),             OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.col.b_gain, 0, 0x3FF, value16_disp } } },
    { "Clamp strength",                         OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.clamp_str,      OPT_NOWRAP, 0, CLAMP_STR_MAX, value_disp } } },
    { "Clamp/ALC position",                     OPT_AVCONFIG_NUMVAL_U16,  { .num_u16 = { &tc.isl_cfg.clamp_alc_start,  CLAMP_POS_MIN, CLAMP_POS_MAX, value16_disp } } },
    { "Clamp width",                            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.clamp_alc_width,      OPT_NOWRAP, 0, 0xff, pixels_disp } } },
    { "Clamp/ALC on coast",                     OPT_AVCONFIG_SELECTION, { .sel = { &tc.isl_cfg.coast_clamp,    OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { "Auto level ctl (ALC)",                   OPT_AVCONFIG_SELECTION, { .sel = { &tc.isl_cfg.alc_enable,    OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { "ALC V filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.alc_v_filter,  OPT_NOWRAP, 0, ALC_V_FILTER_MAX, alc_v_filter_disp } } },
    { "ALC H filter",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.alc_h_filter,  OPT_NOWRAP, 0, ALC_H_FILTER_MAX, alc_h_filter_disp } } },
}))

MENU(menu_isl_sync_opt, P99_PROTECT({
    //{ LNG("Analog sync LPF","ｱﾅﾛｸﾞﾄﾞｳｷ LPF"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sync_lpf,    OPT_WRAP,   SETTING_ITEM(sync_lpf_desc) } } },
    //{ "Analog STC LPF",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.stc_lpf,    OPT_WRAP,   SETTING_ITEM(stc_lpf_desc) } } },
    { LNG("Analog sync Vth","ｱﾅﾛｸﾞﾄﾞｳｷ Vth"),    OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.sog_vth,    OPT_NOWRAP, 0, SYNC_VTH_MAX, sog_vth_disp } } },
    { "TTL Hsync Vth",                          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.hsync_vth,    OPT_NOWRAP, 0, SYNC_VTH_MAX, hsync_vth_disp } } },
    { "Sync glitch filt len",                   OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.sync_gf,     OPT_WRAP, 0, 15, sync_gf_disp } } },
    { "H-PLL Pre-Coast",                        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.pre_coast,   OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
    { "H-PLL Post-Coast",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.post_coast,  OPT_NOWRAP, 0, PLL_COAST_MAX, lines_disp } } },
    { "H-PLL Loop Gain",                        OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.isl_cfg.pll_loop_gain, OPT_NOWRAP, 0, PLL_LOOP_GAIN_MAX, value_disp } } },
}))

#ifndef DExx_FW
MENU(menu_adv_video_opt, P99_PROTECT({
    { "Default RGB range",                      OPT_AVCONFIG_SELECTION, { .sel = { &tc.adv761x_cfg.default_rgb_range,    OPT_WRAP,   SETTING_ITEM(adv761x_rgb_range_desc) } } },
}))
#endif

MENU(menu_pure_lm, P99_PROTECT({
    { LNG("240p/288p proc","240p/288pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_240p,         OPT_WRAP, SETTING_ITEM(pm_240p_desc) } } },
    { LNG("384p/400p proc","384p/400pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_384p,         OPT_WRAP, SETTING_ITEM(pm_384p_desc) } } },
    { LNG("480i/576i proc","480i/576iｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480i,         OPT_WRAP, SETTING_ITEM(pm_480i_desc) } } },
    { LNG("480p/576p proc","480p/576pｼｮﾘ"),     OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_480p,         OPT_WRAP, SETTING_ITEM(pm_480p_desc) } } },
    { LNG("960i/1080i proc","960i/1080iｼｮﾘ"),   OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_1080i,        OPT_WRAP, SETTING_ITEM(pm_1080i_desc) } } },
    { LNG("Line2x mode","Line2xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l2_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5_mode_desc) } } },
    { LNG("Line3x mode","Line3xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l3_mode,         OPT_WRAP, SETTING_ITEM(l3_mode_desc) } } },
    { LNG("Line4x mode","Line4xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l4_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5_mode_desc) } } },
    { LNG("Line5x mode","Line5xﾓｰﾄﾞ"),          OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_mode,         OPT_WRAP, SETTING_ITEM(l2l4l5_mode_desc) } } },
    { LNG("Line5x format","Line5xｹｲｼｷ"),        OPT_AVCONFIG_SELECTION, { .sel = { &tc.l5_fmt,          OPT_WRAP, SETTING_ITEM(l5_fmt_desc) } } },
    { LNG("256x240 aspect","256x240ｱｽﾍﾟｸﾄ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.ar_256col,       OPT_WRAP, SETTING_ITEM(ar_256col_desc) } } },
    { "480p in preset",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.s480p_mode,     OPT_WRAP, SETTING_ITEM(s480p_mode_desc) } } },
    { "400p in preset",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.s400p_mode,     OPT_WRAP, SETTING_ITEM(s400p_mode_desc) } } },
    { LNG("Allow upsample2x","ｱｯﾌﾟｻﾝﾌﾟﾙ2xｷｮﾖｳ"), OPT_AVCONFIG_SELECTION, { .sel = { &tc.upsample2x,      OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    //{ LNG("<Adv. timing   >","<ｶｸｼｭﾀｲﾐﾝｸﾞ>"),    OPT_SUBMENU,            { .sub = { &menu_advtiming, &vm_arg_info, vm_select } } },
}))

MENU(menu_adap_lm, P99_PROTECT({
    { LNG("240p proc","240pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_240p,      OPT_WRAP, SETTING_ITEM(pm_ad_240p_desc) } } },
    { LNG("288p proc","288pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_288p,      OPT_WRAP, SETTING_ITEM(pm_ad_288p_desc) } } },
    { LNG("480i proc","480iｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_480i,      OPT_WRAP, SETTING_ITEM(pm_ad_480i_desc) } } },
    { LNG("576i proc","576iｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_576i,      OPT_WRAP, SETTING_ITEM(pm_ad_576i_desc) } } },
    { LNG("480p proc","480pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_480p,      OPT_WRAP, SETTING_ITEM(pm_ad_480p_desc) } } },
    { LNG("576p proc","576pｼｮﾘ"),               OPT_AVCONFIG_SELECTION, { .sel = { &tc.pm_ad_576p,      OPT_WRAP, SETTING_ITEM(pm_ad_576p_desc) } } },
    { LNG("240p/288p mode","240p/288pﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_240p_288p, OPT_WRAP, SETTING_ITEM(sm_ad_240p_288p_desc) } } },
    { LNG("480i/576i mode","480i/576iﾓｰﾄﾞ"),    OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_480i_576i, OPT_WRAP, SETTING_ITEM(sm_ad_480i_576i_desc) } } },
    { LNG("480p mode","480pﾓｰﾄﾞ"),              OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_480p,      OPT_WRAP, SETTING_ITEM(sm_ad_480p_desc) } } },
    { LNG("576p mode","576pﾓｰﾄﾞ"),              OPT_AVCONFIG_SELECTION, { .sel = { &tc.sm_ad_576p,      OPT_WRAP, SETTING_ITEM(sm_ad_576p_desc) } } },
}))

MENU(menu_output, P99_PROTECT({
    { "Adaptive LM priority",                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.adapt_lm,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    { "LM deinterlace mode",                   OPT_AVCONFIG_SELECTION, { .sel = { &tc.lm_deint_mode,   OPT_WRAP, SETTING_ITEM(lm_deint_mode_desc) } } },
    { LNG("TX mode","TXﾓｰﾄﾞ"),                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.adv7513_cfg.tx_mode,  OPT_WRAP, SETTING_ITEM(tx_mode_desc) } } },
    //{ "HDMI ITC",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.hdmi_itc,        OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
}))

MENU(menu_scanlines, P99_PROTECT({
    { LNG("Scanlines","ｽｷｬﾝﾗｲﾝ"),                  OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_mode,     OPT_WRAP,   SETTING_ITEM(sl_mode_desc) } } },
    { LNG("Sl. strength","ｽｷｬﾝﾗｲﾝﾂﾖｻ"),            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_str,      OPT_NOWRAP, 0, SCANLINESTR_MAX, sl_str_disp } } },
    { "Sl. hybrid str.",                          OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.sl_hybr_str, OPT_NOWRAP, 0, SL_HYBRIDSTR_MAX, sl_hybr_str_disp } } },
    { "Sl. method",                               OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_method,   OPT_WRAP,   SETTING_ITEM(sl_method_desc) } } },
    { "Sl. alternating",                          OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_altern,   OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { LNG("Sl. alignment","ｽｷｬﾝﾗｲﾝﾎﾟｼﾞｼｮﾝ"),        OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_id,       OPT_WRAP,   SETTING_ITEM(sl_id_desc) } } },
    { "Sl. alt interval",                         OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_altiv,    OPT_WRAP,   SETTING_ITEM(off_on_desc) } } },
    { LNG("Sl. type","ｽｷｬﾝﾗｲﾝﾙｲ"),                 OPT_AVCONFIG_SELECTION, { .sel = { &tc.sl_type,     OPT_WRAP,   SETTING_ITEM(sl_type_desc) } } },
    { "<  Custom Sl.  >",                         OPT_SUBMENU,            { .sub = { &menu_cust_sl, NULL, NULL } } },
}))

MENU(menu_postproc, P99_PROTECT({
    { LNG("Horizontal mask","ｽｲﾍｲﾏｽｸ"),           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.h_mask,      OPT_NOWRAP, 0, H_MASK_MAX, pixels_disp } } },
    { LNG("Vertical mask","ｽｲﾁｮｸﾏｽｸ"),            OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.v_mask,      OPT_NOWRAP, 0, V_MASK_MAX, pixels_disp } } },
    { "Mask color",                              OPT_AVCONFIG_SELECTION, { .sel = { &tc.mask_color,  OPT_NOWRAP,   SETTING_ITEM(mask_color_desc) } } },
    { LNG("Mask brightness","ﾏｽｸｱｶﾙｻ"),           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.mask_br,     OPT_NOWRAP, 0, HV_MASK_MAX_BR, value_disp } } },
    { LNG("Reverse LPF","ｷﾞｬｸLPF"),              OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.reverse_lpf, OPT_NOWRAP, 0, REVERSE_LPF_MAX, value_disp } } },
    //{ LNG("<DIY lat. test>","DIYﾁｴﾝﾃｽﾄ"),         OPT_FUNC_CALL,          { .fun = { latency_test, &lt_arg_info } } },
}))

MENU(menu_compatibility, P99_PROTECT({
    { LNG("Full TX setup","ﾌﾙTXｾｯﾄｱｯﾌﾟ"),         OPT_AVCONFIG_SELECTION, { .sel = { &tc.full_tx_setup,    OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    { "Default HDMI VIC",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.default_vic,     OPT_NOWRAP, 0, HDMI_1080p50, value_disp } } },
}))

MENU(menu_audio, P99_PROTECT({
#ifdef DExx_FW
    { "Sampling format",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.adv7513_cfg.i2s_fs,  OPT_WRAP, SETTING_ITEM(audio_fs_desc) } } },
#else
    { "Sampling format",                        OPT_AVCONFIG_SELECTION, { .sel = { &tc.pcm_cfg.fs,      OPT_WRAP, SETTING_ITEM(audio_fs_desc) } } },
    { "Quad stereo",                            OPT_AVCONFIG_SELECTION,  { .sel = { &tc.adv7513_cfg.i2s_chcfg, OPT_WRAP, SETTING_ITEM(audio_sr_desc) } } },
    { "Pre-ADC gain",                           OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.pcm_cfg.gain,    OPT_NOWRAP, PCM_GAIN_M12DB, PCM_GAIN_12DB, aud_db_disp } } },
    { "AV1 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[0], OPT_NOWRAP, 0, 3, audio_src_disp } } },
    { "AV2 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[1], OPT_NOWRAP, 1, 3, audio_src_disp } } },
    { "AV3 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[2], OPT_NOWRAP, 1, 3, audio_src_disp } } },
    { "AV4 audio source",                       OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.audio_src_map[3], OPT_NOWRAP, 1, 4, audio_src_disp } } },
    { "3.5mm jack assign",                      OPT_AVCONFIG_SELECTION,  { .sel = { &tc.audmux_sel,     OPT_WRAP, SETTING_ITEM(audmux_sel_desc) } } },
#endif
}))


MENU(menu_settings, P99_PROTECT({
    //{ LNG("<Load profile >","<ﾌﾟﾛﾌｧｲﾙﾛｰﾄﾞ    >"),   OPT_FUNC_CALL,         { .fun = { load_profile, &profile_arg_info } } },
    //{ LNG("<Save profile >","<ﾌﾟﾛﾌｧｲﾙｾｰﾌﾞ    >"),  OPT_FUNC_CALL,          { .fun = { save_profile, &profile_arg_info } } },
    { LNG("<Reset settings>","<ｾｯﾃｲｵｼｮｷｶ    >"),  OPT_FUNC_CALL,          { .fun = { reset_target_avconfig, NULL } } },
    //{ LNG("Link prof->input","Link prof->input"), OPT_AVCONFIG_NUMVALUE,  { .num = { &tc.link_av,  OPT_WRAP, AV1_RGBs, AV_LAST, link_av_desc } } },
    //{ LNG("Link input->prof","Link input->prof"),   OPT_AVCONFIG_SELECTION, { .sel = { &profile_link,  OPT_WRAP, SETTING_ITEM(off_on_desc) } } },
    //{ LNG("Initial input","ｼｮｷﾆｭｳﾘｮｸ"),          OPT_AVCONFIG_SELECTION, { .sel = { &def_input,       OPT_WRAP, SETTING_ITEM(avinput_str) } } },
    //{ "Autodetect input",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_input,     OPT_WRAP, SETTING_ITEM(auto_input_desc) } } },
    //{ "Auto AV1 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av1_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    //{ "Auto AV2 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av2_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    //{ "Auto AV3 Y/Gs",                          OPT_AVCONFIG_SELECTION, { .sel = { &auto_av3_ypbpr,     OPT_WRAP, SETTING_ITEM(rgsb_ypbpr_desc) } } },
    { "OSD",                                    OPT_AVCONFIG_SELECTION, { .sel = { &osd_enable_pre,   OPT_WRAP,   SETTING_ITEM(osd_enable_desc) } } },
    { "OSD status disp.",                       OPT_AVCONFIG_SELECTION, { .sel = { &osd_status_timeout_pre,   OPT_WRAP,   SETTING_ITEM(osd_status_desc) } } },
    //{     "<Import sett.  >",                     OPT_FUNC_CALL,        { .fun = { import_userdata, NULL } } },
    //{ LNG("<Fw. update    >","<ﾌｧｰﾑｳｪｱｱｯﾌﾟﾃﾞｰﾄ>"), OPT_FUNC_CALL,          { .fun = { fw_update, NULL } } },
}))


MENU(menu_main, P99_PROTECT({
    { "AV1-3 video in opt.>",                   OPT_SUBMENU,            { .sub = { &menu_isl_video_opt, NULL, NULL } } },
    { "AV1-3 sync opt.    >",                   OPT_SUBMENU,            { .sub = { &menu_isl_sync_opt, NULL, NULL } } },
#ifndef DExx_FW
    { "AV4 video in opt.  >",                   OPT_SUBMENU,            { .sub = { &menu_adv_video_opt, NULL, NULL } } },
#endif
    { "Pure LM opt.       >",                   OPT_SUBMENU,            { .sub = { &menu_pure_lm, NULL, NULL } } },
    { "Adapt. LM opt.     >",                   OPT_SUBMENU,            { .sub = { &menu_adap_lm, NULL, NULL } } },
    { LNG("Output opt.    >","ｼｭﾂﾘｮｸｵﾌﾟｼｮﾝ  >"),  OPT_SUBMENU,            { .sub = { &menu_output, NULL, NULL } } },
    //{ LNG("Scanline opt.  >","ｽｷｬﾝﾗｲﾝｵﾌﾟｼｮﾝ >"),  OPT_SUBMENU,            { .sub = { &menu_scanlines, NULL, NULL } } },
    //{ LNG("Post-proc.     >","ｱﾄｼｮﾘ         >"),  OPT_SUBMENU,            { .sub = { &menu_postproc, NULL, NULL } } },
    { LNG("Compatibility  >","ｺﾞｶﾝｾｲ        >"), OPT_SUBMENU,             { .sub = { &menu_compatibility, NULL, NULL } } },
    { LNG("Audio options  >","ｵｰﾃﾞｨｵｵﾌﾟｼｮﾝ  >"), OPT_SUBMENU,             { .sub = { &menu_audio, NULL, NULL } } },
    { "Settings opt   >",                       OPT_SUBMENU,             { .sub = { &menu_settings, NULL, NULL } } },
}))


int is_menu_active() {
    return !!menu_active;
}

void init_menu() {
    menu_active = 0;
    memset(navi, 0, sizeof(navi));
    navi[0].m = &menu_main;
    navlvl = 0;

    // Setup OSD
    osd_enable = osd_enable_pre;
    osd_status_timeout = osd_status_timeout_pre;
    osd->osd_config.x_size = 0;
    osd->osd_config.y_size = 0;
    osd->osd_config.x_offset = 3;
    osd->osd_config.y_offset = 3;
    osd->osd_config.enable = !!osd_enable;
    osd->osd_config.status_timeout = osd_status_timeout;
    osd->osd_config.border_color = 1;
}

menunavi* get_current_menunavi() {
    return &navi[navlvl];
}

void write_option_value(menuitem_t *item, int func_called, int retval)
{
    switch (item->type) {
        case OPT_AVCONFIG_SELECTION:
            strncpy(menu_row2, item->sel.setting_str[*(item->sel.data)], US2066_ROW_LEN+1);
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
        case OPT_FUNC_CALL:
            if (func_called) {
                if (retval == 0)
                    strncpy(menu_row2, "Done", US2066_ROW_LEN+1);
                else if (retval < 0)
                    sniprintf(menu_row2, US2066_ROW_LEN+1, "Failed (%d)", retval);
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

    if (!menu_active || (osd_enable != 1))
        return;

    for (i=0; i < navi[navlvl].m->num_items; i++) {
        item = &navi[navlvl].m->items[i];
        strncpy((char*)osd->osd_array.data[i][0], item->name, OSD_CHAR_COLS);
        row_mask[0] |= (1<<i);

        if ((item->type != OPT_SUBMENU) && (item->type != OPT_FUNC_CALL)) {
            write_option_value(item, 0, 0);
            strncpy((char*)osd->osd_array.data[i][1], menu_row2, OSD_CHAR_COLS);
            row_mask[1] |= (1<<i);
        }
    }

    osd->osd_sec_enable[0].mask = row_mask[0];
    osd->osd_sec_enable[1].mask = row_mask[1];
}

void display_menu(rc_code_t remote_code)
{
    menucode_id code = NO_ACTION;
    menuitem_t *item;
    uint8_t *val, val_wrap, val_min, val_max;
    uint16_t *val_u16, val_u16_min, val_u16_max;
    int i, func_called = 0, retval = 0, forcedisp=0;

    if (remote_code == RC_MENU) {
        menu_active ^= 1;
        osd->osd_config.menu_active = menu_active;
        if (menu_active)
            render_osd_menu();
    } else if ((remote_code >= RC_OK) && (remote_code < RC_INFO)) {
        code = remote_code;
    }

    if (!menu_active) {
        ui_disp_status(0);
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
            navlvl--;
            render_osd_menu();
        } else {
            menu_active = 0;
            osd->osd_config.menu_active = 0;
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

                if (code == VAL_MINUS)
                    *val = (*val > val_min) ? (*val-1) : (val_wrap ? val_max : val_min);
                else
                    *val = (*val < val_max) ? (*val+1) : (val_wrap ? val_min : val_max);
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
    item = &navi[navlvl].m->items[navi[navlvl].mp];
    strncpy(menu_row1, item->name, US2066_ROW_LEN+1);
    write_option_value(item, func_called, retval);
    strncpy((char*)osd->osd_array.data[navi[navlvl].mp][1], menu_row2, OSD_CHAR_COLS);
    osd->osd_row_color.mask = (1<<navi[navlvl].mp);
    if (func_called || ((item->type == OPT_FUNC_CALL) && item->fun.arg_info != NULL) || ((item->type == OPT_SUBMENU) && item->sub.arg_info != NULL))
        osd->osd_sec_enable[1].mask |= (1<<navi[navlvl].mp);

    ui_disp_menu(0);
}

void update_osd_size(mode_data_t *vm_out) {
    uint8_t osd_size = vm_out->timings.v_active / 700;

    osd->osd_config.x_size = osd_size;
    osd->osd_config.y_size = osd_size;
}

void update_settings() {
    if ((osd_enable != osd_enable_pre) || (osd_status_timeout != osd_status_timeout_pre)) {
        osd_enable = osd_enable_pre;
        osd_status_timeout = osd_status_timeout_pre;
        osd->osd_config.enable = !!osd_enable;
        osd->osd_config.status_timeout = osd_status_timeout;
        if (is_menu_active()) {
            render_osd_menu();
            display_menu(0);
        }
    }
}

/*static void vm_select() {
    vm_edit = vm_sel;
    tc_h_samplerate = video_modes[vm_edit].h_total;
    tc_h_samplerate_adj = (uint16_t)video_modes[vm_edit].h_total_adj;
    tc_h_synclen = (uint16_t)video_modes[vm_edit].h_synclen;
    tc_h_bporch = (uint16_t)video_modes[vm_edit].h_backporch;
    tc_h_active = video_modes[vm_edit].h_active;
    tc_v_synclen = (uint16_t)video_modes[vm_edit].v_synclen;
    tc_v_bporch = (uint16_t)video_modes[vm_edit].v_backporch;
    tc_v_active = video_modes[vm_edit].v_active;
    tc_sampler_phase = video_modes[vm_edit].sampler_phase;
}

static void vm_tweak(uint16_t *v) {
    if (cm.sync_active && (cm.id == vm_edit)) {
        if ((video_modes[cm.id].h_total != tc_h_samplerate) ||
            (video_modes[cm.id].h_total_adj != (uint8_t)tc_h_samplerate_adj) ||
            (video_modes[cm.id].h_synclen != tc_h_synclen) ||
            (video_modes[cm.id].h_backporch != (uint8_t)tc_h_bporch) ||
            (video_modes[cm.id].h_active != tc_h_active) ||
            (video_modes[cm.id].v_synclen != tc_v_synclen) ||
            (video_modes[cm.id].v_backporch != (uint8_t)tc_v_bporch) ||
            (video_modes[cm.id].v_active != tc_v_active) ||
            (video_modes[cm.id].sampler_phase != tc_sampler_phase))
            update_cur_vm = 1;
    }
    video_modes[vm_edit].h_total = tc_h_samplerate;
    video_modes[vm_edit].h_total_adj = (uint8_t)tc_h_samplerate_adj;
    video_modes[vm_edit].h_synclen = (uint8_t)tc_h_synclen;
    video_modes[vm_edit].h_backporch = (uint8_t)tc_h_bporch;
    video_modes[vm_edit].h_active = tc_h_active;
    video_modes[vm_edit].v_synclen = (uint8_t)tc_v_synclen;
    video_modes[vm_edit].v_backporch = (uint8_t)tc_v_bporch;
    video_modes[vm_edit].v_active = tc_v_active;
    video_modes[vm_edit].sampler_phase = tc_sampler_phase;

    if (v == &tc_sampler_phase)
        sniprintf(menu_row2, US2066_ROW_LEN+1, LNG("%d deg","%d ﾄﾞ"), ((*v)*1125)/100);
    else if ((v == &tc_h_samplerate) || (v == &tc_h_samplerate_adj))
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u.%.2u", video_modes[vm_edit].h_total, video_modes[vm_edit].h_total_adj*5);
    else
        sniprintf(menu_row2, US2066_ROW_LEN+1, "%u", *v);
}*/
