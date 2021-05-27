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

`define PO_RESET_WIDTH 27000
//`define PCB_1P3

`define VIP
`define PIXPAR2

module ossc_pro (
    input CLK27_i,
    output FPGA_PCLK1x_o,

    input ISL_PCLK_i,
    input [7:0] ISL_R_i,
    input [7:0] ISL_G_i,
    input [7:0] ISL_B_i,
    input ISL_HS_i,
    input ISL_HSYNC_i,
    input ISL_VSYNC_i,
    input ISL_INT_N_i,
    output ISL_EXT_PCLK_o,
    output ISL_RESET_N_o,
    output ISL_COAST_o,
    output ISL_CLAMP_o,

    input HDMIRX_PCLK_i,
    input HDMIRX_AP_i,
    input [7:0] HDMIRX_R_i,
    input [7:0] HDMIRX_G_i,
    input [7:0] HDMIRX_B_i,
    input HDMIRX_HSYNC_i,
    input HDMIRX_VSYNC_i,
    input HDMIRX_DE_i,
    input HDMIRX_INT_N_i,
    input HDMIRX_I2S_BCK_i,
    input HDMIRX_I2S_WS_i,
    output HDMIRX_RESET_N_o,

    input HDMITX_INT_N_i,
    output HDMITX_PCLK_o,
    output HDMITX_I2S_BCK_o,
    output HDMITX_I2S_WS_o,
    output HDMITX_I2S_DATA_o,
    output HDMITX_SPDIF_o,
    output HDMITX_5V_EN_o,
    output reg [7:0] HDMITX_R_o,
    output reg [7:0] HDMITX_G_o,
    output reg [7:0] HDMITX_B_o,
    output reg HDMITX_HSYNC_o,
    output reg HDMITX_VSYNC_o,
    output reg HDMITX_DE_o,

    input DDR_RZQ_i,
    output [9:0] DDR_CA_o,
    output DDR_CK_o_p,
    output DDR_CK_o_n,
    output DDR_CKE_o,
    output DDR_CS_N_o,
    output [3:0] DDR_DM_o,
    inout [31:0] DDR_DQ_io,
    inout [3:0] DDR_DQS_io_p,
    inout [3:0] DDR_DQS_io_n,

    input PCM_I2S_BCK_i,
    input PCM_I2S_WS_i,
    input PCM_I2S_DATA_i,

    input SI_PCLK_i,
    input SI_CLK_EXTRA_i,
    input SI_INT_N_i,

    inout SCL_io,
    inout SDA_io,

    input SPDIF_EXT_i,
    input IR_RX_i,
    input [5:0] BTN_i,
    output [2:0] LED_o,
    output AUDMUX_o,

    output SD_CLK_o,
    inout SD_CMD_io,
    inout [3:0] SD_DATA_io,
    input SD_DETECT_i,

    inout [5:0] EXT_IO_A_io,

    inout [31:0] EXT_IO_B_io,
    output [1:0] LS_DIR_o
);

wire jtagm_reset_req;

wire [15:0] sys_ctrl;
wire sys_poweron = sys_ctrl[0];
wire isl_reset_n = sys_ctrl[1];
wire hdmirx_reset_n = sys_ctrl[2];
wire emif_hwreset_n = sys_ctrl[3];
wire emif_swreset_n = sys_ctrl[4];
wire emif_powerdn_req = sys_ctrl[5];
wire emif_powerdn_mask = sys_ctrl[6];
wire capture_sel = sys_ctrl[7];
wire isl_hsync_pol = sys_ctrl[8];
wire isl_vsync_pol = sys_ctrl[9];
wire isl_vsync_type = sys_ctrl[10];
wire audmux_sel = sys_ctrl[11];
wire testpattern_enable = sys_ctrl[12];
wire csc_enable = sys_ctrl[13];
wire framelock = sys_ctrl[14];
wire hdmirx_spdif = sys_ctrl[15];

reg ir_rx_sync1_reg, ir_rx_sync2_reg;
reg [5:0] btn_sync1_reg, btn_sync2_reg;

wire [15:0] ir_code;
wire [7:0] ir_code_cnt;

