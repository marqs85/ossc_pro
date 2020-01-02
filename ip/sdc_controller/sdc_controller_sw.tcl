# Create a new driver
create_driver sdc_controller_driver

# Associate it with hardware
set_sw_property hw_class_name sdc_controller 
# The version of this driver
#set_sw_property version 18.0

# This driver may be incompatible with versions of hardware less
# than specified below. Updates to hardware and device drivers
# rendering the driver incompatible with older versions of
# hardware are noted with this property assignment.
set_sw_property min_compatible_hw_version 1.0

# Initialize the driver in alt_sys_init()
#set_sw_property auto_initialize true

#set_sw_property csr_interfaces "csr,descriptor_slave,response;csr,descriptor_slave;csr,prefetcher_csr"

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory drivers

# Interrupt properties: This driver supports enhanced
# interrupt APIs, as well as ISR preemption.
#set_sw_property isr_preemption_supported true
#set_sw_property supported_interrupt_apis "enhanced_interrupt_api"

#
# Source file listings...
#

# C/C++ source files
add_sw_property c_source src/mmc.c
add_sw_property c_source src/ocsdc.c

# Include files
add_sw_property include_source inc/mmc.h

# This driver supports HAL & UCOSII BSP (OS) types
add_sw_property supported_bsp_type HAL
#add_sw_property supported_bsp_type UCOSII
#add_sw_property supported_bsp_type UCOSIII

# End of file
