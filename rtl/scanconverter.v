//
// Copyright (C) 2019-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

module scanconverter (
    input PCLK_CAP_i,
    input PCLK_OUT_i,
    input reset_n,
    input [7:0] R_i,
    input [7:0] G_i,
    input [7:0] B_i,
    input HSYNC_i,
    input VSYNC_i,
    input DE_i,
    input FID_i,
    input interlaced_in_i,
    input frame_change_i,
    input [10:0] xpos_i,
    input [10:0] ypos_i,
    input [31:0] hv_out_config,
    input [31:0] hv_out_config2,
    input [31:0] hv_out_config3,
    input [31:0] xy_out_config,
    input [31:0] xy_out_config2,
    input [31:0] misc_config,
    input [31:0] sl_config,
    input [31:0] sl_config2,
    input testpattern_enable,
    output PCLK_o,
    output [7:0] R_o,
    output [7:0] G_o,
    output [7:0] B_o,
    output HSYNC_o,
    output VSYNC_o,
    output DE_o,
    output [10:0] xpos_o,
    output [10:0] ypos_o,
    output reg resync_strobe
);

localparam NUM_LINE_BUFFERS = 40;

localparam FID_EVEN = 1'b0;
localparam FID_ODD = 1'b1;

localparam PP_PL_START          = 1;
localparam PP_LINEBUF_START     = PP_PL_START + 1;
localparam PP_LINEBUF_LENGTH    = 1;
localparam PP_LINEBUF_END       = PP_LINEBUF_START + PP_LINEBUF_LENGTH;
localparam PP_SLGEN_START       = PP_LINEBUF_END;
localparam PP_SLGEN_LENGTH      = 1;
localparam PP_SLGEN_END         = PP_SLGEN_START + PP_SLGEN_LENGTH;
localparam PP_PL_END            = PP_SLGEN_END;

wire [11:0] H_TOTAL = hv_out_config[11:0];
wire [10:0] H_ACTIVE = hv_out_config[22:12];
wire [8:0] H_BACKPORCH = hv_out_config[31:23];
wire [8:0] H_SYNCLEN = hv_out_config2[8:0];

wire V_INTERLACED = hv_out_config2[31];
wire [10:0] V_TOTAL = hv_out_config2[19:9] >> V_INTERLACED;
wire [10:0] V_ACTIVE = hv_out_config2[30:20];
wire [8:0] V_BACKPORCH = hv_out_config3[8:0];
wire [4:0] V_SYNCLEN = hv_out_config3[13:9];

wire [10:0] V_STARTLINE = hv_out_config3[24:14];

wire [10:0] V_STARTLINE_PREV = (V_STARTLINE == 0) ? (V_TOTAL-1) : (V_STARTLINE-1);

wire [10:0] X_SIZE = xy_out_config[10:0];
wire [10:0] Y_SIZE = xy_out_config[21:11];
wire signed [9:0] X_OFFSET = xy_out_config[31:22];
wire signed [8:0] Y_OFFSET = xy_out_config2[8:0];

wire [7:0] X_START_LB = xy_out_config2[16:9];
wire signed [6:0] Y_START_LB = {xy_out_config2[22], xy_out_config2[22:17]};

wire [2:0] X_RPT = xy_out_config2[25:23];
wire [2:0] Y_RPT = xy_out_config2[28:26];