wire pclk_capture, pclk_out;
reg pclk_capture_div2, pclk_out_div2;

reg [15:0] po_reset_ctr = 0;
reg po_reset_n = 1'b0;

wire pll_locked;

`ifdef SIMULATION
wire sys_reset_n = (po_reset_n & pll_locked);
`else
wire sys_reset_n = (po_reset_n & ~jtagm_reset_req & pll_locked);
`endif

wire emif_status_init_done;
wire emif_status_cal_success;
wire emif_status_cal_fail;
wire emif_status_powerdn_ack;

wire sd_detect = ~SD_DETECT_i;

wire cvi_overflow, cvo_underflow;

wire [31:0] controls = {2'h0, btn_sync2_reg, ir_code_cnt, ir_code};
wire [31:0] sys_status = {cvi_overflow, cvo_underflow, 25'h0, sd_detect, emif_status_powerdn_ack, emif_status_cal_fail, emif_status_cal_success, emif_status_init_done};

wire [31:0] hv_in_config, hv_in_config2, hv_in_config3, hv_out_config, hv_out_config2, hv_out_config3, xy_out_config, xy_out_config2;
wire [31:0] misc_config, sl_config, sl_config2;

reg [23:0] resync_led_ctr;
reg resync_strobe_sync1_reg, resync_strobe_sync2_reg, resync_strobe_prev;
wire resync_strobe_i;
wire resync_strobe = resync_strobe_sync2_reg;

//BGR
assign LED_o = sys_poweron ? {framelock, (ir_code == 0), (resync_led_ctr != 0)} : 3'b001;
//assign LED_o = {emif_status_init_done, emif_status_cal_success, emif_status_cal_fail};

wire [11:0] xpos_sc;
wire [10:0] ypos_sc;
wire osd_enable;
wire [1:0] osd_color;

assign ISL_RESET_N_o = isl_reset_n;
assign HDMIRX_RESET_N_o = hdmirx_reset_n;

reg emif_hwreset_n_sync1_reg, emif_hwreset_n_sync2_reg, emif_swreset_n_sync1_reg, emif_swreset_n_sync2_reg;

assign HDMITX_5V_EN_o = 1'b1;

wire sd_cmd_oe_o, sd_cmd_out_o, sd_dat_oe_o;
wire [3:0] sd_dat_out_o;

assign SD_CMD_io = sd_cmd_oe_o ? sd_cmd_out_o : 1'bz;
assign SD_DATA_io = sd_dat_oe_o ? sd_dat_out_o : 4'bzzzz;

assign FPGA_PCLK1x_o = pclk_capture;

// ISL51002 RGB digitizer
reg [7:0] ISL_R, ISL_G, ISL_B;
reg ISL_HS;
reg ISL_VS_sync1_reg, ISL_VS_sync2_reg;
reg ISL_HSYNC_sync1_reg, ISL_HSYNC_sync2_reg;
reg ISL_VSYNC_sync1_reg, ISL_VSYNC_sync2_reg;
always @(posedge ISL_PCLK_i) begin
    ISL_R <= ISL_R_i;
    ISL_G <= ISL_G_i;
    ISL_B <= ISL_B_i;
    ISL_HS <= ISL_HS_i;

    // sync to pclk
    ISL_VS_sync1_reg <= ISL_VSYNC_i;
    ISL_VS_sync2_reg <= ISL_VS_sync1_reg;
end
always @(posedge CLK27_i) begin
    // sync to always-running fixed meas clk
    ISL_HSYNC_sync1_reg <= ISL_HSYNC_i;
    ISL_HSYNC_sync2_reg <= ISL_HSYNC_sync1_reg;
    ISL_VSYNC_sync1_reg <= ISL_VSYNC_i;
    ISL_VSYNC_sync2_reg <= ISL_VSYNC_sync1_reg;
end

wire [7:0] ISL_R_post, ISL_G_post, ISL_B_post;
wire ISL_HSYNC_post, ISL_VSYNC_post, ISL_DE_post, ISL_FID_post;
wire ISL_fe_interlace, ISL_fe_frame_change;
wire [19:0] ISL_fe_pcnt_frame;
wire [10:0] ISL_fe_vtotal, ISL_fe_xpos, ISL_fe_ypos;
isl51002_frontend u_isl_frontend ( 
    .PCLK_i(ISL_PCLK_i),
    .CLK_MEAS_i(CLK27_i),
    .reset_n(sys_reset_n),
    .R_i(ISL_R),
    .G_i(ISL_G),
    .B_i(ISL_B),
    .HS_i(ISL_HS),
    .VS_i(ISL_VS_sync2_reg),
    .HSYNC_i(ISL_HSYNC_sync2_reg),
    .VSYNC_i(ISL_VSYNC_sync2_reg),
    .DE_i(1'b0),
    .FID_i(1'b0),
    .hsync_i_polarity(isl_hsync_pol),
    .vsync_i_polarity(isl_vsync_pol),
    .vsync_i_type(isl_vsync_type),
    .csc_enable(csc_enable),
    .csc_cs(misc_config[14]),
    .hv_in_config(hv_in_config),
    .hv_in_config2(hv_in_config2),
    .hv_in_config3(hv_in_config3),
    .R_o(ISL_R_post),
    .G_o(ISL_G_post),
    .B_o(ISL_B_post),
    .HSYNC_o(ISL_HSYNC_post),
    .VSYNC_o(ISL_VSYNC_post),
    .DE_o(ISL_DE_post),
    .FID_o(ISL_FID_post),
    .interlace_flag(ISL_fe_interlace),
    .xpos_o(ISL_fe_xpos),
    .ypos_o(ISL_fe_ypos),
    .vtotal(ISL_fe_vtotal),
    .frame_change(ISL_fe_frame_change),
    .pcnt_frame(ISL_fe_pcnt_frame)
);

// ADV7611 HDMI RX
reg [7:0] HDMIRX_R, HDMIRX_G, HDMIRX_B;
reg HDMIRX_HSYNC, HDMIRX_VSYNC, HDMIRX_DE;
reg [7:0] HDMIRX_R_iq, HDMIRX_G_iq, HDMIRX_B_iq;
reg HDMIRX_HSYNC_iq, HDMIRX_VSYNC_iq, HDMIRX_DE_iq;
always @(posedge HDMIRX_PCLK_i) begin
    HDMIRX_R_iq <= HDMIRX_R_i;
    HDMIRX_G_iq <= HDMIRX_G_i;
    HDMIRX_B_iq <= HDMIRX_B_i;
    HDMIRX_HSYNC_iq <= HDMIRX_HSYNC_i;
    HDMIRX_VSYNC_iq <= HDMIRX_VSYNC_i;
    HDMIRX_DE_iq <= HDMIRX_DE_i;

    HDMIRX_R <= HDMIRX_R_iq;
    HDMIRX_G <= HDMIRX_G_iq;
    HDMIRX_B <= HDMIRX_B_iq;
    HDMIRX_HSYNC <= HDMIRX_HSYNC_iq;
    HDMIRX_VSYNC <= HDMIRX_VSYNC_iq;
    HDMIRX_DE <= HDMIRX_DE_iq;
end

wire [7:0] HDMIRX_R_post, HDMIRX_G_post, HDMIRX_B_post;
wire HDMIRX_HSYNC_post, HDMIRX_VSYNC_post, HDMIRX_DE_post, HDMIRX_FID_post;
wire HDMIRX_fe_interlace, HDMIRX_fe_frame_change;
wire [10:0] HDMIRX_fe_xpos, HDMIRX_fe_ypos;
adv7611_frontend u_hdmirx_frontend ( 
    .PCLK_i(HDMIRX_PCLK_i),
    .reset_n(sys_reset_n),
    .R_i(HDMIRX_R),
    .G_i(HDMIRX_G),
    .B_i(HDMIRX_B),
    .HSYNC_i(HDMIRX_HSYNC),
    .VSYNC_i(HDMIRX_VSYNC),
    .DE_i(HDMIRX_DE),
    .R_o(HDMIRX_R_post),
    .G_o(HDMIRX_G_post),
    .B_o(HDMIRX_B_post),
    .HSYNC_o(HDMIRX_HSYNC_post),
    .VSYNC_o(HDMIRX_VSYNC_post),
    .DE_o(HDMIRX_DE_post),
    .FID_o(HDMIRX_FID_post),
    .interlace_flag(HDMIRX_fe_interlace),
    .xpos_o(HDMIRX_fe_xpos),
    .ypos_o(HDMIRX_fe_ypos),
    .frame_change(HDMIRX_fe_frame_change)
);

// capture clock mux
cyclonev_clkselect clkmux_capture ( 
    .clkselect({1'b0, capture_sel}),
    .inclk({2'b00, HDMIRX_PCLK_i, ISL_PCLK_i}),
    .outclk(pclk_capture)
);

// capture data mux
reg [7:0] R_capt, G_capt, B_capt;
reg HSYNC_capt, VSYNC_capt, DE_capt, FID_capt;
reg interlace_flag_capt, frame_change_capt;
reg [10:0] xpos_capt, ypos_capt;
always @(posedge pclk_capture) begin
    R_capt <= capture_sel ? HDMIRX_R_post : ISL_R_post;
    G_capt <= capture_sel ? HDMIRX_G_post : ISL_G_post;
    B_capt <= capture_sel ? HDMIRX_B_post : ISL_B_post;
    HSYNC_capt <= capture_sel ? HDMIRX_HSYNC_post : ISL_HSYNC_post;
    VSYNC_capt <= capture_sel ? HDMIRX_VSYNC_post : ISL_VSYNC_post;
    DE_capt <= capture_sel ? HDMIRX_DE_post : ISL_DE_post;
    FID_capt <= capture_sel ? HDMIRX_FID_post : ISL_FID_post;
    interlace_flag_capt <= capture_sel ? HDMIRX_fe_interlace : ISL_fe_interlace;
    frame_change_capt <= capture_sel ? HDMIRX_fe_frame_change : ISL_fe_frame_change;
    xpos_capt <= capture_sel ? HDMIRX_fe_xpos : ISL_fe_xpos;
    ypos_capt <= capture_sel ? HDMIRX_fe_ypos : ISL_fe_ypos;
end

// output clock assignment
wire PCLK_sc;
assign pclk_out = PCLK_sc;
`ifdef PCB_1P3
assign HDMITX_PCLK_o = pclk_out;
`else
assign HDMITX_PCLK_o = ~pclk_out;
`endif


