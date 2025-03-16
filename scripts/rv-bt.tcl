#Select the master service type and check for available service paths.
while 1 {
    set service_paths [get_service_paths master]
    if {[llength $service_paths] > 0} {
        break
    }
    puts "Refreshing connections..."
    refresh_connections
    after 100
}

#Set the master service path.
set master_service_path [lindex $service_paths 0]

#Open the master service.
set claim_path [claim_service master $master_service_path mylib]

puts "Halting CPU"
master_write_32 $claim_path 0x0 0x10000

puts "Next PC: [master_read_32 $claim_path 0x2000 1]"
puts "Prev PC: [master_read_32 $claim_path 0x2004 1]"
puts "MEPC:    [master_read_32 $claim_path 0x4d04 1]"
puts "MCAUSE:  [master_read_32 $claim_path 0x4d08 1]"

set offset 0x404
foreach i {ra sp gp tp t0 t1 t2 s0 s1 a0 a1 a2 a3 a4 a5} {
    puts "$i: [master_read_32 $claim_path [format 0x%x $offset] 1]"
    set offset [expr $offset + 4]
}

master_write_32 $claim_path 0x0 0x00000
