// Coefficient data generated using https://github.com/MiSTer-devel/Filters_MiSTer/blob/master/Matlab/Scaler_Filters/polyphase_filter_calculator.m

const pp_coeff pp_coeff_nearest = {.name="nearest",
                                   .bits=9,
                                   .v={
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0, 128,   0,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0},
                                    {0,   0, 128,   0}}};

const pp_coeff pp_coeff_lanczos3 = {.name="lanczos3",
                                    .bits=9,
                                    .v={
                                    {   0, 128,   0,  -0},
                                    {  -2, 128,   2,  -0},
                                    {  -3, 129,   3,  -1},
                                    {  -5, 129,   5,  -1},
                                    {  -6, 129,   7,  -2},
                                    {  -7, 128,   9,  -2},
                                    {  -9, 129,  11,  -3},
                                    { -10, 128,  13,  -3},
                                    { -11, 128,  15,  -4},
                                    { -12, 126,  18,  -4},
                                    { -13, 126,  20,  -5},
                                    { -14, 126,  22,  -6},
                                    { -15, 124,  25,  -6},
                                    { -15, 123,  27,  -7},
                                    { -16, 121,  30,  -7},
                                    { -16, 120,  32,  -8},
                                    { -17, 119,  35,  -9},
                                    { -17, 117,  37,  -9},
                                    { -18, 116,  40, -10},
                                    { -18, 115,  42, -11},
                                    { -18, 112,  45, -11},
                                    { -19, 111,  48, -12},
                                    { -19, 108,  51, -12},
                                    { -19, 107,  53, -13},
                                    { -19, 104,  56, -13},
                                    { -19, 102,  59, -14},
                                    { -19, 100,  62, -15},
                                    { -19,  98,  64, -15},
                                    { -18,  95,  67, -16},
                                    { -18,  92,  70, -16},
                                    { -18,  91,  72, -17},
                                    { -18,  88,  75, -17},
                                    { -17,  84,  78, -17},
                                    { -17,  75,  88, -18},
                                    { -17,  72,  91, -18},
                                    { -16,  70,  92, -18},
                                    { -16,  67,  95, -18},
                                    { -15,  64,  98, -19},
                                    { -15,  62, 100, -19},
                                    { -14,  59, 102, -19},
                                    { -13,  56, 104, -19},
                                    { -13,  53, 107, -19},
                                    { -12,  51, 108, -19},
                                    { -12,  48, 111, -19},
                                    { -11,  45, 112, -18},
                                    { -11,  42, 115, -18},
                                    { -10,  40, 116, -18},
                                    {  -9,  37, 117, -17},
                                    {  -9,  35, 119, -17},
                                    {  -8,  32, 120, -16},
                                    {  -7,  30, 121, -16},
                                    {  -7,  27, 123, -15},
                                    {  -6,  25, 124, -15},
                                    {  -6,  22, 126, -14},
                                    {  -5,  20, 126, -13},
                                    {  -4,  18, 126, -12},
                                    {  -4,  15, 128, -11},
                                    {  -3,  13, 128, -10},
                                    {  -3,  11, 129,  -9},
                                    {  -2,   9, 128,  -7},
                                    {  -2,   7, 129,  -6},
                                    {  -1,   5, 129,  -5},
                                    {  -1,   3, 129,  -3},
                                    {  -0,   2, 128,  -2}}};