// VIP
wire vip_select = misc_config[15];

always @(posedge pclk_capture) begin
    pclk_capture_div2 <= pclk_capture_div2 ^ 1'b1;
end

always @(posedge pclk_out) begin
    pclk_out_div2 <= pclk_out_div2 ^ 1'b1;
end

`ifdef VIP
`ifdef PIXPAR2
wire [47:0] VIP_DATA_o;
wire [1:0] VIP_HSYNC_o, VIP_VSYNC_o, VIP_DE_o;

`ifdef DIV2_SYNC
reg [47:0] VIP_DATA_i;
reg [1:0] VIP_HSYNC_i, VIP_VSYNC_i, VIP_DE_i, VIP_FID_i;
reg [7:0] R_vip, G_vip, B_vip;
reg HSYNC_vip, VSYNC_vip, DE_vip;

always @(posedge pclk_capture) begin
    if (pclk_capture_div2 == 0) begin
        VIP_DATA_i[47:24] <= {R_capt, G_capt, B_capt};
        {VIP_HSYNC_i[1], VIP_VSYNC_i[1], VIP_DE_i[1], VIP_FID_i[1]} <= {~HSYNC_capt, ~VSYNC_capt, DE_capt, ~FID_capt};
    end else begin
        VIP_DATA_i[23:0] <= {R_capt, G_capt, B_capt};
        {VIP_HSYNC_i[0], VIP_VSYNC_i[0], VIP_DE_i[0], VIP_FID_i[0]} <= {~HSYNC_capt, ~VSYNC_capt, DE_capt, ~FID_capt};
    end
end

always @(posedge pclk_out) begin
    if (pclk_out_div2 == 0) begin
        {R_vip, G_vip, B_vip} <= VIP_DATA_o[23:0];
        {HSYNC_vip, VSYNC_vip, DE_vip} <= {~VIP_HSYNC_o[0], ~VIP_VSYNC_o[0], VIP_DE_o[0]};
    end else begin
        {R_vip, G_vip, B_vip} <= VIP_DATA_o[47:24];
        {HSYNC_vip, VSYNC_vip, DE_vip} <= {~VIP_HSYNC_o[1], ~VIP_VSYNC_o[1], VIP_DE_o[1]};
    end
end
`else // DIV2_SYNC
wire [47:0] VIP_DATA_i;
wire [1:0] VIP_HSYNC_i, VIP_VSYNC_i, VIP_DE_i, VIP_FID_i;
wire [7:0] R_vip, G_vip, B_vip;
wire HSYNC_vip, VSYNC_vip, DE_vip;
wire [3:0] unused1, unused0;
wire [4:0] unused_out;

