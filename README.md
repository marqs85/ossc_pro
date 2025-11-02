OSSC Pro FW files
==============

Open Source Scan Converter Pro is a low-latency advanced video digitizer and scan conversion board designed mainly for connecting retro video game consoles and home computers into modern displays. Please check the [wikipage](http://junkerhq.net/xrgb/index.php?title=OSSC_Pro) for more detailed description and latest features. This repository hosts the files required to build firmware.

Requirements for building and debugging firmware
---------------------------------------------------
* Hardware
  * OSSC Pro board
  * USB Blaster compatible JTAG debugger, e.g. Terasic Blaster (for FW installation and debugging)
  * micro SD/SDHC card (for FW update via SD card)

* Software
  * [Altera Quartus II + Cyclone V support](http://dl.altera.com/?edition=lite) (v 21.1 or higher - free Lite Edition suffices)
  * [RISC-V GNU Compiler Toolchain](https://github.com/riscv/riscv-gnu-toolchain)
  * [Picolibc](https://github.com/picolibc/picolibc) for RISC-V
  * GCC (or another C compiler) for host architecture (for building a SD card image)
  * Make


Architecture
------------------------------
TODO


SW toolchain build procedure
--------------------------
1. Download, configure, build and install RISC-V toolchain (with RV32EMC support) and Picolibc.

From sources:
~~~~
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
git clone --recursive https://github.com/picolibc/picolibc
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv32emc --with-abi=ilp32e
sudo make    # sudo needed if installing under default /opt/riscv location
~~~~
On Debian-style Linux distros:
~~~~
sudo apt install gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf picolibc-riscv64-unknown-elf
~~~~


Building RTL (bitstream)
--------------------------
1. Initialize project submodules (once after cloning ossc_pro project or when submoduled have been updated)
~~~~
git submodule update --init --recursive
~~~~
2. Load the project (ossc_pro.qpf) in Quartus
3. Generate QSYS output files (only needed before first compilation or when QSYS structure has been modified)
    * Open Platform Designer (Tools -> Platform Designer)
    * Load platform configuration (sys.qsys)
    * Generate output (Generate -> Generate HDL, Generate)
    * Close Platform Designer
    * Run "patch -p0 <scripts/qsys.patch" to patch generated files to use block RAM for large FIFOs
    * Run "touch software/sys_controller_bsp/bsp_timestamp" to acknowledge QSYS update
4. Generate the FPGA bitstream (Processing -> Start Compilation)
5. Ensure that there are no severe timing violations by looking into Timing Analyzer report

NOTE: If you want to generate distributable bitstream and do not have Intel VIP license, disable alt_vip_* modules in Qsys before platform generation and comment out \`define VIP at the top of rtl/ossc_pro.v.

Building RTL using the Docker image
--------------------------
Once you built the image (we'll assume it is named ossc_pro), you can run this command while in the repo :

~~~~
$ docker run -it --rm \
   -e LM_LICENSE_FILE=/opt/license.dat \ # If you have a license
   -v /path/to/license.dat:/opt/license.dat:ro \ # If you have a license
   -v $(pwd):/build \
   ossc_pro \
   /bin/bash
~~~~

It will spawn a bash prompt with Quartus Lite, the RISC-V toolchain installed.

Here are the commands used for building the RTL without GUI:
~~~~
# qsys-generate --synthesis=VERILOG sys.qsys
# patch -p0 < scripts/qsys.patch
# touch software/sys_controller_bsp/bsp_timestamp
# quartus_sh --flow compile ossc_pro.qpf
~~~~

Building software image
--------------------------
1. Enter software root directory:
~~~~
cd software/sys_controller
~~~~
2. Build SW for target configuration:
~~~~
make
~~~~
3. Optionally test updated SW by directly downloading SW image to flash via JTAG (requires valid FPGA bitstream to be present):
~~~~
make rv-reprogram
~~~~
NOTE: If you generated bitstream without Intel VIP license, comment out "#define VIP" from config/sysconfig.h before compilation.


Installing firmware via JTAG
--------------------------
The bitstream can be either directly programmed into FPGA (volatile method, suitable for quick testing), or into serial flash chip alongside SW image where it is automatically loaded every time FPGA is subsequently powered on (nonvolatile method, suitable for long-term use).

To directly program FPGA, open Programmer in Quartus, select your USB Blaster device, add configuration file (output_files/ossc_pro.sof) and press Start. Download SW image if it not present / up to date in flash.

To program flash, a combined FPGA image must be first generated and converted into JTAG indirect Configuration file (.jic). Open conversion tool ("File->Convert Programming Files") in Quartus, click "Open Conversion Setup Data", select "ossc_pro.cof" and press Generate. Then open Programmer and ensure that "Initiate configuration after programming" and "Unprotect EPCS/EPCQ devices selected for Erase/Program operation" are checked in Tools->Options. Then clear file list, add generated file (output_files/ossc_pro.jic) and press Start after which flash is programmed. Installed/updated firmware is activated when programming finishes (or after power-cycling the board in case of a fresh flash chip).


Generating SD card image
--------------------------
Bitstream file must be wrapped with custom header structure (including checksums) so that it can be processed reliably on the CPU. This can be done with included helper application which generates an image file which can written on FAT32-formatted SD card and subsequently loaded on OSSC Pro:

1. Compile tools/create_fw_img.c
~~~~
cd tools && gcc create_fw_img.c -o create_fw_img
~~~~
2. Generate the firmware image:
~~~~
./create_fw_img <num_images> <img1> <offset1> <img2> <offset2> ... <version>
~~~~
where
* \<num_images\> is the number of separate images in the file which is typically 2 (FPGA bitstream + SW image)
* \<imgX\> is path to the image file
* \<offsetX\> is offset in the flash where the image will be placed
* \<version\> is version string (e.g. 0.79)

The primary firmware has FPGA bitstream at offset 0x0 and SW image at 0x2d0000 (0xA00000 before 0.79) so the command is typically as follows:
~~~~
./create_fw_img 2 ../output_files/ossc_pro.rbf 0x0 ../software/sys_controller/mem_init/flash.bin 0x2d0000 0.79
~~~~
This creates ossc_pro_\<version\>.bin which can be copied to fw folder of SD card.

Alternative firmware images and dynamic reconfiguration
--------------------------
TODO

Debugging
--------------------------
1. Rebuild the software in debug mode:
~~~~
make clean && make APP_CFLAGS_DEBUG_LEVEL="-DDEBUG"
~~~~

2. Flash SW image via JTAG and open terminal for UART
~~~~
make rv-reprogram && nios2-terminal
~~~~
Remember to close nios2-terminal after debug session, otherwise any JTAG transactions will hang/fail.


Links
--------------------------
* [HW repository](https://github.com/marqs85/ossc_pro_hw)

License
---------------------------------------------------
[GPL3](LICENSE)