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
    input frame_change_i,
    input [10:0] xpos_i,
    input [10:0] ypos_i,
    input [31:0] h_out_config,
    input [31:0] h_out_config2,
    input [31:0] v_out_config,
    input [31:0] v_out_config2,
    input [31:0] misc_config,
    input [31:0] sl_config,
    input [31:0] sl_config2,
    output PCLK_o,
    output [7:0] R_o,
    output [7:0] G_o,
    output [7:0] B_o,
    output reg HSYNC_o,
    output reg VSYNC_o,
    output reg DE_o,
    output reg resync_strobe
);

wire [8:0] H_SYNCLEN = h_out_config[28:20];
wire [8:0] H_BACKPORCH = h_out_config[19:11];
wire [10:0] H_ACTIVE = h_out_config[10:0];
wire [11:0] H_TOTAL = h_out_config2[11:0];

wire [4:0] V_SYNCLEN = v_out_config[24:20];
wire [8:0] V_BACKPORCH = v_out_config[19:11];
wire [10:0] V_ACTIVE = v_out_config[10:0];
wire [10:0] V_TOTAL = v_out_config2[10:0];
wire [10:0] V_STARTLINE = v_out_config2[21:11];

reg frame_change_sync1_reg, frame_change_sync2_reg, frame_change_prev;
wire frame_change = frame_change_sync2_reg;

wire [2:0] X_RPT = h_out_config[31:29];
wire [2:0] Y_RPT = v_out_config[27:25];

wire [2:0] X_SKIP = h_out_config2[23:21];

wire [8:0] X_START = h_out_config2[20:12];
wire [8:0] Y_START = 0;

reg [11:0] h_cnt;
reg [10:0] v_cnt;

reg [10:0] xpos;
reg [10:0] ypos;
reg [2:0] x_ctr;
reg [2:0] y_ctr;

assign PCLK_o = PCLK_OUT_i;

wire [13:0] linebuf_wraddr = {ypos_i[2:0], xpos_i};
wire [13:0] linebuf_rdaddr = {ypos[2:0], xpos};

linebuf linebuf_rgb (
    .data({R_i, G_i, B_i}),
    .rdaddress(linebuf_rdaddr),
    .rdclock(PCLK_OUT_i),
    .wraddress(linebuf_wraddr),
    .wrclock(PCLK_CAP_i),
    .wren(DE_i),
    .q({R_o, G_o, B_o})
);

always @(posedge PCLK_OUT_i) begin
    frame_change_sync1_reg <= frame_change_i;
    frame_change_sync2_reg <= frame_change_sync1_reg;
    frame_change_prev <= frame_change_sync2_reg;
end

always @(posedge PCLK_OUT_i) begin
    // TODO: fix functionality when V_STARTLINE=0
    if (~frame_change_prev & frame_change & ((v_cnt != V_STARTLINE-1) & (v_cnt != V_STARTLINE))) begin
        h_cnt <= 0;
        v_cnt <= V_STARTLINE;
        resync_strobe <= 1'b1;
    end else begin
        if (h_cnt == H_TOTAL-1) begin
            if (v_cnt == V_TOTAL-1) begin
                v_cnt <= 0;
                resync_strobe <= 1'b0;
            end else begin
                v_cnt <= v_cnt + 1'b1;
            end
            h_cnt <= 0;
        end else begin
            h_cnt <= h_cnt + 1'b1;
        end
    end

    HSYNC_o <= (h_cnt < H_SYNCLEN) ? 1'b0 : 1'b1;
    VSYNC_o <= (v_cnt < V_SYNCLEN) ? 1'b0 : 1'b1;
    DE_o <= (h_cnt >= H_SYNCLEN+H_BACKPORCH) & (h_cnt < H_SYNCLEN+H_BACKPORCH+H_ACTIVE) & (v_cnt >= V_SYNCLEN+V_BACKPORCH) & (v_cnt < V_SYNCLEN+V_BACKPORCH+V_ACTIVE);

    if (h_cnt == H_SYNCLEN+H_BACKPORCH+X_START-1'b1) begin
        if (v_cnt == V_SYNCLEN+V_BACKPORCH+Y_START) begin
            ypos <= 0;
            y_ctr <= 0;
        end else begin
            if (y_ctr == Y_RPT) begin
                ypos <= ypos + 1'b1;
                y_ctr <= 0;
            end else begin
                y_ctr <= y_ctr + 1'b1;
            end
        end
        xpos <= 0;
        x_ctr <= 0;
    end else begin
        if (x_ctr == X_RPT) begin
            xpos <= xpos + 1'b1 + X_SKIP;
            x_ctr <= 0;
        end else begin
            x_ctr <= x_ctr + 1'b1;
        end
    end
end

endmodule

