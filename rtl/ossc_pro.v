//
// Copyright (C) 2019-2024  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
`define PCB_1P5
`define EXTRA_AV_OUT
`define LEGACY_AV_IN

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
`ifndef PCB_1P5
    output ISL_COAST_o,
    output ISL_CLAMP_o,
`endif

    input HDMIRX_PCLK_i,
    input HDMIRX_AP_i,
    input [7:0] HDMIRX_R_i,
    input [7:0] HDMIRX_G_i,
    input [7:0] HDMIRX_B_i,
    input HDMIRX_HSYNC_i,
    input HDMIRX_VSYNC_i,
    input HDMIRX_DE_i,
    input HDMIRX_I2S_BCK_i,
    input HDMIRX_I2S_WS_i,
    output HDMIRX_RESET_N_o,

    input HDMI_INT_N_i,

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
    output FAN_PWM_o,

    output SD_CLK_o,
    inout SD_CMD_io,
    inout [3:0] SD_DATA_io,
    input SD_DETECT_i,

`ifdef PCB_1P5
    inout USB_DP_io,
    inout USB_DN_io,
`endif

    inout [5:0] EXT_IO_A_io,

    inout [31:0] EXT_IO_B_io,
    output [1:0] LS_DIR_o
);

localparam EXP_SEL_OFF = 0;
localparam EXP_SEL_EXTRA_OUT = 1;
localparam EXP_SEL_LEGAGY_IN = 2;
localparam EXP_SEL_UVC_BDG = 3;

localparam EXTRA_OUT_RGBHV = 0;
localparam EXTRA_OUT_RGBCS_RGBS = 1;
localparam EXTRA_OUT_RGsB = 2;
localparam EXTRA_OUT_YPbPr = 3;

wire jtagm_reset_req, ndmreset_req;
reg ndmreset_ack, ndmreset_pulse;

wire [31:0] sys_ctrl;
wire sys_poweron = sys_ctrl[0];
wire isl_reset_n = sys_ctrl[1];
wire hdmirx_reset_n = sys_ctrl[2];
wire emif_hwreset_n = sys_ctrl[3];
wire emif_swreset_n = sys_ctrl[4];
wire emif_powerdn_req = sys_ctrl[5];
wire emif_mpfe_reset_n = sys_ctrl[6];
wire [1:0] capture_sel = sys_ctrl[8:7];
wire isl_hsync_pol = sys_ctrl[9];
wire isl_vsync_pol = sys_ctrl[10];
wire isl_vsync_type = sys_ctrl[11];
wire testpattern_enable = sys_ctrl[12];
wire csc_enable = sys_ctrl[13];
wire framelock = sys_ctrl[14];
wire hdmirx_aud_sel = sys_ctrl[15];
wire [3:0] fan_duty = sys_ctrl[19:16];
wire [3:0] led_duty = sys_ctrl[23:20];
wire dram_refresh_enable = sys_ctrl[24];
wire vip_dil_reset_n = sys_ctrl[25];
wire [1:0] extra_out_mode = sys_ctrl[27:26];
wire [1:0] exp_sel = sys_ctrl[29:28];
wire audmux_sel = sys_ctrl[30];
wire legacy_aud_sel = sys_ctrl[31];

reg ir_rx_sync1_reg, ir_rx_sync2_reg;
reg [5:0] btn_sync1_reg, btn_sync2_reg;

wire [15:0] ir_code;
wire [7:0] ir_code_cnt;

wire pclk_capture, pclk_out;
reg pclk_capture_div2, pclk_out_div2;

reg [15:0] po_reset_ctr = 0;
reg po_reset_n = 1'b0;

wire pll_locked, emif_pll_locked;

`ifdef SIMULATION
wire sys_reset_n = (po_reset_n & pll_locked);
`else
wire sys_reset_n = (po_reset_n & ~jtagm_reset_req & ~ndmreset_pulse & pll_locked);
`endif

wire emif_status_init_done, emif_status_cal_success, emif_status_cal_fail, emif_status_powerdn_ack;
wire dram_refresh_req, dram_refresh_ack;

/* EMIF IF for LM */
wire emif_br_clk, emif_br_reset;
wire [27:0] emif_rd_addr, emif_wr_addr;
wire [255:0] emif_rd_rdata, emif_wr_wdata;
wire [5:0] emif_rd_burstcount, emif_wr_burstcount;
wire emif_rd_read, emif_rd_waitrequest, emif_rd_readdatavalid, emif_wr_write, emif_wr_waitrequest;

wire sd_detect = ~SD_DETECT_i;

wire cvi_overflow, cvo_underflow, cvo_sof;

