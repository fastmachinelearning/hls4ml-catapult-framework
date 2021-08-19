set MAIN_PATH $::env(MAIN_PATH)
set ALL_PROJECTS_PATH $MAIN_PATH/projects

set PROJECT_NAME $::env(PROJECT_NAME)
set PROJECT_PATH $ALL_PROJECTS_PATH/$PROJECT_NAME
set PROJECT_FILE_PATH $PROJECT_PATH/$PROJECT_NAME.ccs

set RUN_TEST $::env(TEST)

if {[catch {project load $PROJECT_FILE_PATH} msg]} {
    set_working_dir $PROJECT_PATH
    project new -name $PROJECT_NAME
    project save
}

source $PROJECT_PATH/tcl/00_analyze.tcl
go analyze

source $PROJECT_PATH/tcl/01_compile.tcl
go compile

if {$RUN_TEST == 1} { flow run /SCVerify/launch_make ./scverify/Verify_orig_cxx_osci.mk {} SIMTOOL=osci sim }
