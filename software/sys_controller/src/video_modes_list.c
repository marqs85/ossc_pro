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

const mode_data_t video_modes_default[] = {
    /* 240p modes */
    { "1600x240",      HDMI_Unknown,     {1600,  240,   6000,  2046, 0,  262,  202, 15,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "1280x240",      HDMI_Unknown,     {1280,  240,   6000,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },
    { "960x240",       HDMI_Unknown,     { 960,  240,   6000,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "512x240",       HDMI_Unknown,     { 512,  240,   6000,   682, 0,  262,   77, 14,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },
    { "384x240",       HDMI_Unknown,     { 384,  240,   6000,   512, 0,  262,   59, 14,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },
    { "320x240",       HDMI_Unknown,     { 320,  240,   6000,   426, 0,  262,   49, 14,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },
    { "256x240",       HDMI_Unknown,     { 256,  240,   6000,   341, 0,  262,   39, 14,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },
    { "240p",          HDMI_240p60_PR2x, { 720,  240,   6005,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    /* 288p modes */
    { "1600x240L",     HDMI_Unknown,     {1600,  240,   5000,  2046, 0,  312,  202, 43,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "1280x288",      HDMI_Unknown,     {1280,  288,   5000,  1560, 0,  312,  170, 19,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },
    { "960x288",       HDMI_Unknown,     { 960,  288,   5000,  1170, 0,  312,  128, 19,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "512x240LB",     HDMI_Unknown,     { 512,  240,   5000,   682, 0,  312,   77, 41,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },
    { "384x240LB",     HDMI_Unknown,     { 384,  240,   5000,   512, 0,  312,   59, 41,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },
    { "320x240LB",     HDMI_Unknown,     { 320,  240,   5000,   426, 0,  312,   49, 41,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },
    { "256x240LB",     HDMI_Unknown,     { 256,  240,   5000,   341, 0,  312,   39, 41,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },
    { "288p",          HDMI_288p50,      { 720,  288,   5008,   864, 0,  312,   69, 19,   63, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    /* 360p: GBI */
    { "480x360",       HDMI_Unknown,     { 480,  360,   6000,   600, 0,  375,   63, 10,   38, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "240x360",       HDMI_Unknown,     { 256,  360,   6000,   300, 0,  375,   24, 10,   18, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                      TX_1X, TX_1X,  1, {0} },
    /* 384p: Sega Model 2 (real vtotal=423, avoid collision with PC88/98 and VGA400p) */
    { "384p",          HDMI_Unknown,     { 496,  384,   5500,   640, 0,  408,   50, 29,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 400p line3x */
    { "1600x400",      HDMI_Unknown,     {1600,  400,   7000,  2000, 0,  449,  120, 34,  240, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                       TX_1X, TX_1X,  1, {0} },
    /* 720x400@70Hz, VGA Mode 3+/7+ */
    { "720x400_70",    HDMI_Unknown,     { 720,  400,   7000,   900, 0,  449,   64, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 640x400@70Hz, VGA Mode 13h */
    { "640x400_70",    HDMI_Unknown,     { 640,  400,   7000,   800, 0,  449,   48, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 384p: X68k @ 24kHz */
    { "640x384",       HDMI_Unknown,     { 640,  384,   5500,   800, 0,  492,   48, 63,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* ~525-line modes */
    { "480i",          HDMI_480i60_PR2x, { 720,  240,   5994,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    { "480p",          HDMI_480p60,      { 720,  480,   5994,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "640x480_60",    HDMI_640x480p60,  { 640,  480,   6000,   800, 0,  525,   48, 33,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* X68k @ 31kHz */
    { "640x512",       HDMI_Unknown,     { 640,  512,   6000,   800, 0,  568,   48, 28,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* ~625-line modes */
    { "576i",          HDMI_576i50,      { 720,  288,   5000,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_576I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    { "576p",          HDMI_576p50,      { 720,  576,   5000,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_576P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "800x600_60",    HDMI_Unknown,     { 800,  600,   6000,  1056, 0,  628,   88, 23,  128, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    /* CEA 720p modes */
    { "720p_50",       HDMI_720p50,      {1280,  720,   5000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "720p_60",       HDMI_720p60,      {1280,  720,   6000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    /* CEA 720p 100/120Hz */
    { "720p_100",      HDMI_Unknown,     {1280,  720,  10000,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3731, 40, 72, 544, 0, 4, 0, 0, 0} },
    { "720p_120",      HDMI_Unknown,     {1280,  720,  12000,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3235, 10, 18, 256, 0, 1, 0, 0, 0} },
    /* VESA XGA,1280x960 and SXGA modes */
    { "1024x768_60",   HDMI_Unknown,     {1024,  768,   6000,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    { "1280x960_60",   HDMI_Unknown,     {1280,  960,   6000,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },
    { "1280x1024_60",  HDMI_Unknown,     {1280, 1024,   6000,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },
    /* PS2 GSM 960i mode */
    { "640x960i",      HDMI_Unknown,     { 640,  480,   6000,   800, 0, 1050,   48, 33,   96, 2,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* CEA 1080i/p modes */
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   5000,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "1080i_60",      HDMI_1080i60,     {1920,  540,   6000,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   5000,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,   6000,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },
    /* 1080p 100/120Hz (CVT-RB) */
    { "1080p_100",     HDMI_Unknown,     {1920, 1080,  10000,  2080, 0, 1133,   80, 45,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3953, 7, 9, 0, 0, 1, 0, 0, 3} },
    { "1080p_120",     HDMI_Unknown,     {1920, 1080,  12000,  2080, 0, 1144,   80, 56,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {4901, 25, 27, 0, 0, 1, 0, 0, 3} },
    /* VESA UXGA mode */
    { "1600x1200_60",  HDMI_Unknown,     {1600, 1200,   6000,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  6, {0} },
    /* CVT 1920x1200 modes (60Hz with reduced blanking) */
    { "1920x1200_50",  HDMI_Unknown,     {1920, 1200,   5000,  2560, 0, 1238,  320, 29,  200, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2488, 8, 9, 0, 0, 1, 0, 0, 3} },
    { "1920x1200_60",  HDMI_Unknown,     {1920, 1200,   6000,  2080, 0, 1235,   80, 26,   32, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2408, 8, 27, 0, 0, 1, 0, 0, 3} },
    /* CVT 1920x1440 modes (60Hz with reduced blanking) */
    { "1920x1440_50",  HDMI_Unknown,     {1920, 1440,   5000,  2592, 0, 1484,  336, 37,  200, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3133, 17, 27, 0, 0, 1, 0, 0, 3} },
    { "1920x1440_60",  HDMI_Unknown,     {1920, 1440,   6000,  2080, 0, 1481,   80, 34,   32, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2991, 11, 27, 0, 0, 1, 0, 0, 3} },
    /* 2560x1440 (CVT-RB) */
    { "2560x1440_50",  HDMI_Unknown,     {2560, 1440,   5000,  2720, 0, 1474,   80, 26,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3285, 1, 3, 0, 0, 1, 0, 0, 3} },
    { "2560x1440_60",  HDMI_Unknown,     {2560, 1440,   6000,  2720, 0, 1481,   80, 33,   32, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {4067, 5, 9, 0, 0, 1, 0, 0, 3} },
};

smp_preset_t smp_presets_default[] = {
    /* Generic 240p presets */
    { "720x240",      SM_GEN_4_3,        { 720,  240,   6500,   858, 0,  262,   57, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "960x240",      SM_GEN_4_3,        { 960,  240,   6500,  1170, 0,  262,  128, 15,   54, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1280x240",     SM_GEN_4_3,        {1280,  240,   6500,  1560, 0,  262,  170, 15,   72, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1600x240",     SM_GEN_4_3,        {1600,  240,   6500,  1950, 0,  262,  212, 15,   90, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1920x240",     SM_GEN_4_3,        {1920,  240,   6500,  2340, 0,  262,  256, 15,  108, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* Generic 288p presets */
    { "720x288",      SM_GEN_4_3,        { 720,  288,   5500,   864, 0,  312,   69, 19,   63, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "1536x288",     SM_GEN_4_3,        {1536,  288,   5500,  1872, 0,  312,  150, 19,  136, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "1920x288",     SM_GEN_4_3,        {1920,  288,   5500,  2340, 0,  312,  187, 19,  171, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    /* Generic 480i presets */
    { "720x480i",     SM_GEN_4_3,        { 720,  240,   6500,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "1280x480i",    SM_GEN_4_3,        {1280,  240,   6500,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "1920x480i",    SM_GEN_4_3,        {1920,  240,   6500,  2340, 0,  525,  256, 15,  108, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Generic 480i 16:9 presets */
    { "1280x480i",    SM_GEN_16_9,       {1280,  240,   6500,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "1707x480i",    SM_GEN_16_9,       {1707,  240,   6500,  2080, 0,  525,  228, 15,   96, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Generic 576i presets */
    { "720x576i",     SM_GEN_4_3,        { 720,  288,   5500,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    { "1536x576i",    SM_GEN_4_3,        {1536,  288,   5500,  1872, 0,  625,  150, 19,  136, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    /* Generic 480p presets */
    { "720x480",      SM_GEN_4_3,        { 720,  480,   6500,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "1280x480",     SM_GEN_4_3,        {1280,  480,   6500,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "1920x480",     SM_GEN_4_3,        {1920,  480,   6500,  2340, 0,  525,  256, 30,  108, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* Generic 480p 16:9 presets */
    { "1280x480",     SM_GEN_16_9,       {1280,  480,   6500,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "1707x480",     SM_GEN_16_9,       {1707,  480,   6500,  2080, 0,  525,  228, 30,   96, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* Generic 576p presets */
    { "720x576",      SM_GEN_4_3,        { 720,  576,   5500,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    { "1536x576",     SM_GEN_4_3,        {1536,  576,   5500,  1872, 0,  625,  150, 39,  136, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },

    /* DTV 480i */
    { "480i",         SM_OPT_DTV480I,    { 720,  240,   6500,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "480i wide",    SM_OPT_DTV480I_WS, { 720,  240,   6500,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* DTV 576i */
    { "576i",         SM_OPT_DTV576I,    { 720,  288,   5500,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    { "576i wide",    SM_OPT_DTV576I_WS, { 720,  288,   5500,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    /* 384p: Sega Model 2 */
    { "384p",         SM_OPT_PC_HDTV,    { 496,  384,      0,   640, 0,  423,   50, 29,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_384P },
    /* 720x400@70Hz, VGA Mode 3+/7+ */
    { "720x400_70",   SM_OPT_PC_HDTV,    { 720,  400,   7500,   900, 0,  449,   64, 34,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,    GROUP_384P },
    /* 640x400@70Hz, VGA Mode 13h */
    { "640x400_70",   SM_OPT_PC_HDTV,    { 640,  400,   7500,   800, 0,  449,   48, 34,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,    GROUP_384P },
    /* VESA 640x480_60 */
    { "640x480_60",   SM_OPT_VGA480P60,  { 640,  480,   6500,   800, 0,  525,   48, 33,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* DTV 480p */
    { "480p",         SM_OPT_DTV480P,    { 720,  480,   6500,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "480p wide",    SM_OPT_DTV480P_WS, { 720,  480,   6500,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* DTV 576p */
    { "576p",         SM_OPT_DTV576P,    { 720,  576,   5500,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    { "576p wide",    SM_OPT_DTV576P_WS, { 720,  576,   5500,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    /* DTV 720p */
    { "720p_50",      SM_OPT_PC_HDTV,    {1280,  720,   5500,  1980, 0,  750,  220, 20,   40, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },
    { "720p_60",      SM_OPT_PC_HDTV,    {1280,  720,      0,  1650, 0,  750,  220, 20,   40, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },
    /* DTV 1080i */
    { "1080i_50",     SM_OPT_PC_HDTV,    {1920,  540,   5500,  2640, 0, 1125,  148, 15,   44, 5,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_1080I},
    { "1080i_60",     SM_OPT_PC_HDTV,    {1920,  540,      0,  2200, 0, 1125,  148, 15,   44, 5,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_1080I},
    /* DTV 1080p */
    { "1080p_50",     SM_OPT_PC_HDTV,    {1920, 1080,   5500,  2640, 0, 1125,  148, 36,   44, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },
    { "1080p_60",     SM_OPT_PC_HDTV,    {1920, 1080,   6500,  2200, 0, 1125,  148, 36,   44, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_HDTV,  GROUP_NONE },

    /* NES/SNES */
    { "SNES 256x240", SM_OPT_SNES_256COL,{ 256,  240,      0,   341, 0,  262,   39, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SNES 512x240", SM_OPT_SNES_512COL,{ 512,  240,      0,   682, 0,  262,   78, 14,   50, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* MD */
    { "MD 256x224",   SM_OPT_MD_256COL,  { 256,  224,      0,   342, 0,  262,   39, 24,   25, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "MD 320x224",   SM_OPT_MD_320COL,  { 320,  224,      0,   427,10,  262,   52, 24,   31, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* PSX */
    { "PSX 256x240",  SM_OPT_PSX_256COL, { 256,  240,      0,   341, 6,  263,   37, 14,   25, 3,  0},  9,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 320x240",  SM_OPT_PSX_320COL, { 320,  240,      0,   426,12,  263,   47, 14,   31, 3,  0},  7,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 384x240",  SM_OPT_PSX_384COL, { 384,  240,      0,   487,11,  263,   43, 14,   38, 3,  0},  6,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 512x240",  SM_OPT_PSX_512COL, { 512,  240,      0,   682,12,  263,   74, 14,   50, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 640x240",  SM_OPT_PSX_640COL, { 640,  240,      0,   853, 5,  263,   94, 14,   62, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* Saturn */
    { "SAT 320x240",  SM_OPT_SAT_320COL, { 320,  240,      0,   426,10,  263,   48, 15,   31, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 640x240",  SM_OPT_SAT_640COL, { 640,  240,      0,   853, 0,  263,   96, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 352x240",  SM_OPT_SAT_352COL, { 352,  240,      0,   455, 0,  263,   45, 15,   34, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SAT 704x240",  SM_OPT_SAT_704COL, { 704,  240,      0,   910, 0,  263,   90, 15,   68, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* N64 */
    { "N64 320x240",  SM_OPT_N64_320COL, { 320,  240,      0,   386,15,  263,   36, 14,   22, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "N64 640x240",  SM_OPT_N64_640COL, { 640,  240,      0,   773,10,  263,   72, 14,   44, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* Neo Geo */
    { "NG 320x224",   SM_OPT_NG_320COL,  { 320,  224,      0,   384, 0,  264,   28, 21,   29, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
};

const fl_config_t framelock_configs[] = {
    /* Generic 261-line modes */
    { STDMODE_480p,             SMPPRESET_GEN_720x240,  SMPPRESET_GEN_720x240,  261,  {7984,    16,    29,  3712, 0, 1,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  261,  {5712,   656,  1131,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  261,  {5712,   656,  1131,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 261,  {4154,  2348,  2610,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 261,  {4156,   166,   377,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 261,  {4156,   166,   377,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 261,  {3222,   282,   377,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 261,  {5472,   352,   783,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 261,  {2204,    68,   377,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 261,  {2072,   152,   783,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 261,  {2070,  1058,  2349,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 261,  {2865,  1543, 30537,     0, 0, 1,  0, 0, 3} },

    /* Generic 262-line modes */
    { STDMODE_480p,             SMPPRESET_GEN_720x240,  SMPPRESET_GEN_720x240,  262,  {8016,   256,  1048,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  262,  {5688,  1400,  1703,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  262,  {5688,  1400,  1703,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 262,  {4137,   228,  2620,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 262,  {4138,  1050,  1703,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 262,  {4138,  1050,  1703,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 262,  {3208,   840,  1703,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 262,  {5449,   239,   393,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 262,  {2193,  1385,  1703,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 262,  {2062,   130,   393,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 262,  {2060,   700,  1179,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 262,  {2852,  2468, 15327,     0, 0, 1,  0, 0, 3} },

    /* Generic 263-line modes */
    { STDMODE_480p,             SMPPRESET_GEN_720x240,  SMPPRESET_GEN_720x240,  263,  {7983,   860,  1052,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  263,  {5665,   837,  3419,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  263,  {5665,   837,  3419,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 263,  {4119,  1078,  2630,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 263,  {4120,  3192,  3419,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 263,  {4120,  3192,  3419,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 263,  {3194,  1186,  3419,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 263,  {5426,   742,   789,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 263,  {2183,  1795,  3419,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 263,  {2052,   428,   789,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 263,  {2050,  1922,  2367,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 263,  {2839, 11371, 30771,     0, 0, 1,  0, 0, 3} },

    /* Generic 264-line modes */
    { STDMODE_480p,             SMPPRESET_GEN_720x240,  SMPPRESET_GEN_720x240,  264,  {8015,    48,   176,  3776, 0, 2,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  264,  {5641,    11,    13,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_960x240,  SMPPRESET_GEN_960x240,  264,  {5641,    11,    13,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 264,  {4101,   208,   240,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 264,  {4103,    20,    52,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 264,  {4103,    20,    52,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 264,  {3180,     4,    13,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_1280x240, SMPPRESET_GEN_1280x240, 264,  {5404,     4,     9,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 264,  {2173,    45,   143,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_GEN_1600x240, SMPPRESET_GEN_1600x240, 264,  {2042,    82,    99,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 264,  {2041,    31,   297,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x240, SMPPRESET_GEN_1920x240, 264,  {2826,  2606,  3861,     0, 0, 1,  0, 0, 3} },

    /* Generic 311-line modes */
    { STDMODE_576p,             SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  311,  {7976,   232,   311,  3712, 0, 1,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  311,  {7976,   232,   311,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  311,  {7976,   232,   311,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 311,  {3405,  3569,  4043,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 311,  {3405,  3569,  4043,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 311,  {3633,    65,  2799,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 311,  {2275,  6391, 36387,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 311,  {2194,  4386, 20215,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 311,  {2308, 26228, 36387,     0, 0, 1,  0, 0, 3} },

    /* Generic 312-line modes */
    { STDMODE_576p,             SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  312,  {8013,   800,  1248,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  312,  {7949,    56,   104,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  312,  {7949,    56,   104,  256,  0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 312,  {3393,   220,   676,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 312,  {3393,   220,   676,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 312,  {3619,   259,   351,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 312,  {2266,  1106,  4563,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 312,  {2185,   459,   845,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 312,  {2299,  3103,  4563,     0, 0, 1,  0, 0, 3} },

    /* Generic 313-line modes */
    { STDMODE_576p,             SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  313,  {7986,   504,  1252,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  313,  {7922,   158,   313,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  313,  {7922,   158,   313,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 313,  {3380,  3452,  4069,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 313,  {3380,  3452,  4069,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 313,  {3606,  1514,  2817,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 313,  {2257, 13411, 36621,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 313,  {2176, 18816, 20345,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 313,  {2290, 25526, 36621,     0, 0, 1,  0, 0, 3} },

    /* Generic 314-line modes */
    { STDMODE_576p,             SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  314,  {7959,   424,  1256,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  314,  {7895,   202,   314,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x288,  SMPPRESET_GEN_720x288,  314,  {7895,   202,   314,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 314,  {3368,   920,  2041,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 314,  {3368,   920,  2041,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 314,  {3593,   595,  1413,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x288, SMPPRESET_GEN_1536x288, 314,  {2248, 10040, 18369,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 314,  {2168,  3688, 10205,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_50,     SMPPRESET_GEN_1920x288, SMPPRESET_GEN_1920x288, 314,  {2281, 14167, 18369,     0, 0, 1,  0, 0, 3} },

    /* Generic 525-line interlace modes */
    { STDMODE_240p,             SMPPRESET_GEN_720x480i, SMPPRESET_GEN_720x480i, 525,  {8015,   127,   175,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x480i,SMPPRESET_GEN_1280x480i,525,  {4128,   608,  2625,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x480i,SMPPRESET_GEN_1280x480i,525,  {4129,    69,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x480i,SMPPRESET_GEN_1280x480i,525,  {4129,    69,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_1280x480i,SMPPRESET_GEN_1280x480i,525,  {5438,   398,  1575,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x480i,SMPPRESET_GEN_1920x480i,525,  {2055,  3277,  4725,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x480i,SMPPRESET_GEN_1920x480i,525,  {2845, 46259, 61425,     0, 0, 1,  0, 0, 3} },

    /* Generic 525-line interlace 16:9 modes */
    { STDMODE_1080i_60,         SMPPRESET_GEN_1707x480i_WS, SMPPRESET_GEN_1707x480i_WS, 525,  {2969,    29,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1707x480i_WS, SMPPRESET_GEN_1707x480i_WS, 525,  {2969,    29,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1280x480i_WS, SMPPRESET_GEN_1280x480i_WS, 525,  {4524, 12892, 20475,     0, 0, 1,  0, 0, 3} },

    /* Generic 625-line interlace modes */
    { STDMODE_288p,             SMPPRESET_GEN_720x576i, SMPPRESET_GEN_720x576i, 625,  {8018,   206,   625,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_1536x576i,SMPPRESET_GEN_1536x576i,625,  {3387,     1,    13,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_1536x576i,SMPPRESET_GEN_1536x576i,625,  {3387,     1,    13,   256, 0, 1,  0, 0, 0} },

    /* Generic 524-line modes */
    { STDMODE_240p,             SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  524,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  524,  {3715,  1419,  1703,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  524,  {3715,  1419,  1703,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 524,  {4137,   228,  2620,   544, 0, 4,  1, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 524,  {4138,  1050,  1703,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 524,  {4138,  1050,  1703,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  524,  {4907,   253,   393,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 524,  {2060,   700,  1179,     0, 0, 1,  1, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 524,  {2852,  2468, 15327,     0, 0, 1,  1, 0, 3} },

    /* Generic 525-line modes */
    { STDMODE_240p,             SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  525,  {3751,   302,   350,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  525,  {3707,    71,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  525,  {3707,    71,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 525,  {4128,   608,  2625,   544, 0, 4,  1, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 525,  {4129,    69,    91,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 525,  {4129,    69,    91,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  525,  {4897,   101,   315,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 525,  {2055,  3277,  4725,     0, 0, 1,  1, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 525,  {2845, 46259, 61425,     0, 0, 1,  1, 0, 3} },

    /* Generic 525-line 16:9 modes */
    { STDMODE_1080i_60,         SMPPRESET_GEN_1707x480_WS, SMPPRESET_GEN_1707x480_WS,   525,  {2969,    29,    91,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1707x480_WS, SMPPRESET_GEN_1707x480_WS,   525,  {2969,    29,    91,   256, 0, 1,  1, 0, 0} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1280x480_WS, SMPPRESET_GEN_1280x480_WS,   525,  {4524, 12892, 20475,     0, 0, 1,  1, 0, 3} },

    /* Generic 526-line modes */
    { STDMODE_240p,             SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  526,  {3743,   796,  1052,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  526,  {3699,  2591,  3419,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  526,  {3699,  2591,  3419,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 526,  {4119,  1078,  2630,   544, 0, 4,  1, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 526,  {4120,  3192,  3419,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_GEN_1280x480, SMPPRESET_GEN_1280x480, 526,  {4120,  3192,  3419,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_GEN_720x480,  SMPPRESET_GEN_720x480,  526,  {4887,    29,   789,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 526,  {2050,  1922,  2367,     0, 0, 1,  1, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_GEN_1920x480, SMPPRESET_GEN_1920x480, 526,  {2839, 11371, 30771,     0, 0, 1,  1, 0, 3} },

    /* Generic 624-line modes */
    { STDMODE_288p,             SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3718,   160,   208,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3718,   160,   208,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3718,   160,   208,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3718,   160,   208,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  624,  {3964,     8,   162,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x576, SMPPRESET_GEN_1536x576, 624,  {2266,  1106,  4563,     0, 0, 1,  1, 0, 3} },

    /* Generic 625-line modes */
    { STDMODE_288p,             SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3753,   103,   625,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3712,     0,     1,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3712,     0,     1,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3712,     0,     1,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3712,     0,     1,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  625,  {3956,  2996,  3375,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x576, SMPPRESET_GEN_1536x576, 625,  {2261, 11659, 14625,     0, 0, 1,  1, 0, 3} },

    /* Generic 626-line modes */
    { STDMODE_288p,             SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3746,   110,   313,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3705,   158,   626,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3705,   158,   626,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3705,   158,   626,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3705,   158,   626,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_100,        SMPPRESET_GEN_720x576,  SMPPRESET_GEN_720x576,  626,  {3949,  6329,  8451,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_50,     SMPPRESET_GEN_1536x576, SMPPRESET_GEN_1536x576, 626,  {2257, 13411, 36621,     0, 0, 1,  1, 0, 3} },


    /* DTV 480i 4:3 & 16:9 modes */
    { STDMODE_240p,             SMPPRESET_DTV480I,      SMPPRESET_DTV480I,        0,  {8015,   127,   175,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_DTV480I,      SMPPRESET_DTV480I_WS,     0,  {7927,    51,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_DTV480I,      SMPPRESET_DTV480I_WS,     0,  {7927,    51,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_DTV480I,      SMPPRESET_DTV480I,        0,  {7924,   412,   525,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_DTV480I,      SMPPRESET_DTV480I_WS,     0,  {7927,    51,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_DTV480I,      SMPPRESET_DTV480I_WS,     0,  {7927,    51,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1920x1440_60,     SMPPRESET_DTV480I,      SMPPRESET_DTV480I,        0,  {6490,  2774,  3465,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_DTV480I,      SMPPRESET_DTV480I_WS,     0,  {8645, 22903, 45045,     0, 0, 1,  0, 0, 3} },

    /* DTV 576i 4:3 modes */
    { STDMODE_288p,             SMPPRESET_DTV576I,      SMPPRESET_DTV576I,        0,  {8018,   206,   625,  8032, 0, 4,  0, 0, 0} },
    { STDMODE_720p_50,          SMPPRESET_DTV576I,      SMPPRESET_DTV576I,        0,  {7936,     0,     1,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_100,         SMPPRESET_DTV576I,      SMPPRESET_DTV576I,        0,  {7936,     0,     1,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080i_50,         SMPPRESET_DTV576I,      SMPPRESET_DTV576I,        0,  {7936,     0,     1,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_50,         SMPPRESET_DTV576I,      SMPPRESET_DTV576I,        0,  {7936,     0,     1,   256, 0, 1,  0, 0, 0} },

    /* VESA 640x480_60 modes */
    { STDMODE_240p,             SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4095, 34656,140000,  8096, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4013,    10,    14,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4013,    10,    14,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4012,  7904, 35000,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4013,    10,    14,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4013,    10,    14,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {5289,  1303,  2625,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {3243,   661,  2625,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_VGA480P60,    SMPPRESET_VGA480P60,      0,  {4398,  1874,  2625,     0, 0, 1,  0, 0, 3} },

    /* DTV 480p 4:3 & 16:9 modes */
    { STDMODE_720p_60,          SMPPRESET_DTV480P,      SMPPRESET_DTV480P,        0,  {3707,    71,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_DTV480P,      SMPPRESET_DTV480P,        0,  {3707,    71,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_DTV480P,      SMPPRESET_DTV480P,        0,  {3706,   206,   525,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_DTV480P,      SMPPRESET_DTV480P_WS,     0,  {3707,    71,    91,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_DTV480P,      SMPPRESET_DTV480P_WS,     0,  {3707,    71,    91,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_DTV480P,      SMPPRESET_DTV480P_WS,     0,  {4897,   101,   315,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_DTV480P,      SMPPRESET_DTV480P,        0,  {2989,  1387,  3465,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_DTV480P,      SMPPRESET_DTV480P_WS,     0,  {4066, 33974, 45045,     0, 0, 1,  0, 0, 3} },

    /* CEA 720p60 */
    { STDMODE_1280x1024_60,     SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {2558,  9278,  9375,   544, 0, 4,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {2560,     0,     1,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {3425,  5543,  5625,     0, 0, 1,  1, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {1722,     2,    11,     0, 0, 1,  1, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {1613,  7637, 12375,     0, 0, 1,  1, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {2037,  1201, 61875,     0, 0, 1,  1, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_DTV720P60,    SMPPRESET_DTV720P60,      0,  {2821, 20609, 61875,     0, 0, 1,  1, 0, 3} },


    /* (S)NES 256x240 & 512x240 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4812,  3344, 16244,  3712, 0, 1,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4805,  1118,  8122,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {6306,  1062,  4061,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {3356, 12572, 44671,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {3168, 13920, 44671,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {3901, 17597, 44671,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_SNES_256x240, SMPPRESET_SNES_512x240,   0,  {5259, 16139, 44671,     0, 0, 1,  0, 0, 3} },

    /* MD 256x224 & 320x224 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3767,    88,  1048,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3730,  1662,  2489,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3730,  1662,  2489,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3729, 10169, 37335,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3730,  1662,  2489,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3730,  1662,  2489,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {4926, 14786, 22401,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {2573,  1435,  2489,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {2423,   755,  1179,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {3008,  8768, 22401,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_MD_256x224,   SMPPRESET_MD_320x224,     0,  {4091, 13189, 22401,     0, 0, 1,  0, 0, 3} },

    /* PSX 256x240, 320x240, 384x240, 512x240 & 640x240 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3759,499651,897619,  3744, 0, 4,  1, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3723,183535,897619,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3721,728469,897619,   544, 0, 4,  1, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3723,183535,897619,   256, 0, 1,  1, 0, 0} },
    { STDMODE_1600x1200_60,     SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {2568,133480,897619,     0, 0, 1,  1, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {2418,427530,897619,     0, 0, 1,  1, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {3002,178354,897619,     0, 0, 1,  1, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_PSX_256x240,  SMPPRESET_PSX_640x240,    0,  {4083,440375,897619,     0, 0, 1,  1, 0, 3} },

    /* Saturn 320x240 & 640x240 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {8033,276890,448678,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {7960,199992,224339,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {7960,199992,224339,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {7958, 23518,224339,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {7960,199992,224339,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {7960,199992,224339,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1600x1200_60,     SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {5650, 23082,224339,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {5350,150382,224339,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {6518,102590,224339,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_SAT_320x240,  SMPPRESET_SAT_640x240,    0,  {8681,151413,224339,     0, 0, 1,  0, 0, 3} },

    /* Saturn 352x240 & 704x240 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7498,   360,  1052,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7430,  4114, 23933,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7430,  4114, 23933,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7424,  5153,  9205,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7430,  4114, 23933,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {7430,  4114, 23933,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1600x1200_60,     SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {5264,  2992, 29333,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {4983,   825,  1841,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {6078,   162,  1841,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_SAT_352x240,  SMPPRESET_SAT_704x240,    0,  {8105, 19323, 23933,     0, 0, 1,  0, 0, 3} },

    /* N64 320x240 & 640x240 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4199,  8638,  8942,  3744, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4159,352269,406861,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4159,352269,406861,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4158, 10306, 31297,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4159,352269,406861,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4159,352269,406861,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_120,        SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {5476, 26524, 31297,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1600x1200_60,     SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {2885,293183,406861,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {2720, 19296, 31297,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {3364, 16348, 31297,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_N64_320x240,  SMPPRESET_N64_640x240,    0,  {4557,  7143, 23933,     0, 0, 1,  0, 0, 3} },

    /* Neo Geo 320x224 modes (NTSC) */
    { STDMODE_480p,             SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {9085,  2688,  4096,  3808, 0, 4,  0, 0, 0} },
    { STDMODE_720p_60,          SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {8863,     0,   128,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_720p_120,         SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {8863,     0,   128,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1280x1024_60,     SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {8859,  1408,  1536,   544, 0, 4,  0, 0, 0} },
    { STDMODE_1080i_60,         SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {8863,     0,   128,  1024, 0, 1,  0, 0, 0} },
    { STDMODE_1080p_60,         SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {8863,     0,   128,   256, 0, 1,  0, 0, 0} },
    { STDMODE_1600x1200_60,     SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {6306,    32,   176,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1200_60,     SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {5974,  1376,  1584,     0, 0, 1,  0, 0, 3} },
    { STDMODE_1920x1440_60,     SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {7266,  1568,  1584,     0, 0, 1,  0, 0, 3} },
    { STDMODE_2560x1440_60,     SMPPRESET_NG_320x224,   SMPPRESET_NG_320x224,     0,  {9660,   832,  1584,     0, 0, 1,  0, 0, 3} },
};

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
                                     STDMODE_1080p_100,
                                     STDMODE_1080p_120,
                                     STDMODE_1600x1200_60,
                                     STDMODE_1920x1200_60,
                                     STDMODE_1920x1440_60,
                                     STDMODE_2560x1440_60};