dc_fifo_in  dc_fifo_in_inst (
    .data({R_capt, G_capt, B_capt, ~HSYNC_capt, ~VSYNC_capt, DE_capt, ~FID_capt, 4'h0}),
    .rdclk(pclk_capture_div2),
    .rdreq(1),
    .wrclk(pclk_capture),
    .wrreq(1),
    .q({VIP_DATA_i[47:24], VIP_HSYNC_i[1], VIP_VSYNC_i[1], VIP_DE_i[1], VIP_FID_i[1], unused1, VIP_DATA_i[23:0], VIP_HSYNC_i[0], VIP_VSYNC_i[0], VIP_DE_i[0], VIP_FID_i[0], unused0})
);

dc_fifo_out  dc_fifo_out_inst (
    .data({VIP_DATA_o[47:24], VIP_HSYNC_o[1], VIP_VSYNC_o[1], VIP_DE_o[1], 5'h0, VIP_DATA_o[23:0], VIP_HSYNC_o[0], VIP_VSYNC_o[0], VIP_DE_o[0], 5'h0}),
    .rdclk(pclk_out),
    .rdreq(1),
    .wrclk(pclk_out_div2),
    .wrreq(1),
    .q({R_vip, G_vip, B_vip, HSYNC_vip, VSYNC_vip, DE_vip, unused_out})
);
`endif // DIV2_SYNC
`else // PIXPAR2
wire [23:0] VIP_DATA_i = {R_capt, G_capt, B_capt};
wire VIP_HSYNC_i = ~HSYNC_capt;
wire VIP_VSYNC_i = ~VSYNC_capt;
wire VIP_DE_i = DE_capt;
wire VIP_FID_i = ~FID_capt;
wire [23:0] VIP_DATA_o;
wire [7:0] R_vip = VIP_DATA_o[23:16];
wire [7:0] G_vip = VIP_DATA_o[15:8];
wire [7:0] B_vip = VIP_DATA_o[7:0];
wire VIP_HSYNC_o, VIP_VSYNC_o, VIP_DE_o;
wire HSYNC_vip = VIP_HSYNC_o;
wire VSYNC_vip = VIP_VSYNC_o;
wire DE_vip = VIP_DE_o;
`endif // PIXPAR2

reg VSYNC_vip_prev;
wire vip_frame_start = VSYNC_vip_prev & ~VSYNC_vip;

always @(posedge pclk_out) begin
    VSYNC_vip_prev <= VSYNC_vip;
end
`endif // VIP

// output data assignment (2 stages and launch on negedge for timing closure)
reg [7:0] R_out, G_out, B_out;
reg HSYNC_out, VSYNC_out, DE_out;
wire [7:0] R_sc, G_sc, B_sc;
wire HSYNC_sc, VSYNC_sc, DE_sc;

always @(posedge pclk_out) begin
    if (osd_enable) begin
        if (osd_color == 2'h0) begin
            {R_out, G_out, B_out} <= 24'h000000;
        end else if (osd_color == 2'h1) begin
            {R_out, G_out, B_out} <= 24'h0000ff;
        end else if (osd_color == 2'h2) begin
            {R_out, G_out, B_out} <= 24'hffff00;
        end else begin
            {R_out, G_out, B_out} <= 24'hffffff;
        end
    end else begin
        {R_out, G_out, B_out} <= {R_sc, G_sc, B_sc};
    end

    HSYNC_out <= HSYNC_sc;
    VSYNC_out <= VSYNC_sc;
    DE_out <= DE_sc;
end

// PCB v1.3 uses ADV7513 and has crosstalk issue with R[0] while v1.4- use SiI1136
`ifdef PCB_1P3
always @(negedge pclk_out) begin
    HDMITX_R_o <= {R_out[7:1], 1'b0};
`else
always @(posedge pclk_out) begin
    HDMITX_R_o <= R_out;
`endif
    HDMITX_G_o <= G_out;
    HDMITX_B_o <= B_out;
    HDMITX_HSYNC_o <= HSYNC_out;
    HDMITX_VSYNC_o <= VSYNC_out;
    HDMITX_DE_o <= DE_out;
end

//audio
assign HDMITX_I2S_BCK_o = capture_sel ? HDMIRX_I2S_BCK_i : PCM_I2S_BCK_i;
assign HDMITX_I2S_WS_o = capture_sel ? HDMIRX_I2S_WS_i : PCM_I2S_WS_i;
assign HDMITX_I2S_DATA_o = capture_sel ? HDMIRX_AP_i : PCM_I2S_DATA_i;
assign HDMITX_SPDIF_o = hdmirx_spdif ? HDMIRX_AP_i : SPDIF_EXT_i;

assign AUDMUX_o = ~audmux_sel;

// 0=input 1=output
assign LS_DIR_o = 2'b11;

assign EXT_IO_A_io = {6{&btn_sync2_reg}};
assign EXT_IO_B_io = {32{&btn_sync2_reg}};

// Power-on reset pulse generation (seems to be needed for booting from flash)
always @(posedge CLK27_i)
begin
    if (po_reset_ctr == `PO_RESET_WIDTH)
        po_reset_n <= 1'b1;
    else
        po_reset_ctr <= po_reset_ctr + 1'b1;
end

always @(posedge CLK27_i) begin
    if (~resync_strobe_prev & resync_strobe) begin
        resync_led_ctr <= {24{1'b1}};
    end else if (resync_led_ctr > 0) begin
        resync_led_ctr <= resync_led_ctr - 1'b1;
    end

    resync_strobe_sync1_reg <= resync_strobe_i;
    resync_strobe_sync2_reg <= resync_strobe_sync1_reg;
    resync_strobe_prev <= resync_strobe_sync2_reg;
end

// Insert synchronizers to async inputs (synchronize to CPU clock)
always @(posedge CLK27_i or negedge po_reset_n) begin
    if (!po_reset_n) begin
        btn_sync1_reg <= 2'b11;
        btn_sync2_reg <= 2'b11;
        ir_rx_sync1_reg <= 1'b1;
        ir_rx_sync2_reg <= 1'b1;
    end else begin
        btn_sync1_reg <= BTN_i;
        btn_sync2_reg <= btn_sync1_reg;
        ir_rx_sync1_reg <= IR_RX_i;
        ir_rx_sync2_reg <= ir_rx_sync1_reg;
    end
end

always @(posedge CLK27_i or negedge po_reset_n) begin
    if (!po_reset_n) begin
        emif_hwreset_n_sync1_reg <= 1'b0;
        emif_hwreset_n_sync2_reg <= 1'b0;
        emif_swreset_n_sync1_reg <= 1'b0;
        emif_swreset_n_sync2_reg <= 1'b0;
    end else begin
        emif_hwreset_n_sync1_reg <= emif_hwreset_n;
        emif_hwreset_n_sync2_reg <= emif_hwreset_n_sync1_reg;
        emif_swreset_n_sync1_reg <= emif_swreset_n;
        emif_swreset_n_sync2_reg <= emif_swreset_n_sync1_reg;
    end
end


// Qsys system
sys sys_inst (
    .clk_clk                                (CLK27_i),
    .reset_reset_n                          (sys_reset_n),
    .pll_0_reset_reset                      (~po_reset_n),
    .pll_0_locked_export                    (pll_locked),
    .pulpino_0_config_testmode_i            (1'b0),
    .pulpino_0_config_fetch_enable_i        (1'b1),
    .pulpino_0_config_clock_gating_i        (1'b0),
    .pulpino_0_config_boot_addr_i           (32'h02500000),
    .master_0_master_reset_reset            (jtagm_reset_req),
    .sdc_controller_0_sd_sd_cmd_dat_i       (SD_CMD_io),
    .sdc_controller_0_sd_sd_cmd_out_o       (sd_cmd_out_o),
    .sdc_controller_0_sd_sd_cmd_oe_o        (sd_cmd_oe_o),
    .sdc_controller_0_sd_sd_dat_dat_i       (SD_DATA_io),
    .sdc_controller_0_sd_sd_dat_out_o       (sd_dat_out_o),
    .sdc_controller_0_sd_sd_dat_oe_o        (sd_dat_oe_o),
    .sdc_controller_0_sd_clk_o_clk          (SD_CLK_o),
    .i2c_opencores_0_export_scl_pad_io      (SCL_io),
    .i2c_opencores_0_export_sda_pad_io      (SDA_io),
    .i2c_opencores_0_export_spi_miso_pad_i  (1'b0),
    .pio_0_sys_ctrl_out_export              (sys_ctrl),
    .pio_1_controls_in_export               (controls),
    .pio_2_sys_status_in_export             (sys_status),
    .sc_config_0_sc_if_fe_status_i          ({20'h0, ISL_fe_interlace, ISL_fe_vtotal}),
    .sc_config_0_sc_if_fe_status2_i         ({12'h0, ISL_fe_pcnt_frame}),
    .sc_config_0_sc_if_lt_status_i          (32'h00000000),
    .sc_config_0_sc_if_hv_in_config_o       (hv_in_config),
    .sc_config_0_sc_if_hv_in_config2_o      (hv_in_config2),
    .sc_config_0_sc_if_hv_in_config3_o      (hv_in_config3),
    .sc_config_0_sc_if_hv_out_config_o      (hv_out_config),
    .sc_config_0_sc_if_hv_out_config2_o     (hv_out_config2),
    .sc_config_0_sc_if_hv_out_config3_o     (hv_out_config3),
    .sc_config_0_sc_if_xy_out_config_o      (xy_out_config),
    .sc_config_0_sc_if_xy_out_config2_o     (xy_out_config2),
    .sc_config_0_sc_if_misc_config_o        (misc_config),
    .sc_config_0_sc_if_sl_config_o          (sl_config),
    .sc_config_0_sc_if_sl_config2_o         (sl_config2),
    .osd_generator_0_osd_if_vclk            (PCLK_sc),
    .osd_generator_0_osd_if_xpos            (xpos_sc),
    .osd_generator_0_osd_if_ypos            (ypos_sc),
    .osd_generator_0_osd_if_osd_enable      (osd_enable),
    .osd_generator_0_osd_if_osd_color       (osd_color),
    .mem_if_lpddr2_emif_0_global_reset_reset_n     (emif_hwreset_n_sync2_reg),
    .mem_if_lpddr2_emif_0_soft_reset_reset_n       (emif_swreset_n_sync2_reg),
    .mem_if_lpddr2_emif_0_status_local_init_done   (emif_status_init_done),
    .mem_if_lpddr2_emif_0_status_local_cal_success (emif_status_cal_success),
    .mem_if_lpddr2_emif_0_status_local_cal_fail    (emif_status_cal_fail),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_req  (emif_powerdn_req),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_chip (emif_powerdn_mask),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_ack  (emif_status_powerdn_ack),
    .memory_mem_ca                                 (DDR_CA_o),
    .memory_mem_ck                                 (DDR_CK_o_p),
    .memory_mem_ck_n                               (DDR_CK_o_n),
    .memory_mem_cke                                (DDR_CKE_o),
    .memory_mem_cs_n                               (DDR_CS_N_o),
    .memory_mem_dm                                 (DDR_DM_o),
    .memory_mem_dq                                 (DDR_DQ_io),
    .memory_mem_dqs                                (DDR_DQS_io_p),
    .memory_mem_dqs_n                              (DDR_DQS_io_n),
    .oct_rzqin                                     (DDR_RZQ_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_clk                    (
`ifdef PIXPAR2
    pclk_capture_div2
`else
    pclk_capture
`endif
    ),
    .alt_vip_cl_cvi_0_clocked_video_vid_data                   (VIP_DATA_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_de                     (VIP_DE_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_datavalid              (1'b1),
    .alt_vip_cl_cvi_0_clocked_video_vid_locked                 (1'b1),
    .alt_vip_cl_cvi_0_clocked_video_vid_f                      (VIP_FID_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_v_sync                 (VIP_VSYNC_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_h_sync                 (VIP_HSYNC_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_color_encoding         (0),
    .alt_vip_cl_cvi_0_clocked_video_vid_bit_width              (0),
    .alt_vip_cl_cvi_0_clocked_video_sof                        (),
    .alt_vip_cl_cvi_0_clocked_video_sof_locked                 (),
    .alt_vip_cl_cvi_0_clocked_video_refclk_div                 (),
    .alt_vip_cl_cvi_0_clocked_video_clipping                   (),
    .alt_vip_cl_cvi_0_clocked_video_padding                    (),
    .alt_vip_cl_cvi_0_clocked_video_overflow                   (cvi_overflow),
    .alt_vip_cl_cvo_0_clocked_video_vid_clk                    (
`ifdef PIXPAR2
    pclk_out_div2
`else
    pclk_out
`endif
    ),
    .alt_vip_cl_cvo_0_clocked_video_vid_data                   (VIP_DATA_o),
    .alt_vip_cl_cvo_0_clocked_video_underflow                  (cvo_underflow),
    .alt_vip_cl_cvo_0_clocked_video_vid_mode_change            (),
    .alt_vip_cl_cvo_0_clocked_video_vid_std                    (),
    .alt_vip_cl_cvo_0_clocked_video_vid_datavalid              (VIP_DE_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_v_sync                 (VIP_VSYNC_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_h_sync                 (VIP_HSYNC_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_f                      (),
    .alt_vip_cl_cvo_0_clocked_video_vid_h                      (),
    .alt_vip_cl_cvo_0_clocked_video_vid_v                      ()
);

scanconverter scanconverter_inst (
    .PCLK_CAP_i(pclk_capture),
    .PCLK_OUT_i(SI_PCLK_i),
    .reset_n(sys_reset_n),  //TODO: sync to pclk_capture
    .R_i(R_capt),
    .G_i(G_capt),
    .B_i(B_capt),
    .HSYNC_i(HSYNC_capt),
    .VSYNC_i(VSYNC_capt),
    .DE_i(DE_capt),
    .FID_i(FID_capt),
    .interlaced_in_i(interlace_flag_capt),
    .frame_change_i(frame_change_capt),
    .xpos_i(xpos_capt),
    .ypos_i(ypos_capt),
    .hv_out_config(hv_out_config),
    .hv_out_config2(hv_out_config2),
    .hv_out_config3(hv_out_config3),
    .xy_out_config(xy_out_config),
    .xy_out_config2(xy_out_config2),
    .misc_config(misc_config),
    .sl_config(sl_config),
    .sl_config2(sl_config2),
    .testpattern_enable(testpattern_enable),
    .ext_sync_mode(vip_select),
    .ext_frame_change_i(vip_frame_start),
    .ext_R_i(R_vip),
    .ext_G_i(G_vip),
    .ext_B_i(B_vip),
    .PCLK_o(PCLK_sc),
    .R_o(R_sc),
    .G_o(G_sc),
    .B_o(B_sc),
    .HSYNC_o(HSYNC_sc),
    .VSYNC_o(VSYNC_sc),
    .DE_o(DE_sc),
    .xpos_o(xpos_sc),
    .ypos_o(ypos_sc),
    .resync_strobe(resync_strobe_i)
);

ir_rcv ir0 (
    .clk27          (CLK27_i),
    .reset_n        (po_reset_n),
    .ir_rx          (ir_rx_sync2_reg),
    .ir_code        (ir_code),
    .ir_code_ack    (),
    .ir_code_cnt    (ir_code_cnt)
);

endmodule
