module sdc_controller_top(
    // common
    input clk_i,
    input rst_i,
    // avalon slave
    input [31:0] avalon_s_writedata,
    output [31:0] avalon_s_readdata,
    input [7:0] avalon_s_address,
    input [3:0] avalon_s_byteenable,
    input avalon_s_write,
    input avalon_s_read,
    input avalon_s_chipselect,
    output avalon_s_waitrequest_n,
    // avalon master
    output [31:0] avalon_m_writedata,
    input [31:0] avalon_m_readdata,
    output [31:0] avalon_m_address,
    output [3:0] avalon_m_byteenable,
    output avalon_m_write,
    output avalon_m_read,
    output avalon_m_chipselect,
    input avalon_m_waitrequest_n,
    // SD BUS
    input sd_cmd_dat_i,
    output sd_cmd_out_o,
    output sd_cmd_oe_o,
    input [3:0] sd_dat_dat_i, 
    output [3:0] sd_dat_out_o, 
    output sd_dat_oe_o, 
    output sd_clk_o_pad,
    input sd_clk_i_pad,
    // Interrupts
    output int_cmd, 
    output int_data
);

    wire m_wb_we_o;
    wire m_wb_cyc_o;

    wire avalon_s_we = avalon_s_write & ~avalon_s_read;
    wire avalon_s_cyc = avalon_s_write | avalon_s_read;

    assign avalon_m_write = m_wb_cyc_o & m_wb_we_o;
    assign avalon_m_read = m_wb_cyc_o & ~m_wb_we_o;

sdc_controller sdc0(
    // WISHBONE common
    .wb_clk_i(clk_i), 
    .wb_rst_i(rst_i), 
    // WISHBONE slave
    .wb_dat_i(avalon_s_writedata), 
    .wb_dat_o(avalon_s_readdata),
    .wb_adr_i(avalon_s_address), 
    .wb_sel_i(avalon_s_byteenable), 
    .wb_we_i(avalon_s_we), 
    .wb_cyc_i(avalon_s_cyc), 
    .wb_stb_i(avalon_s_chipselect), 
    .wb_ack_o(avalon_s_waitrequest_n),
    // WISHBONE master
    .m_wb_dat_o({avalon_m_writedata[7:0], avalon_m_writedata[15:8], avalon_m_writedata[23:16], avalon_m_writedata[31:24]}),
    .m_wb_dat_i({avalon_m_readdata[7:0], avalon_m_readdata[15:8], avalon_m_readdata[23:16], avalon_m_readdata[31:24]}),
    .m_wb_adr_o(avalon_m_address), 
    .m_wb_sel_o({avalon_m_byteenable[0], avalon_m_byteenable[1], avalon_m_byteenable[2], avalon_m_byteenable[3]}),
    .m_wb_we_o(m_wb_we_o),
    .m_wb_cyc_o(m_wb_cyc_o),
    .m_wb_stb_o(avalon_m_chipselect), 
    .m_wb_ack_i(avalon_m_waitrequest_n),
    .m_wb_cti_o(), 
    .m_wb_bte_o(),
    //SD BUS
    .sd_cmd_dat_i(sd_cmd_dat_i), 
    .sd_cmd_out_o(sd_cmd_out_o), 
    .sd_cmd_oe_o(sd_cmd_oe_o), 
    //card_detect,
    .sd_dat_dat_i(sd_dat_dat_i), 
    .sd_dat_out_o(sd_dat_out_o), 
    .sd_dat_oe_o(sd_dat_oe_o), 
    .sd_clk_o_pad(sd_clk_o_pad),
    .sd_clk_i_pad(sd_clk_i_pad),
    .int_cmd(int_cmd), 
    .int_data(int_data)
);

endmodule
