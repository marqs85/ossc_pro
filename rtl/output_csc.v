//
// Copyright (C) 2024  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

module output_csc (
    input PCLK_i,
    input reset_n,
    input enable,
    input [7:0] R_i,
    input [7:0] G_i,
    input [7:0] B_i,
    input HSYNC_i,
    input VSYNC_i,
    input DE_i,
    output reg [7:0] R_o,
    output reg [7:0] G_o,
    output reg [7:0] B_o,
    output reg HSYNC_o,
    output reg VSYNC_o,
    output reg DE_o
);

localparam PP_PL_START      = 1;
localparam PP_CSC_START     = 0;
localparam PP_CSC_LENGTH    = 5;
localparam PP_CSC_END       = PP_CSC_START + PP_CSC_LENGTH;
localparam PP_BIASMUX_START = PP_CSC_END;
localparam PP_BIASMUX_LENGTH = 1;
localparam PP_BIASMUX_END   = PP_BIASMUX_START + PP_BIASMUX_LENGTH;
localparam PP_PL_END        = PP_BIASMUX_END;

reg HSYNC_pp[PP_PL_START:PP_CSC_END] /* synthesis ramstyle = "logic" */;
reg VSYNC_pp[PP_PL_START:PP_CSC_END] /* synthesis ramstyle = "logic" */;
reg DE_pp[PP_PL_START:PP_CSC_END] /* synthesis ramstyle = "logic" */;

// RGB->YPbPr709 decimal values * 2^15
wire signed [17:0] Y_R_coeff = 6966;
wire signed [17:0] Y_G_coeff = 23435;
wire signed [17:0] Y_B_coeff = 2365;
wire signed [17:0] Pb_R_coeff = -3754;
wire signed [17:0] Pb_G_coeff = -12630;
wire signed [17:0] Pb_B_coeff = 16384;
wire signed [17:0] Pr_R_coeff = 16384;
wire signed [17:0] Pr_G_coeff = -14882;
wire signed [17:0] Pr_B_coeff = -1502;

wire signed [35:0] Y_R_pre, Y_G_pre, Y_B_pre;
wire signed [35:0] Pb_R_pre, Pb_G_pre, Pb_B_pre;
wire signed [35:0] Pr_R_pre, Pr_G_pre, Pr_B_pre;

reg signed [9:0] Y_R, Y_G, Y_B;
reg signed [9:0] Pb_R, Pb_G, Pb_B;
reg signed [9:0] Pr_R, Pr_G, Pr_B;

reg signed [9:0] Y_csc_sum, Pb_csc_sum, Pr_csc_sum;

reg [7:0] Y_csc, Pb_csc, Pr_csc;

