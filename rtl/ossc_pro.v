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

`define PO_RESET_WIDTH 27
`define DISABLE_SDC_CONTROLLER

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
    input ISL_DE_i,
    input ISL_FID_i,
    input ISL_INT_N_i,
    output ISL_EXT_PCLK_o,
    output ISL_RESET_N_o,
    output ISL_COAST_o,
    output ISL_CLAMP_o,

    input HDMIRX_PCLK_i,
    input HDMIRX_MCLK_i,
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
    output HDMITX_MCLK_o,
    output HDMITX_I2S_BCK_o,
    output HDMITX_I2S_WS_o,
    output HDMITX_I2S_DATA_o,
    output HDMITX_SPDIF_o,
    output HDMITX_PD_o,
    output reg [7:0] HDMITX_R_o,
    output reg [7:0] HDMITX_G_o,
    output reg [7:0] HDMITX_B_o,
    output reg HDMITX_HSYNC_o,
    output reg HDMITX_VSYNC_o,
    output reg HDMITX_DE_o,

    /*input DDR_RZQ_i,
    output [9:0] DDR_CA_o,
    output DDR_CK_o,
    output DDR_CK_N_o,
    output DDR_CKE_o,
    output DDR_CS_N_o,
    output [2:0] DDR_DM_o,
    inout [23:0] DDR_DQ_io,
    inout [2:0] DDR_DQS_io,
    inout [2:0] DDR_DQS_N_io,*/

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
    input [1:0] BTN_i,
    output [2:0] LED_o,
    output AUDMUX_o,

    output SD_CLK_o,
    inout SD_CMD_io,
    inout [3:0] SD_DATA_io,

    inout [28:0] EXT_IO_io,
    output [1:0] LS_DIR_o,
    output LS_OE_N_o
);

//wire clk_osc;
wire jtagm_reset_req;

wire [15:0] sys_ctrl;
wire sys_extra = sys_ctrl[0];
wire isl_reset_n = sys_ctrl[1];
wire hdmirx_reset_n = sys_ctrl[2];
wire emif_hwreset_n = sys_ctrl[3];
wire emif_swreset_n = sys_ctrl[4];
wire capture_sel = sys_ctrl[5];
wire isl_vs_pol = sys_ctrl[6];
wire isl_vs_type = sys_ctrl[7];
wire audmux_sel = sys_ctrl[8];
wire testpattern_enable = sys_ctrl[9];

//reg [1:0] clk_osc_div = 2'h0;

reg ir_rx_sync1_reg, ir_rx_sync2_reg;
reg [1:0] btn_sync1_reg, btn_sync2_reg;

wire [15:0] ir_code;
wire [7:0] ir_code_cnt;

//wire clk25 = clk_osc_div[1];
wire clk27 = EXT_IO_io[27];
//wire pll_refclk = pll_refclk_buf;
wire pclk_capture, pclk_out;

reg [7:0] po_reset_ctr = 0;
reg po_reset_n = 1'b0;

`ifdef SIMULATION
wire sys_reset_n = po_reset_n;
`else
wire sys_reset_n = (po_reset_n & ~jtagm_reset_req);
`endif

wire emif_status_init_done;
wire emif_status_cal_success;
wire emif_status_cal_fail;

wire avl_waitrequest_n;
wire avl_beginbursttransfer;
wire [22:0] avl_address;
wire avl_readdatavalid;
wire [47:0] avl_readdata;
wire [47:0] avl_writedata;
wire [5:0] avl_byteenable;
wire avl_read;
wire avl_write;
wire [2:0] avl_burstcount;

wire [31:0] sys_status = {3'h0, emif_status_cal_fail, emif_status_cal_success, emif_status_init_done, btn_sync2_reg, ir_code_cnt, ir_code};

wire [31:0] h_in_config, h_in_config2, v_in_config, h_out_config, h_out_config2, v_out_config, v_out_config2, xy_out_config;

reg [23:0] resync_led_ctr;
reg resync_strobe_sync1_reg, resync_strobe_sync2_reg, resync_strobe_prev;
wire resync_strobe_i;
wire resync_strobe = resync_strobe_sync2_reg;

//BGR?
assign LED_o = {CLK27_i, (ir_code == 0), (resync_led_ctr != 0)};
//assign LED_o = {emif_status_init_done, emif_status_cal_success, emif_status_cal_fail};

assign ISL_RESET_N_o = isl_reset_n;
assign HDMIRX_RESET_N_o = hdmirx_reset_n;

reg emif_hwreset_n_sync1_reg, emif_hwreset_n_sync2_reg, emif_swreset_n_sync1_reg, emif_swreset_n_sync2_reg;

assign HDMITX_PD_o = 1'b1;

`ifndef DISABLE_SDC_CONTROLLER
wire sd_cmd_oe_o, sd_cmd_out_o, sd_dat_oe_o;
wire [3:0] sd_dat_out_o;

