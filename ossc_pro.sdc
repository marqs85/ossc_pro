### Clock definitions ###

create_clock -period 27MHz -name clk27 [get_ports CLK27_i]
#create_clock -period 25MHz -name clk25 clk_osc_div[1]

create_clock -period 165MHz -name pclk_isl [get_ports ISL_PCLK_i]
create_clock -period 165MHz -name pclk_si [get_ports SI_PCLK_i]
create_clock -period 165MHz -name si_clk_extra [get_ports SI_CLK_EXTRA_i]
create_clock -period 165MHz -name pclk_hdmirx [get_ports HDMIRX_PCLK_i]

create_clock -period 5MHz -name bck_pcm [get_ports PCM_I2S_BCK_i]
create_clock -period 5MHz -name bck_hdmirx [get_ports HDMIRX_I2S_BCK_i]

#create_generated_clock -name sd_clk -divide_by 2 -source [get_ports CLK27_i] [get_pins sys:sys_inst|sdc_controller_top:sdc_controller_0|sdc_controller:sdc0|sd_clock_divider:clock_divider0|SD_CLK_O|q]
#                           {sd_clk} \

# output clocks
#set pclk_out_port [get_ports HDMITX_PCLK_o]
set i2s_bck_out_port [get_ports HDMITX_I2S_BCK_o]
#create_generated_clock -name pclk_out -master_clock pclk_isl -source [get_ports ISL_PCLK_i] -multiply_by 1 $pclk_out_port
#create_generated_clock -name pclk_out -master_clock pclk_hdmirx -source [get_ports HDMIRX_PCLK_i] -multiply_by 1 $pclk_out_port
#create_generated_clock -name bck_out -master_clock bck_pcm -source [get_ports PCM_I2S_BCK_i] -multiply_by 1 $i2s_bck_out_port
create_generated_clock -name bck_out -master_clock bck_hdmirx -source [get_ports HDMIRX_I2S_BCK_i] -multiply_by 1 $i2s_bck_out_port

# retrieve post-mapping clkmux output pin
set clkmux_output [get_pins clkmux_capture|outclk]

# specify postmux clocks for RGB capture clock
create_generated_clock -name pclk_isl_postmux -master_clock pclk_isl -source [get_pins clkmux_capture|inclk[0]] -multiply_by 1 $clkmux_output
create_generated_clock -name pclk_hdmirx_postmux -master_clock pclk_hdmirx -source [get_pins clkmux_capture|inclk[1]] -multiply_by 1 $clkmux_output -add

# specify output clocks that drive PCLK output pin
set pclk_out_port [get_ports HDMITX_PCLK_o]
create_generated_clock -name pclk_si_out -master_clock pclk_si -source [get_ports SI_PCLK_i] -multiply_by 1 $pclk_out_port
#create_generated_clock -name pclk_isl_out -master_clock pclk_isl_postmux -source $clkmux_output -multiply_by 1 $pclk_out_port -add
#create_generated_clock -name pclk_hdmirx_out -master_clock pclk_hdmirx_postmux -source $clkmux_output -multiply_by 1 $pclk_out_port -add


derive_clock_uncertainty


### Clock relationships ###

set_clock_groups -asynchronous -group \
                            {clk27} \
                            {pclk_isl pclk_isl_postmux} \
                            {pclk_hdmirx pclk_hdmirx_postmux} \
                            {pclk_si pclk_si_out} \
                            {si_clk_extra} \
                            {bck_hdmirx bck_pcm bck_out}


### IO delays ###

# SD card
#set_input_delay 0 -clock sd_clk -reference_pin [get_ports {SD_CLK_o}] [get_ports {SD_CMD_io SD_DATA_io[*]}]
#set_output_delay 0 -clock sd_clk -reference_pin [get_ports {SD_CLK_o}] [get_ports {SD_CMD_io SD_DATA_io[*]}]

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
set_false_path -from [get_ports {HDMIRX_INT_N_i}]
set_false_path -to [get_ports {HDMIRX_RESET_N_o}]

# ADV7513
set hdmitx_dmin -0.7
set hdmitx_dmax 1
set hdmitx_data_outputs [get_ports {HDMITX_R_o* HDMITX_G_o* HDMITX_B_o* HDMITX_HSYNC_o HDMITX_VSYNC_o HDMITX_DE_o}]
foreach_in_collection c [get_clocks pclk_*_out] {
    set_output_delay -clock $c -min $hdmitx_dmin $hdmitx_data_outputs -add_delay
    set_output_delay -clock $c -max $hdmitx_dmax $hdmitx_data_outputs -add_delay
}

set_output_delay -clock bck_out -min $hdmitx_dmin [get_ports {HDMITX_I2S_WS_o HDMITX_I2S_DATA_o}] -add_delay
set_output_delay -clock bck_out -max $hdmitx_dmax [get_ports {HDMITX_I2S_WS_o HDMITX_I2S_DATA_o}] -add_delay
set_false_path -from [get_ports {HDMITX_INT_N_i}]
set_false_path -to [get_ports {HDMITX_SPDIF_o HDMITX_PD_o}]

# PCM1862
set_input_delay 0 -clock bck_pcm -clock_fall [get_ports {PCM_I2S_WS_i PCM_I2S_DATA_i}]

# Misc
set_false_path -from [get_ports {BTN_i* IR_RX_i SCL_io SDA_io SI_INT_N_i SPDIF_EXT_i}]
set_false_path -to [get_ports {LED_o* AUDMUX_o SCL_io SDA_io FPGA_PCLK1x_o}]
#set_false_path -from {emif_hwreset_n_sync2_reg emif_swreset_n_sync2_reg}
#set_false_path -to {emif_hwreset_n_sync1_reg emif_swreset_n_sync1_reg}
#set_false_path -to sys:sys_inst|sys_pio_1:pio_1|readdata[0]
#set_false_path -to sys:sys_inst|sys_pio_1:pio_1|readdata[1]
#set_false_path -to sys:sys_inst|sys_pio_1:pio_1|readdata[2]


### JTAG Signal Constraints ###

# max 10MHz JTAG clock
remove_clock altera_reserved_tck
create_clock -name altera_reserved_tck -period "10MHz" [get_ports altera_reserved_tck]
set_clock_groups -exclusive -group [get_clocks altera_reserved_tck]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tms]
set_output_delay -clock altera_reserved_tck 20 [get_ports altera_reserved_tdo]
