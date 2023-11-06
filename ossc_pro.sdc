set extra_av_out 1

### Clock definitions ###

create_clock -period 27MHz -name clk27 [get_ports CLK27_i]

create_clock -period 165MHz -name pclk_isl [get_ports ISL_PCLK_i]
create_clock -period 200MHz -name pclk_si [get_ports SI_PCLK_i]
create_clock -period 165MHz -name si_clk_extra [get_ports SI_CLK_EXTRA_i]
create_clock -period 165MHz -name pclk_hdmirx [get_ports HDMIRX_PCLK_i]

create_clock -period 5MHz -name bck_pcm [get_ports PCM_I2S_BCK_i]
create_clock -period 5MHz -name bck_hdmirx [get_ports HDMIRX_I2S_BCK_i]

create_generated_clock -source {sys_inst|pll_0|altera_pll_i|general[0].gpll~FRACTIONAL_PLL|refclkin} -divide_by 2 -multiply_by 88 -duty_cycle 50.00 -name pll_0_vco {sys_inst|pll_0|altera_pll_i|general[0].gpll~FRACTIONAL_PLL|vcoph[0]}
create_generated_clock -source {sys_inst|pll_0|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|vco0ph[0]} -divide_by 11 -duty_cycle 50.00 -name clk108 {sys_inst|pll_0|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk}
create_generated_clock -source {sys_inst|pll_0|altera_pll_i|general[1].gpll~PLL_OUTPUT_COUNTER|vco0ph[0]} -divide_by 8 -duty_cycle 50.00 -name clk148p5 {sys_inst|pll_0|altera_pll_i|general[1].gpll~PLL_OUTPUT_COUNTER|divclk}

create_generated_clock -name sd_clk -divide_by 2 -source {sys_inst|pll_0|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk} [get_pins sys:sys_inst|sdc_controller_top:sdc_controller_0|sdc_controller:sdc0|sd_clock_divider:clock_divider0|SD_CLK_O|q]

# output clocks
#set pclk_out_port [get_ports HDMITX_PCLK_o]
set i2s_bck_out_port [get_ports HDMITX_I2S_BCK_o]
#create_generated_clock -name pclk_out -master_clock pclk_isl -source [get_ports ISL_PCLK_i] -multiply_by 1 $pclk_out_port
#create_generated_clock -name pclk_out -master_clock pclk_hdmirx -source [get_ports HDMIRX_PCLK_i] -multiply_by 1 $pclk_out_port
#create_generated_clock -name bck_out -master_clock bck_pcm -source [get_ports PCM_I2S_BCK_i] -multiply_by 1 $i2s_bck_out_port
create_generated_clock -name bck_out -master_clock bck_hdmirx -source [get_ports HDMIRX_I2S_BCK_i] -multiply_by 1 $i2s_bck_out_port
create_generated_clock -name sd_clk_out -master_clock sd_clk -source [get_pins sys:sys_inst|sdc_controller_top:sdc_controller_0|sdc_controller:sdc0|sd_clock_divider:clock_divider0|SD_CLK_O|q] -multiply_by 1 [get_ports {SD_CLK_o}]

# retrieve post-mapping clkmux output pin
set clkmux_output [get_pins clkmux_capture|outclk]

# specify postmux clocks for RGB capture clock
create_generated_clock -name pclk_isl_postmux -master_clock pclk_isl -source [get_pins clkmux_capture|inclk[0]] -multiply_by 1 $clkmux_output
create_generated_clock -name pclk_hdmirx_postmux -master_clock pclk_hdmirx -source [get_pins clkmux_capture|inclk[1]] -multiply_by 1 $clkmux_output -add

# specify output clocks that drive PCLK output pin
set pclk_out_port [get_ports HDMITX_PCLK_o]
create_generated_clock -name pclk_si_out -master_clock pclk_si -source [get_ports SI_PCLK_i] -multiply_by 1 -invert $pclk_out_port
#create_generated_clock -name pclk_isl_out -master_clock pclk_isl_postmux -source $clkmux_output -multiply_by 1 $pclk_out_port -add
#create_generated_clock -name pclk_hdmirx_out -master_clock pclk_hdmirx_postmux -source $clkmux_output -multiply_by 1 $pclk_out_port -add