wire Y_SKIP = (Y_RPT == 3'(-1));
wire [1:0] Y_STEP = Y_SKIP+1'b1;

wire [3:0] MISC_MASK_BR = misc_config[3:0];
wire [2:0] MISC_MASK_COLOR = misc_config[6:4];
wire [5:0] MISC_REV_LPF_STR = (misc_config[11:7] + 6'd16);
wire MISC_REV_LPF_ENABLE = (misc_config[11:7] != 5'h0);
wire MISC_LM_DEINT_MODE = misc_config[12];


reg frame_change_sync1_reg, frame_change_sync2_reg, frame_change_prev;
wire frame_change = frame_change_sync2_reg;

reg [11:0] h_cnt;
reg [10:0] v_cnt;
reg src_fid, dst_fid;

reg [10:0] xpos_lb;
reg [6:0] ypos_lb;
reg [2:0] x_ctr;
reg [2:0] y_ctr;

reg [5:0] ypos_i_wraddr;
reg [10:0] ypos_i_prev;
reg [10:0] xpos_i_wraddr;
reg [23:0] DATA_i_wrdata;
reg DE_i_wren;

// Pipeline registers
reg [7:0] R_pp[PP_LINEBUF_END:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg [7:0] G_pp[PP_LINEBUF_END:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg [7:0] B_pp[PP_LINEBUF_END:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg HSYNC_pp[PP_PL_START:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg VSYNC_pp[PP_PL_START:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg DE_pp[PP_PL_START:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg [10:0] xpos_pp[PP_PL_START:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg [10:0] ypos_pp[PP_PL_START:PP_PL_END] /* synthesis ramstyle = "logic" */;
reg mask_enable_pp[PP_LINEBUF_START:PP_SLGEN_START] /* synthesis ramstyle = "logic" */;

reg [10:0] xpos_lb_start;

assign PCLK_o = PCLK_OUT_i;

wire [16:0] linebuf_wraddr = {ypos_i_wraddr, xpos_i_wraddr};
wire [16:0] linebuf_rdaddr = {ypos_lb[5:0], xpos_lb};

wire [7:0] R_linebuf, G_linebuf, B_linebuf;

linebuf linebuf_rgb (
    .data(DATA_i_wrdata),
    .rdaddress(linebuf_rdaddr),
    .rdclock(PCLK_OUT_i),
    .wraddress(linebuf_wraddr),
    .wrclock(PCLK_CAP_i),
    .wren(DE_i_wren),
    .q({R_linebuf, G_linebuf, B_linebuf})
);

// Linebuffer write address calculation
always @(posedge PCLK_CAP_i) begin
    if (ypos_i == 0) begin
        ypos_i_wraddr <= 0;
    end else if (ypos_i != ypos_i_prev) begin
        if (ypos_i_wraddr == NUM_LINE_BUFFERS-1)
            ypos_i_wraddr <= 0;
        else
            ypos_i_wraddr <= ypos_i_wraddr + 1'b1;
    end

    xpos_i_wraddr <= xpos_i;
    ypos_i_prev <= ypos_i;
    DATA_i_wrdata <= {R_i, G_i, B_i};
    DE_i_wren <= DE_i;
end


// Frame change strobe synchronization
always @(posedge PCLK_OUT_i) begin
    frame_change_sync1_reg <= frame_change_i;
    frame_change_sync2_reg <= frame_change_sync1_reg;
    frame_change_prev <= frame_change_sync2_reg;
end

// H/V counters
always @(posedge PCLK_OUT_i) begin
    if (~frame_change_prev & frame_change & ((v_cnt != V_STARTLINE_PREV) & (v_cnt != V_STARTLINE))) begin
        h_cnt <= 0;
        v_cnt <= V_STARTLINE;
        src_fid <= (~interlaced_in_i | (V_STARTLINE < (V_TOTAL/2))) ? FID_ODD : FID_EVEN;
        dst_fid <= (~V_INTERLACED | (V_STARTLINE < (V_TOTAL/2))) ? FID_ODD : FID_EVEN;
        resync_strobe <= 1'b1;
    end else begin
        if (h_cnt == H_TOTAL-1) begin
            if ((~V_INTERLACED & (v_cnt == V_TOTAL-1)) |
                (V_INTERLACED & (dst_fid == FID_ODD) & (v_cnt == V_TOTAL)) |
                (V_INTERLACED & (dst_fid == FID_EVEN) & (v_cnt == V_TOTAL-1)))
            begin
                v_cnt <= 0;
                src_fid <= interlaced_in_i ? (src_fid ^ 1'b1) : FID_ODD;
                dst_fid <= V_INTERLACED ? (dst_fid ^ 1'b1) : FID_ODD;
                resync_strobe <= 1'b0;
            end else begin
                v_cnt <= v_cnt + 1'b1;
            end
            h_cnt <= 0;
        end else begin
            h_cnt <= h_cnt + 1'b1;
        end
    end
end

// Postprocess pipeline structure
// |    0     |    1     |    2    |    3    |    4    |    5    |
// |----------|----------|---------|---------|---------|---------|
// | SYNC/DE  |          |         |         |         |         |
// | X/Y POS  |          |         |         |         |         |
// |          |   MASK   |         |         |         |         |
// |          |          | LINEBUF |         |         |         |
// |          |          |         |  SLGEN  |         |         |


// Pipeline stage 0
always @(posedge PCLK_OUT_i) begin
    HSYNC_pp[1] <= (h_cnt < H_SYNCLEN) ? 1'b0 : 1'b1;
    if (dst_fid == FID_ODD)
        VSYNC_pp[1] <= ((v_cnt < V_SYNCLEN) | ((v_cnt == V_TOTAL) & (h_cnt >= (H_TOTAL/2)))) ? 1'b0 : 1'b1;
    else
        VSYNC_pp[1] <= ((v_cnt < V_SYNCLEN-1) | ((v_cnt == V_SYNCLEN-1) & (h_cnt < (H_TOTAL/2)))) ? 1'b0 : 1'b1;
    DE_pp[1] <= (h_cnt >= H_SYNCLEN+H_BACKPORCH) & (h_cnt < H_SYNCLEN+H_BACKPORCH+H_ACTIVE) & (v_cnt >= V_SYNCLEN+V_BACKPORCH) & (v_cnt < V_SYNCLEN+V_BACKPORCH+V_ACTIVE);

    if (h_cnt == H_SYNCLEN+H_BACKPORCH) begin
        if (v_cnt == V_SYNCLEN+V_BACKPORCH) begin
            ypos_pp[1] <= 0;
            // Bob deinterlace adjusts linebuf start position and y_ctr on even fields
            if (~MISC_LM_DEINT_MODE & (Y_RPT > 0) & ~V_INTERLACED & (src_fid == FID_EVEN)) begin
                ypos_lb <= Y_START_LB - 1'b1;
                y_ctr <= ((Y_RPT+1'b1) >> 1);
            end else begin
                ypos_lb <= (Y_SKIP & (dst_fid == FID_EVEN)) ? (Y_START_LB + 1'b1) : Y_START_LB;
                y_ctr <= 0;
            end
            xpos_lb_start <= (X_OFFSET < 10'sd0) ? 11'd0 : {1'b0, X_OFFSET};
        end else begin
            if (ypos_pp[1] < V_ACTIVE) begin
                ypos_pp[1] <= ypos_pp[1] + 1'b1;
            end

            if ((y_ctr == Y_RPT) | Y_SKIP) begin
                if ((ypos_lb >= NUM_LINE_BUFFERS-Y_STEP) & (ypos_lb < NUM_LINE_BUFFERS))
                    ypos_lb <= ypos_lb + Y_STEP - NUM_LINE_BUFFERS;
                else
                    ypos_lb <= ypos_lb + Y_STEP;
                y_ctr <= 0;
            end else begin
                y_ctr <= y_ctr + 1'b1;
            end
        end
        xpos_pp[1] <= 0;
        xpos_lb <= X_START_LB;
        x_ctr <= 0;
    end else begin
        if (xpos_pp[1] < H_ACTIVE) begin
            xpos_pp[1] <= xpos_pp[1] + 1'b1;
        end

        if (xpos_pp[1] >= xpos_lb_start) begin
            if (x_ctr == X_RPT) begin
                xpos_lb <= xpos_lb + 1'b1;
                x_ctr <= 0;
            end else begin
                x_ctr <= x_ctr + 1'b1;
            end
        end
    end
end

// Pipeline stages 1-
integer pp_idx;
always @(posedge PCLK_OUT_i) begin

    for(pp_idx = PP_LINEBUF_START; pp_idx <= PP_PL_END; pp_idx = pp_idx+1) begin
        HSYNC_pp[pp_idx] <= HSYNC_pp[pp_idx-1];
        VSYNC_pp[pp_idx] <= VSYNC_pp[pp_idx-1];
        DE_pp[pp_idx] <= DE_pp[pp_idx-1];
        xpos_pp[pp_idx] <= xpos_pp[pp_idx-1];
        ypos_pp[pp_idx] <= ypos_pp[pp_idx-1];
    end

    if (($signed({1'b0, xpos_pp[PP_LINEBUF_START-1]}) >= X_OFFSET) &
        ($signed({1'b0, xpos_pp[PP_LINEBUF_START-1]}) < X_OFFSET+X_SIZE) &
        ($signed({1'b0, ypos_pp[PP_LINEBUF_START-1]}) >= Y_OFFSET) &
        ($signed({1'b0, ypos_pp[PP_LINEBUF_START-1]}) < Y_OFFSET+Y_SIZE))
    begin
        mask_enable_pp[PP_LINEBUF_START] <= 1'b0;
    end else begin
        mask_enable_pp[PP_LINEBUF_START] <= 1'b1;
    end
    for(pp_idx = PP_LINEBUF_START+1; pp_idx <= PP_SLGEN_START; pp_idx = pp_idx+1) begin
        mask_enable_pp[pp_idx] <= mask_enable_pp[pp_idx-1];
    end

    R_pp[PP_SLGEN_END] <= testpattern_enable ? (xpos_pp[PP_SLGEN_START] ^ ypos_pp[PP_SLGEN_START]) : (mask_enable_pp[PP_SLGEN_START] ? 8'h00 : R_linebuf);
    G_pp[PP_SLGEN_END] <= testpattern_enable ? (xpos_pp[PP_SLGEN_START] ^ ypos_pp[PP_SLGEN_START]) : (mask_enable_pp[PP_SLGEN_START] ? 8'h00 : G_linebuf);
    B_pp[PP_SLGEN_END] <= testpattern_enable ? (xpos_pp[PP_SLGEN_START] ^ ypos_pp[PP_SLGEN_START]) : (mask_enable_pp[PP_SLGEN_START] ? 8'h00 : B_linebuf);
end

// Output
assign R_o = R_pp[PP_PL_END];
assign G_o = G_pp[PP_PL_END];
assign B_o = B_pp[PP_PL_END];
assign HSYNC_o = HSYNC_pp[PP_PL_END];
assign VSYNC_o = VSYNC_pp[PP_PL_END];
assign DE_o = DE_pp[PP_PL_END];
assign xpos_o = xpos_pp[PP_PL_END];
assign ypos_o = ypos_pp[PP_PL_END];

endmodule
