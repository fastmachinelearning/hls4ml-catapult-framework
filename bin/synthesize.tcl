set MAIN_PATH $::env(MAIN_PATH)
set ALL_PROJECTS_PATH $MAIN_PATH/projects

set PROJECT_NAME $::env(PROJECT_NAME)
set PROJECT_PATH $ALL_PROJECTS_PATH/$PROJECT_NAME
set PROJECT_FILE_PATH $PROJECT_PATH/$PROJECT_NAME.ccs

set RUN_TEST $::env(TEST)
set COMPILE_ONLY $::env(COMPILE_ONLY)

foreach var [array names env] {
    if {[string match "HLS_*" $var]} {
        set [string range $var 4 [expr {[string length $var] - 1}]] $::env($var)
    }
}

if {[catch {project load $PROJECT_FILE_PATH} msg]} {
    set_working_dir $PROJECT_PATH
    project new -name $PROJECT_NAME
    project save

    solution new -state new -version v1 dummy
}

if {[catch {solution new -state new -version v1 synthesize} msg]} {
    solution remove -solution synthesize.v1 -delete
    solution new -state new -version v1 synthesize
}

source $PROJECT_PATH/tcl/00_analyze.tcl
go analyze

source $PROJECT_PATH/tcl/01_compile.tcl
go compile

if {$RUN_TEST == 1} {
    flow run /SCVerify/launch_make ./scverify/Verify_orig_cxx_osci.mk {} SIMTOOL=osci sim
}

if {$COMPILE_ONLY == 0} {
    source $PROJECT_PATH/tcl/02_libraries.tcl
    go libraries

    directive set REGISTER_THRESHOLD 10240
    source $PROJECT_PATH/tcl/03_assembly.tcl
    go assembly

    source $PROJECT_PATH/tcl/04_architect.tcl
    go architect

    source $PROJECT_PATH/tcl/05_schedule.tcl
    go schedule

    go extract

    solution report -timing true -transcript true
}