if {$extra_av_out} {
    create_generated_clock -name pclk_si_out_vga -master_clock pclk_si -source [get_ports SI_PCLK_i] -multiply_by 1 -invert [get_ports EXT_IO_B_io[27]]
}

# specify div2 capture and output clocks
set pclk_capture_div_pin [get_pins pclk_capture_div2|q]
create_generated_clock -name pclk_isl_postmux_div2 -master_clock pclk_isl_postmux -source $clkmux_output -divide_by 2 $pclk_capture_div_pin
create_generated_clock -name pclk_hdmirx_postmux_div2 -master_clock pclk_hdmirx_postmux -source $clkmux_output -divide_by 2 $pclk_capture_div_pin -add
set pclk_si_div_pin [get_pins pclk_out_div2|q]
create_generated_clock -name pclk_si_div2 -master_clock pclk_si -source [get_ports SI_PCLK_i] -divide_by 2 $pclk_si_div_pin

derive_clock_uncertainty


### Clock relationships ###

set_clock_groups -asynchronous -group \
                            {clk27} \
                            {clk108 sd_clk sd_clk_out} \
                            {clk148p5} \
                            {pclk_isl pclk_isl_postmux} \
                            {pclk_isl_postmux_div2} \
                            {pclk_hdmirx pclk_hdmirx_postmux} \
                            {pclk_hdmirx_postmux_div2} \
                            {pclk_si pclk_si_out pclk_si_out_vga} \
                            {pclk_si_div2} \
                            {si_clk_extra} \
                            {bck_hdmirx bck_pcm bck_out}


### IO delays ###

# SD card (IO constraints from Kingston specsheet, slightly reduced t_ODLY)
set_input_delay -clock sd_clk_out -min 2.5 [get_ports {SD_CMD_io SD_DATA_io[*]}] -add_delay
set_input_delay -clock sd_clk_out -max 12 [get_ports {SD_CMD_io SD_DATA_io[*]}] -add_delay
set_output_delay -clock sd_clk_out -min -2 [get_ports {SD_CMD_io SD_DATA_io[*]}] -add_delay
set_output_delay -clock sd_clk_out -max 6 [get_ports {SD_CMD_io SD_DATA_io[*]}] -add_delay

# ISL51002
set ISL_dmin [expr 3.4-0.5*(1000/165)]
set ISL_dmax [expr 0.5*(1000/165)-1.8]
set ISL_inputs [remove_from_collection [get_ports ISL_*_i*] ISL_PCLK_i]
set_input_delay -clock pclk_isl -clock_fall -min $ISL_dmin $ISL_inputs -add_delay
set_input_delay -clock pclk_isl -clock_fall -max $ISL_dmax $ISL_inputs -add_delay
set_false_path -to [get_ports ISL_*_o*]

# ADV7611
set hdmirx_dmin -2.2
set hdmirx_dmax 0.3
set hdmirx_data_inputs [get_ports {HDMIRX_R_i* HDMIRX_G_i* HDMIRX_B_i* HDMIRX_HSYNC_i HDMIRX_VSYNC_i HDMIRX_DE_i}]
set_input_delay -clock pclk_hdmirx -clock_fall -min $hdmirx_dmin $hdmirx_data_inputs -add_delay
set_input_delay -clock pclk_hdmirx -clock_fall -max $hdmirx_dmax $hdmirx_data_inputs -add_delay
set_input_delay 0 -clock bck_hdmirx -clock_fall [get_ports {HDMIRX_I2S_WS_i HDMIRX_AP_i}]
set_false_path -from [get_ports {HDMI_INT_N_i}]
set_false_path -to [get_ports {HDMIRX_RESET_N_o}]

# ADV7513 (0ns video clock delay adjustment)
#set hdmitx_dmin -1.9
#set hdmitx_dmax -0.2
#set hdmitx_data_outputs [get_ports {HDMITX_R_o* HDMITX_G_o* HDMITX_B_o* HDMITX_HSYNC_o HDMITX_VSYNC_o HDMITX_DE_o}]
#foreach_in_collection c [get_clocks pclk_*_out] {
#    set_output_delay -clock $c -min $hdmitx_dmin $hdmitx_data_outputs -add_delay
#    set_output_delay -clock $c -max $hdmitx_dmax $hdmitx_data_outputs -add_delay
#}