always @(posedge PCLK_i) begin
    if (enable) begin
        // Cycle 1
        Y_R <= Y_R_pre[24:15];
        Y_G <= Y_G_pre[24:15];
        Y_B <= Y_B_pre[24:15];
        Pb_R <= Pb_R_pre[24:15];
        Pb_G <= Pb_G_pre[24:15];
        Pb_B <= Pb_B_pre[24:15];
        Pr_R <= Pr_R_pre[24:15];
        Pr_G <= Pr_G_pre[24:15];
        Pr_B <= Pr_B_pre[24:15];

        // Cycle 2
        Y_csc_sum <= Y_R + Y_G + Y_B;
        Pb_csc_sum <= Pb_R + Pb_G + Pb_B + 10'd128;
        Pr_csc_sum <= Pr_R + Pr_G + Pr_B + 10'd128;

        // Cycle 3
        if (Y_csc_sum[9] == 1'b1)
            Y_csc <= 8'h00;
        else if (Y_csc_sum[8] == 1'b1)
            Y_csc <= 8'hFF;
        else
            Y_csc <= Y_csc_sum[7:0];

        if (Pb_csc_sum[9] == 1'b1)
            Pb_csc <= 8'h00;
        else if (Pb_csc_sum[8] == 1'b1)
            Pb_csc <= 8'hFF;
        else
            Pb_csc <= Pb_csc_sum[7:0];

        if (Pr_csc_sum[9] == 1'b1)
            Pr_csc <= 8'h00;
        else if (Pr_csc_sum[8] == 1'b1)
            Pr_csc <= 8'hFF;
        else
            Pr_csc <= Pr_csc_sum[7:0];
    end
end

// Pipeline stages 1-
integer pp_idx;
always @(posedge PCLK_i) begin
    HSYNC_pp[1] <= HSYNC_i;
    VSYNC_pp[1] <= VSYNC_i;
    DE_pp[1] <= DE_i;

    for(pp_idx = PP_PL_START+1; pp_idx <= PP_CSC_END; pp_idx = pp_idx+1) begin
        HSYNC_pp[pp_idx] <= HSYNC_pp[pp_idx-1];
        VSYNC_pp[pp_idx] <= VSYNC_pp[pp_idx-1];
        DE_pp[pp_idx] <= DE_pp[pp_idx-1];
    end

    R_o <= enable ? (DE_pp[PP_CSC_END] ? Pr_csc : 8'h80) : R_i;
    G_o <= enable ? (DE_pp[PP_CSC_END] ? Y_csc  : 8'h00) : G_i;
    B_o <= enable ? (DE_pp[PP_CSC_END] ? Pb_csc : 8'h80) : B_i;
    HSYNC_o <= enable ? HSYNC_pp[PP_CSC_END] : HSYNC_i;
    VSYNC_o <= enable ? VSYNC_pp[PP_CSC_END] : VSYNC_i;
    DE_o <= enable ? DE_pp[PP_CSC_END] : DE_i;
end

lpm_mult csc_mult_component_0 (
    // Inputs
    .dataa  ({10'h0, R_i}),
    .datab  (Y_R_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Y_R_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_0.lpm_widtha             = 18,
    csc_mult_component_0.lpm_widthb             = 18,
    csc_mult_component_0.lpm_widthp             = 36,
    csc_mult_component_0.lpm_widths             = 1,
    csc_mult_component_0.lpm_type               = "LPM_MULT",
    csc_mult_component_0.lpm_representation     = "SIGNED",
    csc_mult_component_0.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_1 (
    // Inputs
    .dataa  ({10'h0, G_i}),
    .datab  (Y_G_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Y_G_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_1.lpm_widtha             = 18,
    csc_mult_component_1.lpm_widthb             = 18,
    csc_mult_component_1.lpm_widthp             = 36,
    csc_mult_component_1.lpm_widths             = 1,
    csc_mult_component_1.lpm_type               = "LPM_MULT",
    csc_mult_component_1.lpm_representation     = "SIGNED",
    csc_mult_component_1.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_2 (
    // Inputs
    .dataa  ({10'h0, B_i}),
    .datab  (Y_B_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Y_B_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_2.lpm_widtha             = 18,
    csc_mult_component_2.lpm_widthb             = 18,
    csc_mult_component_2.lpm_widthp             = 36,
    csc_mult_component_2.lpm_widths             = 1,
    csc_mult_component_2.lpm_type               = "LPM_MULT",
    csc_mult_component_2.lpm_representation     = "SIGNED",
    csc_mult_component_2.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_3 (
    // Inputs
    .dataa  ({10'h0, R_i}),
    .datab  (Pb_R_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pb_R_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_3.lpm_widtha             = 18,
    csc_mult_component_3.lpm_widthb             = 18,
    csc_mult_component_3.lpm_widthp             = 36,
    csc_mult_component_3.lpm_widths             = 1,
    csc_mult_component_3.lpm_type               = "LPM_MULT",
    csc_mult_component_3.lpm_representation     = "SIGNED",
    csc_mult_component_3.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_4 (
    // Inputs
    .dataa  ({10'h0, G_i}),
    .datab  (Pb_G_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pb_G_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_4.lpm_widtha             = 18,
    csc_mult_component_4.lpm_widthb             = 18,
    csc_mult_component_4.lpm_widthp             = 36,
    csc_mult_component_4.lpm_widths             = 1,
    csc_mult_component_4.lpm_type               = "LPM_MULT",
    csc_mult_component_4.lpm_representation     = "SIGNED",
    csc_mult_component_4.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_5 (
    // Inputs
    .dataa  ({10'h0, B_i}),
    .datab  (Pb_B_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pb_B_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_5.lpm_widtha             = 18,
    csc_mult_component_5.lpm_widthb             = 18,
    csc_mult_component_5.lpm_widthp             = 36,
    csc_mult_component_5.lpm_widths             = 1,
    csc_mult_component_5.lpm_type               = "LPM_MULT",
    csc_mult_component_5.lpm_representation     = "SIGNED",
    csc_mult_component_5.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_6 (
    // Inputs
    .dataa  ({10'h0, R_i}),
    .datab  (Pr_R_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pr_R_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_6.lpm_widtha             = 18,
    csc_mult_component_6.lpm_widthb             = 18,
    csc_mult_component_6.lpm_widthp             = 36,
    csc_mult_component_6.lpm_widths             = 1,
    csc_mult_component_6.lpm_type               = "LPM_MULT",
    csc_mult_component_6.lpm_representation     = "SIGNED",
    csc_mult_component_6.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_7 (
    // Inputs
    .dataa  ({10'h0, G_i}),
    .datab  (Pr_G_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pr_G_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_7.lpm_widtha             = 18,
    csc_mult_component_7.lpm_widthb             = 18,
    csc_mult_component_7.lpm_widthp             = 36,
    csc_mult_component_7.lpm_widths             = 1,
    csc_mult_component_7.lpm_type               = "LPM_MULT",
    csc_mult_component_7.lpm_representation     = "SIGNED",
    csc_mult_component_7.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

lpm_mult csc_mult_component_8 (
    // Inputs
    .dataa  ({10'h0, B_i}),
    .datab  (Pr_B_coeff),
    .aclr   (1'b0),
    .clken  (enable),
    .clock  (PCLK_i),

    // Outputs
    .result (Pr_B_pre),
    .sum    (1'b0)
);
defparam
    csc_mult_component_8.lpm_widtha             = 18,
    csc_mult_component_8.lpm_widthb             = 18,
    csc_mult_component_8.lpm_widthp             = 36,
    csc_mult_component_8.lpm_widths             = 1,
    csc_mult_component_8.lpm_type               = "LPM_MULT",
    csc_mult_component_8.lpm_representation     = "SIGNED",
    csc_mult_component_8.lpm_hint               = "LPM_PIPELINE=2,MAXIMIZE_SPEED=5";

endmodule