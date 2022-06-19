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

module pwm_2ch (
    input clk,
    input reset_n,
    input [3:0] ch1_duty,
    input [3:0] ch2_duty,
    output reg ch1_pwm,
    output reg ch2_pwm
);

parameter PERIOD = 100;

reg [($clog2(PERIOD)-1):0] ctr;

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        ctr <= 0;
    end else begin
        if (ctr >= PERIOD-1) begin
            ctr <= 0;
        end else begin
            ctr <= ctr + 1'b1;
        end
    end
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        ch1_pwm <= 0;
        ch2_pwm <= 0;
    end else begin
        if (ch1_duty == 0) begin
            ch1_pwm <= 0;
        end else if (ch1_duty >= 10) begin
            ch1_pwm <= 1;
        end else begin
            if (ctr >= PERIOD-1) begin
                ch1_pwm <= 1;
            end else if (ctr == (ch1_duty * (PERIOD/10))) begin
                ch1_pwm <= 0;
            end
        end

        if (ch2_duty == 0) begin
            ch2_pwm <= 0;
        end else if (ch2_duty >= 10) begin
            ch2_pwm <= 1;
        end else begin
            if (ctr >= PERIOD-1) begin
                ch2_pwm <= 1;
            end else if (ctr == (ch2_duty * (PERIOD/10))) begin
                ch2_pwm <= 0;
            end
        end
    end
end

endmodule