assign SD_CMD_io = sd_cmd_oe_o ? sd_cmd_out_o : 1'bz;
assign SD_DATA_io = sd_dat_oe_o ? sd_dat_out_o : 4'bzzzz;
`endif

assign FPGA_PCLK1x_o = pclk_capture;

// ISL51002 RGB digitizer
reg [7:0] ISL_R, ISL_G, ISL_B;
reg ISL_HS;
reg ISL_DE;
reg ISL_FID;
reg ISL_HSYNC_sync1_reg, ISL_HSYNC_sync2_reg;
reg ISL_VSYNC_sync1_reg, ISL_VSYNC_sync2_reg;
always @(posedge ISL_PCLK_i) begin
    ISL_R <= ISL_R_i;
    ISL_G <= ISL_G_i;
    ISL_B <= ISL_B_i;
    ISL_HS <= ISL_HS_i;
    ISL_DE <= ISL_DE_i;
    ISL_FID <= ISL_FID_i;

    // sync to pclk
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
    .CLK_MEAS_i(clk27),
    .reset_n(sys_reset_n),
    .R_i(ISL_R),
    .G_i(ISL_G),
    .B_i(ISL_B),
    .HS_i(ISL_HS),
    .HSYNC_i(ISL_HSYNC_sync2_reg),
    .VSYNC_i(ISL_VSYNC_sync2_reg),
    .DE_i(ISL_DE),
    .FID_i(ISL_FID),
    .vs_type(isl_vs_type),
    .vs_polarity(isl_vs_pol),
    .h_in_config(h_in_config),
    .h_in_config2(h_in_config2),
    .v_in_config(v_in_config),
    .R_o(ISL_R_post),
    .G_o(ISL_G_post),
    .B_o(ISL_B_post),
    .HSYNC_o(ISL_HSYNC_post),
    .VSYNC_o(ISL_VSYNC_post),
    .DE_o(ISL_DE_post),
    .FID_o(ISL_FID_post),
    .interlace_flag(ISL_fe_interlace),
    .xpos(ISL_fe_xpos),
    .ypos(ISL_fe_ypos),
    .vtotal(ISL_fe_vtotal),
    .frame_change(ISL_fe_frame_change),
    .pcnt_frame(ISL_fe_pcnt_frame)
);

// ADV7611 HDMI RX
reg [7:0] HDMIRX_R, HDMIRX_G, HDMIRX_B;
reg HDMIRX_HSYNC, HDMIRX_VSYNC, HDMIRX_DE;
always @(posedge HDMIRX_PCLK_i) begin
    HDMIRX_R <= HDMIRX_R_i;
    HDMIRX_G <= HDMIRX_G_i;
    HDMIRX_B <= HDMIRX_B_i;
    HDMIRX_HSYNC <= HDMIRX_HSYNC_i;
    HDMIRX_VSYNC <= HDMIRX_VSYNC_i;
    HDMIRX_DE <= HDMIRX_DE_i;
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
    .xpos(HDMIRX_fe_xpos),
    .ypos(HDMIRX_fe_ypos),
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
assign pclk_out = PCLK_sc;
assign HDMITX_PCLK_o = pclk_out;

// output data assignment
wire [7:0] R_sc, G_sc, B_sc;
wire HSYNC_sc, VSYNC_sc, DE_sc;
always @(posedge pclk_out) begin
    HDMITX_R_o <= R_sc;
    HDMITX_G_o <= G_sc;
    HDMITX_B_o <= B_sc;
    HDMITX_HSYNC_o <= HSYNC_sc;
    HDMITX_VSYNC_o <= VSYNC_sc;
    HDMITX_DE_o <= DE_sc;
end

//audio
assign HDMITX_I2S_BCK_o = capture_sel ? HDMIRX_I2S_BCK_i : PCM_I2S_BCK_i;
assign HDMITX_I2S_WS_o = capture_sel ? HDMIRX_I2S_WS_i : PCM_I2S_WS_i;
assign HDMITX_I2S_DATA_o = capture_sel ? HDMIRX_AP_i : PCM_I2S_DATA_i;
assign HDMITX_SPDIF_o = SPDIF_EXT_i;

assign AUDMUX_o = audmux_sel;

//assign EXT_IO_io = BTN_i[0] ? {29{1'b1}} : {29{1'b0}};
assign LS_OE_N_o = 1'b0;
//assign LS_DIR_o = 2'b01;
assign LS_DIR_o = 2'b00;

// Power-on reset pulse generation (not strictly necessary)
always @(posedge clk27)
begin
    if (po_reset_ctr == `PO_RESET_WIDTH)
        po_reset_n <= 1'b1;
    else
        po_reset_ctr <= po_reset_ctr + 1'b1;