const pp_coeff pp_coeff_lanczos3_13 = {.name="lanczos3_13",
                                       .bits=9,
                                       .v={
                                    { -19, 161, -19,   5},
                                    { -20, 161, -18,   5},
                                    { -21, 161, -17,   5},
                                    { -22, 160, -15,   5},
                                    { -22, 159, -14,   5},
                                    { -23, 158, -12,   5},
                                    { -23, 156, -10,   5},
                                    { -23, 154,  -8,   5},
                                    { -23, 152,  -6,   5},
                                    { -23, 150,  -4,   5},
                                    { -23, 148,  -2,   5},
                                    { -23, 146,   1,   4},
                                    { -23, 143,   3,   5},
                                    { -23, 141,   6,   4},
                                    { -22, 138,   9,   3},
                                    { -22, 135,  12,   3},
                                    { -21, 131,  15,   3},
                                    { -20, 128,  18,   2},
                                    { -20, 125,  21,   2},
                                    { -19, 122,  24,   1},
                                    { -18, 118,  27,   1},
                                    { -18, 115,  31,   0},
                                    { -17, 111,  34,  -0},
                                    { -16, 107,  38,  -1},
                                    { -15, 103,  42,  -2},
                                    { -14,  99,  45,  -2},
                                    { -13,  95,  49,  -3},
                                    { -12,  91,  53,  -4},
                                    { -12,  88,  57,  -5},
                                    { -11,  84,  60,  -5},
                                    { -10,  80,  64,  -6},
                                    {  -9,  76,  68,  -7},
                                    {  -8,  72,  72,  -8},
                                    {  -7,  68,  76,  -9},
                                    {  -6,  64,  80, -10},
                                    {  -5,  60,  84, -11},
                                    {  -5,  57,  88, -12},
                                    {  -4,  53,  91, -12},
                                    {  -3,  49,  95, -13},
                                    {  -2,  45,  99, -14},
                                    {  -2,  42, 103, -15},
                                    {  -1,  38, 107, -16},
                                    {  -0,  34, 111, -17},
                                    {   0,  31, 115, -18},
                                    {   1,  27, 118, -18},
                                    {   1,  24, 122, -19},
                                    {   2,  21, 125, -20},
                                    {   2,  18, 128, -20},
                                    {   3,  15, 131, -21},
                                    {   3,  12, 135, -22},
                                    {   3,   9, 138, -22},
                                    {   4,   6, 141, -23},
                                    {   5,   3, 143, -23},
                                    {   4,   1, 146, -23},
                                    {   5,  -2, 148, -23},
                                    {   5,  -4, 150, -23},
                                    {   5,  -6, 152, -23},
                                    {   5,  -8, 154, -23},
                                    {   5, -10, 156, -23},
                                    {   5, -12, 158, -23},
                                    {   5, -14, 159, -22},
                                    {   5, -15, 160, -22},
                                    {   5, -17, 161, -21},
                                    {   5, -18, 161, -20}}};

const pp_coeff pp_coeff_lanczos4 = {.name="lanczos4",
                                    .bits=9,
                                    .v={
                                    {   0, 128,   0,  -0},
                                    {  -2, 129,   2,  -1},
                                    {  -3, 128,   4,  -1},
                                    {  -5, 129,   6,  -2},
                                    {  -7, 130,   8,  -3},
                                    {  -8, 129,  10,  -3},
                                    { -10, 130,  12,  -4},
                                    { -11, 130,  14,  -5},
                                    { -12, 130,  16,  -6},
                                    { -13, 128,  19,  -6},
                                    { -14, 128,  21,  -7},
                                    { -15, 127,  24,  -8},
                                    { -16, 127,  26,  -9},
                                    { -17, 125,  29,  -9},
                                    { -18, 125,  31, -10},
                                    { -19, 124,  34, -11},
                                    { -20, 124,  36, -12},
                                    { -20, 122,  39, -13},
                                    { -21, 120,  42, -13},
                                    { -21, 119,  44, -14},
                                    { -21, 117,  47, -15},
                                    { -22, 115,  50, -15},
                                    { -22, 114,  52, -16},
                                    { -22, 112,  55, -17},
                                    { -22, 109,  58, -17},
                                    { -22, 107,  61, -18},
                                    { -22, 106,  63, -19},
                                    { -22, 103,  66, -19},
                                    { -22, 101,  69, -20},
                                    { -22,  99,  71, -20},
                                    { -22,  97,  74, -21},
                                    { -22,  94,  77, -21},
                                    { -21,  91,  79, -21},
                                    { -21,  77,  94, -22},
                                    { -21,  74,  97, -22},
                                    { -20,  71,  99, -22},
                                    { -20,  69, 101, -22},
                                    { -19,  66, 103, -22},
                                    { -19,  63, 106, -22},
                                    { -18,  61, 107, -22},
                                    { -17,  58, 109, -22},
                                    { -17,  55, 112, -22},
                                    { -16,  52, 114, -22},
                                    { -15,  50, 115, -22},
                                    { -15,  47, 117, -21},
                                    { -14,  44, 119, -21},
                                    { -13,  42, 120, -21},
                                    { -13,  39, 122, -20},
                                    { -12,  36, 124, -20},
                                    { -11,  34, 124, -19},
                                    { -10,  31, 125, -18},
                                    {  -9,  29, 125, -17},
                                    {  -9,  26, 127, -16},
                                    {  -8,  24, 127, -15},
                                    {  -7,  21, 128, -14},
                                    {  -6,  19, 128, -13},
                                    {  -6,  16, 130, -12},
                                    {  -5,  14, 130, -11},
                                    {  -4,  12, 130, -10},
                                    {  -3,  10, 129,  -8},
                                    {  -3,   8, 130,  -7},
                                    {  -2,   6, 129,  -5},
                                    {  -1,   4, 128,  -3},
                                    {  -1,   2, 129,  -2}}};

