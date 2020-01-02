#!/bin/sh

make rv-reprogram

if [ $# -eq 1 ] && [ $1 = "jtag_uart" ]; then
    nios2-terminal
fi
