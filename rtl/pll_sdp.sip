set_global_assignment -entity "pll_sdp" -library "lib_pll_sdp" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "pll_sdp" -library "lib_pll_sdp" -name IP_TOOL_VERSION "21.1"
set_global_assignment -entity "pll_sdp" -library "lib_pll_sdp" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_pll_sdp" -name SPD_FILE [file join $::quartus(sip_path) "pll_sdp.spd"]

set_global_assignment -library "lib_pll_sdp" -name MISC_FILE [file join $::quartus(sip_path) "pll_sdp_sim/pll_sdp.vo"]