// GS_Sharpness_085.txt
const pp_coeff pp_coeff_gs_sharp = {.name="gs_sharp",
                                    .bits=9,
                                    .v={
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 128,   0,   0},
                                    {   0, 127,   1,   0},
                                    {   0, 127,   1,   0},
                                    {   0, 127,   1,   0},
                                    {   0, 127,   1,   0},
                                    {   0, 126,   2,   0},
                                    {   0, 125,   3,   0},
                                    {   0, 124,   4,   0},
                                    {   0, 123,   5,   0},
                                    {   0, 121,   7,   0},
                                    {   0, 119,   9,   0},
                                    {   0, 116,  12,   0},
                                    {   0, 112,  16,   0},
                                    {   0, 107,  21,   0},
                                    {   0, 100,  28,   0},
                                    {   0,  93,  35,   0},
                                    {   0,  84,  44,   0},
                                    {   0,  74,  54,   0},
                                    {   0,  64,  64,   0},
                                    {   0,  54,  74,   0},
                                    {   0,  44,  84,   0},
                                    {   0,  35,  93,   0},
                                    {   0,  28, 100,   0},
                                    {   0,  21, 107,   0},
                                    {   0,  16, 112,   0},
                                    {   0,  12, 116,   0},
                                    {   0,   9, 119,   0},
                                    {   0,   7, 121,   0},
                                    {   0,   5, 123,   0},
                                    {   0,   4, 124,   0},
                                    {   0,   3, 125,   0},
                                    {   0,   2, 126,   0},
                                    {   0,   1, 127,   0},
                                    {   0,   1, 127,   0},
                                    {   0,   1, 127,   0},
                                    {   0,   1, 127,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0},
                                    {   0,   0, 128,   0}}};

// GS_Sharpness_070.txt
const pp_coeff pp_coeff_gs_medium = {.name="gs_medium",
                                    .bits=9,
                                    .v={
                                    {   2, 124,   2,   0},
                                    {   1, 125,   2,   0},
                                    {   1, 125,   2,   0},
                                    {   1, 124,   3,   0},
                                    {   1, 124,   3,   0},
                                    {   1, 124,   3,   0},
                                    {   1, 123,   4,   0},
                                    {   1, 123,   4,   0},
                                    {   1, 122,   5,   0},
                                    {   0, 122,   6,   0},
                                    {   0, 121,   7,   0},
                                    {   0, 121,   7,   0},
                                    {   0, 120,   8,   0},
                                    {   0, 119,   9,   0},
                                    {   0, 117,  11,   0},
                                    {   0, 116,  12,   0},
                                    {   0, 115,  13,   0},
                                    {   0, 113,  15,   0},
                                    {   0, 111,  17,   0},
                                    {   0, 109,  19,   0},
                                    {   0, 107,  21,   0},
                                    {   0, 104,  24,   0},
                                    {   0, 102,  26,   0},
                                    {   0,  99,  29,   0},
                                    {   0,  96,  32,   0},
                                    {   0,  92,  36,   0},
                                    {   0,  89,  39,   0},
                                    {   0,  85,  43,   0},
                                    {   0,  81,  47,   0},
                                    {   0,  77,  51,   0},
                                    {   0,  73,  55,   0},
                                    {   0,  68,  60,   0},
                                    {   0,  64,  64,   0},
                                    {   0,  60,  68,   0},
                                    {   0,  55,  73,   0},
                                    {   0,  51,  77,   0},
                                    {   0,  47,  81,   0},
                                    {   0,  43,  85,   0},
                                    {   0,  39,  89,   0},
                                    {   0,  36,  92,   0},
                                    {   0,  32,  96,   0},
                                    {   0,  29,  99,   0},
                                    {   0,  26, 102,   0},
                                    {   0,  24, 104,   0},
                                    {   0,  21, 107,   0},
                                    {   0,  19, 109,   0},
                                    {   0,  17, 111,   0},
                                    {   0,  15, 113,   0},
                                    {   0,  13, 115,   0},
                                    {   0,  12, 116,   0},
                                    {   0,  11, 117,   0},
                                    {   0,   9, 119,   0},
                                    {   0,   8, 120,   0},
                                    {   0,   7, 121,   0},
                                    {   0,   7, 121,   0},
                                    {   0,   6, 122,   0},
                                    {   0,   5, 122,   1},
                                    {   0,   4, 123,   1},
                                    {   0,   4, 123,   1},
                                    {   0,   3, 124,   1},
                                    {   0,   3, 124,   1},
                                    {   0,   3, 124,   1},
                                    {   0,   2, 125,   1},
                                    {   0,   2, 125,   1}}};

