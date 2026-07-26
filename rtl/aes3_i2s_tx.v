//
// Copyright (C) 2026  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

module aes3_i2s_tx (
    input clk,
    input reset_n,
    input aes3_sclk,
    input aes3_lrck,
    input aes3_sdata,
    output i2s_bck,
    output reg i2s_ws,
    output reg i2s_data
);

reg [23:0] sample[1:0] /* synthesis ramstyle = "logic" */;
reg [4:0] ctr;
reg aes3_sclk_q, aes3_lrck_q;

always @(posedge clk or negedge reset_n)
begin
    if (!reset_n) begin
        ctr <= 0;
    end else begin
        if ((aes3_sclk_q == 1'b1) && (aes3_sclk == 1'b0)) begin
            if (aes3_lrck_q ^ aes3_lrck) begin
                ctr <= 0;
                i2s_ws <= aes3_lrck_q;
            end else begin
                if (ctr == 2) begin
                    i2s_ws <= ~i2s_ws;
                end else if ((ctr >= 3) && (ctr < 27)) begin
                    sample[~i2s_ws][ctr-3] <= aes3_sdata;
                    i2s_data <= sample[i2s_ws][26-ctr];
                end else begin
                    i2s_data <= 1'b0;
                end

                ctr <= ctr + 1'b1;
            end

            aes3_lrck_q <= aes3_lrck;
        end

        aes3_sclk_q <= aes3_sclk;
    end
end

assign i2s_bck = aes3_sclk;

endmodule
