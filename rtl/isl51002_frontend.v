//
// Copyright (C) 2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

module isl51002_frontend (
    input PCLK_i,
    input CLK_MEAS_i,
    input reset_n,
    input [7:0] R_i,
    input [7:0] G_i,
    input [7:0] B_i,
    input HS_i,
    input HSYNC_i,
    input VSYNC_i,
    input DE_i,
    input FID_i,
    input vs_type,
    input vs_polarity,
    input [31:0] h_in_config,
    input [31:0] h_in_config2,
    input [31:0] v_in_config,
    output reg [7:0] R_o,
    output reg [7:0] G_o,
    output reg [7:0] B_o,
    output reg HSYNC_o,
    output reg VSYNC_o,
    output reg DE_o,
    output reg FID_o,
    output reg interlace_flag,
    output reg [10:0] xpos,
    output reg [10:0] ypos,
    output reg [10:0] vtotal,
    output reg frame_change,
    output reg [19:0] pcnt_frame
);

localparam FID_EVEN = 1'b0;
localparam FID_ODD = 1'b1;

localparam VSYNC_SEPARATED = 1'b0;
localparam VSYNC_RAW = 1'b1;

reg [11:0] h_ctr;
reg [10:0] v_ctr;
reg [10:0] vmax_ctr;
reg HS_i_prev;
reg VSYNC_i_np_prev;
reg [1:0] fid_next_ctr;
reg fid_next;

reg [7:0] R, G, B;
reg HSYNC, VSYNC, FID;

// Measurement registers
reg [19:0] pcnt_ctr;
reg frame_change_meas_sync1_reg, frame_change_meas_sync2_reg, frame_change_meas_prev;
wire frame_change_meas = frame_change_meas_sync2_reg;

wire [7:0] H_SYNCLEN = h_in_config[27:20];
wire [8:0] H_BACKPORCH = h_in_config[19:11];
wire [10:0] H_ACTIVE = h_in_config[10:0];
wire [11:0] H_TOTAL = h_in_config2[11:0];

wire [2:0] V_SYNCLEN = v_in_config[19:17];
wire [5:0] V_BACKPORCH = v_in_config[16:11];
wire [10:0] V_ACTIVE = v_in_config[10:0];

wire [11:0] even_min_thold_hv = (H_TOTAL / 12'd4);
wire [11:0] even_max_thold_hv = (H_TOTAL / 12'd2) + (H_TOTAL / 12'd4);
wire [11:0] even_min_thold_ss = (H_TOTAL / 12'd2);
wire [11:0] even_max_thold_ss = H_TOTAL;
wire [11:0] even_min_thold = (vs_type == VSYNC_SEPARATED) ? even_min_thold_ss : even_min_thold_hv;
wire [11:0] even_max_thold = (vs_type == VSYNC_SEPARATED) ? even_max_thold_ss : even_max_thold_hv;

// TODO: calculate V polarity independently
wire VSYNC_i_np = (VSYNC_i ^ ~vs_polarity);

always @(posedge PCLK_i) begin
    R <= R_i;
    G <= G_i;
    B <= B_i;

    HS_i_prev <= HS_i;
    VSYNC_i_np_prev <= VSYNC_i_np;

    if (HS_i_prev & ~HS_i) begin
        h_ctr <= 0;
        HSYNC <= 1'b0;

        if (fid_next_ctr > 0)
            fid_next_ctr <= fid_next_ctr - 1'b1;

        if (fid_next_ctr == 2'h1) begin
            v_ctr <= 0;
            if (~(interlace_flag & (fid_next == FID_EVEN))) begin
                vmax_ctr <= 0;
                vtotal <= vmax_ctr + 1'b1;
                frame_change <= 1'b1;
            end else begin
                vmax_ctr <= vmax_ctr + 1'b1;
            end
        end else begin
            v_ctr <= v_ctr + 1'b1;
            vmax_ctr <= vmax_ctr + 1'b1;
            frame_change <= 1'b0;
        end
    end else begin
        h_ctr <= h_ctr + 1'b1;
        if (h_ctr == H_SYNCLEN-1)
            HSYNC <= 1'b1;
    end

    // vsync leading edge processing per quadrant
    if (VSYNC_i_np_prev & ~VSYNC_i_np) begin
        if (h_ctr < even_min_thold) begin
            fid_next <= FID_ODD;
            fid_next_ctr <= 2'h1;
        end else if (h_ctr > even_max_thold) begin
            fid_next <= FID_ODD;
            fid_next_ctr <= 2'h2;
        end else begin
            fid_next <= FID_EVEN;
            fid_next_ctr <= 2'h2;
        end
    end

    if (((fid_next == FID_ODD) & (HS_i_prev & ~HS_i)) | ((fid_next == FID_EVEN) & (h_ctr == (H_TOTAL/2)-1'b1))) begin
        if (fid_next_ctr == 2'h1) begin
            VSYNC <= 1'b0;
            FID <= fid_next;
            interlace_flag <= FID ^ fid_next;
        end else begin
            if (v_ctr == V_SYNCLEN-1)
                VSYNC <= 1'b1;
        end
    end
end

always @(posedge PCLK_i) begin
    R_o <= R;
    G_o <= G;
    B_o <= B;
    HSYNC_o <= HSYNC;
    VSYNC_o <= VSYNC;
    FID_o <= FID;

    DE_o <= (h_ctr >= H_SYNCLEN+H_BACKPORCH) & (h_ctr < H_SYNCLEN+H_BACKPORCH+H_ACTIVE) & (v_ctr >= V_SYNCLEN+V_BACKPORCH) & (v_ctr < V_SYNCLEN+V_BACKPORCH+V_ACTIVE);
    xpos <= (h_ctr-H_SYNCLEN-H_BACKPORCH);
    ypos <= (v_ctr-V_SYNCLEN-V_BACKPORCH);
end

//Calculate exact vertical frequency
always @(posedge CLK_MEAS_i) begin
    if (~frame_change_meas_prev & frame_change_meas) begin
        pcnt_ctr <= 1;
        pcnt_frame <= pcnt_ctr;
    end else if (pcnt_ctr < 20'hfffff) begin
        pcnt_ctr <= pcnt_ctr + 1'b1;
    end

    frame_change_meas_sync1_reg <= frame_change;
    frame_change_meas_sync2_reg <= frame_change_meas_sync1_reg;
    frame_change_meas_prev <= frame_change_meas;
end

// TODO: CSC

endmodule
