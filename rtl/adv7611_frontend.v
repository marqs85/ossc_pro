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

module adv7611_frontend (
    input PCLK_i,
    input reset_n,
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
    output reg DE_o,
    output reg FID_o,
    output reg interlace_flag,
    output reg [10:0] xpos,
    output reg [10:0] ypos,
    output reg frame_change
);

localparam FID_EVEN = 1'b0;
localparam FID_ODD = 1'b1;

reg HSYNC_i_prev, VSYNC_i_prev, DE_i_prev;
reg FID_prev;

always @(posedge PCLK_i) begin
    R_o <= R_i;
    G_o <= G_i;
    B_o <= B_i;
    HSYNC_o <= HSYNC_i;
    VSYNC_o <= VSYNC_i;
    DE_o <= DE_i;

    if (VSYNC_i_prev & ~VSYNC_i) begin
        if (HSYNC_i_prev & ~HSYNC_i) begin
            FID_o <= FID_ODD;
            interlace_flag <= (FID_o == FID_EVEN);
            frame_change <= 1'b1;
        end else begin
            FID_o <= FID_EVEN;
            interlace_flag <= (FID_o == FID_ODD);
            frame_change <= ~interlace_flag;
        end

        xpos <= 0;
        ypos <= 0;
    end else begin
        if (HSYNC_i_prev & ~HSYNC_i) begin
            frame_change <= 1'b0;
        end

        if (DE_i_prev & ~DE_i) begin
            xpos <= 0;
            ypos <= ypos + 1'b1;
        end else if (DE_i_prev & DE_i) begin
            xpos <= xpos + 1'b1;
        end
    end

    HSYNC_i_prev <= HSYNC_i;
    VSYNC_i_prev <= VSYNC_i;
    DE_i_prev <= DE_i;
end

endmodule
