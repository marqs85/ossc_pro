#
# request TCL package from ACDS 16.1
#
package require -exact qsys 16.1

#
# module
#
set_module_property DESCRIPTION "Avalon Bridge Mod"
set_module_property NAME avalon_bridge_mod
#set_module_property VERSION 18.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP "Interface Protocols"
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME avalon_bridge_mod
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE false
set_module_property ELABORATION_CALLBACK elaborate

#
# file sets
#
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL avalon_bridge_mod
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file avalon_bridge_mod.v VERILOG PATH avalon_bridge_mod.v

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE false
set_fileset_property SIM_VERILOG TOP_LEVEL avalon_bridge_mod
add_fileset_file avalon_bridge_mod.v VERILOG PATH avalon_bridge_mod.v

#
# parameters
#
# HDL params
add_parameter AW integer 10 "Address width"
set_parameter_property AW HDL_PARAMETER true
add_parameter DW integer 8 "Data width"
set_parameter_property DW ALLOWED_RANGES {256 128 64 32 16 8}
set_parameter_property DW HDL_PARAMETER true


#
# connection point clk
#
add_interface clk clock end
add_interface_port clk clk clk Input 1

#
# connection point reset
#
add_interface reset reset end
set_interface_property reset associatedClock clk
set_interface_property reset enabled true
set_interface_property reset synchronousEdges DEASSERT

add_interface_port reset reset reset Input 1


#
# connection point avalon_master_wr
#
add_interface avalon_master_wr avalon start
set_interface_property avalon_master_wr associatedClock clk
set_interface_property avalon_master_wr associatedReset reset
set_interface_property avalon_master_wr burstOnBurstBoundariesOnly false
set_interface_property avalon_master_wr doStreamReads false
set_interface_property avalon_master_wr doStreamWrites false
set_interface_property avalon_master_wr linewrapBursts false

#
# connection point avalon_master_rd
#
add_interface avalon_master_rd avalon start
set_interface_property avalon_master_rd associatedClock clk
set_interface_property avalon_master_rd associatedReset reset
set_interface_property avalon_master_rd burstOnBurstBoundariesOnly false
set_interface_property avalon_master_rd doStreamReads false
set_interface_property avalon_master_rd doStreamWrites false
set_interface_property avalon_master_rd linewrapBursts false

#
# connection point external_interface
#
add_interface external_interface conduit end


proc elaborate {} {
	set addr_width [get_parameter_value "AW"]
	set data_width [get_parameter_value "DW"]

    add_interface_port avalon_master_wr avalon_wr_waitrequest waitrequest Input 1
    add_interface_port avalon_master_wr avalon_wr_write write Output 1
    add_interface_port avalon_master_wr avalon_wr_writedata writedata Output $data_width
    add_interface_port avalon_master_wr avalon_wr_address address Output $addr_width
    add_interface_port avalon_master_wr avalon_wr_burstcount burstcount Output 6

    add_interface_port avalon_master_rd avalon_rd_readdata readdata Input $data_width
    add_interface_port avalon_master_rd avalon_rd_waitrequest waitrequest Input 1
    add_interface_port avalon_master_rd avalon_rd_readdatavalid readdatavalid Input 1
    add_interface_port avalon_master_rd avalon_rd_read read Output 1
    add_interface_port avalon_master_rd avalon_rd_address address Output $addr_width
    add_interface_port avalon_master_rd avalon_rd_burstcount burstcount Output 6

    add_interface_port external_interface clk_o clk_o Output 1
    add_interface_port external_interface wr_address wr_address Input $addr_width
    add_interface_port external_interface rd_address rd_address Input $addr_width
    add_interface_port external_interface rd_read rd_read Input 1
    add_interface_port external_interface wr_write wr_write Input 1
    add_interface_port external_interface wr_write_data wr_write_data Input $data_width
    add_interface_port external_interface wr_burstcount wr_burstcount Input 6
    add_interface_port external_interface rd_burstcount rd_burstcount Input 6
    add_interface_port external_interface wr_waitrequest wr_waitrequest Output 1
    add_interface_port external_interface rd_readdatavalid rd_readdatavalid Output 1
    add_interface_port external_interface rd_waitrequest rd_waitrequest Output 1
    add_interface_port external_interface rd_read_data rd_read_data Output $data_width
}
