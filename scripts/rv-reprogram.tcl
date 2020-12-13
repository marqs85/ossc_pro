# flash details
set flash_base                  0x01000000
set flash_imem_offset           0x00500000
set flash_imem_base             [format 0x%.8x [expr $flash_base + $flash_imem_offset]]
set flash_secsize               65536

# flash controller register addresses
set control_register            0x8000
set operating_protocols_setting 0x8010
set read_instr                  0x8014
set write_instr                 0x8018
set flash_cmd_setting           0x801c
set flash_cmd_ctrl              0x8020
set flash_cmd_addr_register     0x8024
set flash_cmd_write_data_0      0x8028
set flash_cmd_read_data_0       0x8030

# target file details
set bin_file mem_init/flash.bin
set bin_size [file size $bin_file]
set num_sectors [expr ($bin_size / $flash_secsize) + (($bin_size % $flash_secsize) ne 0)]

#Select the master service type and check for available service paths.
set service_paths [get_service_paths master]

#Set the master service path.
set master_service_path [lindex $service_paths 0]

#Open the master service.
set claim_path [claim_service master $master_service_path mylib]

puts "Halting CPU"
master_write_32 $claim_path 0x0 0x10000

puts "Erasing $num_sectors flash sectors"
master_write_32 $claim_path $flash_cmd_setting 0x00000006
master_write_32 $claim_path $flash_cmd_ctrl 0x1
master_write_32 $claim_path $flash_cmd_setting 0x000003D8
set addr $flash_imem_base
for {set i 0} {$i<$num_sectors} {incr i} {
    master_write_32 $claim_path $flash_cmd_addr_register $flash_imem_offset
    master_write_32 $claim_path $flash_cmd_ctrl 0x1
    set addr [expr $addr + $flash_secsize]
    after 2
}

puts "Writing flash"
# does hang for some reason after ~70 bytes. Write polling issue?
#master_write_from_file $claim_path mem_init/flash.bin $flash_imem_base

set chunks [llength [glob mem_init/chunks/*]]
puts "Programming $chunks chunks"
set addr $flash_imem_base
for {set i 0} {$i<$chunks} {incr i} {
    set file [format "flash.%04d" $i]
    master_write_from_file $claim_path mem_init/chunks/$file $addr
    set addr [expr $addr + 64]
}
#master_read_to_file $claim_path mem_init/flash_readback.bin $flash_imem_base $bin_size
#master_read_to_file $claim_path mem_init/ram_readback.bin 0x010000 65536

close_service master $claim_path


set jtag_debug_list [get_service_paths jtag_debug]
set jd [ lindex $jtag_debug_list 0 ]
open_service jtag_debug $jd
puts "Resetting system"
jtag_debug_reset_system $jd
close_service jtag_debug $jd
puts "Done"
