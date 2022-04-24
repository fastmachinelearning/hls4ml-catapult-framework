# When calling this TCL script, the following environmental vars must be set:
# - MAIN_PATH: the absolute path to a Catapult-related working directory (i.e.
# the one with 'bin' containing the Catapult scripts, 'projects' etc.)
# - PROJECT_NAME: the name of an hls4ml Catapult project to be processed (i.e.
# the name of an appropriate directory inside 'projects')
# - RUN_TEST: whether a C++ testbench should be run (0 if no, 1 if yes)
# - COMPILE_ONLY: whether only the first two steps of the Catapult flow should
# be executed (0 if no, 1 if yes; if no, the entire Catapult flow will be run)

# Set necessary variables.
set MAIN_PATH $::env(MAIN_PATH)
set ALL_PROJECTS_PATH $MAIN_PATH/projects

set PROJECT_NAME $::env(PROJECT_NAME)
set PROJECT_PATH $ALL_PROJECTS_PATH/$PROJECT_NAME
set PROJECT_FILE_PATH $PROJECT_PATH/$PROJECT_NAME.ccs

set RUN_TEST $::env(TEST)
set COMPILE_ONLY $::env(COMPILE_ONLY)

# Replace HLS_* variables with the ones without the HLS_ prefix. These can be
# then accessed by other TCL files in a project.
foreach var [array names env] {
    if {[string match "HLS_*" $var]} {
        set [string range $var 4 [expr {[string length $var] - 1}]] $::env($var)
    }
}

# Try to load an existing Catapult project. In case of failure, create a new one.
if {[catch {project load $PROJECT_FILE_PATH} msg]} {
    set_working_dir $PROJECT_PATH
    project new -name $PROJECT_NAME
    project save

    # Create a dummy solution so that the project has at least 1 solution. This
    # is necessary for the code marked with (*) to run properly in all cases.
    solution new -state new -version v1 dummy
}

# Try to create a completely new solution named synthesize.v1. In case of
# failure, remove the already-existing solution and try again.
if {[catch {solution new -state new -version v1 synthesize} msg]} {
    # (*) starts
    solution remove -solution synthesize.v1 -delete
    # (*) ends
    
    solution new -state new -version v1 synthesize
}

# Run the first step of the Catapult flow.
source $PROJECT_PATH/tcl/00_analyze.tcl
go analyze

# Run the second step of the Catapult flow.
source $PROJECT_PATH/tcl/01_compile.tcl
go compile

# Run a C++ testbench if RUN_TEST is 1.
if {$RUN_TEST == 1} {
    flow run /SCVerify/launch_make ./scverify/Verify_orig_cxx_osci.mk {} SIMTOOL=osci sim
}

# Run other steps of the Catapult flow if COMPILE_ONLY is 0.
if {$COMPILE_ONLY == 0} {
    # Run the third step of the Catapult flow.
    source $PROJECT_PATH/tcl/02_libraries.tcl
    go libraries

    # Run the fourth step of the Catapult flow, having set REGISTER_THRESHOLD to
    # a high value beforehand. Setting REGISTER_THRESHOLD is necessary for
    # models to be processed correctly without adding extra memory libraries.
    directive set REGISTER_THRESHOLD 1024000
    source $PROJECT_PATH/tcl/03_assembly.tcl
    go assembly

    # Run the fifth step of the Catapult flow.
    source $PROJECT_PATH/tcl/04_architect.tcl
    go architect

    # Run the sixth step of the Catapult flow.
    source $PROJECT_PATH/tcl/05_schedule.tcl
    go schedule

    # Run the seventh step of the Catapult flow (RTL generation).
    go extract

    # Run cosimulation using QuestaSIM.
    flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_rtl_vhdl_msim.mk {} SIMTOOL=msim sim

    # Run power estimation if the target device is an ASIC. Otherwise,
    # print a timing report to be parsed by the 'syntheize' script.
    if {$RTL_SYN_TOOL == "DesignCompiler" || $RTL_SYN_TOOL == "OasysRTL" ||
        $RTL_SYN_TOOL == "RTLCompiler"} {
        flow package option set /LowPower/SWITCHING_ACTIVITY_TYPE saif
        
        go switching
        flow run /PowerAnalysis/report_pre_pwropt_VHDL

        # Print a timing and power report to be parsed by the 'synthesize'
        # script.
        solution report -timing true -transcript true -power_report true
    } else {
        solution report -timing true -transcript true
    }
}
