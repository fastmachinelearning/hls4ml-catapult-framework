# Put here the code to be executed immediately before running 'go assembly'
# in Catapult.

if {![info exists CLOCK_PERIOD]} {
   set CLOCK_PERIOD 5.0
}

if {![info exists CLOCK_UNCERTAINTY]} {
   set CLOCK_UNCERTAINTY [expr {0.125 * $CLOCK_PERIOD}]
}

set CLOCK_HIGH_TIME [expr {$CLOCK_PERIOD / 2}]

eval "directive set -CLOCKS {clk {-CLOCK_PERIOD ${CLOCK_PERIOD} -CLOCK_EDGE rising -CLOCK_HIGH_TIME ${CLOCK_HIGH_TIME} -CLOCK_OFFSET 0.000000 -CLOCK_UNCERTAINTY ${CLOCK_UNCERTAINTY} -RESET_KIND sync -RESET_SYNC_NAME rst -RESET_SYNC_ACTIVE high -RESET_ASYNC_NAME arst_n -RESET_ASYNC_ACTIVE low -ENABLE_NAME {} -ENABLE_ACTIVE high}}"
