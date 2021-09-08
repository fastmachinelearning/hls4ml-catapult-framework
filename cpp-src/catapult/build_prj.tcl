#set_working_dir
solution new -state initial
solution options defaults
solution options set Flows/ModelSim/VLOG_OPTS {-suppress 12110}
solution options set Flows/ModelSim/VSIM_OPTS {-t ps -suppress 12110}
solution options set Flows/DesignCompiler/OutNetlistFormat verilog
solution options set /Input/CppStandard c++11
set CATAPULT_VERSION  [string map { / - } [string map { . - } [application get /SYSTEM/RELEASE_VERSION]]]
solution options set Cache/UserCacheHome "catapult_cache_$CATAPULT_VERSION"
solution options set Cache/DefaultCacheHomeEnabled false
flow package require /SCVerify
directive set -REGISTER_THRESHOLD 8192
solution options set Flows/QuestaSIM/SCCOM_OPTS {-g -x c++ -Wall -Wno-unused-label -Wno-unknown-pragmas -DRTL_SIM -D__ASIC__}
solution options set /Input/CompilerFlags {-DMNTR_CATAPULT_HLS -D__ASIC__}
solution options set /Input/SearchPath {../OutputDir/firmware/ ../OutputDir/firmware/ap_types ../OutputDir/firmware/weights ../OutputDir/firmware/nnet_utils}
solution file add ../OutputDir/firmware/ProjectName.cpp -type C++
solution file add ../OutputDir/ProjectName_test.cpp -type C++ -exclude true
solution file set ../OutputDir/ProjectName_test.cpp -args {-D__WEIGHTS_FROM_FILE__ -D__WEIGHTS_DIR__=../OutputDir/firmware/weights/}
solution option set Output/OutputVHDL false
solution option set Output/OutputVerilog true
solution option set Output/PackageOutput true
solution option set Output/PackageStaticFiles true
solution option set Output/PrefixStaticFiles true
#solution options set Output/SubBlockNamePrefix "encoder_"
solution option set Output/DoNotModifyNames true
go new
go analyze
directive set -DESIGN_HIERARCHY ProjectName
solution library add nangate-45nm_beh -- -rtlsyntool RTLCompiler -vendor Nangate -technology 045nm
solution library add ccs_sample_mem
go libraries
directive set -CLOCKS { \
        clk { \
            -CLOCK_PERIOD 25.0 \
            -CLOCK_EDGE rising \
            -CLOCK_HIGH_TIME 12.5 \
            -CLOCK_OFFSET 0.000000 \
            -CLOCK_UNCERTAINTY 0.0 \
            -RESET_KIND sync \
            -RESET_SYNC_NAME rst \
            -RESET_SYNC_ACTIVE high \
            -RESET_ASYNC_NAME arst_n \
            -RESET_ASYNC_ACTIVE low \
            -ENABLE_NAME {} \
            -ENABLE_ACTIVE high \
        } \
    }
directive set /ProjectName -GATE_EFFORT normal
go assembly


directive set /ProjectName/input_1:rsc -MAP_TO_MODULE ccs_ioport.ccs_in
directive set /ProjectName/input_1:rsc -PACKING_MODE sidebyside
directive set /ProjectName/layer5_out:rsc -MAP_TO_MODULE ccs_ioport.ccs_out
directive set /ProjectName/layer5_out:rsc -PACKING_MODE sidebyside
directive set /ProjectName/const_size_in_1:rsc -MAP_TO_MODULE ccs_ioport.ccs_out
directive set /ProjectName/const_size_out_1:rsc -MAP_TO_MODULE ccs_ioport.ccs_out
directive set /ProjectName/core -MAX_LATENCY 8
directive set /ProjectName/core -DESIGN_GOAL Area
flow run /SCVerify/launch_make ./scverify/Verify_orig_cxx_osci.mk {} SIMTOOL=osci sim
go architect
go allocate
go extract
