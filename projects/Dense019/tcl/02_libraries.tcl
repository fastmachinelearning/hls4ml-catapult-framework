# Put here the code to be executed immediately before running 'go libraries'
# in Catapult.
#
# You must not remove the code responsible for setting RTL_SYN_TOOL as it is
# used by the 'synthesize' script to determine what RTL synthesis tool should
# be run after the Catapult flow is finished. If you want to alter it, make
# sure that the "set RTL_SYN_TOOL" part is still present (it doesn't have to
# be exactly "set RTL_SYN_TOOL Vivado").

if {![info exists FPGA_LIBRARY]} {
   set FPGA_LIBRARY mgc_Xilinx-VIRTEX-uplus-2L_beh
}

if {![info exists RTL_SYN_TOOL]} {
   set RTL_SYN_TOOL Vivado
}

if {![info exists FPGA_MANUFACTURER]} {
   set FPGA_MANUFACTURER Xilinx
}

if {![info exists FPGA_FAMILY]} {
   set FPGA_FAMILY VIRTEX-uplus
}

if {![info exists FPGA_SPEED]} {
   set FPGA_SPEED -2L
}

if {![info exists FPGA_PART]} {
   set FPGA_PART xcvu9p-flgb2104-2L-e
}

if {$RTL_SYN_TOOL == "Vivado"} {
   options set Flows/Vivado/VivadoMode Non-project
}

solution library remove *
solution library add $FPGA_LIBRARY -- -rtlsyntool $RTL_SYN_TOOL -manufacturer $FPGA_MANUFACTURER -family $FPGA_FAMILY -speed $FPGA_SPEED -part $FPGA_PART