end

// 25MHz clock from internal oscillator
/*always @(posedge clk_osc)
begin
    clk_osc_div <= clk_osc_div + 1'b1;
end*/

always @(posedge clk27) begin
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

always @(posedge clk27 or negedge po_reset_n) begin
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
    .clk_clk                                (clk27),
    .reset_reset_n                          (sys_reset_n),
    .pulpino_0_config_testmode_i            (1'b0),
    .pulpino_0_config_fetch_enable_i        (1'b1),
    .pulpino_0_config_clock_gating_i        (1'b0),
    .pulpino_0_config_boot_addr_i           (32'h00010000),
    .master_0_master_reset_reset            (jtagm_reset_req),
`ifndef DISABLE_SDC_CONTROLLER
    .sdc_controller_0_sd_sd_cmd_dat_i       (SD_CMD_io),
    .sdc_controller_0_sd_sd_cmd_out_o       (sd_cmd_out_o),
    .sdc_controller_0_sd_sd_cmd_oe_o        (sd_cmd_oe_o),
    .sdc_controller_0_sd_sd_dat_dat_i       (SD_DATA_io),
    .sdc_controller_0_sd_sd_dat_out_o       (sd_dat_out_o),
    .sdc_controller_0_sd_sd_dat_oe_o        (sd_dat_oe_o),
    .sdc_controller_0_sd_sd_clk_o_pad       (SD_CLK_o),
    .sdc_controller_0_sd_sd_clk_i_pad       (CLK27_i),
`endif
    .i2c_opencores_0_export_scl_pad_io      (SCL_io),
    .i2c_opencores_0_export_sda_pad_io      (SDA_io),
    .i2c_opencores_0_export_spi_miso_pad_i  (1'b0),
    .pio_0_sys_ctrl_out_export              (sys_ctrl),
    .pio_1_controls_in_export               (sys_status),
    .sc_config_0_sc_if_sc_status_i          ({20'h0, ISL_fe_interlace, ISL_fe_vtotal}),
    .sc_config_0_sc_if_sc_status2_i         ({12'h0, ISL_fe_pcnt_frame}),
    .sc_config_0_sc_if_lt_status_i          (32'h00000000),
    .sc_config_0_sc_if_h_in_config_o        (h_in_config),
    .sc_config_0_sc_if_h_in_config2_o       (h_in_config2),
    .sc_config_0_sc_if_v_in_config_o        (v_in_config),
    .sc_config_0_sc_if_misc_config_o        (),
    .sc_config_0_sc_if_sl_config_o          (),
    .sc_config_0_sc_if_sl_config2_o         (),
    .sc_config_0_sc_if_h_out_config_o       (h_out_config),
    .sc_config_0_sc_if_h_out_config2_o      (h_out_config2),
    .sc_config_0_sc_if_v_out_config_o       (v_out_config),
    .sc_config_0_sc_if_v_out_config2_o      (v_out_config2),
    .sc_config_0_sc_if_xy_out_config_o      (xy_out_config),
    /*.int_osc_0_oscena_oscena                (1'b1),
    .int_osc_0_clkout_clk                   (clk_osc)*/
    /*,
    .mem_if_lpddr2_emif_0_global_reset_reset_n     (emif_hwreset_n_sync2_reg),
    .mem_if_lpddr2_emif_0_soft_reset_reset_n       (emif_swreset_n_sync2_reg),
    .mem_if_lpddr2_emif_0_pll_ref_clk_clk          (pll_refclk),
    .mem_if_lpddr2_emif_0_status_local_init_done   (emif_status_init_done),
    .mem_if_lpddr2_emif_0_status_local_cal_success (emif_status_cal_success),
    .mem_if_lpddr2_emif_0_status_local_cal_fail    (emif_status_cal_fail),
    .memory_mem_ca                                 (DDR_CA_o),
    .memory_mem_ck                                 (DDR_CK_o),
    .memory_mem_ck_n                               (DDR_CK_N_o),
    .memory_mem_cke                                (DDR_CKE_o),
    .memory_mem_cs_n                               (DDR_CS_N_o),
    .memory_mem_dm                                 (DDR_DM_o),
    .memory_mem_dq                                 (DDR_DQ_io),
    .memory_mem_dqs                                (DDR_DQS_io),
    .memory_mem_dqs_n                              (DDR_DQS_N_io),
    .oct_rzqin                                     (DDR_RZQ_i),
    .mem_if_lpddr2_emif_0_avl_0_waitrequest_n      (avl_waitrequest_n),
    .mem_if_lpddr2_emif_0_avl_0_beginbursttransfer (avl_beginbursttransfer),
    .mem_if_lpddr2_emif_0_avl_0_address            (avl_address),
    .mem_if_lpddr2_emif_0_avl_0_readdatavalid      (avl_readdatavalid),
    .mem_if_lpddr2_emif_0_avl_0_readdata           (avl_readdata),
    .mem_if_lpddr2_emif_0_avl_0_writedata          (avl_writedata),
    .mem_if_lpddr2_emif_0_avl_0_byteenable         (avl_byteenable),
    .mem_if_lpddr2_emif_0_avl_0_read               (avl_read),
    .mem_if_lpddr2_emif_0_avl_0_write              (avl_write),
    .mem_if_lpddr2_emif_0_avl_0_burstcount         (avl_burstcount),
    .mem_if_mapper_0_avl_export_0_waitrequest_n      (avl_waitrequest_n),
    .mem_if_mapper_0_avl_export_0_beginbursttransfer (avl_beginbursttransfer),
    .mem_if_mapper_0_avl_export_0_address            (avl_address),
    .mem_if_mapper_0_avl_export_0_readdatavalid      (avl_readdatavalid),
    .mem_if_mapper_0_avl_export_0_readdata           (avl_readdata),
    .mem_if_mapper_0_avl_export_0_writedata          (avl_writedata),
    .mem_if_mapper_0_avl_export_0_byteenable         (avl_byteenable),
    .mem_if_mapper_0_avl_export_0_read               (avl_read),
    .mem_if_mapper_0_avl_export_0_write              (avl_write),
    .mem_if_mapper_0_avl_export_0_burstcount         (avl_burstcount)*/
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
    .frame_change_i(frame_change_capt),
    .xpos_i(xpos_capt),
    .ypos_i(ypos_capt),
    .h_out_config(h_out_config),
    .h_out_config2(h_out_config2),
    .v_out_config(v_out_config),
    .v_out_config2(v_out_config2),
    .xy_out_config(xy_out_config),
    .misc_config(32'h0),
    .sl_config(32'h0),
    .sl_config2(32'h0),
    .testpattern_enable(testpattern_enable),
    .PCLK_o(PCLK_sc),
    .R_o(R_sc),
    .G_o(G_sc),
    .B_o(B_sc),
    .HSYNC_o(HSYNC_sc),
    .VSYNC_o(VSYNC_sc),
    .DE_o(DE_sc),
    .xpos_o(),
    .ypos_o(),
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

/*videogen vg0 (
    .clk27          (SI_PCLK_i),
    .reset_n        (po_reset_n),
    .lt_active      (1'b0),
    .lt_mode        (2'b00),
    .R_out          (HDMITX_R_o),
    .G_out          (HDMITX_G_o),
    .B_out          (HDMITX_B_o),
    .HSYNC_out      (HDMITX_HSYNC_o),
    .VSYNC_out      (HDMITX_VSYNC_o),
    .PCLK_out       (HDMITX_PCLK_o),
    .ENABLE_out     (HDMITX_DE_o)
);*/

endmodule