# SiI1136
set hdmitx_dmin -0.45
set hdmitx_dmax 1.36
set hdmitx_data_outputs [get_ports {HDMITX_R_o* HDMITX_G_o* HDMITX_B_o* HDMITX_HSYNC_o HDMITX_VSYNC_o HDMITX_DE_o}]
foreach_in_collection c [get_clocks pclk_*_out] {
    set_output_delay -clock $c -clock_fall -min $hdmitx_dmin $hdmitx_data_outputs -add_delay
    set_output_delay -clock $c -clock_fall -max $hdmitx_dmax $hdmitx_data_outputs -add_delay
}

set_output_delay -clock bck_out -min $hdmitx_dmin [get_ports {HDMITX_I2S_WS_o HDMITX_I2S_DATA_o}] -add_delay
set_output_delay -clock bck_out -max $hdmitx_dmax [get_ports {HDMITX_I2S_WS_o HDMITX_I2S_DATA_o}] -add_delay
set_false_path -to [get_ports {HDMITX_SPDIF_o HDMITX_5V_EN_o}]

# extra_av_out
if {$extra_av_out} {
    # ADV7125
    set vga_dmin -1.5
    set vga_dmax 0.2
    set vga_data_outputs [get_ports {EXT_IO_B_io[6] EXT_IO_B_io[7] EXT_IO_B_io[4] EXT_IO_B_io[5] EXT_IO_B_io[2] EXT_IO_B_io[3] EXT_IO_B_io[0] EXT_IO_B_io[1]
                                     EXT_IO_B_io[14] EXT_IO_B_io[15] EXT_IO_B_io[12] EXT_IO_B_io[13] EXT_IO_B_io[10] EXT_IO_B_io[11] EXT_IO_B_io[8] EXT_IO_B_io[9]
                                     EXT_IO_B_io[24] EXT_IO_B_io[25] EXT_IO_B_io[22] EXT_IO_B_io[23] EXT_IO_B_io[20] EXT_IO_B_io[21] EXT_IO_B_io[18] EXT_IO_B_io[19]
                                     EXT_IO_B_io[29] EXT_IO_B_io[30] EXT_IO_B_io[17] EXT_IO_B_io[16]}]
    set_output_delay -clock pclk_si_out_vga -min $vga_dmin $vga_data_outputs -add_delay
    set_output_delay -clock pclk_si_out_vga -max $vga_dmax $vga_data_outputs -add_delay
    set_false_path -to [get_ports {EXT_IO_A_io[4] EXT_IO_A_io[5]}]
} else {
    set_false_path -to [get_ports {EXT_IO_A_io* EXT_IO_B_io*}]
}

# PCM1862
set_input_delay 0 -clock bck_pcm -clock_fall [get_ports {PCM_I2S_WS_i PCM_I2S_DATA_i}]

# Misc
set_false_path -from [get_ports {BTN_i* IR_RX_i SCL_io SDA_io SI_INT_N_i SPDIF_EXT_i SD_DETECT_i}]
set_false_path -to [get_ports {LED_o* AUDMUX_o SCL_io SDA_io FPGA_PCLK1x_o FAN_PWM_o}]
set_false_path -from {emif_hwreset_n_sync2_reg emif_swreset_n_sync2_reg}
set_false_path -to {emif_hwreset_n_sync1_reg emif_swreset_n_sync1_reg}
set_false_path -to sys:sys_inst|sys_pio_1:pio_2|readdata[0]
set_false_path -to sys:sys_inst|sys_pio_1:pio_2|readdata[1]
set_false_path -to sys:sys_inst|sys_pio_1:pio_2|readdata[2]
set_false_path -setup -to [get_registers sys:sys_inst|sys_alt_vip_cl_cvo_0:alt_vip_cl_cvo_0|alt_vip_cvo_core:cvo_core|alt_vip_cvo_sync_conditioner:pixel_channel_sync_conditioner|alt_vip_common_sync_generation:sync_generation_generate.sync_generation|sof*]


### JTAG Signal Constraints ###

# max 10MHz JTAG clock
remove_clock altera_reserved_tck
create_clock -name altera_reserved_tck -period "10MHz" [get_ports altera_reserved_tck]
set_clock_groups -exclusive -group [get_clocks altera_reserved_tck]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]
set_output_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdo]
