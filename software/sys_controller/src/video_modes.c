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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "system.h"
#include "video_modes.h"
#include "avconfig.h"

#define LINECNT_MAX_TOLERANCE   30

#define VM_OUT_YMULT        (vm_conf->y_rpt+1)
#define VM_OUT_XMULT        (vm_conf->x_rpt+1)
#define VM_OUT_PCLKMULT     (((vm_conf->x_rpt+1)*(vm_conf->y_rpt+1))/(vm_conf->h_skip+1))


const mode_data_t video_modes_default[] = { \
    /* 240p modes */ \
    { "1600x240",      HDMI_Unknown,     {1600,  240,    0,  2046, 0,  262,  202, 15,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "1280x240",      HDMI_Unknown,     {1280,  240,    0,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },  \
    { "960x240",       HDMI_Unknown,     { 960,  240,    0,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "512x240",       HDMI_Unknown,     { 512,  240,    0,   682, 0,  262,   77, 14,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "384x240",       HDMI_Unknown,     { 384,  240,    0,   512, 0,  262,   59, 14,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "320x240",       HDMI_Unknown,     { 320,  240,    0,   426, 0,  262,   49, 14,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "256x240",       HDMI_Unknown,     { 256,  240,    0,   341, 0,  262,   39, 14,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "240p",          HDMI_240p60_PR2x, { 720,  240,    0,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    /* 288p modes */ \
    { "1600x240L",     HDMI_Unknown,     {1600,  240,    0,  2046, 0,  312,  202, 43,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "1280x288",      HDMI_Unknown,     {1280,  288,    0,  1560, 0,  312,  170, 19,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },  \
    { "960x288",       HDMI_Unknown,     { 960,  288,    0,  1170, 0,  312,  128, 19,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "512x240LB",     HDMI_Unknown,     { 512,  240,    0,   682, 0,  312,   77, 41,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "384x240LB",     HDMI_Unknown,     { 384,  240,    0,   512, 0,  312,   59, 41,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "320x240LB",     HDMI_Unknown,     { 320,  240,    0,   426, 0,  312,   49, 41,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "256x240LB",     HDMI_Unknown,     { 256,  240,    0,   341, 0,  312,   39, 41,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "288p",          HDMI_288p50,      { 720,  288,    0,   864, 0,  312,   69, 19,   63, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    /* 360p: GBI */ \
    { "480x360",       HDMI_Unknown,     { 480,  360,    0,   600, 0,  375,   63, 10,   38, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "240x360",       HDMI_Unknown,     { 256,  360,    0,   300, 0,  375,   24, 10,   18, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                      TX_1X, TX_1X,  1, {0} },  \
    /* 384p: Sega Model 2 (real vtotal=423) */ \
    { "384p",          HDMI_Unknown,     { 496,  384,    0,   640, 0,  413,   50, 29,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 400p line3x */ \
    { "1600x400",      HDMI_Unknown,     {1600,  400,    0,  2000, 0,  449,  120, 34,  240, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                       TX_1X, TX_1X,  1, {0} },  \
    /* 720x400@70Hz, VGA Mode 3+/7+ */ \
    { "720x400_70",    HDMI_Unknown,     { 720,  400,   75,   900, 0,  449,   64, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 640x400@70Hz, VGA Mode 13h */ \
    { "640x400_70",    HDMI_Unknown,     { 640,  400,   75,   800, 0,  449,   48, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 384p: X68k @ 24kHz */ \
    { "640x384",       HDMI_Unknown,     { 640,  384,    0,   800, 0,  492,   48, 63,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* ~525-line modes */ \
    { "480i",          HDMI_480i60_PR2x, { 720,  240,    0,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    { "480p",          HDMI_480p60,      { 720,  480,    0,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "640x480_60",    HDMI_640x480p60,  { 640,  480,   65,   800, 0,  525,   48, 33,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* X68k @ 31kHz */ \
    { "640x512",       HDMI_Unknown,     { 640,  512,    0,   800, 0,  568,   48, 28,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* ~625-line modes */ \
    { "576i",          HDMI_576i50,      { 720,  288,   55,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_576I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    { "576p",          HDMI_576p50,      { 720,  576,   55,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_576P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "800x600_60",    HDMI_Unknown,     { 800,  600,   65,  1056, 0,  628,   88, 23,  128, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    /* CEA 720p modes */ \
    { "720p_50",       HDMI_720p50,      {1280,  720,   55,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },  \
    { "720p_60",       HDMI_720p60,      {1280,  720,    0,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },  \
    /* VESA XGA,1280x960 and SXGA modes */ \
    { "1024x768_60",   HDMI_Unknown,     {1024,  768,   65,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    { "1280x960_60",   HDMI_Unknown,     {1280,  960,   65,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },  \
    { "1280x1024_60",  HDMI_Unknown,     {1280, 1024,   65,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },  \
    /* PS2 GSM 960i mode */ \
    { "640x960i",      HDMI_Unknown,     { 640,  480,    0,   800, 0, 1050,   48, 33,   96, 2,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* CEA 1080i/p modes */ \
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   55,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },  \
    { "1080i_60",      HDMI_1080i60,     {1920,  540,    0,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },  \
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   55,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },  \
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,    0,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },  \
    /* VESA UXGA mode */ \
    { "1600x1200_60",  HDMI_Unknown,     {1600, 1200,   65,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  6, {0} },  \
    /* CVT 1920x1200 modes (60Hz with reduced blanking) */ \
    { "1920x1200_50",  HDMI_Unknown,     {1920, 1200,   55,  2560, 0, 1238,  320, 29,  200, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    { "1920x1200_60",  HDMI_Unknown,     {1920, 1200,    0,  2080, 0, 1235,   80, 26,   32, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2408, 8, 27, 0, 0, 1, 0, 0, 3} },  \
    /* CVT 1920x1440 modes (60Hz with reduced blanking) */ \
    { "1920x1440_50",  HDMI_Unknown,     {1920, 1440,   55,  2592, 0, 1484,  336, 37,  200, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    { "1920x1440_60",  HDMI_Unknown,     {1920, 1440,    0,  2080, 0, 1481,   80, 34,   32, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2991, 11, 27, 0, 0, 1, 0, 0, 3} },  \
    /* CVT 2560x1440 with reduced blanking and pixelrep */ \
    { "2560x1440_60",  HDMI_Unknown,     {1280, 1440,    0,  1360, 0, 1481,   40, 33,   16, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_2X, TX_1X,  4, {0} },  \
};

ad_preset_t ad_presets_default[] = { \
    /* Generic 240p presets */ \
    { "720x240",                         { 720,  240,   80,   858, 0,  262,   57, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_GEN_4_3 }, \
    { "960x240",                         { 960,  240,   80,  1170, 0,  262,  128, 15,   54, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_GEN_4_3 }, \
    { "1280x240",                        {1280,  240,   80,  1560, 0,  262,  170, 15,   72, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_GEN_4_3 }, \
    { "1600x240",                        {1600,  240,   80,  1950, 0,  262,  212, 15,   90, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_GEN_4_3 }, \
    { "1920x240",                        {1920,  240,   80,  2340, 0,  262,  256, 15,  108, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_GEN_4_3 }, \
    /* Generic 288p presets */ \
    { "720x240",                         { 720,  288,   70,   864, 0,  312,   69, 19,   63, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P,  SM_GEN_4_3 }, \
    { "1536x240",                        {1536,  288,   70,  1872, 0,  312,  150, 19,  136, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P,  SM_GEN_4_3 }, \
    { "1920x240",                        {1920,  288,   70,  2340, 0,  312,  187, 19,  171, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P,  SM_GEN_4_3 }, \
    /* Generic 480i presets */ \
    { "720x480i",                        { 720,  240,   80,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I,  SM_GEN_4_3 }, \
    { "1280x480i",                       {1280,  240,   80,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I,  SM_GEN_4_3 }, \
    { "1920x480i",                       {1920,  240,   80,  2340, 0,  525,  256, 15,  108, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I,  SM_GEN_4_3 }, \
    /* Generic 576i presets */ \
    { "720x576i",                        { 720,  288,   70,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I,  SM_GEN_4_3 }, \
    { "1536x576i",                       {1536,  288,   70,  1872, 0,  625,  150, 19,  136, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I,  SM_GEN_4_3 }, \
    /* Generic 480p presets */ \
    { "720x480",                         { 720,  480,   80,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P,  SM_GEN_4_3 }, \
    { "1280x480",                        {1280,  480,   80,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P,  SM_GEN_4_3 }, \
    { "1920x480",                        {1920,  480,   80,  2340, 0,  525,  256, 30,  108, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P,  SM_GEN_4_3 }, \
    /* Generic 576p presets */ \
    { "720x576",                         { 720,  576,   70,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P,  SM_GEN_4_3 }, \
    { "1536x576",                        {1536,  576,   70,  1872, 0,  625,  150, 39,  136, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P,  SM_GEN_4_3 }, \
    /* VESA 640x480_60 */ \
    { "640x480_60",                      { 640,  480,   80,   800, 0,  525,   48, 33,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P,  SM_OPT_VGA480P60 }, \
    /* DTV 480p */ \
    { "480p",                            { 720,  480,   80,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P,  SM_OPT_DTV480P }, \
    /* NES/SNES */ \
    { "SNES 256x240",                    { 256,  240,    0,   341, 0,  262,   39, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_SNES_256COL }, \
    { "SNES 512x240",                    { 512,  240,    0,   682, 0,  262,   78, 14,   50, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_SNES_512COL }, \
    /* MD */ \
    { "MD 256x224",                      { 256,  224,    0,   342, 0,  262,   39, 22,   25, 3,  0},  2,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_MD_256COL }, \
    { "MD 320x224",                      { 320,  224,    0,   427,10,  262,   49, 22,   31, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_MD_320COL }, \
    /* PSX */ \
    { "PSX 256x240",                     { 256,  240,    0,   341, 5,  263,   37, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_PSX_256COL }, \
    { "PSX 320x240",                     { 320,  240,    0,   426,12,  263,   47, 14,   31, 3,  0},  7,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_PSX_320COL }, \
    { "PSX 384x240",                     { 384,  240,    0,   487,11,  263,   43, 14,   38, 3,  0},  6,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_PSX_384COL }, \
    { "PSX 512x240",                     { 512,  240,    0,   682,12,  263,   74, 14,   50, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_PSX_512COL }, \
    { "PSX 640x240",                     { 640,  240,    0,   853, 5,  263,   94, 14,   62, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P,  SM_OPT_PSX_640COL }, \
};

//const ad_mode_data_t adaptive_modes_default[] = {
const ad_mode_data_t adaptive_modes[] = { \
    /* Generic 261-line modes */ \
    { ADMODE_480p,                       ADPRESET_GEN_720x240,  261,  0, 1,  0, 0,  {7984,    16,    29,  3712, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_GEN_960x240,  261,  0, 2,  0, 0,  {5712,   656,  1131,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x240, 261,  0, 3,  0, 0,  {4154,  2348,  2610,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x240, 261,  0, 1,  0, 0,  {4156,   166,   377,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x240, 261,  0, 3,  0, 0,  {4156,   166,   377,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {3222,   282,   377,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200_60,               ADPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {2204,    68,   377,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {2072,   152,   783,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x240, 261,  0, 5,  0, 0,  {2070,  1058,  2349,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 262-line modes */ \
    { ADMODE_480p,                       ADPRESET_GEN_720x240,  262,  0, 1,  0, 0,  {8016,   256,  1048,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_GEN_960x240,  262,  0, 2,  0, 0,  {5688,  1400,  1703,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x240, 262,  0, 3,  0, 0,  {4137,   228,  2620,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x240, 262,  0, 1,  0, 0,  {4138,  1050,  1703,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x240, 262,  0, 3,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {3208,   840,  1703,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200_60,               ADPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {2193,  1385,  1703,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {2062,   130,   393,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x240, 262,  0, 5,  0, 0,  {2060,   700,  1179,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 263-line modes */ \
    { ADMODE_480p,                       ADPRESET_GEN_720x240,  263,  0, 1,  0, 0,  {7983,   860,  1052,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_GEN_960x240,  263,  0, 2,  0, 0,  {5665,   837,  3419,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x240, 263,  0, 3,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x240, 263,  0, 1,  0, 0,  {4120,  3192,  3419,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x240, 263,  0, 3,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {3194,  1186,  3419,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200_60,               ADPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {2183,  1795,  3419,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {2052,   428,   789,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x240, 263,  0, 5,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 264-line modes */ \
    { ADMODE_480p,                       ADPRESET_GEN_720x240,  264,  0, 1,  0, 0,  {8015,    48,   176,  3776, 0, 2,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_GEN_960x240,  264,  0, 2,  0, 0,  {5641,    11,    13,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x240, 264,  0, 3,  0, 0,  {4101,   208,   240,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x240, 264,  0, 1,  0, 0,  {4103,    20,    52,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x240, 264,  0, 3,  0, 0,  {4103,    20,    52,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {3180,     4,    13,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200_60,               ADPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {2173,    45,   143,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {2042,    82,    99,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x240, 264,  0, 5,  0, 0,  {2041,    31,   297,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 311-line modes */ \
    { ADMODE_576p,                       ADPRESET_GEN_720x288,  311,  0, 1,  0, 0,  {7976,   232,   311,  3712, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080i_50_CR,                ADPRESET_GEN_1536x288, 311,  0, 1,  0, 0,  {3405,  3569,  4043,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_50_CR,                ADPRESET_GEN_1536x288, 311,  0, 3,  0, 0,  {3405,  3569,  4043,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x288, 311,  0, 3,  0, 0,  {2275,  6391, 36387,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_50,               ADPRESET_GEN_1920x288, 311,  0, 4,  0, 0,  {2194,  4386, 20215,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 312-line modes */ \
    { ADMODE_576p,                       ADPRESET_GEN_720x288,  312,  0, 1,  0, 0,  {8013,   800,  1248,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_50_CR,                ADPRESET_GEN_1536x288, 312,  0, 1,  0, 0,  {3393,   220,   676,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_50_CR,                ADPRESET_GEN_1536x288, 312,  0, 3,  0, 0,  {3393,   220,   676,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x288, 312,  0, 3,  0, 0,  {2266,  1106,  4563,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_50,               ADPRESET_GEN_1920x288, 312,  0, 4,  0, 0,  {2185,   459,   845,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 313-line modes */ \
    { ADMODE_576p,                       ADPRESET_GEN_720x288,  313,  0, 1,  0, 0,  {7986,   504,  1252,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_50_CR,                ADPRESET_GEN_1536x288, 313,  0, 1,  0, 0,  {3380,  3452,  4069,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_50_CR,                ADPRESET_GEN_1536x288, 313,  0, 3,  0, 0,  {3380,  3452,  4069,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x288, 313,  0, 3,  0, 0,  {2257, 13411, 36621,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_50,               ADPRESET_GEN_1920x288, 313,  0, 4,  0, 0,  {2176, 18816, 20345,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 314-line modes */ \
    { ADMODE_576p,                       ADPRESET_GEN_720x288,  314,  0, 1,  0, 0,  {7959,   424,  1256,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_50_CR,                ADPRESET_GEN_1536x288, 314,  0, 1,  0, 0,  {3368,   920,  2041,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_50_CR,                ADPRESET_GEN_1536x288, 314,  0, 3,  0, 0,  {3368,   920,  2041,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x288, 314,  0, 3,  0, 0,  {2248, 10040, 18369,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_50,               ADPRESET_GEN_1920x288, 314,  0, 4,  0, 0,  {2168,  3688, 10205,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 525-line interlace modes */ \
    { ADMODE_240p,                       ADPRESET_GEN_720x480i, 525,  0, 0,  0, 0,  {8015,   127,   175,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x480i,525,  0, 3,  0, 0,  {4128,   608,  2625,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x480i,525,  0, 1,  0, 0,  {4129,    69,    91,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x480i,525,  0, 3,  0, 0,  {4129,    69,    91,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x480i,525,  0, 5,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 625-line interlace modes */ \
    { ADMODE_288p,                       ADPRESET_GEN_720x576i, 625,  0, 0,  0, 0,  {8018,   206,   625,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_50_CR,                ADPRESET_GEN_1536x576i,625,  0, 1,  0, 0,  {3387,     1,    13,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_50_CR,                ADPRESET_GEN_1536x576i,625,  0, 3,  0, 0,  {3387,     1,    13,   256, 0, 1,  0, 0, 0} },  \

    /* Generic 524-line modes */ \
    { ADMODE_240p,                       ADPRESET_GEN_720x480,  524,  0,-1,  0, 0,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x480, 524,  0, 1,  0, 0,  {4137,   228,  2620,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x480, 524,  0, 0,  0, 0,  {4138,  1050,  1703,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x480, 524,  0, 1,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x480, 524,  0, 2,  0, 0,  {2060,   700,  1179,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 525-line modes */ \
    { ADMODE_240p,                       ADPRESET_GEN_720x480,  525,  0,-1,  0, 0,  {3751,   302,   350,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x480, 525,  0, 1,  0, 0,  {4128,   608,  2625,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x480, 525,  0, 0,  0, 0,  {4129,    69,    91,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x480, 525,  0, 1,  0, 0,  {4129,    69,    91,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x480, 525,  0, 2,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 526-line modes */ \
    { ADMODE_240p,                       ADPRESET_GEN_720x480,  526,  0,-1,  0, 0,  {3743,   796,  1052,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_GEN_1280x480, 526,  0, 1,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_GEN_1280x480, 526,  0, 0,  0, 0,  {4120,  3192,  3419,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_GEN_1280x480, 526,  0, 1,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_GEN_1920x480, 526,  0, 2,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 624-line modes */ \
    { ADMODE_288p,                       ADPRESET_GEN_720x576,  624,  0,-1,  0, 0,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x576, 624,  0, 1,  0, 0,  {2266,  1106,  4563,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 625-line modes */ \
    { ADMODE_288p,                       ADPRESET_GEN_720x576,  625,  0,-1,  0, 0,  {3753,   103,   625,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x576, 625,  0, 1,  0, 0,  {2261, 11659, 14625,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 626-line modes */ \
    { ADMODE_288p,                       ADPRESET_GEN_720x576,  626,  0,-1,  0, 0,  {3746,   110,   313,  8032, 0, 4,  0, 0, 0} },  \
    { ADMODE_1920x1200_50,               ADPRESET_GEN_1536x576, 626,  0, 1,  0, 0,  {2257, 13411, 36621,     0, 0, 1,  1, 0, 3} },  \


    /* VESA 525-line modes */ \
    { ADMODE_240p,                       ADPRESET_OPT_VGA480P60,  0,  0,-1,  0, 0,  {4095, 34656,140000,  8096, 0, 4,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_OPT_VGA480P60,  0,  1, 1,  0, 0,  {4012,  7904, 35000,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_OPT_VGA480P60,  0,  1, 0,  0, 0,  {4013,    10,    14,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_OPT_VGA480P60,  0,  1, 1,  0, 0,  {4013,    10,    14,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_OPT_VGA480P60,  0,  2, 2,  0, 0,  {3243,   661,  2625,     0, 0, 1,  0, 0, 3} },  \

    /* DTV 525-line modes */ \
    { ADMODE_1280x1024_60,               ADPRESET_OPT_DTV480P,    0,  1, 1,  0, 0,  {3706,   206,   525,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_OPT_DTV480P,    0,  1, 0,  0, 0,  {3707,    71,    91,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_OPT_DTV480P,    0,  1, 1,  0, 0,  {3707,    71,    91,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1440_60,               ADPRESET_OPT_DTV480P,    0,  2, 2,  0, 0,  {2989,  1387,  3465,     0, 0, 1,  0, 0, 3} },  \

    /* SNES modes */
    { ADMODE_480p,                       ADPRESET_SNES_256_240,   0,  1, 1,  0, 0,  {4812,  3344, 16244,  3712, 0, 1,  0, 0, 0} },  \
    { ADMODE_480p,                       ADPRESET_SNES_512_240,   0,  0, 1,  0, 0,  {4812,  3344, 16244,  3712, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_SNES_256_240,   0,  3, 2,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_SNES_512_240,   0,  1, 2,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_SNES_256_240,   0,  4, 3,  0, 0,  {4805,  1118,  8122,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1280x1024_60,               ADPRESET_SNES_512_240,   0,  1, 3,  0, 0,  {4805,  1118,  8122,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_SNES_256_240,   0,  4, 1,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080i_60_LB,                ADPRESET_SNES_512_240,   0,  1, 1,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_SNES_256_240,   0,  4, 3,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_LB,                ADPRESET_SNES_512_240,   0,  1, 3,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_SNES_256_240,   0,  5, 4,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_60_CR,                ADPRESET_SNES_512_240,   0,  2, 4,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200_60,               ADPRESET_SNES_256_240,   0,  5, 4,  0, 0,  {3356, 12572, 44671,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1600x1200_60,               ADPRESET_SNES_512_240,   0,  2, 4,  0, 0,  {3356, 12572, 44671,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_SNES_256_240,   0,  5, 4,  0, 0,  {3168, 13920, 44671,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200_60,               ADPRESET_SNES_256_240,   0,  2, 4,  0, 0,  {3168, 13920, 44671,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_SNES_256_240,   0,  6, 5,  0, 0,  {3901, 17597, 44671,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440_60,               ADPRESET_SNES_512_240,   0,  3, 5,  0, 0,  {3901, 17597, 44671,     0, 0, 1,  0, 0, 3} },  \

    /* MD modes */
    { ADMODE_720p_60,                    ADPRESET_MD_256_224,     0,  3, 2,  0, 0,  {6559,   281,  2489,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_MD_320_224,     0,  2, 2,  0, 0,  {7619,  1941,  2489,   960, 0, 2,  0, 0, 0} },  \

    /* PSX modes */
    { ADMODE_720p_60,                    ADPRESET_PSX_256_240,    0,  3, 2,  0, 0,  {4782, 18698, 23933,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_PSX_320_240,    0,  2, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_PSX_384_240,    0,  2, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_PSX_512_240,    0,  1, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },  \
    { ADMODE_720p_60,                    ADPRESET_PSX_640_240,    0,  1, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },  \
};

const stdmode_t ad_mode_id_map[] = {STDMODE_240p,
                                    STDMODE_288p,
                                    STDMODE_480p,
                                    STDMODE_576p,
                                    STDMODE_720p_60,
                                    STDMODE_1280x1024_60,
                                    STDMODE_1080i_50,
                                    STDMODE_1080i_60,
                                    STDMODE_1080p_50,
                                    STDMODE_1080p_60,
                                    STDMODE_1080p_60,
                                    STDMODE_1600x1200_60,
                                    STDMODE_1920x1200_50,
                                    STDMODE_1920x1200_60,
                                    STDMODE_1920x1440_50,
                                    STDMODE_1920x1440_60};

const stdmode_t stdmode_idx_arr[] = {STDMODE_240p,
                                     STDMODE_288p,
                                     STDMODE_480i,
                                     STDMODE_480p,
                                     STDMODE_576i,
                                     STDMODE_576p,
                                     STDMODE_720p_60,
                                     STDMODE_1280x1024_60,
                                     STDMODE_1080i_60,
                                     STDMODE_1080p_60,
                                     STDMODE_1600x1200_60,
                                     STDMODE_1920x1200_60,
                                     STDMODE_1920x1440_60};

const unsigned num_stdmodes = sizeof(stdmode_idx_arr)/sizeof(stdmode_t);

mode_data_t video_modes[sizeof(video_modes_default)/sizeof(mode_data_t)];
//ad_mode_data_t adaptive_modes[sizeof(adaptive_modes_default)/sizeof(ad_mode_data_t)];


void set_default_vm_table() {
    memcpy(video_modes, video_modes_default, sizeof(video_modes_default));
    //memcpy(adaptive_modes, adaptive_modes_default, sizeof(adaptive_modes_default));
}

void vmode_hv_mult(mode_data_t *vmode, uint8_t h_mult, uint8_t v_mult) {
    // TODO: check limits
    vmode->timings.h_synclen *= h_mult;
    vmode->timings.h_backporch *= h_mult;
    vmode->timings.h_active *= h_mult;
    vmode->timings.h_total = h_mult * vmode->timings.h_total + ((h_mult * vmode->timings.h_total_adj * 5 + 50) / 100);

    vmode->timings.v_synclen *= v_mult;
    vmode->timings.v_backporch *= v_mult;
    vmode->timings.v_active *= v_mult;
    if (vmode->timings.interlaced && ((v_mult % 2) == 0)) {
        vmode->timings.interlaced = 0;
        vmode->timings.v_total *= (v_mult / 2);
    } else {
        vmode->timings.v_total *= v_mult;
    }
}

uint32_t estimate_dotclk(mode_data_t *vm_in, uint32_t h_hz) {
    if ((vm_in->type & VIDEO_SDTV) ||
        (vm_in->type & VIDEO_EDTV))
    {
        return h_hz * 858;
    } else {
        return vm_in->timings.h_total * h_hz;
    }
}

int get_adaptive_lm_mode(mode_data_t *vm_in, mode_data_t *vm_out, vm_mult_config_t *vm_conf)
{
    int i;
    ad_mode_id_t target_ad_id;
    ad_sampling_mode_t target_sm;
    ad_preset_t *ad_preset;
    int32_t v_linediff;
    uint32_t in_interlace_mult, out_interlace_mult, vtotal_ref;
    unsigned num_modes = sizeof(adaptive_modes)/sizeof(ad_mode_data_t);
    avconfig_t* cc = get_current_avconfig();
    memset(vm_out, 0, sizeof(mode_data_t));

    const ad_mode_id_t pm_ad_240p_map[] = {-1, ADMODE_480p, ADMODE_720p_60, ADMODE_1280x1024_60, ADMODE_1080i_60_LB, ADMODE_1080p_60_LB, ADMODE_1080p_60_CR, ADMODE_1600x1200_60, ADMODE_1920x1200_60, ADMODE_1920x1440_60};
    const ad_mode_id_t pm_ad_288p_map[] = {-1, ADMODE_576p, ADMODE_1080i_50_CR, ADMODE_1080p_50_CR, ADMODE_1920x1200_50, ADMODE_1920x1440_50};
    const ad_mode_id_t pm_ad_480i_map[] = {-1, ADMODE_240p, ADMODE_1280x1024_60, ADMODE_1080i_60_LB, ADMODE_1080p_60_LB, ADMODE_1920x1440_60};
    const ad_mode_id_t pm_ad_576i_map[] = {-1, ADMODE_288p, ADMODE_1080i_50_CR, ADMODE_1080p_50_CR};
    const ad_mode_id_t pm_ad_480p_map[] = {-1, ADMODE_240p, ADMODE_1280x1024_60, ADMODE_1080i_60_LB, ADMODE_1080p_60_LB, ADMODE_1920x1440_60};
    const ad_mode_id_t pm_ad_576p_map[] = {-1, ADMODE_288p, ADMODE_1920x1200_50};

    const ad_sampling_mode_t sm_240p_288p_map[] = {SM_GEN_4_3, SM_OPT_SNES_256COL, SM_OPT_SNES_512COL, SM_OPT_MD_256COL, SM_OPT_MD_320COL, SM_OPT_PSX_256COL, SM_OPT_PSX_320COL, SM_OPT_PSX_384COL, SM_OPT_PSX_512COL, SM_OPT_PSX_640COL};
    const ad_sampling_mode_t sm_480i_576i_map[] = {SM_GEN_4_3};
    const ad_sampling_mode_t sm_480p_map[] = {SM_GEN_4_3, SM_OPT_DTV480P, SM_OPT_VGA480P60};
    const ad_sampling_mode_t sm_576p_map[] = {SM_GEN_4_3};

    if (!cc->adapt_lm)
        return -1;

    for (i=0; i<num_modes; i++) {
        ad_preset = &ad_presets_default[adaptive_modes[i].preset_id];

        if (ad_preset->group == GROUP_240P) {
            target_ad_id = pm_ad_240p_map[cc->pm_ad_240p];
            target_sm = sm_240p_288p_map[cc->sm_ad_240p_288p];
        } else if (ad_preset->group == GROUP_288P) {
            target_ad_id = pm_ad_288p_map[cc->pm_ad_288p];
            target_sm = sm_240p_288p_map[cc->sm_ad_240p_288p];
        } else if (ad_preset->group == GROUP_480I) {
            target_ad_id = pm_ad_480i_map[cc->pm_ad_480i];
            target_sm = sm_480i_576i_map[cc->sm_ad_480i_576i];
        } else if (ad_preset->group == GROUP_576I) {
            target_ad_id = pm_ad_576i_map[cc->pm_ad_576i];
            target_sm = sm_480i_576i_map[cc->sm_ad_480i_576i];
        } else if (ad_preset->group == GROUP_480P) {
            target_ad_id = pm_ad_480p_map[cc->pm_ad_480p];
            target_sm = sm_480p_map[cc->sm_ad_480p];
        } else if (ad_preset->group == GROUP_576P) {
            target_ad_id = pm_ad_576p_map[cc->pm_ad_576p];
            target_sm = sm_576p_map[cc->sm_ad_576p];
        } else {
            target_ad_id = -1;
            target_sm = -1;
        }

        if (ad_preset->timings_i.v_hz_max && (vm_in->timings.v_hz_max > ad_preset->timings_i.v_hz_max))
            continue;

        if (((adaptive_modes[i].v_total_override && (vm_in->timings.v_total == adaptive_modes[i].v_total_override)) || (!adaptive_modes[i].v_total_override && (vm_in->timings.v_total == ad_preset->timings_i.v_total))) &&
            (!vm_in->timings.h_total || (vm_in->timings.h_total == ad_preset->timings_i.h_total)) &&
            (vm_in->timings.interlaced == ad_preset->timings_i.interlaced) &&
            (target_ad_id == adaptive_modes[i].id) &&
            (vm_in->timings.h_total || (target_sm == ad_preset->sm)))
        {
            if (!vm_in->timings.h_active)
                vm_in->timings.h_active = ad_preset->timings_i.h_active;
            if (!vm_in->timings.v_active)
                vm_in->timings.v_active = ad_preset->timings_i.v_active;
            if ((!vm_in->timings.h_synclen) || (!vm_in->timings.h_backporch))
                vm_in->timings.h_synclen = ad_preset->timings_i.h_synclen;
            if (!vm_in->timings.v_synclen)
                vm_in->timings.v_synclen = ad_preset->timings_i.v_synclen;
            if (!vm_in->timings.h_backporch)
                vm_in->timings.h_backporch = ad_preset->timings_i.h_backporch;
            if (!vm_in->timings.v_backporch)
                vm_in->timings.v_backporch = ad_preset->timings_i.v_backporch;
            vm_in->timings.h_total = ad_preset->timings_i.h_total;
            vm_in->timings.h_total_adj = ad_preset->timings_i.h_total_adj;
            vm_in->sampler_phase = ad_preset->sampler_phase;
            vm_in->type = ad_preset->type;
            strncpy(vm_in->name, ad_preset->name, 14);
            in_interlace_mult = vm_in->timings.interlaced ? 2 : 1;

            memcpy(vm_out, &video_modes_default[ad_mode_id_map[adaptive_modes[i].id]], sizeof(mode_data_t));
            out_interlace_mult = vm_out->timings.interlaced ? 2 : 1;

            vm_conf->x_rpt = adaptive_modes[i].x_rpt;
            vm_conf->y_rpt = adaptive_modes[i].y_rpt;
            vm_conf->h_skip = ad_preset->h_skip;
            vm_conf->x_offset = ((vm_out->timings.h_active-vm_in->timings.h_active*(vm_conf->x_rpt+1))/2) + adaptive_modes[i].x_offset_i;
            vm_conf->x_start_lb = (vm_conf->x_offset >= 0) ? 0 : (-vm_conf->x_offset / (vm_conf->x_rpt+1));
            vm_conf->x_size = vm_in->timings.h_active*(vm_conf->x_rpt+1);
            if (vm_conf->x_size >= 2048)
                vm_conf->x_size = 2047;
            if (vm_conf->y_rpt == (uint8_t)(-1)) {
                vm_conf->y_start_lb = ((vm_in->timings.v_active - (vm_out->timings.v_active*2))/2) + adaptive_modes[i].y_offset_i*2;
                vm_conf->y_offset = -vm_conf->y_start_lb/2;
                vm_conf->y_size = vm_in->timings.v_active/2;
            } else {
                vm_conf->y_start_lb = ((vm_in->timings.v_active - (vm_out->timings.v_active/(vm_conf->y_rpt+1)))/2) + adaptive_modes[i].y_offset_i;
                vm_conf->y_offset = -(vm_conf->y_rpt+1)*vm_conf->y_start_lb;
                vm_conf->y_size = vm_in->timings.v_active*(vm_conf->y_rpt+1);
            }

            vm_out->si_pclk_mult = 0;
            memcpy(&vm_out->si_ms_conf, &adaptive_modes[i].si_ms_conf, sizeof(si5351_ms_config_t));

            // calculate the time (in output lines, rounded up) from source frame start to the point where first to-be-visible line has been written into linebuf
            v_linediff = (((vm_in->timings.v_synclen + vm_in->timings.v_backporch + ((vm_conf->y_start_lb < 0) ? 0 : vm_conf->y_start_lb) + 1) * vm_out->timings.v_total * in_interlace_mult) / (vm_in->timings.v_total * out_interlace_mult)) + 1;

            // subtract the previous value from the total number of output blanking/empty lines. Resulting value indicates how many lines output framestart must be offset
            v_linediff = (vm_out->timings.v_synclen + vm_out->timings.v_backporch + ((vm_conf->y_offset < 0) ? 0 : vm_conf->y_offset)) - v_linediff;

            // if linebuf is read faster than written, output framestart must be delayed accordingly to avoid read pointer catching write pointer
            vtotal_ref = (vm_conf->y_rpt == (uint8_t)(-1)) ? ((vm_in->timings.v_total*out_interlace_mult)/2) : (vm_in->timings.v_total*out_interlace_mult*(vm_conf->y_rpt+1));
            if (vm_out->timings.v_total * in_interlace_mult > vtotal_ref)
                v_linediff -= (((vm_in->timings.v_active * vm_out->timings.v_total * in_interlace_mult) / (vm_in->timings.v_total * out_interlace_mult)) - vm_conf->y_size);

            vm_conf->framesync_line = (v_linediff < 0) ? (vm_out->timings.v_total/out_interlace_mult)+v_linediff : v_linediff;

            printf("framesync_line = %u\nx_start_lb: %d, x_offset: %d, x_size: %u\ny_start_lb: %d, y_offset: %d, y_size: %u\n", vm_conf->framesync_line, vm_conf->x_start_lb, vm_conf->x_offset, vm_conf->x_size, vm_conf->y_start_lb, vm_conf->y_offset, vm_conf->y_size);

            return i;
        }
    }

    return -1;
}

int get_pure_lm_mode(mode_data_t *vm_in, mode_data_t *vm_out, vm_mult_config_t *vm_conf)
{
    int i;
    unsigned num_modes = sizeof(video_modes)/sizeof(mode_data_t);
    avconfig_t* cc = get_current_avconfig();
    mode_flags valid_lm[] = { MODE_PT, (MODE_L2 | (MODE_L2<<cc->l2_mode)), (MODE_L3_GEN_16_9<<cc->l3_mode), (MODE_L4_GEN_4_3<<cc->l4_mode), (MODE_L5_GEN_4_3<<cc->l5_mode) };
    mode_flags target_lm;
    uint8_t pt_only = 0;
    uint8_t nonsampled_h_mult = 0, nonsampled_v_mult = 0;
    uint8_t upsample2x = vm_in->timings.h_total ? 0 : 1;

    // one for each video_group
    uint8_t* group_ptr[] = { &pt_only, &cc->pm_240p, &cc->pm_240p, &cc->pm_384p, &cc->pm_480i, &cc->pm_480i, &cc->pm_480p, &cc->pm_480p, &cc->pm_1080i };

    for (i=0; i<num_modes; i++) {
        switch (video_modes[i].group) {
            case GROUP_384P:
                //fixed Line2x/3x mode for 240x360p
                valid_lm[2] = MODE_L2_240x360;
                valid_lm[3] = MODE_L3_240x360;
                valid_lm[4] = MODE_L3_GEN_16_9;
                if ((!vm_in->timings.h_total) && (video_modes[i].timings.v_total == 449)) {
                    if (!strncmp(video_modes[i].name, "720x400_70", 10)) {
                        if (cc->s400p_mode == 0)
                            continue;
                    } else if (!strncmp(video_modes[i].name, "640x400_70", 10)) {
                        if (cc->s400p_mode == 1)
                            continue;
                    }
                }
                break;
            case GROUP_480I:
            case GROUP_576I:
                //fixed Line3x/4x mode for 480i
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L4_GEN_4_3;
                break;
            case GROUP_480P:
                 if (video_modes[i].vic == HDMI_480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                            if (vm_in->timings.h_synclen > 82)
                                continue;
                            break;
                        case 1: // DTV 480p
                            break;
                        default:
                            continue;
                    }
                } else if (video_modes[i].vic == HDMI_640x480p60) {
                    switch (cc->s480p_mode) {
                        case 0: // Auto
                        case 2: // VESA 640x480@60
                            break;
                        default:
                            continue;
                    }
                }
                break;
            default:
                break;
        }

        if (video_modes[i].timings.v_hz_max && (vm_in->timings.v_hz_max > video_modes[i].timings.v_hz_max))
            continue;

        target_lm = valid_lm[*group_ptr[video_modes[i].group]];

        // HDMI input modes
        if (vm_in->timings.h_total) {
            if (target_lm >= MODE_L5_GEN_4_3)
                nonsampled_v_mult = 5;
            else if (target_lm >= MODE_L4_GEN_4_3)
                nonsampled_v_mult = 4;
            else if (target_lm >= MODE_L3_GEN_16_9)
                nonsampled_v_mult = 3;
            else if (target_lm >= MODE_L2)
                nonsampled_v_mult = 2;
            else
                nonsampled_v_mult = 1;

            target_lm = MODE_PT;
        }

        if ((target_lm & video_modes[i].flags) &&
            (vm_in->timings.interlaced == video_modes[i].timings.interlaced) &&
            (vm_in->timings.v_total <= (video_modes[i].timings.v_total+LINECNT_MAX_TOLERANCE)))
        {

            if (!vm_in->timings.h_active)
                vm_in->timings.h_active = video_modes[i].timings.h_active;
            if (!vm_in->timings.v_active)
                vm_in->timings.v_active = video_modes[i].timings.v_active;
            if ((!vm_in->timings.h_synclen) || (!vm_in->timings.h_backporch))
                vm_in->timings.h_synclen = video_modes[i].timings.h_synclen;
            if (!vm_in->timings.v_synclen)
                vm_in->timings.v_synclen = video_modes[i].timings.v_synclen;
            if (!vm_in->timings.h_backporch)
                vm_in->timings.h_backporch = video_modes[i].timings.h_backporch;
            if (!vm_in->timings.v_backporch)
                vm_in->timings.v_backporch = video_modes[i].timings.v_backporch;
            if (!vm_in->timings.h_total)
                vm_in->timings.h_total = video_modes[i].timings.h_total;
            vm_in->timings.h_total_adj = video_modes[i].timings.h_total_adj;
            vm_in->sampler_phase = video_modes[i].sampler_phase;
            vm_in->type = video_modes[i].type;
            if (!vm_in->vic)
                vm_in->vic = video_modes[i].vic;
            if (vm_in->name[0] == 0)
                strncpy(vm_in->name, video_modes[i].name, 14);

            memcpy(vm_out, vm_in, sizeof(mode_data_t));
            vm_out->vic = HDMI_Unknown;
            vm_out->si_pclk_mult = 1;
            vm_out->tx_pixelrep = TX_1X;
            vm_out->hdmitx_pixr_ifr = TX_1X;

            vm_conf->x_rpt = 0;
            vm_conf->y_rpt = 0;
            vm_conf->h_skip = 0;
            vm_conf->x_offset = 0;
            vm_conf->y_offset = 0;
            vm_conf->x_size = 0;
            vm_conf->y_size = 0;
            vm_conf->x_start_lb = 0;
            vm_conf->y_start_lb = 0;

            target_lm &= video_modes[i].flags;    //ensure L2 mode uniqueness

            if (nonsampled_v_mult) {
                if (nonsampled_v_mult > 1)
                    nonsampled_h_mult = (((((uint32_t)vm_in->timings.v_active*nonsampled_v_mult*40)/3)/vm_in->timings.h_active)+5)/10;

                vm_conf->x_rpt = (nonsampled_h_mult == 0) ? 0 : (nonsampled_h_mult-1);
                vm_conf->y_rpt = nonsampled_v_mult-1;

                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);

                if ((vm_out->timings.v_hz_max*vm_out->timings.v_total*vm_out->timings.h_total)>>vm_out->timings.interlaced < 25000000UL) {
                    vm_out->tx_pixelrep = TX_2X;
                    vm_out->hdmitx_pixr_ifr = TX_2X;
                }

                vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
            } else {
                switch (target_lm) {
                    case MODE_PT:
                        vm_out->vic = vm_in->vic;
                        // Upsample / pixel-repeat horizontal resolution if necessary to fulfill min. 25MHz TMDS clock requirement
                        if ((vm_out->timings.v_hz_max*vm_out->timings.v_total*vm_out->timings.h_total)>>vm_out->timings.interlaced < 25000000UL) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vm_out->tx_pixelrep = TX_2X;
                            }
                            vm_out->hdmitx_pixr_ifr = TX_2X;
                        }
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2:
                        vm_conf->y_rpt = 1;

                        // Upsample / pixel-repeat horizontal resolution of 384p/480p/960i modes
                        if ((video_modes[i].group == GROUP_384P) || (video_modes[i].group == GROUP_480P) || (video_modes[i].group == GROUP_576P) || ((video_modes[i].group == GROUP_1080I) && (video_modes[i].timings.h_total < 1200))) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_512_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = 1;
                        vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_256_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = 2;
                        vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_320_COL:
                    case MODE_L2_384_COL:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = 1;
                        vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L2_240x360:
                        vm_conf->y_rpt = 1;
                        vm_conf->x_rpt = 4;
                        vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_GEN_16_9:
                        vm_conf->y_rpt = 2;

                        // Upsample / pixel-repeat horizontal resolution of 480i mode
                        if ((video_modes[i].group == GROUP_480I) || (video_modes[i].group == GROUP_576I)) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_GEN_4_3:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_size = vm_out->timings.h_active;
                        vm_out->timings.h_synclen /= 3;
                        vm_out->timings.h_backporch /= 3;
                        vm_out->timings.h_active /= 3;
                        vm_conf->x_offset = vm_out->timings.h_active/2;
                        vm_out->timings.h_total /= 3;
                        vm_out->timings.h_total_adj = 0;
                        vmode_hv_mult(vm_out, 4, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = 4;
                        break;
                    case MODE_L3_512_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = 1;
                        vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_384_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = 2;
                        vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_320_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = 3;
                        vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_256_COL:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = 4;
                        vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L3_240x360:
                        vm_conf->y_rpt = 2;
                        vm_conf->x_rpt = 6;
                        vm_conf->h_skip = 6;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 13;
                        break;
                    case MODE_L4_GEN_4_3:
                        vm_conf->y_rpt = 3;

                        // Upsample / pixel-repeat horizontal resolution of 480i mode
                        if ((video_modes[i].group == GROUP_480I) || (video_modes[i].group == GROUP_576I)) {
                            if (upsample2x) {
                                vmode_hv_mult(vm_in, 2, 1);
                                vmode_hv_mult(vm_out, 2, VM_OUT_YMULT);
                            } else {
                                vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                                vm_out->tx_pixelrep = TX_2X;
                            }
                        } else {
                            vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        }
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_512_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = 1;
                        vm_conf->h_skip = 1;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_384_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = 2;
                        vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_320_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = 3;
                        vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L4_256_COL:
                        vm_conf->y_rpt = 3;
                        vm_conf->x_rpt = 4;
                        vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L5_GEN_4_3:
                        vm_conf->y_rpt = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        break;
                    case MODE_L5_512_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = 2;
                        vm_conf->h_skip = 2;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 40;
                        break;
                    case MODE_L5_384_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = 3;
                        vm_conf->h_skip = 3;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 30;
                        break;
                    case MODE_L5_320_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = 4;
                        vm_conf->h_skip = 4;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 24;
                        break;
                    case MODE_L5_256_COL:
                        vm_conf->y_rpt = 4;
                        vm_conf->x_rpt = 5;
                        vm_conf->h_skip = 5;
                        vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                        vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                        //cm.hsync_cut = 20;
                        break;
                    default:
                        printf("WARNING: invalid target_lm\n");
                        continue;
                        break;
                }
            }

            // Line5x format
            if (vm_conf->y_rpt == 4) {
                // adjust output width to 1920
                if ((cc->l5_fmt != 1) && (nonsampled_h_mult == 0)) {
                    vm_conf->x_size = vm_out->timings.h_active;
                    vm_conf->x_offset = (1920-vm_out->timings.h_active)/2;
                    vm_out->timings.h_synclen = (vm_out->timings.h_total - 1920)/4;
                    vm_out->timings.h_backporch = (vm_out->timings.h_total - 1920)/2;
                    vm_out->timings.h_active = 1920;
                }

                // adjust output height to 1080
                if (cc->l5_fmt == 0) {
                    vm_conf->y_start_lb = (vm_out->timings.v_active-1080)/10;
                    vm_out->timings.v_backporch += 5*vm_conf->y_start_lb;
                    vm_out->timings.v_active = 1080;
                }
            }

            vm_conf->framesync_line = vm_in->timings.interlaced ? ((vm_out->timings.v_total>>vm_out->timings.interlaced)-(vm_conf->y_rpt+1)) : 0;

            if (vm_conf->x_size == 0)
                vm_conf->x_size = vm_out->timings.h_active;
            if (vm_conf->y_size == 0)
                vm_conf->y_size = vm_out->timings.v_active;

            /*if (cm.hdmitx_vic == HDMI_Unknown)
                cm.hdmitx_vic = cm.cc.default_vic;*/

            return i;
        }
    }

    return -1;
}

int get_standard_mode(unsigned stdmode_idx_arr_idx, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out)
{
    stdmode_idx_arr_idx = stdmode_idx_arr_idx % num_stdmodes;

    memset(vm_conf, 0, sizeof(vm_mult_config_t));
    memset(vm_in, 0, sizeof(mode_data_t));
    memcpy(vm_out, &video_modes_default[stdmode_idx_arr[stdmode_idx_arr_idx]], sizeof(mode_data_t));

    return 0;
}