reg [2:0] cvo_resync_ctr;
wire cvo_resync = (cvo_resync_ctr == '1);

wire [31:0] controls = {2'h0, btn_sync2_reg, ir_code_cnt, ir_code}; // TODO: sync to CPU clock
wire [31:0] sys_status = {cvi_overflow, cvo_underflow, cvo_resync, 23'h0, sd_detect, emif_pll_locked, emif_status_powerdn_ack, emif_status_cal_fail, emif_status_cal_success, emif_status_init_done};

wire [31:0] hv_in_config, hv_in_config2, hv_in_config3, hv_out_config, hv_out_config2, hv_out_config3, xy_out_config, xy_out_config2, xy_out_config3;
wire [31:0] misc_config, misc_config2, sl_config, sl_config2, sl_config3, sl_config4;

reg [23:0] resync_led_ctr;
reg resync_strobe_sync1_reg, resync_strobe_sync2_reg, resync_strobe_prev;
wire resync_strobe_i;
wire resync_strobe = resync_strobe_sync2_reg;

//BGR
wire led_pwm;
assign LED_o = {3{led_pwm}} & (sys_poweron ? {framelock, (ir_code == 0), (resync_led_ctr != 0)} : {2'b00, ~resync_led_ctr[23]});
//assign LED_o = {emif_status_init_done, emif_status_cal_success, emif_status_cal_fail};

//Fan
wire fan_pwm;
assign FAN_PWM_o = ~(sys_poweron & fan_pwm);

wire [11:0] xpos_sc;
wire [11:0] ypos_sc;
wire osd_enable;
wire [2:0] osd_color;
wire [3:0] x_ctr_shmask, y_ctr_shmask;
wire [10:0] shmask_data;

assign ISL_RESET_N_o = isl_reset_n;
assign HDMIRX_RESET_N_o = hdmirx_reset_n;

assign HDMITX_5V_EN_o = sys_poweron;

wire sd_cmd_oe_o, sd_cmd_out_o, sd_dat_oe_o;
wire [3:0] sd_dat_out_o;

assign SD_CMD_io = sd_cmd_oe_o ? sd_cmd_out_o : 1'bz;
assign SD_DATA_io = sd_dat_oe_o ? sd_dat_out_o : 4'bzzzz;

assign FPGA_PCLK1x_o = pclk_capture;

wire [31:0] lumacode_data;
wire [8:0] lumacode_addr;
wire lumacode_rden;

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
wire ISL_HSYNC_post, ISL_VSYNC_post, ISL_DE_post, ISL_FID_post, ISL_datavalid_post;
wire ISL_fe_interlace, ISL_fe_frame_change, ISL_sof_scaler;
wire [19:0] ISL_fe_pcnt_field;
wire [10:0] ISL_fe_vtotal, ISL_fe_xpos, ISL_fe_ypos;
isl51002_frontend u_isl_frontend ( 
    .PCLK_i(ISL_PCLK_i),
    .CLK_MEAS_i(CLK27_i),
    .reset_n(1'b1),  //not used atm
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
    .misc_config(misc_config),
    .misc_config2(misc_config2),
    .lumacode_data(lumacode_data),
    .R_o(ISL_R_post),
    .G_o(ISL_G_post),
    .B_o(ISL_B_post),
    .HSYNC_o(ISL_HSYNC_post),
    .VSYNC_o(ISL_VSYNC_post),
    .DE_o(ISL_DE_post),
    .FID_o(ISL_FID_post),
    .interlace_flag(ISL_fe_interlace),
    .datavalid_o(ISL_datavalid_post),
    .xpos_o(ISL_fe_xpos),
    .ypos_o(ISL_fe_ypos),
    .vtotal(ISL_fe_vtotal),
    .frame_change(ISL_fe_frame_change),
    .sof_scaler(ISL_sof_scaler),
    .pcnt_field(ISL_fe_pcnt_field),
    .lumacode_addr(lumacode_addr),
    .lumacode_rden(lumacode_rden)
);

// ADV7611 HDMI RX
reg [7:0] HDMIRX_R, HDMIRX_G, HDMIRX_B /* synthesis ramstyle = "logic" */;
reg HDMIRX_HSYNC, HDMIRX_VSYNC, HDMIRX_DE;
reg [7:0] HDMIRX_R_iq, HDMIRX_G_iq, HDMIRX_B_iq, HDMIRX_R_iqq, HDMIRX_G_iqq, HDMIRX_B_iqq /* synthesis ramstyle = "logic" */;
reg HDMIRX_HSYNC_iq, HDMIRX_VSYNC_iq, HDMIRX_DE_iq, HDMIRX_HSYNC_iqq, HDMIRX_VSYNC_iqq, HDMIRX_DE_iqq;
always @(posedge HDMIRX_PCLK_i) begin
    HDMIRX_R_iq <= HDMIRX_R_i;
    HDMIRX_G_iq <= HDMIRX_G_i;
    HDMIRX_B_iq <= HDMIRX_B_i;
    HDMIRX_HSYNC_iq <= HDMIRX_HSYNC_i;
    HDMIRX_VSYNC_iq <= HDMIRX_VSYNC_i;
    HDMIRX_DE_iq <= HDMIRX_DE_i;

    HDMIRX_R_iqq <= HDMIRX_R_iq;
    HDMIRX_G_iqq <= HDMIRX_G_iq;
    HDMIRX_B_iqq <= HDMIRX_B_iq;
    HDMIRX_HSYNC_iqq <= HDMIRX_HSYNC_iq;
    HDMIRX_VSYNC_iqq <= HDMIRX_VSYNC_iq;
    HDMIRX_DE_iqq <= HDMIRX_DE_iq;

    HDMIRX_R <= HDMIRX_R_iqq;
    HDMIRX_G <= HDMIRX_G_iqq;
    HDMIRX_B <= HDMIRX_B_iqq;
    HDMIRX_HSYNC <= HDMIRX_HSYNC_iqq;
    HDMIRX_VSYNC <= HDMIRX_VSYNC_iqq;
    HDMIRX_DE <= HDMIRX_DE_iqq;
end

wire [7:0] HDMIRX_R_post, HDMIRX_G_post, HDMIRX_B_post;
wire HDMIRX_HSYNC_post, HDMIRX_VSYNC_post, HDMIRX_DE_post, HDMIRX_FID_post, HDMIRX_datavalid_post;
wire HDMIRX_fe_interlace, HDMIRX_fe_frame_change, HDMIRX_sof_scaler;
wire [10:0] HDMIRX_fe_xpos, HDMIRX_fe_ypos;
adv7611_frontend u_hdmirx_frontend ( 
    .PCLK_i(HDMIRX_PCLK_i),
    .reset_n(1'b1),  //not used atm
    .R_i(HDMIRX_R),
    .G_i(HDMIRX_G),
    .B_i(HDMIRX_B),
    .HSYNC_i(HDMIRX_HSYNC),
    .VSYNC_i(HDMIRX_VSYNC),
    .DE_i(HDMIRX_DE),
    .hv_in_config(hv_in_config),
    .hv_in_config2(hv_in_config2),
    .hv_in_config3(hv_in_config3),
    .sync_passthru(1'b0),
    .R_o(HDMIRX_R_post),
    .G_o(HDMIRX_G_post),
    .B_o(HDMIRX_B_post),
    .HSYNC_o(HDMIRX_HSYNC_post),
    .VSYNC_o(HDMIRX_VSYNC_post),
    .DE_o(HDMIRX_DE_post),
    .FID_o(HDMIRX_FID_post),
    .interlace_flag(HDMIRX_fe_interlace),
    .datavalid_o(HDMIRX_datavalid_post),
    .xpos_o(HDMIRX_fe_xpos),
    .ypos_o(HDMIRX_fe_ypos),
    .frame_change(HDMIRX_fe_frame_change),
    .sof_scaler(HDMIRX_sof_scaler)
);

// ADV7280A SDP
`ifdef LEGACY_AV_IN
wire SDP_PCLK_i = EXT_IO_A_io[0];
wire SDP_PCLK;
wire [7:0] SDP_P_DATA_i = {EXT_IO_B_io[5], EXT_IO_B_io[4], EXT_IO_B_io[7], EXT_IO_B_io[6], EXT_IO_B_io[9], EXT_IO_B_io[8], EXT_IO_B_io[11], EXT_IO_B_io[10]};
wire SDP_HS_i = EXT_IO_B_io[3];
wire SDP_VS_i = EXT_IO_B_io[0];
wire SDP_IRQ_i = EXT_IO_B_io[1];
wire RF_AADC_BCK_i = EXT_IO_B_io[12];
wire RF_AADC_WS_i = EXT_IO_B_io[13];
wire RF_AADC_DATA_i = EXT_IO_B_io[14];

reg SDP_HS, SDP_VS;
reg [7:0] SDP_P_DATA;

always @(posedge SDP_PCLK_i) begin
    SDP_P_DATA <= SDP_P_DATA_i;
    SDP_HS <= SDP_HS_i;
    SDP_VS <= SDP_VS_i;
end

wire [7:0] SDP_R_post, SDP_G_post, SDP_B_post;
wire SDP_HSYNC_post, SDP_VSYNC_post, SDP_DE_post, SDP_FID_post, SDP_datavalid_post;
wire SDP_fe_interlace, SDP_fe_frame_change, SDP_sof_scaler;
wire [19:0] SDP_fe_pcnt_field;
wire [10:0] SDP_fe_vtotal, SDP_fe_xpos, SDP_fe_ypos;
adv7280a_frontend u_sdp_frontend ( 
    .PCLK_i(SDP_PCLK_i),
    .CLK_MEAS_i(CLK27_i),
    .reset_n(1'b1),  //not used atm
    .P_DATA_i(SDP_P_DATA),
    .HS_i(SDP_HS),
    .VS_i(SDP_VS),
    .hv_in_config(hv_in_config),
    .hv_in_config2(hv_in_config2),
    .hv_in_config3(hv_in_config3),
    .R_o(SDP_R_post),
    .G_o(SDP_G_post),
    .B_o(SDP_B_post),
    .HSYNC_o(SDP_HSYNC_post),
    .VSYNC_o(SDP_VSYNC_post),
    .DE_o(SDP_DE_post),
    .FID_o(SDP_FID_post),
    .interlace_flag(SDP_fe_interlace),
    .datavalid_o(SDP_datavalid_post),
    .xpos_o(SDP_fe_xpos),
    .ypos_o(SDP_fe_ypos),
    .vtotal(SDP_fe_vtotal),
    .frame_change(SDP_fe_frame_change),
    .sof_scaler(SDP_sof_scaler),
    .pcnt_field(SDP_fe_pcnt_field)
);
`endif

pll_sdp u_pll_sdp (
    .refclk(SDP_PCLK_i),
    .rst(!capture_sel[1]),
    .outclk_0(SDP_PCLK),
    .locked()
);

// capture clock mux (inputs [3:2] must be PLL outputs)
cyclonev_clkselect clkmux_capture ( 
    .clkselect(capture_sel),
    .inclk({1'b0, SDP_PCLK, HDMIRX_PCLK_i, ISL_PCLK_i}),
    .outclk(pclk_capture)
);

// capture data mux
reg [7:0] R_capt, G_capt, B_capt;
reg HSYNC_capt, VSYNC_capt, DE_capt, FID_capt, datavalid_capt;
reg interlace_flag_capt, frame_change_capt, sof_scaler_capt;
reg [10:0] xpos_capt, ypos_capt;
wire [31:0] fe_status = capture_sel[1] ? {SDP_fe_pcnt_field, SDP_fe_interlace, SDP_fe_vtotal} : {ISL_fe_pcnt_field, ISL_fe_interlace, ISL_fe_vtotal};
always @(posedge pclk_capture) begin
    R_capt <= capture_sel[1] ? SDP_R_post : (capture_sel[0] ? HDMIRX_R_post : ISL_R_post);
    G_capt <= capture_sel[1] ? SDP_G_post : (capture_sel[0] ? HDMIRX_G_post : ISL_G_post);
    B_capt <= capture_sel[1] ? SDP_B_post : (capture_sel[0] ? HDMIRX_B_post : ISL_B_post);
    HSYNC_capt <= capture_sel[1] ? SDP_HSYNC_post : (capture_sel[0] ? HDMIRX_HSYNC_post : ISL_HSYNC_post);
    VSYNC_capt <= capture_sel[1] ? SDP_VSYNC_post : (capture_sel[0] ? HDMIRX_VSYNC_post : ISL_VSYNC_post);
    DE_capt <= capture_sel[1] ? SDP_DE_post : (capture_sel[0] ? HDMIRX_DE_post : ISL_DE_post);
    datavalid_capt <= capture_sel[1] ? SDP_datavalid_post : (capture_sel[0] ? HDMIRX_datavalid_post : ISL_datavalid_post);
    FID_capt <= capture_sel[1] ? SDP_FID_post : (capture_sel[0] ? HDMIRX_FID_post : ISL_FID_post);
    interlace_flag_capt <= capture_sel[1] ? SDP_fe_interlace : (capture_sel[0] ? HDMIRX_fe_interlace : ISL_fe_interlace);
    frame_change_capt <= capture_sel[1] ? SDP_fe_frame_change : (capture_sel[0] ? HDMIRX_fe_frame_change : ISL_fe_frame_change);
    sof_scaler_capt <= capture_sel[1] ? SDP_sof_scaler : (capture_sel[0] ? HDMIRX_sof_scaler : ISL_sof_scaler);
    xpos_capt <= capture_sel[1] ? SDP_fe_xpos : (capture_sel[0] ? HDMIRX_fe_xpos : ISL_fe_xpos);
    ypos_capt <= capture_sel[1] ? SDP_fe_ypos : (capture_sel[0] ? HDMIRX_fe_ypos : ISL_fe_ypos);
end

// output clock assignment
wire PCLK_sc;
assign pclk_out = PCLK_sc;
`ifdef PCB_1P3
assign HDMITX_PCLK_o = pclk_out;
`else
assign HDMITX_PCLK_o = ~pclk_out;
`endif


// VIP / LB
wire vip_select = misc_config2[3];
wire hdmi_csync = misc_config2[4];
wire [1:0] csync_combiner = misc_config2[6:5];
wire lb_enable = sys_poweron & ~testpattern_enable & ~vip_select;

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
        {VIP_HSYNC_i[1], VIP_VSYNC_i[1], VIP_DE_i[1], VIP_FID_i[1]} <= {~HSYNC_capt, ~VSYNC_capt, DE_capt & datavalid_capt, ~FID_capt};
    end else begin
        VIP_DATA_i[23:0] <= {R_capt, G_capt, B_capt};
        {VIP_HSYNC_i[0], VIP_VSYNC_i[0], VIP_DE_i[0], VIP_FID_i[0]} <= {~HSYNC_capt, ~VSYNC_capt, DE_capt & datavalid_capt, ~FID_capt};
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
wire dc_fifo_in_rdempty, dc_fifo_in_wrfull;
wire dc_fifo_out_rdempty, dc_fifo_out_wrfull;
reg dc_fifo_in_rdempty_prev;

dc_fifo_in  dc_fifo_in_inst (
    .data({R_capt, G_capt, B_capt, ~HSYNC_capt, ~VSYNC_capt, DE_capt, ~FID_capt, 4'h0}),
    .rdclk(pclk_capture_div2),
    .rdreq(!dc_fifo_in_rdempty),
    .rdempty(dc_fifo_in_rdempty),
    .wrclk(pclk_capture),
    .wrreq(datavalid_capt & !dc_fifo_in_wrfull),
    .wrfull(dc_fifo_in_wrfull),
    .q({VIP_DATA_i[47:24], VIP_HSYNC_i[1], VIP_VSYNC_i[1], VIP_DE_i[1], VIP_FID_i[1], unused1, VIP_DATA_i[23:0], VIP_HSYNC_i[0], VIP_VSYNC_i[0], VIP_DE_i[0], VIP_FID_i[0], unused0})
);

dc_fifo_out  dc_fifo_out_inst (
    .data({VIP_DATA_o[47:24], VIP_HSYNC_o[1], VIP_VSYNC_o[1], VIP_DE_o[1], 5'h0, VIP_DATA_o[23:0], VIP_HSYNC_o[0], VIP_VSYNC_o[0], VIP_DE_o[0], 5'h0}),
    .rdclk(pclk_out),
    .rdreq(!dc_fifo_out_rdempty),
    .rdempty(dc_fifo_out_rdempty),
    .wrclk(pclk_out_div2),
    .wrreq(!dc_fifo_out_wrfull),
    .wrfull(dc_fifo_out_wrfull),
    .q({R_vip, G_vip, B_vip, HSYNC_vip, VSYNC_vip, DE_vip, unused_out})
);

always @(posedge pclk_capture_div2) begin
    dc_fifo_in_rdempty_prev <= dc_fifo_in_rdempty;
end
`endif // DIV2_SYNC
`else // PIXPAR2
wire [23:0] VIP_DATA_i = {R_capt, G_capt, B_capt};
wire VIP_HSYNC_i = ~HSYNC_capt;
wire VIP_VSYNC_i = ~VSYNC_capt;
wire VIP_DE_i = DE_capt & datavalid_capt;
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

reg VSYNC_vip_prev, cvo_sof_prev;
wire vip_frame_start = VSYNC_vip_prev & ~VSYNC_vip & ~HSYNC_vip;

always @(posedge pclk_out) begin
    VSYNC_vip_prev <= VSYNC_vip;
end

always @(posedge pclk_out_div2) begin
    if (cvo_sof_prev & ~cvo_sof) begin
        if (sof_scaler_capt)
            cvo_resync_ctr <= '0;
        else if (~sof_scaler_capt & (cvo_resync_ctr != '1))
            cvo_resync_ctr <= cvo_resync_ctr + 1'b1;
    end

    cvo_sof_prev <= cvo_sof;
end
`endif // VIP

// output data assignment (2 stages and launch on negedge for timing closure)
reg [7:0] R_out, G_out, B_out;
reg HSYNC_out, VSYNC_out, DE_out;
wire [7:0] R_sc, G_sc, B_sc;
wire HSYNC_sc, VSYNC_sc, DE_sc;
wire CSYNC_out = csync_combiner == 0 ? HSYNC_out & VSYNC_out : ~(HSYNC_out ^ VSYNC_out);

always @(posedge pclk_out) begin
    if (osd_enable) begin
        {R_out, G_out, B_out} <= {{8{osd_color[2]}}, {8{osd_color[1]}}, {8{osd_color[0]}}};
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
    HDMITX_HSYNC_o <= hdmi_csync ? CSYNC_out : HSYNC_out;
    HDMITX_VSYNC_o <= VSYNC_out;
    HDMITX_DE_o <= DE_out;
end

//audio
assign HDMITX_I2S_BCK_o = hdmirx_aud_sel ? HDMIRX_I2S_BCK_i : (legacy_aud_sel ? RF_AADC_BCK_i : PCM_I2S_BCK_i);
assign HDMITX_I2S_WS_o = hdmirx_aud_sel ? HDMIRX_I2S_WS_i : (legacy_aud_sel ? RF_AADC_WS_i : PCM_I2S_WS_i);
assign HDMITX_I2S_DATA_o = hdmirx_aud_sel ? HDMIRX_AP_i : (legacy_aud_sel ? RF_AADC_DATA_i : PCM_I2S_DATA_i);
assign HDMITX_SPDIF_o = sys_poweron ? (hdmirx_aud_sel ? HDMIRX_AP_i : SPDIF_EXT_i) : 1'b0;

assign AUDMUX_o = ~audmux_sel;

// 0=input 1=output
assign LS_DIR_o = (exp_sel == EXP_SEL_LEGAGY_IN) ? 2'b10 : 2'b11;

`ifdef EXTRA_AV_OUT
// CSC for YPbPr
wire [7:0] VGA_CSC_R_out, VGA_CSC_G_out, VGA_CSC_B_out;
wire VGA_CSC_HSYNC_out, VGA_CSC_VSYNC_out, VGA_CSC_DE_out;
output_csc csc_vga_inst (
    .PCLK_i(pclk_out),
    .reset_n(1'b1),
    .enable((exp_sel == EXP_SEL_EXTRA_OUT) & (extra_out_mode == EXTRA_OUT_YPbPr)),
    .R_i(R_out),
    .G_i(G_out),
    .B_i(B_out),
    .HSYNC_i(HSYNC_out),
    .VSYNC_i(VSYNC_out),
    .DE_i(DE_out),
    .R_o(VGA_CSC_R_out),
    .G_o(VGA_CSC_G_out),
    .B_o(VGA_CSC_B_out),
    .HSYNC_o(VGA_CSC_HSYNC_out),
    .VSYNC_o(VGA_CSC_VSYNC_out),
    .DE_o(VGA_CSC_DE_out),
);

// VGA DAC
reg [7:0] VGA_R, VGA_G, VGA_B, VGA_R_pre, VGA_G_pre, VGA_B_pre;
reg VGA_HS, VGA_VS, VGA_SYNC_N, VGA_BLANK_N, VGA_HS_pre, VGA_VS_pre, VGA_SYNC_N_pre, VGA_BLANK_N_pre;
wire VGA_CSC_CSYNC_out = csync_combiner == 0 ? VGA_CSC_HSYNC_out & VGA_CSC_VSYNC_out : ~(VGA_CSC_HSYNC_out ^ VGA_CSC_VSYNC_out);
always @(posedge pclk_out) begin
    if (exp_sel == EXP_SEL_EXTRA_OUT) begin
        VGA_R_pre <= VGA_CSC_R_out;
        VGA_G_pre <= VGA_CSC_G_out;
        VGA_B_pre <= VGA_CSC_B_out;
        VGA_HS_pre <= (extra_out_mode == EXTRA_OUT_RGBHV) ? VGA_CSC_HSYNC_out : VGA_CSC_CSYNC_out;
        VGA_VS_pre <= (extra_out_mode == EXTRA_OUT_RGBHV) ? VGA_CSC_VSYNC_out : 1'b1;
        VGA_BLANK_N_pre <= (extra_out_mode == EXTRA_OUT_YPbPr) ? 1'b1 : VGA_CSC_DE_out;
        VGA_SYNC_N_pre <= (extra_out_mode >= EXTRA_OUT_RGsB) ? VGA_CSC_CSYNC_out : 1'b0;

        VGA_R <= VGA_R_pre;
        VGA_G <= VGA_G_pre;
        VGA_B <= VGA_B_pre;
        VGA_HS <= VGA_HS_pre;
        VGA_VS <= VGA_VS_pre;
        VGA_BLANK_N <= VGA_BLANK_N_pre;
        VGA_SYNC_N <= VGA_SYNC_N_pre;
    end
end
assign EXT_IO_A_io[4] = sys_poweron; // VGA_PSAVE_N
assign EXT_IO_A_io[5] = sys_poweron; // AUDIO_MUTE_N
assign EXT_IO_B_io[27] = (exp_sel == EXP_SEL_EXTRA_OUT) ? ~pclk_out : 'z;
assign {EXT_IO_B_io[6], EXT_IO_B_io[7], EXT_IO_B_io[4], EXT_IO_B_io[5], EXT_IO_B_io[2], EXT_IO_B_io[3], EXT_IO_B_io[0], EXT_IO_B_io[1]} = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_R : 'z;
assign {EXT_IO_B_io[14], EXT_IO_B_io[15], EXT_IO_B_io[12], EXT_IO_B_io[13], EXT_IO_B_io[10], EXT_IO_B_io[11], EXT_IO_B_io[8], EXT_IO_B_io[9]} = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_G : 'z;
assign {EXT_IO_B_io[24], EXT_IO_B_io[25], EXT_IO_B_io[22], EXT_IO_B_io[23], EXT_IO_B_io[20], EXT_IO_B_io[21], EXT_IO_B_io[18], EXT_IO_B_io[19]} = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_B : 'z;
assign EXT_IO_B_io[29] = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_HS : 'z;
assign EXT_IO_B_io[30] = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_VS : 'z;
assign EXT_IO_B_io[17] = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_BLANK_N : 'z;
assign EXT_IO_B_io[16] = (exp_sel == EXP_SEL_EXTRA_OUT) ? VGA_SYNC_N : 'z;
`else
assign EXT_IO_A_io = {6{&btn_sync2_reg}};
assign EXT_IO_B_io = {32{&btn_sync2_reg}};
`endif

// Power-on reset pulse generation (seems to be needed for booting from flash)
always @(posedge CLK27_i)
begin
    if (po_reset_ctr == `PO_RESET_WIDTH)
        po_reset_n <= 1'b1;
    else
        po_reset_ctr <= po_reset_ctr + 1'b1;
end

// ndmreset pulse & ack for RISC-V DM (TODO: sync to CPU clock)
always @(posedge CLK27_i)
begin
    ndmreset_pulse <= !ndmreset_ack & ndmreset_req;
    ndmreset_ack <= ndmreset_req;
end

always @(posedge CLK27_i) begin
    if (sys_poweron) begin
        if (~resync_strobe_prev & resync_strobe) begin
            resync_led_ctr <= {24{1'b1}};
        end else if (resync_led_ctr > 0) begin
            resync_led_ctr <= resync_led_ctr - 1'b1;
        end
    end else begin
        if (pll_locked) begin
            resync_led_ctr <= {24{1'b0}};
        end else begin
            resync_led_ctr <= resync_led_ctr - 1'b1;
        end
    end

    resync_strobe_sync1_reg <= resync_strobe_i;
    resync_strobe_sync2_reg <= resync_strobe_sync1_reg;
    resync_strobe_prev <= resync_strobe_sync2_reg;
end

// Insert synchronizers to async inputs (synchronize to 27MHz clock)
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


// Qsys system
sys sys_inst (
    .clk_clk                                (CLK27_i),
    .reset_po_reset_n                       (po_reset_n),
    .reset_sys_reset_n                      (sys_reset_n),
    .pll_0_locked_export                    (pll_locked),
    .ibex_0_ndm_ndmreset_o                  (ndmreset_req),
    .ibex_0_ndm_ndmreset_ack_i              (ndmreset_ack),
    .ibex_0_config_boot_addr_i              (32'h022d0000),
    .ibex_0_config_core_sleep_o             (),
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
    .sc_config_0_sc_if_fe_status_i          (fe_status),
    .sc_config_0_sc_if_hv_in_config_o       (hv_in_config),
    .sc_config_0_sc_if_hv_in_config2_o      (hv_in_config2),
    .sc_config_0_sc_if_hv_in_config3_o      (hv_in_config3),
    .sc_config_0_sc_if_hv_out_config_o      (hv_out_config),
    .sc_config_0_sc_if_hv_out_config2_o     (hv_out_config2),
    .sc_config_0_sc_if_hv_out_config3_o     (hv_out_config3),
    .sc_config_0_sc_if_xy_out_config_o      (xy_out_config),
    .sc_config_0_sc_if_xy_out_config2_o     (xy_out_config2),
    .sc_config_0_sc_if_xy_out_config3_o     (xy_out_config3),
    .sc_config_0_sc_if_misc_config_o        (misc_config),
    .sc_config_0_sc_if_misc_config2_o       (misc_config2),
    .sc_config_0_sc_if_sl_config_o          (sl_config),
    .sc_config_0_sc_if_sl_config2_o         (sl_config2),
    .sc_config_0_sc_if_sl_config3_o         (sl_config3),
    .sc_config_0_sc_if_sl_config4_o         (sl_config4),
    .sc_config_0_shmask_if_vclk             (PCLK_sc),
    .sc_config_0_shmask_if_shmask_xpos      (x_ctr_shmask),
    .sc_config_0_shmask_if_shmask_ypos      (y_ctr_shmask),
    .sc_config_0_shmask_if_shmask_data      (shmask_data),
    .sc_config_0_lc_ram_if_lumacode_clk_i   (ISL_PCLK_i),
    .sc_config_0_lc_ram_if_lumacode_addr_i  (lumacode_addr),
    .sc_config_0_lc_ram_if_lumacode_rden_i  (lumacode_rden),
    .sc_config_0_lc_ram_if_lumacode_data_o  (lumacode_data),
    .osd_generator_0_osd_if_vclk            (PCLK_sc),
    .osd_generator_0_osd_if_xpos            (xpos_sc),
    .osd_generator_0_osd_if_ypos            (ypos_sc),
    .osd_generator_0_osd_if_osd_enable      (osd_enable),
    .osd_generator_0_osd_if_osd_color       (osd_color),
    .core_usb_0_usb_usb_dp_io               (USB_DP_io),
    .core_usb_0_usb_usb_dn_io               (USB_DN_io), 
    .emif_bridge_0_clk_o                    (emif_br_clk),
    .emif_bridge_0_reset_o                  (emif_br_reset),
    .emif_bridge_0_wr_address               (emif_wr_addr),
    .emif_bridge_0_wr_write                 (emif_wr_write),
    .emif_bridge_0_wr_write_data            (emif_wr_wdata),
    .emif_bridge_0_wr_waitrequest           (emif_wr_waitrequest),
    .emif_bridge_0_wr_burstcount            (emif_wr_burstcount),
    .emif_bridge_0_rd_address               (emif_rd_addr),
    .emif_bridge_0_rd_read                  (emif_rd_read),
    .emif_bridge_0_rd_read_data             (emif_rd_rdata),
    .emif_bridge_0_rd_waitrequest           (emif_rd_waitrequest),
    .emif_bridge_0_rd_readdatavalid         (emif_rd_readdatavalid),
    .emif_bridge_0_rd_burstcount            (emif_rd_burstcount),
    .mem_if_lpddr2_emif_0_global_reset_reset_n     (emif_hwreset_n),
    .mem_if_lpddr2_emif_0_soft_reset_reset_n       (emif_swreset_n),
    .mem_if_lpddr2_emif_0_status_local_init_done   (emif_status_init_done),
    .mem_if_lpddr2_emif_0_status_local_cal_success (emif_status_cal_success),
    .mem_if_lpddr2_emif_0_status_local_cal_fail    (emif_status_cal_fail),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_req  (emif_powerdn_req),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_chip (1'b1),
    .mem_if_lpddr2_emif_0_deep_powerdn_local_deep_powerdn_ack  (emif_status_powerdn_ack),
    .mem_if_lpddr2_emif_0_pll_sharing_pll_locked               (emif_pll_locked),
    .mem_if_lpddr2_emif_0_mpfe_reset_reset_n       (emif_mpfe_reset_n),
    .mem_if_lpddr2_emif_0_user_refresh_local_refresh_req   (dram_refresh_req),
    .mem_if_lpddr2_emif_0_user_refresh_local_refresh_chip  (1'b1),
    .mem_if_lpddr2_emif_0_user_refresh_local_refresh_ack   (dram_refresh_ack),
    .memory_mem_ca                                 (DDR_CA_o),
    .memory_mem_ck                                 (DDR_CK_o_p),
    .memory_mem_ck_n                               (DDR_CK_o_n),
    .memory_mem_cke                                (DDR_CKE_o),
    .memory_mem_cs_n                               (DDR_CS_N_o),
    .memory_mem_dm                                 (DDR_DM_o),
    .memory_mem_dq                                 (DDR_DQ_io),
    .memory_mem_dqs                                (DDR_DQS_io_p),
    .memory_mem_dqs_n                              (DDR_DQS_io_n),
    .oct_rzqin                                     (DDR_RZQ_i)
`ifdef VIP
    ,
    .alt_vip_cl_cvi_0_clocked_video_vid_clk                    (
`ifdef PIXPAR2
    pclk_capture_div2
`else
    pclk_capture
`endif
    ),
    .vip_dil_reset_reset_n                                     (vip_dil_reset_n),
    .alt_vip_cl_cvi_0_clocked_video_vid_data                   (VIP_DATA_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_de                     (VIP_DE_i),
    .alt_vip_cl_cvi_0_clocked_video_vid_datavalid              (!dc_fifo_in_rdempty_prev),
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
    .alt_vip_cl_cvo_0_clocked_video_vid_vcoclk_div             (),
    .alt_vip_cl_cvo_0_clocked_video_vid_sof_locked             (),
    .alt_vip_cl_cvo_0_clocked_video_vid_sof                    (cvo_sof),
    .alt_vip_cl_cvo_0_clocked_video_vid_datavalid              (VIP_DE_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_v_sync                 (VIP_VSYNC_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_h_sync                 (VIP_HSYNC_o),
    .alt_vip_cl_cvo_0_clocked_video_vid_f                      (),
    .alt_vip_cl_cvo_0_clocked_video_vid_h                      (),
    .alt_vip_cl_cvo_0_clocked_video_vid_v                      (),
    .alt_vip_cl_cvo_0_genlock_sof_locked                       (1'b1),
    .alt_vip_cl_cvo_0_genlock_sof                              (sof_scaler_capt)
`endif
);

// These do not work in current Quartus version (24.1) and a patch file (scripts/qsys.patch) must be used after Qsys generation instead
defparam
    sys_inst.mm_interconnect_0.mm_clock_crossing_bridge_1_s0_agent_rsp_fifo.USE_MEMORY_BLOCKS = 1,
    sys_inst.mm_interconnect_0.mm_clock_crossing_bridge_2_s0_agent_rsp_fifo.USE_MEMORY_BLOCKS = 1,
    sys_inst.mm_interconnect_1.mem_if_lpddr2_emif_0_avl_0_agent_rsp_fifo.USE_MEMORY_BLOCKS = 1,
    sys_inst.master_0.fifo.FIFO_DEPTH = 1024;

scanconverter #(
    .EMIF_ENABLE(1),
    .NUM_LINE_BUFFERS(2048)
  ) scanconverter_inst (
    .PCLK_CAP_i(pclk_capture),
    .PCLK_OUT_i(SI_PCLK_i),
    .reset_n(1'b1),  //not used atm
    .R_i(R_capt),
    .G_i(G_capt),
    .B_i(B_capt),
    .HSYNC_i(HSYNC_capt),
    .VSYNC_i(VSYNC_capt),
    .DE_i(DE_capt),
    .FID_i(FID_capt),
    .datavalid_i(datavalid_capt),
    .interlaced_in_i(interlace_flag_capt),
    .frame_change_i(frame_change_capt),
    .xpos_i(xpos_capt),
    .ypos_i(ypos_capt),
    .h_in_active(hv_in_config[23:12]),
    .hv_out_config(hv_out_config),
    .hv_out_config2(hv_out_config2),
    .hv_out_config3(hv_out_config3),
    .xy_out_config(xy_out_config),
    .xy_out_config2(xy_out_config2),
    .xy_out_config3(xy_out_config3),
    .misc_config(misc_config),
    .sl_config(sl_config),
    .sl_config2(sl_config2),
    .sl_config3(sl_config3),
    .sl_config4(sl_config4),
    .testpattern_enable(testpattern_enable),
    .lb_enable(lb_enable),
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
    .x_ctr_shmask(x_ctr_shmask),
    .y_ctr_shmask(y_ctr_shmask),
    .shmask_data(shmask_data),
    .resync_strobe(resync_strobe_i),
    .emif_br_clk(emif_br_clk),
    .emif_br_reset(emif_br_reset),
    .emif_rd_addr(emif_rd_addr),
    .emif_rd_read(emif_rd_read),
    .emif_rd_rdata(emif_rd_rdata),
    .emif_rd_waitrequest(emif_rd_waitrequest),
    .emif_rd_readdatavalid(emif_rd_readdatavalid),
    .emif_rd_burstcount(emif_rd_burstcount),
    .emif_wr_addr(emif_wr_addr),
    .emif_wr_write(emif_wr_write),
    .emif_wr_wdata(emif_wr_wdata),
    .emif_wr_waitrequest(emif_wr_waitrequest),
    .emif_wr_burstcount(emif_wr_burstcount)
);

ir_rcv ir0 (
    .clk27          (CLK27_i),
    .reset_n        (po_reset_n),
    .ir_rx          (ir_rx_sync2_reg),
    .ir_code        (ir_code),
    .ir_code_ack    (),
    .ir_code_cnt    (ir_code_cnt)
);

pwm_2ch #(.PERIOD(1024)) pwm_inst (
    .clk            (CLK27_i),
    .reset_n        (po_reset_n),
    .ch1_duty       (fan_duty),
    .ch2_duty       (led_duty),
    .ch1_pwm        (fan_pwm),
    .ch2_pwm        (led_pwm)
);

dram_refresh_sched #(.REFRESH_INTERVAL(842)) dram_refresh_sched_inst (
    .clk            (emif_br_clk),
    .reset_n        (~emif_br_reset),
    .enable         (dram_refresh_enable),
    .refresh_ack    (dram_refresh_ack),
    .refresh_req    (dram_refresh_req)
);

endmodule
