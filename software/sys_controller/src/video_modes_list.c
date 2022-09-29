//
// Copyright (C) 2020-2021  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

/* Legacy Pure LM modes */
#ifdef VM_STATIC_INCLUDE
static
#endif
const mode_data_t video_modes_plm_default[] = {
    /* 240p modes */
    { "1600x240",      HDMI_Unknown,     {1600,  240,   6000,  2046, 0,  262,  202, 15,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L5_GEN_4_3),                                                        0, 0 },
    { "1280x240",      HDMI_Unknown,     {1280,  240,   6000,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     0, 0 },
    { "960x240",       HDMI_Unknown,     { 960,  240,   6000,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_4_3),                                                        0, 0 },
    { "512x240",       HDMI_Unknown,     { 512,  240,   6000,   682, 0,  262,   77, 14,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  0, 0 },
    { "384x240",       HDMI_Unknown,     { 384,  240,   6000,   512, 0,  262,   59, 14,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  0, 0 },
    { "320x240",       HDMI_Unknown,     { 320,  240,   6000,   426, 0,  262,   49, 14,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  0, 0 },
    { "256x240",       HDMI_Unknown,     { 256,  240,   6000,   341, 0,  262,   39, 14,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  0, 0 },
    { "240p",          HDMI_240p60_PR2x, { 720,  240,   6005,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* 288p modes */
    { "1600x240L",     HDMI_Unknown,     {1600,  240,   5000,  2046, 0,  312,  202, 43,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L5_GEN_4_3),                                                        0, 0 },
    { "1280x288",      HDMI_Unknown,     {1280,  288,   5000,  1560, 0,  312,  170, 19,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     0, 0 },
    { "960x288",       HDMI_Unknown,     { 960,  288,   5000,  1170, 0,  312,  128, 19,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_4_3),                                                        0, 0 },
    { "512x240LB",     HDMI_Unknown,     { 512,  240,   5000,   682, 0,  312,   77, 41,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  0, 0 },
    { "384x240LB",     HDMI_Unknown,     { 384,  240,   5000,   512, 0,  312,   59, 41,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  0, 0 },
    { "320x240LB",     HDMI_Unknown,     { 320,  240,   5000,   426, 0,  312,   49, 41,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  0, 0 },
    { "256x240LB",     HDMI_Unknown,     { 256,  240,   5000,   341, 0,  312,   39, 41,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  0, 0 },
    { "288p",          HDMI_288p50,      { 720,  288,   5008,   864, 0,  312,   69, 19,   63, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* 360p: GBI */
    { "480x360",       HDMI_Unknown,     { 480,  360,   6000,   600, 0,  375,   63, 10,   38, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    { "240x360",       HDMI_Unknown,     { 256,  360,   6000,   300, 0,  375,   24, 10,   18, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                      0, 0 },
    /* 384p: Sega Model 2 (real vtotal=423, avoid collision with PC88/98 and VGA400p) */
    { "384p",          HDMI_Unknown,     { 496,  384,   5500,   640, 0,  408,   50, 29,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* 400p line3x */
    { "1600x400",      HDMI_Unknown,     {1600,  400,   7000,  2000, 0,  449,  120, 34,  240, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                       0, 0 },
    /* 720x400@70Hz, VGA Mode 3+/7+ */
    { "720x400_70",    HDMI_Unknown,     { 720,  400,   7000,   900, 0,  449,   64, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* 640x400@70Hz, VGA Mode 13h */
    { "640x400_70",    HDMI_Unknown,     { 640,  400,   7000,   800, 0,  449,   48, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* 384p: X68k @ 24kHz */
    { "640x384",       HDMI_Unknown,     { 640,  384,   5500,   800, 0,  492,   48, 63,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* ~525-line modes */
    { "480i",          HDMI_480i60_PR2x, { 720,  240,   5994,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 0, 0 },
    { "480p",          HDMI_480p60,      { 720,  480,   5994,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    { "640x480_60",    HDMI_640x480p60,  { 640,  480,   6000,   800, 0,  525,   48, 33,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* X68k @ 31kHz */
    { "640x512",       HDMI_Unknown,     { 640,  512,   6000,   800, 0,  568,   48, 28,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    /* ~625-line modes */
    { "576i",          HDMI_576i50,      { 720,  288,   5000,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_576I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 0, 0 },
    { "576p",          HDMI_576p50,      { 720,  576,   5000,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_576P,   (MODE_PT | MODE_L2),                                                      0, 0 },
    { "800x600_60",    HDMI_Unknown,     { 800,  600,   6000,  1056, 0,  628,   88, 23,  128, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    /* CEA 720p modes */
    { "720p_50",       HDMI_720p50,      {1280,  720,   5000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   MODE_PT,                                                                  0, 0 },
    { "720p_60",       HDMI_720p60,      {1280,  720,   6000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   MODE_PT,                                                                  0, 0 },
    /* VESA XGA,1280x960 and SXGA modes */
    { "1024x768_60",   HDMI_Unknown,     {1024,  768,   6000,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    { "1280x960_60",   HDMI_Unknown,     {1280,  960,   6000,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    { "1280x1024_60",  HDMI_Unknown,     {1280, 1024,   6000,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    /* PS2 GSM 960i mode */
    { "640x960i",      HDMI_Unknown,     { 640,  480,   6000,   800, 0, 1050,   48, 33,   96, 2,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_1080I,  (MODE_PT | MODE_L2),                                                      0, 0 },
    /* CEA 1080i/p modes */
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   5000,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      0, 0 },
    { "1080i_60",      HDMI_1080i60,     {1920,  540,   6000,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      0, 0 },
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   5000,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,   6000,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  0, 0 },
    /* VESA UXGA mode */
    { "1600x1200_60",  HDMI_Unknown,     {1600, 1200,   6000,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  0, 0 },
};

/* Output modes for Adaptive LM and Scaler */
#ifdef VM_STATIC_INCLUDE
static
#endif
const mode_data_t video_modes_default[] = {
    /* 240p/288p CRT modes */
    { "2560x240",      HDMI_Unknown,     {2560,  240,   6000,  3120, 0,  262,  340, 15,  144, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   MODE_CRT,  TX_1X, TX_1X },
    { "2560x288",      HDMI_Unknown,     {2560,  288,   5000,  3120, 0,  312,  340, 19,  144, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   MODE_CRT,  TX_1X, TX_1X },
    /* DTV 480i/480p */
    { "480i",          HDMI_480i60_PR2x, { 720,  240,   5994,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_480I,   0,         TX_2X, TX_2X },
    { "480p",          HDMI_480p60,      { 720,  480,   5994,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_480P,   0,         TX_1X, TX_1X },
    /* 640x480 VESA/GTF modes with 4x horizontal for CRT */
    { "2560x480_60",   HDMI_Unknown,     {2560,  480,   6000,  3200, 0,  525,  321, 33,  255, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   MODE_CRT,  TX_1X, TX_1X },
    { "2560x480_100",  HDMI_Unknown,     {2560,  480,  10000,  3392, 0,  509,  417, 25,  255, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   MODE_CRT,  TX_1X, TX_1X },
    { "2560x480_120",  HDMI_Unknown,     {2560,  480,  12000,  3392, 0,  515,  417, 31,  255, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   MODE_CRT,  TX_1X, TX_1X },
    /* 540p CRT modes, based on 1080i */
    { "1920x540_50",   HDMI_Unknown,     {1920,  540,   5000,  2640, 0,  562,  148, 15,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,               GROUP_1080I,  MODE_CRT,  TX_1X, TX_1X },
    { "1920x540_60",   HDMI_Unknown,     {1920,  540,   6000,  2200, 0,  562,  148, 15,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,               GROUP_1080I,  MODE_CRT,  TX_1X, TX_1X },
    /* DTV 576i/576p */
    { "576i",          HDMI_576i50,      { 720,  288,   5000,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_576I,   0,         TX_2X, TX_2X },
    { "576p",          HDMI_576p50,      { 720,  576,   5000,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_576P,   0,         TX_1X, TX_1X },
    /* CEA 720p modes */
    { "720p_50",       HDMI_720p50,      {1280,  720,   5000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   0,         TX_1X, TX_1X },
    { "720p_60",       HDMI_720p60,      {1280,  720,   6000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   0,         TX_1X, TX_1X },
    /* CEA 720p 100/120Hz */
    { "720p_100",      HDMI_Unknown,     {1280,  720,  10000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   0,         TX_1X, TX_1X },
    { "720p_120",      HDMI_Unknown,     {1280,  720,  12000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_720P,   0,         TX_1X, TX_1X },
    /* VESA XGA and SXGA modes */
    { "1024x768_60",   HDMI_Unknown,     {1024,  768,   6000,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    { "1280x1024_60",  HDMI_Unknown,     {1280, 1024,   6002,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    /* CEA 1080i/p modes */
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   5000,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  0,         TX_1X, TX_1X },
    { "1080i_60",      HDMI_1080i60,     {1920,  540,   6000,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  0,         TX_1X, TX_1X },
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   5000,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   0,         TX_1X, TX_1X },
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,   6000,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   0,         TX_1X, TX_1X },
    /* 1080p 100/120Hz (CVT-RB) */
    { "1080p_100",     HDMI_Unknown,     {1920, 1080,  10000,  2080, 0, 1133,   80, 45,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   0,         TX_1X, TX_1X },
    { "1080p_120",     HDMI_Unknown,     {1920, 1080,  12000,  2080, 0, 1144,   80, 56,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   0,         TX_1X, TX_1X },
    /* VESA UXGA mode */
    { "1600x1200_60",  HDMI_Unknown,     {1600, 1200,   6000,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    /* CVT 1920x1200 modes (60Hz with reduced blanking) */
    { "1920x1200_50",  HDMI_Unknown,     {1920, 1200,   5000,  2560, 0, 1238,  320, 29,  200, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    { "1920x1200_60",  HDMI_Unknown,     {1920, 1200,   6000,  2080, 0, 1235,   80, 26,   32, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    /* CVT 1920x1440 modes (60Hz with reduced blanking) */
    { "1920x1440_50",  HDMI_Unknown,     {1920, 1440,   5000,  2592, 0, 1484,  336, 37,  200, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    { "1920x1440_60",  HDMI_Unknown,     {1920, 1440,   6000,  2080, 0, 1481,   80, 34,   32, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    /* 2560x1440 (CVT-RB) */
    { "2560x1440_50",  HDMI_Unknown,     {2560, 1440,   5000,  2720, 0, 1474,   80, 26,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
    { "2560x1440_60",  HDMI_Unknown,     {2560, 1440,   6000,  2720, 0, 1481,   80, 33,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   0,         TX_1X, TX_1X },
};

/* Sampling presets for Adaptive LM and Scaler */
#ifdef VM_STATIC_INCLUDE
static
#endif
const smp_preset_t smp_presets_default[] = {
    /* Generic 240p presets */
    { "Gen. 704x240",   SM_GEN_4_3,        { 704,  240,      0,   858, 0,  262,   65, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "Gen. 960x240",   SM_GEN_4_3,        { 960,  240,      0,  1170, 0,  262,  128, 15,   54, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "Gen. 1280x240",  SM_GEN_4_3,        {1280,  240,      0,  1560, 0,  262,  170, 15,   72, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "Gen. 1600x240",  SM_GEN_4_3,        {1600,  240,      0,  1950, 0,  262,  212, 15,   90, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "Gen. 1920x240",  SM_GEN_4_3,        {1920,  240,      0,  2340, 0,  262,  256, 15,  108, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* Generic 288p presets */
    { "Gen. 704x288",   SM_GEN_4_3,        { 704,  288,      0,   864, 0,  312,   77, 19,   63, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "Gen. 1536x288",  SM_GEN_4_3,        {1536,  288,      0,  1872, 0,  312,  150, 19,  136, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "Gen. 1920x288",  SM_GEN_4_3,        {1920,  288,      0,  2340, 0,  312,  187, 19,  171, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    /* Generic 384p presets */
    { "Gen. 1024x384",  SM_GEN_4_3,        {1024,  384,      0,  1280, 0,  429,  91,  34,  137, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_384P },
    { "Gen. 1600x400",  SM_GEN_4_3,        {1600,  400,      0,  2000, 0,  429,  142, 34,  213, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_384P },
    { "Gen. 1920x360",  SM_GEN_4_3,        {1920,  400,      0,  2400, 0,  429,  171, 34,  255, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_384P },
    /* Generic 480i presets */
    { "Gen. 704x480i",  SM_GEN_4_3,        { 704,  240,      0,   858, 0,  525,   65, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "Gen. 1280x480i", SM_GEN_4_3,        {1280,  240,      0,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "Gen. 1920x480i", SM_GEN_4_3,        {1920,  240,      0,  2340, 0,  525,  256, 15,  108, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Generic 480i 16:9 presets */
    { "Gen. 1280x480i", SM_GEN_16_9,       {1280,  240,      0,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "Gen. 1707x480i", SM_GEN_16_9,       {1707,  240,      0,  2080, 0,  525,  228, 15,   96, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Generic 576i presets */
    { "Gen. 704x576i",  SM_GEN_4_3,        { 704,  288,      0,   864, 0,  625,   77, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    { "Gen. 1536x576i", SM_GEN_4_3,        {1536,  288,      0,  1872, 0,  625,  150, 19,  136, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    /* Generic 480p presets */
    { "Gen. 704x480",   SM_GEN_4_3,        { 704,  480,      0,   858, 0,  525,   68, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "Gen. 1280x480",  SM_GEN_4_3,        {1280,  480,      0,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "Gen. 1920x480",  SM_GEN_4_3,        {1920,  480,      0,  2340, 0,  525,  256, 30,  108, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* Generic 480p 16:9 presets */
    { "Gen. 1280x480",  SM_GEN_16_9,       {1280,  480,      0,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "Gen. 1707x480",  SM_GEN_16_9,       {1707,  480,      0,  2080, 0,  525,  228, 30,   96, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* Generic 576p presets */
    { "Gen. 704x576",   SM_GEN_4_3,        { 704,  576,      0,   864, 0,  625,   76, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    { "Gen. 1536x576",  SM_GEN_4_3,        {1536,  576,      0,  1872, 0,  625,  150, 39,  136, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },

    /* DTV 480i */
    { "DTV 480i",       SM_OPT_DTV480I,    { 720,  240,   6500,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "DTV 480i wide",  SM_OPT_DTV480I_WS, { 720,  240,   6500,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* DTV 576i */
    { "DTV 576i",       SM_OPT_DTV576I,    { 720,  288,   5500,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    { "DTV 576i wide",  SM_OPT_DTV576I_WS, { 720,  288,   5500,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    /* DTV 480p */
    { "DTV 480p",       SM_OPT_DTV480P,    { 720,  480,   6500,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "DTV 480p wide",  SM_OPT_DTV480P_WS, { 720,  480,   6500,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* DTV 576p */
    { "DTV 576p",       SM_OPT_DTV576P,    { 720,  576,   5500,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    { "DTV 576p wide",  SM_OPT_DTV576P_WS, { 720,  576,   5500,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    /* DTV 720p */
    { "DTV 720p_50",    SM_OPT_PC_HDTV,    {1280,  720,   5500,  1980, 0,  750,  220, 20,   40, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_720P },
    { "DTV 720p_60",    SM_OPT_PC_HDTV,    {1280,  720,      0,  1650, 0,  750,  220, 20,   40, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_720P },
    /* DTV 1080i */
    { "DTV 1080i_50",   SM_OPT_PC_HDTV,    {1920,  540,   5500,  2640, 0, 1125,  148, 15,   44, 5,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_1080I},
    { "DTV 1080i_60",   SM_OPT_PC_HDTV,    {1920,  540,      0,  2200, 0, 1125,  148, 15,   44, 5,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_1080I},
    /* DTV 1080p */
    { "DTV 1080p_50",   SM_OPT_PC_HDTV,    {1920, 1080,   5500,  2640, 0, 1125,  148, 36,   44, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },
    { "DTV 1080p_60",   SM_OPT_PC_HDTV,    {1920, 1080,      0,  2200, 0, 1125,  148, 36,   44, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },

    /* 640x350@70Hz, VGA Mode 0*,1*,2*,3*,F,10 */
    { "PC 640x350_70",  SM_OPT_VGA_640x350, { 640,  350,   7500,   800, 0,  449,   48, 59,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_384P },
    /* 720x350@70Hz, VGA Mode 7 */
    { "PC 720x350_70",  SM_OPT_VGA_720x350, { 720,  350,   7500,   900, 0,  449,   64, 59,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_384P },
    /* 640x400@70Hz, VGA Mode 0-6,D-E,13 */
    { "PC 640x400_70",  SM_OPT_VGA_640x400, { 640,  400,   7500,   800, 0,  449,   48, 34,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_384P },
    /* 720x400@70Hz, VGA Mode 0+,1+,2+,3+,7+ */
    { "PC 720x400_70",  SM_OPT_VGA_720x400, { 720,  400,   7500,   900, 0,  449,   64, 34,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_384P },
    /* VESA DMT 640x480 */
    { "PC 640x480_60",  SM_OPT_VESA_640x480, { 640,  480,   6500,   800, 0,  525,   48, 33,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_480P },
    { "PC 640x480_72",  SM_OPT_VESA_640x480, { 640,  480,   7300,   832, 0,  520,  128, 29,   40, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_480P },
    { "PC 640x480_75",  SM_OPT_VESA_640x480, { 640,  480,   8000,   840, 0,  500,  120, 16,   64, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_480P },
    { "PC 640x480_85",  SM_OPT_VESA_640x480, { 640,  480,      0,   832, 0,  509,   80, 25,   56, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,  GROUP_480P },
    /* Other VESA modes */
    { "PC 800x600_60",  SM_OPT_PC_HDTV,    { 800,  600,      0,  1056, 0,  628,   88, 23,  128, 4,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },
    { "PC 1024x768_60", SM_OPT_PC_HDTV,    {1024,  768,      0,  1344, 0,  806,  160, 29,  136, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },
    { "PC 1280x960_60", SM_OPT_PC_HDTV,    {1280,  960,      0,  1800, 0, 1000,  312, 36,  112, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },
    { "PC 1280x1024_60",SM_OPT_PC_HDTV,    {1280, 1024,      0,  1688, 0, 1066,  248, 38,  112, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },
    { "PC 1600x1200_60",SM_OPT_PC_HDTV,    {1600, 1200,      0,  2160, 0, 1250,  304, 46,  192, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },
    /* CVT misc */
    { "PC 1920x1200_60",SM_OPT_PC_HDTV,    {1920, 1200,      0,  2080, 0, 1235,   80, 26,   32, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,    VIDEO_PC,  GROUP_NONE },

    /* NES/SNES */
    { "SNES 256x240",   SM_OPT_SNES_256COL,{ 256,  240,      0,   341, 0,  262,   39, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SNES 512x240",   SM_OPT_SNES_512COL,{ 512,  240,      0,   682, 0,  262,   78, 14,   50, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SNES 512x448i",  SM_OPT_SNES_512COL,{ 512,  224,      0,   682, 0,  525,   78, 14,   50, 3,  1},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* MD */
    { "MD 256x224",     SM_OPT_MD_256COL,  { 256,  224,      0,   342, 0,  262,   39, 24,   25, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "MD 320x224",     SM_OPT_MD_320COL,  { 320,  224,      0,   427,10,  262,   52, 24,   31, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "MD 256x448i",    SM_OPT_MD_256COL,  { 256,  224,      0,   342, 0,  525,   39, 24,   25, 3,  1},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "MD 320x448i",    SM_OPT_MD_320COL,  { 320,  224,      0,   427,10,  525,   52, 24,   31, 3,  1},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* PSX */
    { "PSX 256x240",    SM_OPT_PSX_256COL, { 256,  240,      0,   341, 6,  263,   37, 14,   25, 3,  0},  9,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 320x240",    SM_OPT_PSX_320COL, { 320,  240,      0,   426,12,  263,   47, 14,   31, 3,  0},  7,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 384x240",    SM_OPT_PSX_384COL, { 384,  240,      0,   487,11,  263,   43, 14,   38, 3,  0},  6,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 512x240",    SM_OPT_PSX_512COL, { 512,  240,      0,   682,12,  263,   74, 14,   50, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 640x240",    SM_OPT_PSX_640COL, { 640,  240,      0,   853, 5,  263,   94, 14,   62, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 512x480i",   SM_OPT_PSX_512COL, { 512,  240,      0,   682,12,  525,   74, 14,   50, 3,  1},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "PSX 640x480i",   SM_OPT_PSX_640COL, { 640,  240,      0,   853, 5,  525,   94, 14,   62, 3,  1},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Saturn */
    { "SAT 320x240",    SM_OPT_SAT_320COL, { 320,  240,      0,   426,10,  263,   48, 15,   31, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 640x240",    SM_OPT_SAT_640COL, { 640,  240,      0,   853, 0,  263,   96, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 352x240",    SM_OPT_SAT_352COL, { 352,  240,      0,   455, 0,  263,   45, 15,   34, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 704x240",    SM_OPT_SAT_704COL, { 704,  240,      0,   910, 0,  263,   90, 15,   68, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 640x480i",   SM_OPT_SAT_640COL, { 640,  240,      0,   853, 0,  525,   96, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "SAT 704x480i",   SM_OPT_SAT_704COL, { 704,  240,      0,   910, 0,  525,   90, 15,   68, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* N64 */
    { "N64 320x240",    SM_OPT_N64_320COL, { 320,  240,      0,   386,15,  263,   36, 14,   22, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "N64 640x240",    SM_OPT_N64_640COL, { 640,  240,      0,   773,10,  263,   72, 14,   44, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "N64 640x480i",   SM_OPT_N64_640COL, { 640,  240,      0,   773,10,  525,   72, 14,   44, 3,  1},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Neo Geo */
    { "NeoGeo 320x224", SM_OPT_NG_320COL,  { 320,  224,      0,   384, 0,  264,   28, 21,   29, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* GBI */
    { "GBI 240x360",    SM_OPT_GBI_240COL, { 240,  360,      0,   300, 0,  375,   24, 10,   18, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_384P },
    /* DC/PS2/GC 640col AR-correct modes. DTV 480 preset should be used for games which render full 480 lines */
    { "DC 640x448i",    SM_OPT_DC_640COL,  { 640,  224,      0,   858, 0,  525,   95, 23,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "DC 640x448",     SM_OPT_DC_640COL,  { 640,  448,      0,   858, 0,  525,   95, 46,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* PS2 GSM modes */
    { "PS2 512x448",    SM_OPT_PS2_512COL, { 512,  448,      0,   858, 0,  525,  160, 46,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "PS2 640x960i",   SM_OPT_PC_HDTV,    { 640,  480,      0,   800, 0, 1050,   48, 33,   96, 2,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_1080I},

    { "PC98 640x400",   SM_OPT_PC98_640COL,{ 640,  400,      0,   848, 0,  440,   80, 31,   64, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,    GROUP_384P },
};
