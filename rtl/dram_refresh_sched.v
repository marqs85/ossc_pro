//
// Copyright (C) 2022  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

module dram_refresh_sched #(
    parameter REFRESH_INTERVAL=1000
) (
    input clk,
    input reset_n,
    input enable,
    input refresh_ack,
    output reg refresh_req
);

reg [($clog2(REFRESH_INTERVAL)-1):0] ctr;

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        ctr <= 0;
    end else begin
        if (!enable || (ctr >= REFRESH_INTERVAL-1)) begin
            ctr <= 0;
        end else begin
            ctr <= ctr + 1'b1;
        end
    end
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        refresh_req <= 0;
    end else begin
        if (!enable || refresh_ack)
            refresh_req <= 0;
        else if (ctr == REFRESH_INTERVAL-1)
            refresh_req <= 1;
    end
end

endmodule
