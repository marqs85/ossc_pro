//
// Copyright (C) 2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
    { "1600x240",      HDMI_Unknown,     {1600,  240,    0,  2046, 0,  262,  202, 15,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "1280x240",      HDMI_Unknown,     {1280,  240,    0,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },
    { "960x240",       HDMI_Unknown,     { 960,  240,    0,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "512x240",       HDMI_Unknown,     { 512,  240,    0,   682, 0,  262,   77, 14,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },
    { "384x240",       HDMI_Unknown,     { 384,  240,    0,   512, 0,  262,   59, 14,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },
    { "320x240",       HDMI_Unknown,     { 320,  240,    0,   426, 0,  262,   49, 14,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },
    { "256x240",       HDMI_Unknown,     { 256,  240,    0,   341, 0,  262,   39, 14,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },
    { "240p",          HDMI_240p60_PR2x, { 720,  240,    0,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_240P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    /* 288p modes */
    { "1600x240L",     HDMI_Unknown,     {1600,  240,    0,  2046, 0,  312,  202, 43,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "1280x288",      HDMI_Unknown,     {1280,  288,    0,  1560, 0,  312,  170, 19,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },
    { "960x288",       HDMI_Unknown,     { 960,  288,    0,  1170, 0,  312,  128, 19,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },
    { "512x240LB",     HDMI_Unknown,     { 512,  240,    0,   682, 0,  312,   77, 41,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },
    { "384x240LB",     HDMI_Unknown,     { 384,  240,    0,   512, 0,  312,   59, 41,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },
    { "320x240LB",     HDMI_Unknown,     { 320,  240,    0,   426, 0,  312,   49, 41,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },
    { "256x240LB",     HDMI_Unknown,     { 256,  240,    0,   341, 0,  312,   39, 41,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },
    { "288p",          HDMI_288p50,      { 720,  288,    0,   864, 0,  312,   69, 19,   63, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_288P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    /* 360p: GBI */
    { "480x360",       HDMI_Unknown,     { 480,  360,    0,   600, 0,  375,   63, 10,   38, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "240x360",       HDMI_Unknown,     { 256,  360,    0,   300, 0,  375,   24, 10,   18, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                      TX_1X, TX_1X,  1, {0} },
    /* 384p: Sega Model 2 (real vtotal=423, avoid collision with PC88/98 and VGA400p) */
    { "384p",          HDMI_Unknown,     { 496,  384,    0,   640, 0,  408,   50, 29,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 400p line3x */
    { "1600x400",      HDMI_Unknown,     {1600,  400,    0,  2000, 0,  449,  120, 34,  240, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                       TX_1X, TX_1X,  1, {0} },
    /* 720x400@70Hz, VGA Mode 3+/7+ */
    { "720x400_70",    HDMI_Unknown,     { 720,  400,   75,   900, 0,  449,   64, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 640x400@70Hz, VGA Mode 13h */
    { "640x400_70",    HDMI_Unknown,     { 640,  400,   75,   800, 0,  449,   48, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* 384p: X68k @ 24kHz */
    { "640x384",       HDMI_Unknown,     { 640,  384,    0,   800, 0,  492,   48, 63,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* ~525-line modes */
    { "480i",          HDMI_480i60_PR2x, { 720,  240,    0,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    { "480p",          HDMI_480p60,      { 720,  480,    0,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "640x480_60",    HDMI_640x480p60,  { 640,  480,   65,   800, 0,  525,   48, 33,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* X68k @ 31kHz */
    { "640x512",       HDMI_Unknown,     { 640,  512,    0,   800, 0,  568,   48, 28,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* ~625-line modes */
    { "576i",          HDMI_576i50,      { 720,  288,   55,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,               GROUP_576I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },
    { "576p",          HDMI_576p50,      { 720,  576,   55,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_576P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    { "800x600_60",    HDMI_Unknown,     { 800,  600,   65,  1056, 0,  628,   88, 23,  128, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    /* CEA 720p modes */
    { "720p_50",       HDMI_720p50,      {1280,  720,   55,  1980, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "720p_60",       HDMI_720p60,      {1280,  720,    0,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    /* VESA XGA,1280x960 and SXGA modes */
    { "1024x768_60",   HDMI_Unknown,     {1024,  768,   65,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    { "1280x960_60",   HDMI_Unknown,     {1280,  960,   65,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },
    { "1280x1024_60",  HDMI_Unknown,     {1280, 1024,   65,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },
    /* PS2 GSM 960i mode */
    { "640x960i",      HDMI_Unknown,     { 640,  480,    0,   800, 0, 1050,   48, 33,   96, 2,  1},  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,               GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },
    /* CEA 1080i/p modes */
    { "1080i_50",      HDMI_1080i50,     {1920,  540,   55,  2640, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "1080i_60",      HDMI_1080i60,     {1920,  540,    0,  2200, 0, 1125,  148, 15,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },
    { "1080p_50",      HDMI_1080p50,     {1920, 1080,   55,  2640, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },
    { "1080p_60",      HDMI_1080p60,     {1920, 1080,    0,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },
    /* VESA UXGA mode */
    { "1600x1200_60",  HDMI_Unknown,     {1600, 1200,   65,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  6, {0} },
    /* CVT 1920x1200 modes (60Hz with reduced blanking) */
    { "1920x1200_50",  HDMI_Unknown,     {1920, 1200,   55,  2560, 0, 1238,  320, 29,  200, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    { "1920x1200_60",  HDMI_Unknown,     {1920, 1200,    0,  2080, 0, 1235,   80, 26,   32, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2408, 8, 27, 0, 0, 1, 0, 0, 3} },
    /* CVT 1920x1440 modes (60Hz with reduced blanking) */
    { "1920x1440_50",  HDMI_Unknown,     {1920, 1440,   55,  2592, 0, 1484,  336, 37,  200, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },
    { "1920x1440_60",  HDMI_Unknown,     {1920, 1440,    0,  2080, 0, 1481,   80, 34,   32, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2991, 11, 27, 0, 0, 1, 0, 0, 3} },
    /* CVT 2560x1440 with reduced blanking and pixelrep */
    { "2560x1440_60",  HDMI_Unknown,     {1280, 1440,    0,  1360, 0, 1481,   40, 33,   16, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_2X, TX_1X,  4, {0} },
};

smp_preset_t smp_presets_default[] = {
    /* Generic 240p presets */
    { "720x240",      SM_GEN_4_3,        { 720,  240,   80,   858, 0,  262,   57, 15,   62, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "960x240",      SM_GEN_4_3,        { 960,  240,   80,  1170, 0,  262,  128, 15,   54, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1280x240",     SM_GEN_4_3,        {1280,  240,   80,  1560, 0,  262,  170, 15,   72, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1600x240",     SM_GEN_4_3,        {1600,  240,   80,  1950, 0,  262,  212, 15,   90, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "1920x240",     SM_GEN_4_3,        {1920,  240,   80,  2340, 0,  262,  256, 15,  108, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* Generic 288p presets */
    { "720x240",      SM_GEN_4_3,        { 720,  288,   70,   864, 0,  312,   69, 19,   63, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "1536x240",     SM_GEN_4_3,        {1536,  288,   70,  1872, 0,  312,  150, 19,  136, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    { "1920x240",     SM_GEN_4_3,        {1920,  288,   70,  2340, 0,  312,  187, 19,  171, 3,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_288P },
    /* Generic 480i presets */
    { "720x480i",     SM_GEN_4_3,        { 720,  240,   80,   858, 0,  525,   57, 15,   62, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "1280x480i",    SM_GEN_4_3,        {1280,  240,   80,  1560, 0,  525,  170, 15,   72, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    { "1920x480i",    SM_GEN_4_3,        {1920,  240,   80,  2340, 0,  525,  256, 15,  108, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_480I },
    /* Generic 576i presets */
    { "720x576i",     SM_GEN_4_3,        { 720,  288,   70,   864, 0,  625,   69, 19,   63, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    { "1536x576i",    SM_GEN_4_3,        {1536,  288,   70,  1872, 0,  625,  150, 19,  136, 3,  1},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_576I },
    /* Generic 480p presets */
    { "720x480",      SM_GEN_4_3,        { 720,  480,   80,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "1280x480",     SM_GEN_4_3,        {1280,  480,   80,  1560, 0,  525,  170, 30,   72, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    { "1920x480",     SM_GEN_4_3,        {1920,  480,   80,  2340, 0,  525,  256, 30,  108, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* Generic 576p presets */
    { "720x576",      SM_GEN_4_3,        { 720,  576,   70,   864, 0,  625,   68, 39,   64, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    { "1536x576",     SM_GEN_4_3,        {1536,  576,   70,  1872, 0,  625,  150, 39,  136, 5,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_576P },
    /* VESA 640x480_60 */
    { "640x480_60",   SM_OPT_VGA480P60,  { 640,  480,   80,   800, 0,  525,   48, 33,   96, 2,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* DTV 480p */
    { "480p",         SM_OPT_DTV480P,    { 720,  480,   80,   858, 0,  525,   60, 30,   62, 6,  0},  0,  DEFAULT_SAMPLER_PHASE,  VIDEO_EDTV,  GROUP_480P },
    /* NES/SNES */
    { "SNES 256x240", SM_OPT_SNES_256COL,{ 256,  240,    0,   341, 0,  262,   39, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "SNES 512x240", SM_OPT_SNES_512COL,{ 512,  240,    0,   682, 0,  262,   78, 14,   50, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* MD */
    { "MD 256x224",   SM_OPT_MD_256COL,  { 256,  224,    0,   342, 0,  262,   39, 24,   25, 3,  0},  2,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "MD 320x224",   SM_OPT_MD_320COL,  { 320,  224,    0,   427,10,  262,   52, 24,   31, 3,  0},  1,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* PSX */
    { "PSX 256x240",  SM_OPT_PSX_256COL, { 256,  240,    0,   341, 5,  263,   37, 14,   25, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 320x240",  SM_OPT_PSX_320COL, { 320,  240,    0,   426,12,  263,   47, 14,   31, 3,  0},  7,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 384x240",  SM_OPT_PSX_384COL, { 384,  240,    0,   487,11,  263,   43, 14,   38, 3,  0},  6,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 512x240",  SM_OPT_PSX_512COL, { 512,  240,    0,   682,12,  263,   74, 14,   50, 3,  0},  4,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    { "PSX 640x240",  SM_OPT_PSX_640COL, { 640,  240,    0,   853, 5,  263,   94, 14,   62, 3,  0},  3,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
    /* N64 */
    { "N64 320x240",  SM_OPT_N64_320COL, { 320,  240,    0,   386,12,  262,   18, 14,   11, 3,  0},  7,  DEFAULT_SAMPLER_PHASE,  VIDEO_SDTV,  GROUP_240P },
};

//const ad_mode_data_t adaptive_modes_default[] = {
const ad_mode_data_t adaptive_modes[] = {
    /* Generic 261-line modes */
    { ADMODE_480p,                       SMPPRESET_GEN_720x240,  261,  0, 1,  0, 0,  {7984,    16,    29,  3712, 0, 1,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_GEN_960x240,  261,  0, 2,  0, 0,  {5712,   656,  1131,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x240, 261,  0, 3,  0, 0,  {4154,  2348,  2610,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x240, 261,  0, 1,  0, 0,  {4156,   166,   377,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x240, 261,  0, 3,  0, 0,  {4156,   166,   377,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {3222,   282,   377,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {2204,    68,   377,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_GEN_1600x240, 261,  0, 4,  0, 0,  {2072,   152,   783,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x240, 261,  0, 5,  0, 0,  {2070,  1058,  2349,     0, 0, 1,  0, 0, 3} },

    /* Generic 262-line modes */
    { ADMODE_480p,                       SMPPRESET_GEN_720x240,  262,  0, 1,  0, 0,  {8016,   256,  1048,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_GEN_960x240,  262,  0, 2,  0, 0,  {5688,  1400,  1703,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x240, 262,  0, 3,  0, 0,  {4137,   228,  2620,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x240, 262,  0, 1,  0, 0,  {4138,  1050,  1703,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x240, 262,  0, 3,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {3208,   840,  1703,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {2193,  1385,  1703,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_GEN_1600x240, 262,  0, 4,  0, 0,  {2062,   130,   393,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x240, 262,  0, 5,  0, 0,  {2060,   700,  1179,     0, 0, 1,  0, 0, 3} },

    /* Generic 263-line modes */
    { ADMODE_480p,                       SMPPRESET_GEN_720x240,  263,  0, 1,  0, 0,  {7983,   860,  1052,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_GEN_960x240,  263,  0, 2,  0, 0,  {5665,   837,  3419,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x240, 263,  0, 3,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x240, 263,  0, 1,  0, 0,  {4120,  3192,  3419,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x240, 263,  0, 3,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {3194,  1186,  3419,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {2183,  1795,  3419,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_GEN_1600x240, 263,  0, 4,  0, 0,  {2052,   428,   789,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x240, 263,  0, 5,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  0, 0, 3} },

    /* Generic 264-line modes */
    { ADMODE_480p,                       SMPPRESET_GEN_720x240,  264,  0, 1,  0, 0,  {8015,    48,   176,  3776, 0, 2,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_GEN_960x240,  264,  0, 2,  0, 0,  {5641,    11,    13,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x240, 264,  0, 3,  0, 0,  {4101,   208,   240,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x240, 264,  0, 1,  0, 0,  {4103,    20,    52,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x240, 264,  0, 3,  0, 0,  {4103,    20,    52,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {3180,     4,    13,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {2173,    45,   143,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_GEN_1600x240, 264,  0, 4,  0, 0,  {2042,    82,    99,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x240, 264,  0, 5,  0, 0,  {2041,    31,   297,     0, 0, 1,  0, 0, 3} },

    /* Generic 311-line modes */
    { ADMODE_576p,                       SMPPRESET_GEN_720x288,  311,  0, 1,  0, 0,  {7976,   232,   311,  3712, 0, 1,  0, 0, 0} },
    { ADMODE_1080i_50_CR,                SMPPRESET_GEN_1536x288, 311,  0, 1,  0, 0,  {3405,  3569,  4043,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_50_CR,                SMPPRESET_GEN_1536x288, 311,  0, 3,  0, 0,  {3405,  3569,  4043,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x288, 311,  0, 3,  0, 0,  {2275,  6391, 36387,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_50,               SMPPRESET_GEN_1920x288, 311,  0, 4,  0, 0,  {2194,  4386, 20215,     0, 0, 1,  0, 0, 3} },

    /* Generic 312-line modes */
    { ADMODE_576p,                       SMPPRESET_GEN_720x288,  312,  0, 1,  0, 0,  {8013,   800,  1248,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_50_CR,                SMPPRESET_GEN_1536x288, 312,  0, 1,  0, 0,  {3393,   220,   676,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_50_CR,                SMPPRESET_GEN_1536x288, 312,  0, 3,  0, 0,  {3393,   220,   676,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x288, 312,  0, 3,  0, 0,  {2266,  1106,  4563,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_50,               SMPPRESET_GEN_1920x288, 312,  0, 4,  0, 0,  {2185,   459,   845,     0, 0, 1,  0, 0, 3} },

    /* Generic 313-line modes */
    { ADMODE_576p,                       SMPPRESET_GEN_720x288,  313,  0, 1,  0, 0,  {7986,   504,  1252,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_50_CR,                SMPPRESET_GEN_1536x288, 313,  0, 1,  0, 0,  {3380,  3452,  4069,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_50_CR,                SMPPRESET_GEN_1536x288, 313,  0, 3,  0, 0,  {3380,  3452,  4069,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x288, 313,  0, 3,  0, 0,  {2257, 13411, 36621,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_50,               SMPPRESET_GEN_1920x288, 313,  0, 4,  0, 0,  {2176, 18816, 20345,     0, 0, 1,  0, 0, 3} },

    /* Generic 314-line modes */
    { ADMODE_576p,                       SMPPRESET_GEN_720x288,  314,  0, 1,  0, 0,  {7959,   424,  1256,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_50_CR,                SMPPRESET_GEN_1536x288, 314,  0, 1,  0, 0,  {3368,   920,  2041,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_50_CR,                SMPPRESET_GEN_1536x288, 314,  0, 3,  0, 0,  {3368,   920,  2041,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x288, 314,  0, 3,  0, 0,  {2248, 10040, 18369,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_50,               SMPPRESET_GEN_1920x288, 314,  0, 4,  0, 0,  {2168,  3688, 10205,     0, 0, 1,  0, 0, 3} },

    /* Generic 525-line interlace modes */
    { ADMODE_240p,                       SMPPRESET_GEN_720x480i, 525,  0, 0,  0, 0,  {8015,   127,   175,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x480i,525,  0, 3,  0, 0,  {4128,   608,  2625,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x480i,525,  0, 1,  0, 0,  {4129,    69,    91,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x480i,525,  0, 3,  0, 0,  {4129,    69,    91,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x480i,525,  0, 5,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  0, 0, 3} },

    /* Generic 625-line interlace modes */
    { ADMODE_288p,                       SMPPRESET_GEN_720x576i, 625,  0, 0,  0, 0,  {8018,   206,   625,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_50_CR,                SMPPRESET_GEN_1536x576i,625,  0, 1,  0, 0,  {3387,     1,    13,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_50_CR,                SMPPRESET_GEN_1536x576i,625,  0, 3,  0, 0,  {3387,     1,    13,   256, 0, 1,  0, 0, 0} },

    /* Generic 524-line modes */
    { ADMODE_240p,                       SMPPRESET_GEN_720x480,  524,  0,-1,  0, 0,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x480, 524,  0, 1,  0, 0,  {4137,   228,  2620,   544, 0, 4,  1, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x480, 524,  0, 0,  0, 0,  {4138,  1050,  1703,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x480, 524,  0, 1,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  1, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x480, 524,  0, 2,  0, 0,  {2060,   700,  1179,     0, 0, 1,  1, 0, 3} },

    /* Generic 525-line modes */
    { ADMODE_240p,                       SMPPRESET_GEN_720x480,  525,  0,-1,  0, 0,  {3751,   302,   350,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x480, 525,  0, 1,  0, 0,  {4128,   608,  2625,   544, 0, 4,  1, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x480, 525,  0, 0,  0, 0,  {4129,    69,    91,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x480, 525,  0, 1,  0, 0,  {4129,    69,    91,   256, 0, 1,  1, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x480, 525,  0, 2,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  1, 0, 3} },

    /* Generic 526-line modes */
    { ADMODE_240p,                       SMPPRESET_GEN_720x480,  526,  0,-1,  0, 0,  {3743,   796,  1052,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_GEN_1280x480, 526,  0, 1,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  1, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_GEN_1280x480, 526,  0, 0,  0, 0,  {4120,  3192,  3419,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_GEN_1280x480, 526,  0, 1,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  1, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_GEN_1920x480, 526,  0, 2,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  1, 0, 3} },

    /* Generic 624-line modes */
    { ADMODE_288p,                       SMPPRESET_GEN_720x576,  624,  0,-1,  0, 0,  {3744,     0,     4,  8000, 0, 2,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x576, 624,  0, 1,  0, 0,  {2266,  1106,  4563,     0, 0, 1,  1, 0, 3} },

    /* Generic 625-line modes */
    { ADMODE_288p,                       SMPPRESET_GEN_720x576,  625,  0,-1,  0, 0,  {3753,   103,   625,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x576, 625,  0, 1,  0, 0,  {2261, 11659, 14625,     0, 0, 1,  1, 0, 3} },

    /* Generic 626-line modes */
    { ADMODE_288p,                       SMPPRESET_GEN_720x576,  626,  0,-1,  0, 0,  {3746,   110,   313,  8032, 0, 4,  0, 0, 0} },
    { ADMODE_1920x1200_50,               SMPPRESET_GEN_1536x576, 626,  0, 1,  0, 0,  {2257, 13411, 36621,     0, 0, 1,  1, 0, 3} },


    /* VESA 640x480_60 modes */
    { ADMODE_240p,                       SMPPRESET_VGA480P60,      0,  0,-1,  0, 0,  {4095, 34656,140000,  8096, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_VGA480P60,      0,  1, 1,  0, 0,  {4012,  7904, 35000,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_VGA480P60,      0,  1, 0,  0, 0,  {4013,    10,    14,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_VGA480P60,      0,  1, 1,  0, 0,  {4013,    10,    14,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_VGA480P60,      0,  2, 2,  0, 0,  {3243,   661,  2625,     0, 0, 1,  0, 0, 3} },

    /* DTV 480p modes */
    { ADMODE_1280x1024_60,               SMPPRESET_DTV480P,        0,  1, 1,  0, 0,  {3706,   206,   525,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_DTV480P,        0,  1, 0,  0, 0,  {3707,    71,    91,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_DTV480P,        0,  1, 1,  0, 0,  {3707,    71,    91,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1920x1440_60,               SMPPRESET_DTV480P,        0,  2, 2,  0, 0,  {2989,  1387,  3465,     0, 0, 1,  0, 0, 3} },

    /* SNES modes */
    { ADMODE_480p,                       SMPPRESET_SNES_256x240,   0,  1, 1,  0, 0,  {4812,  3344, 16244,  3712, 0, 1,  0, 0, 0} },
    { ADMODE_480p,                       SMPPRESET_SNES_512x240,   0,  0, 1,  0, 0,  {4812,  3344, 16244,  3712, 0, 1,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_SNES_256x240,   0,  3, 2,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_SNES_512x240,   0,  1, 2,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_SNES_256x240,   0,  4, 3,  0, 0,  {4805,  1118,  8122,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_SNES_512x240,   0,  1, 3,  0, 0,  {4805,  1118,  8122,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_SNES_256x240,   0,  4, 1,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_SNES_512x240,   0,  1, 1,  0, 0,  {4806,  3602,  4061,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_SNES_256x240,   0,  4, 3,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_SNES_512x240,   0,  1, 3,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_SNES_256x240,   0,  5, 4,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_SNES_512x240,   0,  2, 4,  0, 0,  {4806,  3602,  4061,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_SNES_256x240,   0,  5, 4,  0, 0,  {3356, 12572, 44671,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1600x1200_60,               SMPPRESET_SNES_512x240,   0,  2, 4,  0, 0,  {3356, 12572, 44671,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_SNES_256x240,   0,  5, 4,  0, 0,  {3168, 13920, 44671,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_SNES_256x240,   0,  2, 4,  0, 0,  {3168, 13920, 44671,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_SNES_256x240,   0,  6, 5,  0, 0,  {3901, 17597, 44671,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_SNES_512x240,   0,  3, 5,  0, 0,  {3901, 17597, 44671,     0, 0, 1,  0, 0, 3} },

    /* MD modes */
    { ADMODE_480p,                       SMPPRESET_MD_256x224,     0,  1, 1,  0, 0,  {6619,  2536,  3144,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_480p,                       SMPPRESET_MD_320x224,     0,  1, 1,  0, 0,  {8046,    88,   524,  3744, 0, 4,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_MD_256x224,     0,  3, 2,  0, 0,  {6559,   281,  2489,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_MD_320x224,     0,  2, 2,  0, 0,  {7973,   835,  2489,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_MD_256x224,     0,  4, 3,  0, 0,  {6556, 17636, 22401,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_MD_320x224,     0,  3, 3,  0, 0,  {7970, 20338, 37335,   544, 0, 4,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_MD_256x224,     0,  4, 1,  0, 0,  {6559,   281,  2489,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_MD_320x224,     0,  3, 1,  0, 0,  {7973,   835,  2489,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_MD_256x224,     0,  4, 3,  0, 0,  {6559,   281,  2489,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_MD_320x224,     0,  3, 3,  0, 0,  {7973,   835,  2489,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_MD_256x224,     0,  5, 4,  0, 0,  {6559,   281,  2489,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_MD_320x224,     0,  4, 4,  0, 0,  {7973,   835,  2489,   256, 0, 1,  0, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_MD_256x224,     0,  5, 4,  0, 0,  {4630,  1562,  2489,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1600x1200_60,               SMPPRESET_MD_320x224,     0,  4, 4,  0, 0,  {5659,   381,  2489,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_MD_256x224,     0,  5, 4,  0, 0,  {4380,  2596,  3537,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_MD_320x224,     0,  4, 4,  0, 0,  {5359,   331,  1179,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_MD_256x224,     0,  6, 5,  0, 0,  {5355, 21439, 67203,     0, 0, 1,  0, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_MD_320x224,     0,  5, 5,  0, 0,  {6528, 17536, 22401,     0, 0, 1,  0, 0, 3} },

    /* PSX modes */
    { ADMODE_720p_60,                    SMPPRESET_PSX_256x240,    0,  3, 2,  0, 0,  {4782, 18698, 23933,  1024, 0, 1,  0, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_PSX_320x240,    0,  2, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_PSX_384x240,    0,  2, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_PSX_512x240,    0,  1, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_PSX_640x240,    0,  1, 2,  0, 0,  {3723,183535,897619,  1024, 0, 1,  1, 0, 0} },

    /* N64 modes */
    { ADMODE_480p,                       SMPPRESET_N64_320x240,    0,  1, 1,  0, 0,  {4219,129618,270122,  3744, 0, 4,  1, 0, 0} },
    { ADMODE_720p_60,                    SMPPRESET_N64_320x240,    0,  2, 2,  0, 0,  {4179, 28849,135061,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_1280x1024_60,               SMPPRESET_N64_320x240,    0,  3, 3,  0, 0,  {4177, 90587,135061,   544, 0, 4,  1, 0, 0} },
    { ADMODE_1080i_60_LB,                SMPPRESET_N64_320x240,    0,  3, 1,  0, 0,  {4179, 28849,135061,  1024, 0, 1,  1, 0, 0} },
    { ADMODE_1080p_60_LB,                SMPPRESET_N64_320x240,    0,  3, 3,  0, 0,  {4179, 28849,135061,   256, 0, 1,  1, 0, 0} },
    { ADMODE_1080p_60_CR,                SMPPRESET_N64_320x240,    0,  4, 4,  0, 0,  {4179, 28849,135061,   256, 0, 1,  1, 0, 0} },
    { ADMODE_1600x1200_60,               SMPPRESET_N64_320x240,    0,  4, 4,  0, 0,  {2899,106929,135061,     0, 0, 1,  1, 0, 3} },
    { ADMODE_1920x1200_60,               SMPPRESET_N64_320x240,    0,  4, 4,  0, 0,  {2734,  1582,405183,     0, 0, 1,  1, 0, 3} },
    { ADMODE_1920x1440_60,               SMPPRESET_N64_320x240,    0,  5, 5,  0, 0,  {3380,233524,405183,     0, 0, 1,  1, 0, 3} },
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
