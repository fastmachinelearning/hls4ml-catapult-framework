if {[catch {project load /home/mgraczyk/Desktop/Catapult/projects/$::env(TCL_PROJECT_PATH)} msg]} {
    set_working_dir /home/mgraczyk/Desktop/Catapult/projects/$::env(TCL_PROJECT_NAME)
    project new -name $::env(TCL_PROJECT_NAME)
    project save
}
options set Input/CppStandard c++11
options set Input/TargetPlatform x86_64
solution new -state new
solution file add ./myproject.cpp
solution file set ./myproject.cpp -args "-I/home/mgraczyk/Desktop/Catapult/hls4ml-catapult-template/catapult -I/home/mgraczyk/Desktop/Catapult/projects/$::env(TCL_PROJECT_NAME)/activation_tables"
if {$::env(TEST) == 1} { solution file add ./myproject_test.cpp -exclude true }
go analyze
solution design set myproject -top
go compile
if {$::env(TEST) == 1} { flow run /SCVerify/launch_make ./scverify/Verify_orig_cxx_osci.mk {} SIMTOOL=osci sim }