// GS_Sharpness_055.txt
const pp_coeff pp_coeff_gs_soft = {.name="gs_soft",
                                    .bits=9,
                                    .v={
                                    {  10, 108,  10,   0},
                                    {   9, 108,  11,   0},
                                    {   9, 107,  12,   0},
                                    {   8, 107,  13,   0},
                                    {   7, 107,  14,   0},
                                    {   7, 106,  15,   0},
                                    {   6, 106,  16,   0},
                                    {   6, 105,  17,   0},
                                    {   5, 105,  18,   0},
                                    {   5, 104,  19,   0},
                                    {   5, 103,  20,   0},
                                    {   4, 102,  22,   0},
                                    {   4, 101,  23,   0},
                                    {   4,  99,  25,   0},
                                    {   3,  99,  26,   0},
                                    {   3,  97,  28,   0},
                                    {   3,  96,  29,   0},
                                    {   3,  94,  31,   0},
                                    {   2,  93,  33,   0},
                                    {   2,  91,  35,   0},
                                    {   2,  89,  37,   0},
                                    {   2,  87,  39,   0},
                                    {   2,  85,  41,   0},
                                    {   1,  83,  44,   0},
                                    {   1,  81,  46,   0},
                                    {   1,  79,  48,   0},
                                    {   1,  77,  50,   0},
                                    {   1,  75,  52,   0},
                                    {   1,  73,  54,   0},
                                    {   1,  70,  57,   0},
                                    {   1,  68,  59,   0},
                                    {   1,  65,  61,   1},
                                    {   1,  63,  63,   1},
                                    {   1,  61,  65,   1},
                                    {   0,  59,  68,   1},
                                    {   0,  57,  70,   1},
                                    {   0,  54,  73,   1},
                                    {   0,  52,  75,   1},
                                    {   0,  50,  77,   1},
                                    {   0,  48,  79,   1},
                                    {   0,  46,  81,   1},
                                    {   0,  44,  83,   1},
                                    {   0,  41,  85,   2},
                                    {   0,  39,  87,   2},
                                    {   0,  37,  89,   2},
                                    {   0,  35,  91,   2},
                                    {   0,  33,  93,   2},
                                    {   0,  31,  94,   3},
                                    {   0,  29,  96,   3},
                                    {   0,  28,  97,   3},
                                    {   0,  26,  99,   3},
                                    {   0,  25,  99,   4},
                                    {   0,  23, 101,   4},
                                    {   0,  22, 102,   4},
                                    {   0,  20, 103,   5},
                                    {   0,  19, 104,   5},
                                    {   0,  18, 105,   5},
                                    {   0,  17, 105,   6},
                                    {   0,  16, 106,   6},
                                    {   0,  15, 106,   7},
                                    {   0,  14, 107,   7},
                                    {   0,  13, 107,   8},
                                    {   0,  12, 107,   9},
                                    {   0,  11, 108,   9}}};
