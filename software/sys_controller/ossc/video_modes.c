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
#define VM_OUT_PCLKMULT     ((vm_conf->x_rpt+1)*(vm_conf->y_rpt+1))


const mode_data_t video_modes_default[] = { \
    /* 240p modes */ \
    { "1600x240",   HDMI_Unknown,     {1600,  240,  2046, 0,  262,  202, 15,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "1280x240",   HDMI_Unknown,     {1280,  240,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },  \
    { "960x240",    HDMI_Unknown,     { 960,  240,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "512x240",    HDMI_Unknown,     { 512,  240,   682, 0,  262,   77, 14,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "384x240",    HDMI_Unknown,     { 384,  240,   512, 0,  262,   59, 14,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "320x240",    HDMI_Unknown,     { 320,  240,   426, 0,  262,   49, 14,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "256x240",    HDMI_Unknown,     { 256,  240,   341, 0,  262,   39, 14,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "240p",       HDMI_240p60_PR2x, { 720,  240,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    /* 288p modes */ \
    { "1600x240L",  HDMI_Unknown,     {1600,  240,  2046, 0,  312,  202, 41,  150, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L5_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "1280x288",   HDMI_Unknown,     {1280,  288,  1560, 0,  312,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                                     TX_1X, TX_1X,  1, {0} },  \
    { "960x288",    HDMI_Unknown,     { 960,  288,  1170, 0,  312,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L3_GEN_4_3),                                                        TX_1X, TX_1X,  1, {0} },  \
    { "512x240LB",  HDMI_Unknown,     { 512,  240,   682, 0,  312,   77, 41,   50, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_512_COL | MODE_L3_512_COL | MODE_L4_512_COL | MODE_L5_512_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "384x240LB",  HDMI_Unknown,     { 384,  240,   512, 0,  312,   59, 41,   37, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_384_COL | MODE_L3_384_COL | MODE_L4_384_COL | MODE_L5_384_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "320x240LB",  HDMI_Unknown,     { 320,  240,   426, 0,  312,   49, 41,   31, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_320_COL | MODE_L3_320_COL | MODE_L4_320_COL | MODE_L5_320_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "256x240LB",  HDMI_Unknown,     { 256,  240,   341, 0,  312,   39, 41,   25, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_L2_256_COL | MODE_L3_256_COL | MODE_L4_256_COL | MODE_L5_256_COL),  TX_1X, TX_1X,  1, {0} },  \
    { "288p",       HDMI_288p50,      { 720,  288,   864, 0,  312,   69, 19,   63, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,   (MODE_PT | MODE_L2),                                                      TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    /* 360p: GBI */ \
    { "480x360",    HDMI_Unknown,     { 480,  360,   600, 0,  375,   63, 10,   38, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV),             GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "240x360",    HDMI_Unknown,     { 256,  360,   300, 0,  375,   24, 10,   18, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV),             GROUP_384P,   (MODE_L2_240x360 | MODE_L3_240x360),                                      TX_1X, TX_1X,  1, {0} },  \
    /* 384p: Sega Model 2 */ \
    { "384p",       HDMI_Unknown,     { 496,  384,   640, 0,  423,   50, 29,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV),             GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 400p line3x */ \
    { "1600x400",   HDMI_Unknown,     {1600,  400,  2000, 0,  449,  120, 34,  240, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_L3_GEN_16_9),                                                       TX_1X, TX_1X,  1, {0} },  \
    /* 720x400@70Hz, VGA Mode 3+/7+ */ \
    { "720x400",    HDMI_Unknown,     { 720,  400,   900, 0,  449,   64, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 640x400@70Hz, VGA Mode 13h */ \
    { "640x400",    HDMI_Unknown,     { 640,  400,   800, 0,  449,   48, 34,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 384p: X68k @ 24kHz */ \
    { "640x384",    HDMI_Unknown,     { 640,  384,   800, 0,  492,   48, 63,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_384P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* ~525-line modes */ \
    { "480i",       HDMI_480i60_PR2x, { 720,  240,   858, 0,  525,   57, 15,   62, 3,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    { "480p",       HDMI_480p60,      { 720,  480,   858, 0,  525,   60, 30,   62, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "640x480",    HDMI_640x480p60,  { 640,  480,   800, 0,  525,   48, 33,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_PC | VIDEO_EDTV),  GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* X68k @ 31kHz */ \
    { "640x512",    HDMI_Unknown,     { 640,  512,   800, 0,  568,   48, 28,   96, 2,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_PC | VIDEO_EDTV),  GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* ~625-line modes */ \
    { "576i",       HDMI_576i50,      { 720,  288,   864, 0,  625,   69, 19,   63, 3,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_480I,   (MODE_PT | MODE_L2 | MODE_L3_GEN_16_9 | MODE_L4_GEN_4_3),                 TX_2X, TX_2X,  1, {0, 0, 0, 0, 0, 0, 0, 1, 0} },  \
    { "576p",       HDMI_576p50,      { 720,  576,   864, 0,  625,   68, 39,   64, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,   (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "800x600",    HDMI_Unknown,     { 800,  600,  1056, 0,  628,   88, 23,  128, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    /* 720p modes */ \
    { "720p",       HDMI_720p60,      {1280,  720,  1650, 0,  750,  220, 20,   40, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 1024, 0, 1, 0, 0, 0} },  \
    /* VESA XGA,1280x960 and SXGA modes */ \
    { "1024x768",   HDMI_Unknown,     {1024,  768,  1344, 0,  806,  160, 29,  136, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  1, {0} },  \
    { "1280x960",   HDMI_Unknown,     {1280,  960,  1800, 0, 1000,  312, 36,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },  \
    { "1280x1024",  HDMI_Unknown,     {1280, 1024,  1688, 0, 1066,  248, 38,  112, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  4, {0} },  \
    /* PS2 GSM 960i mode */ \
    { "640x960i",   HDMI_Unknown,     { 640,  480,   800, 0, 1050,   48, 33,   96, 2,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    /* 1080i/p modes */ \
    { "1080i",      HDMI_1080i60,     {1920,  540,  2200, 0, 1125,  148, 16,   44, 5,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_1080I,  (MODE_PT | MODE_L2),                                                      TX_1X, TX_1X,  1, {0} },  \
    { "1080p",      HDMI_1080p60,     {1920, 1080,  2200, 0, 1125,  148, 36,   44, 5,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_HDTV | VIDEO_PC),  GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {3712, 0, 1, 256, 0, 1, 0, 0, 0} },  \
    /* VESA UXGA mode */ \
    { "1600x1200",  HDMI_Unknown,     {1600, 1200,  2160, 0, 1250,  304, 46,  192, 3,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  6, {0} },  \
    /* CVT 1920x1200 with reduced blanking */ \
    { "1920x1200",  HDMI_Unknown,     {1920, 1200,  2080, 0, 1235,   80, 26,   32, 6,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2408, 8, 27, 0, 0, 1, 0, 0, 3} },  \
    /* CVT 1920x1440 with reduced blanking */ \
    { "1920x1440",  HDMI_Unknown,     {1920, 1440,  2080, 0, 1481,   80, 34,   32, 4,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_1X, TX_1X,  0, {2991, 11, 27, 0, 0, 1, 0, 0, 3} },  \
    /* CVT 2560x1440 with reduced blanking and pixelrep */ \
    { "2560x1440",  HDMI_Unknown,     {1280, 1440,  1360, 0, 1481,   40, 33,   16, 5,  0},  DEFAULT_SAMPLER_PHASE,  VIDEO_PC,                 GROUP_NONE,   MODE_PT,                                                                  TX_2X, TX_1X,  4, {0} },  \
};

const ad_mode_data_t adaptive_modes_default[] = { \
    /* Generic 261-line modes */ \
    { ADMODE_480p,                    { 720,  240,   858, 0,  261,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 1,  0, 0,  {7984,    16,    29,  3712, 0, 1,  0, 0, 0} },  \
    { ADMODE_720p,                    { 960,  240,  1170, 0,  261,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 2,  0, 0,  {5712,   656,  1131,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024,               {1280,  240,  1560, 0,  261,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4154,  2348,  2610,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  240,  1560, 0,  261,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4156,   166,   377,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_CR,                {1600,  240,  1950, 0,  261,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {3222,   282,   377,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200,               {1600,  240,  1950, 0,  261,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2204,    68,   377,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200,               {1600,  240,  1950, 0,  261,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2072,   152,   783,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440,               {1920,  240,  2340, 0,  261,  256, 15,  108, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 5,  0, 0,  {2070,  1058,  2349,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 262-line modes */ \
    { ADMODE_480p,                    { 720,  240,   858, 0,  262,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 1,  0, 0,  {8016,   256,  1048,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_720p,                    { 960,  240,  1170, 0,  262,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 2,  0, 0,  {5688,  1400,  1703,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024,               {1280,  240,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4137,   228,  2620,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  240,  1560, 0,  262,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_CR,                {1600,  240,  1950, 0,  262,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {3208,   840,  1703,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200,               {1600,  240,  1950, 0,  262,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2193,  1385,  1703,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200,               {1600,  240,  1950, 0,  262,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2062,   130,   393,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440,               {1920,  240,  2340, 0,  262,  256, 15,  108, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 5,  0, 0,  {2060,   700,  1179,     0, 0, 1,  0, 0, 3} },  \
    /*{ STDMODE_1440p,                  { 960,  240,  1170, 0,  262,  128, 15,   54, 3},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  5,  0, 0,  {4534, 1234, 5109, 256, 0, 1, 0, 0, 0} },         */ \

    /* Generic 263-line modes */ \
    { ADMODE_480p,                    { 720,  240,   858, 0,  263,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 1,  0, 0,  {7983,   860,  1052,  3744, 0, 4,  0, 0, 0} },  \
    { ADMODE_720p,                    { 960,  240,  1170, 0,  263,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 2,  0, 0,  {5665,   837,  3419,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024,               {1280,  240,  1560, 0,  263,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  240,  1560, 0,  263,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_CR,                {1600,  240,  1950, 0,  263,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {3194,  1186,  3419,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200,               {1600,  240,  1950, 0,  263,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2183,  1795,  3419,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200,               {1600,  240,  1950, 0,  263,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2052,   428,   789,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440,               {1920,  240,  2340, 0,  263,  256, 15,  108, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 5,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 264-line modes */ \
    { ADMODE_480p,                    { 720,  240,   858, 0,  264,   57, 15,   62, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 1,  0, 0,  {8015,    48,   176,  3776, 0, 2,  0, 0, 0} },  \
    { ADMODE_720p,                    { 960,  240,  1170, 0,  264,  128, 15,   54, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 2,  0, 0,  {5641,    11,    13,  1024, 0, 1,  0, 0, 0} },  \
    { ADMODE_1280x1024,               {1280,  240,  1560, 0,  264,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4101,   208,   240,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  240,  1560, 0,  264,  170, 15,   72, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 3,  0, 0,  {4103,    20,    52,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1080p_CR,                {1600,  240,  1950, 0,  264,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {3180,     4,    13,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1600x1200,               {1600,  240,  1950, 0,  264,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2173,    45,   143,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1200,               {1600,  240,  1950, 0,  264,  212, 15,   90, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 4,  0, 0,  {2042,    82,    99,     0, 0, 1,  0, 0, 3} },  \
    { ADMODE_1920x1440,               {1920,  240,  2340, 0,  264,  256, 15,  108, 3,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_240P,  0, 5,  0, 0,  {2041,    31,   297,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 525-line interlace modes */ \
    { ADMODE_1280x1024,               {1280,  240,  1560, 0,  525,  170, 15,   72, 3,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_480I,  0, 3,  0, 0,  {4128,   608,  2625,   544, 0, 4,  0, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  240,  1560, 0,  525,  170, 15,   72, 3,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_480I,  0, 3,  0, 0,  {4129,    69,    91,   256, 0, 1,  0, 0, 0} },  \
    { ADMODE_1920x1440,               {1920,  240,  2340, 0,  525,  256, 15,  108, 3,  1},  DEFAULT_SAMPLER_PHASE,  (VIDEO_SDTV | VIDEO_PC),  GROUP_480I,  0, 5,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  0, 0, 3} },  \

    /* Generic 524-line modes */ \
    { ADMODE_1280x1024,               {1280,  480,  1560, 0,  524,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4137,   228,  2620,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  480,  1560, 0,  524,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4138,  1050,  1703,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440,               {1920,  480,  2340, 0,  524,  256, 30,  108, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 2,  0, 0,  {2060,   700,  1179,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 525-line modes */ \
    { ADMODE_1280x1024,               {1280,  480,  1560, 0,  525,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4128,   608,  2625,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  480,  1560, 0,  525,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4129,    69,    91,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440,               {1920,  480,  2340, 0,  525,  256, 30,  108, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 2,  0, 0,  {2055,  3277,  4725,     0, 0, 1,  1, 0, 3} },  \

    /* Generic 526-line modes */ \
    { ADMODE_1280x1024,               {1280,  480,  1560, 0,  526,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4119,  1078,  2630,   544, 0, 4,  1, 0, 0} },  \
    { ADMODE_1080p_LB,                {1280,  480,  1560, 0,  526,  170, 30,   72, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 1,  0, 0,  {4120,  3192,  3419,   256, 0, 1,  1, 0, 0} },  \
    { ADMODE_1920x1440,               {1920,  480,  2340, 0,  526,  256, 30,  108, 6,  0},  DEFAULT_SAMPLER_PHASE,  (VIDEO_EDTV | VIDEO_PC),  GROUP_480P,  0, 2,  0, 0,  {2050,  1922,  2367,     0, 0, 1,  1, 0, 3} },  \
};

const stdmode_t ad_mode_id_map[] = {STDMODE_240p, STDMODE_480p, STDMODE_720p, STDMODE_1280x1024, STDMODE_1080p, STDMODE_1080p, STDMODE_1600x1200, STDMODE_1920x1200, STDMODE_1920x1440};

const stdmode_t stdmode_idx_arr[] = {STDMODE_240p, STDMODE_288p, STDMODE_480p, STDMODE_576p, STDMODE_720p, STDMODE_1280x1024, STDMODE_1080p, STDMODE_1600x1200, STDMODE_1920x1200, STDMODE_1920x1440};
const unsigned num_stdmodes = sizeof(stdmode_idx_arr)/sizeof(stdmode_t);

mode_data_t video_modes[sizeof(video_modes_default)/sizeof(mode_data_t)];
ad_mode_data_t adaptive_modes[sizeof(adaptive_modes_default)/sizeof(ad_mode_data_t)];


void set_default_vm_table() {
    memcpy(video_modes, video_modes_default, sizeof(video_modes_default));
    memcpy(adaptive_modes, adaptive_modes_default, sizeof(adaptive_modes_default));
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
    if ((vm_in->type & VIDEO_LDTV) ||
        (vm_in->type & VIDEO_SDTV) ||
        (vm_in->type & VIDEO_EDTV))
    {
        return h_hz * 858;
    } else {
        return vm_in->timings.h_total * h_hz;
    }
}

int get_adaptive_mode(uint16_t totlines, uint8_t interlaced, uint16_t hz_x100, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out)
{
    int i;
    ad_mode_id_t target_ad_id;
    int32_t v_linediff;
    uint32_t interlace_mult;
    unsigned num_modes = sizeof(adaptive_modes)/sizeof(ad_mode_data_t);
    avconfig_t* cc = get_current_avconfig();
    memset(vm_in, 0, sizeof(mode_data_t));
    memset(vm_out, 0, sizeof(mode_data_t));

    const ad_mode_id_t pm_ad_240p_map[] = {-1, ADMODE_480p, ADMODE_720p, ADMODE_1280x1024, ADMODE_1080p_LB, ADMODE_1080p_CR, ADMODE_1600x1200, ADMODE_1920x1200, ADMODE_1920x1440};
    const ad_mode_id_t pm_ad_480i_map[] = {-1, ADMODE_1280x1024, ADMODE_1080p_LB, ADMODE_1920x1440};
    const ad_mode_id_t pm_ad_480p_map[] = {-1, ADMODE_1280x1024, ADMODE_1080p_LB, ADMODE_1920x1440};

    if (!cc->adapt_lm)
        return -1;

    for (i=0; i<num_modes; i++) {
        if (adaptive_modes[i].group == GROUP_240P)
            target_ad_id = pm_ad_240p_map[cc->pm_ad_240p];
        else if (adaptive_modes[i].group == GROUP_480I)
            target_ad_id = pm_ad_480i_map[cc->pm_ad_480i];
        else if (adaptive_modes[i].group == GROUP_480P)
            target_ad_id = pm_ad_480p_map[cc->pm_ad_480p];
        else
            target_ad_id = -1;

        if ((totlines == adaptive_modes[i].timings_i.v_total) && (interlaced == adaptive_modes[i].timings_i.interlaced) && (target_ad_id == adaptive_modes[i].id)) {
            vm_conf->x_rpt = 0;
            vm_conf->y_rpt = 0;
            vm_conf->x_skip = 0;

            memcpy(&vm_in->timings, &adaptive_modes[i].timings_i, sizeof(sync_timings_t));
            vm_in->sampler_phase = adaptive_modes[i].sampler_phase;
            vm_in->type = adaptive_modes[i].type;

            interlace_mult = vm_in->timings.interlaced ? 2 : 1;

            memcpy(vm_out, &video_modes_default[ad_mode_id_map[adaptive_modes[i].id]], sizeof(mode_data_t));

            vm_conf->y_rpt = adaptive_modes[i].y_rpt;
            vm_conf->x_offset = ((vm_out->timings.h_active-vm_in->timings.h_active)/2) + adaptive_modes[i].x_offset_i;
            vm_conf->x_size = vm_in->timings.h_active;
            vm_conf->linebuf_startline = ((vm_in->timings.v_active - (vm_out->timings.v_active/(adaptive_modes[i].y_rpt+1)))/2) + adaptive_modes[i].y_offset_i;
            vm_conf->y_offset = -(adaptive_modes[i].y_rpt+1)*vm_conf->linebuf_startline;
            vm_conf->y_size = vm_in->timings.v_active*(adaptive_modes[i].y_rpt+1);

            vm_out->si_pclk_mult = 0;
            memcpy(&vm_out->si_ms_conf, &adaptive_modes[i].si_ms_conf, sizeof(si5351_ms_config_t));

            // calculate the time (in output lines, rounded up) from source frame start to the point where first to-be-visible line has been written into linebuf
            v_linediff = (((vm_in->timings.v_synclen + vm_in->timings.v_backporch + ((vm_conf->linebuf_startline < 0) ? 0 : vm_conf->linebuf_startline) + 1) * vm_out->timings.v_total * interlace_mult) / vm_in->timings.v_total) + 1;

            // subtract the previous value from the total number of output blanking/empty lines. Resulting value indicates how many lines output framestart must be offset
            v_linediff = (vm_out->timings.v_synclen + vm_out->timings.v_backporch + ((vm_conf->y_offset < 0) ? 0 : vm_conf->y_offset)) - v_linediff;

            // if linebuf is read faster than written, output framestart must be delayed accordingly to avoid read pointer catching write pointer
            if (vm_out->timings.v_total * interlace_mult > vm_in->timings.v_total*(adaptive_modes[i].y_rpt+1))
                v_linediff -= (((vm_in->timings.v_active * vm_out->timings.v_total * interlace_mult) / vm_in->timings.v_total) - vm_conf->y_size);

            vm_conf->framesync_line = (v_linediff < 0) ? vm_out->timings.v_total+v_linediff : v_linediff;

            printf("framesync_line = %u (linebuf_startline: %d, y_offset: %d, y_size: %u)\n", vm_conf->framesync_line, vm_conf->linebuf_startline, vm_conf->y_offset, vm_conf->y_size);

            return i;
        }
    }

    return -1;
}

/* TODO: rewrite, check hz etc. */
int get_mode_id(uint16_t totlines, uint8_t interlaced, uint16_t hz_x100, video_type typemask, vm_mult_config_t *vm_conf, mode_data_t *vm_in, mode_data_t *vm_out)
{
    int i;
    unsigned num_modes = sizeof(video_modes)/sizeof(mode_data_t);
    video_type mode_type;
    avconfig_t* cc = get_current_avconfig();
    mode_flags valid_lm[] = { MODE_PT, (MODE_L2 | (MODE_L2<<cc->l2_mode)), (MODE_L3_GEN_16_9<<cc->l3_mode), (MODE_L4_GEN_4_3<<cc->l4_mode), (MODE_L5_GEN_4_3<<cc->l5_mode) };
    mode_flags target_lm;
    uint8_t pt_only = 0;
    uint8_t upsample2x = 1;

    // one for each video_group
    uint8_t* group_ptr[] = { &pt_only, &cc->pm_240p, &cc->pm_384p, &cc->pm_480i, &cc->pm_480p, &cc->pm_1080i };

    for (i=0; i<num_modes; i++) {
        mode_type = video_modes[i].type;

        switch (video_modes[i].group) {
            case GROUP_NONE:
            case GROUP_240P:
                break;
            case GROUP_384P:
                //fixed Line2x/3x mode for 240x360p
                valid_lm[2] = MODE_L2_240x360;
                valid_lm[3] = MODE_L3_240x360;
                valid_lm[4] = MODE_L3_GEN_16_9;
                if (video_modes[i].timings.v_total == 449) {
                    if (!strncmp(video_modes[i].name, "720x400", 7)) {
                        if (cc->s400p_mode == 0)
                            continue;
                    } else if (!strncmp(video_modes[i].name, "640x400", 7)) {
                        if (cc->s400p_mode == 1)
                            continue;
                    }
                }
                break;
            case GROUP_480I:
                //fixed Line3x/4x mode for 480i
                valid_lm[2] = MODE_L3_GEN_16_9;
                valid_lm[3] = MODE_L4_GEN_4_3;
                break;
            case GROUP_480P:
                if (video_modes[i].timings.v_total == 525) {
                    if (video_modes[i-1].group == GROUP_480I) { // hit "480p" on the list
                        if (cc->s480p_mode == 0) // Auto
                            mode_type &= ~VIDEO_PC;
                        else if (cc->s480p_mode == 2) // VESA 640x480@60
                            continue;
                    } else { // "640x480" on the list
                        if (cc->s480p_mode == 0) // Auto
                            mode_type &= ~VIDEO_EDTV;
                        else if (cc->s480p_mode == 1) // DTV 480p
                            continue;
                    }
                } else if (video_modes[i].timings.v_total == 625) { // hit "576p" on the list
                    if ((typemask & VIDEO_PC) && (hz_x100 >= 5500))
                        continue;
                }
                break;
            case GROUP_1080I:
                break;
            default:
                printf("WARNING: Corrupted mode (id %d)\n", i);
                continue;
                break;
        }

        target_lm = valid_lm[*group_ptr[video_modes[i].group]];

        if ((typemask & mode_type) && (target_lm & video_modes[i].flags) && (interlaced == video_modes[i].timings.interlaced) && (totlines <= (video_modes[i].timings.v_total+LINECNT_MAX_TOLERANCE))) {

            // defaults
            memcpy(vm_in, &video_modes[i], sizeof(mode_data_t));
            memcpy(vm_out, &video_modes[i], sizeof(mode_data_t));
            vm_in->timings.v_total = totlines;
            vm_out->timings.v_total = totlines;
            vm_out->vic = HDMI_Unknown;

            vm_conf->x_rpt = 0;
            vm_conf->y_rpt = 0;
            vm_conf->x_skip = 0;
            vm_conf->x_offset = 0;
            vm_conf->y_offset = 0;
            vm_conf->x_size = 0;
            vm_conf->y_size = 0;
            vm_conf->linebuf_startline = 0;

            vm_out->si_pclk_mult = 1;
            vm_out->tx_pixelrep = TX_1X;
            vm_out->hdmitx_pixr_ifr = TX_1X;

            target_lm &= video_modes[i].flags;    //ensure L2 mode uniqueness

            switch (target_lm) {
                case MODE_PT:
                    vm_out->vic = vm_in->vic;
                    // Upsample / pixel-repeat horizontal resolution of 240p/480i modes to fulfill min. 25MHz TMDS clock requirement
                    if ((video_modes[i].group == GROUP_240P) || (video_modes[i].group == GROUP_480I)) {
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
                    if ((video_modes[i].group == GROUP_384P) || (video_modes[i].group == GROUP_480P) || ((video_modes[i].group == GROUP_1080I) && (video_modes[i].timings.h_total < 1200))) {
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
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_256_COL:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 5;
                    vm_conf->x_skip = 5;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_320_COL:
                case MODE_L2_384_COL:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L2_240x360:
                    vm_conf->y_rpt = 1;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_GEN_16_9:
                    vm_conf->y_rpt = 2;

                    // Upsample / pixel-repeat horizontal resolution of 480i mode
                    if (video_modes[i].group == GROUP_480I) {
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
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_384_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_320_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_256_COL:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L3_240x360:
                    vm_conf->y_rpt = 2;
                    vm_conf->x_rpt = 6;
                    vm_conf->x_skip = 6;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 13;
                    break;
                case MODE_L4_GEN_4_3:
                    vm_conf->y_rpt = 3;

                    // Upsample / pixel-repeat horizontal resolution of 480i mode
                    if (video_modes[i].group == GROUP_480I) {
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
                    vm_conf->x_skip = 1;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_384_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_320_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L4_256_COL:
                    vm_conf->y_rpt = 3;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    break;
                case MODE_L5_GEN_4_3:
                    vm_conf->y_rpt = 4;
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    if (1) {
                        vm_out->timings.h_synclen = (vm_out->timings.h_total - 1920)/4;
                        vm_out->timings.h_backporch = (vm_out->timings.h_total - 1920)/2;
                        vm_out->timings.h_active = 1920;
                        vm_conf->x_size = vm_in->timings.h_active;
                        vm_conf->x_offset = (1920-vm_in->timings.h_active)/2;

                        vm_out->timings.v_backporch += (vm_out->timings.v_active-1080)/2;
                        vm_out->timings.v_active = 1080;
                    }
                    break;
                case MODE_L5_512_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 2;
                    vm_conf->x_skip = 2;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 40;
                    break;
                case MODE_L5_384_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 3;
                    vm_conf->x_skip = 3;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 30;
                    break;
                case MODE_L5_320_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 4;
                    vm_conf->x_skip = 4;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 24;
                    break;
                case MODE_L5_256_COL:
                    vm_conf->y_rpt = 4;
                    vm_conf->x_rpt = 5;
                    vm_conf->x_skip = 5;
                    vmode_hv_mult(vm_in, VM_OUT_XMULT, 1);
                    vmode_hv_mult(vm_out, VM_OUT_XMULT, VM_OUT_YMULT);
                    vm_out->si_pclk_mult = VM_OUT_PCLKMULT;
                    //cm.hsync_cut = 20;
                    break;
                default:
                    printf("WARNING: invalid target_lm\n");
                    continue;
                    break;
            }

            vm_conf->framesync_line = vm_out->timings.v_total-(vm_conf->y_rpt+1);

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
