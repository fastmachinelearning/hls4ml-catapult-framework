options set Input/CppStandard c++11
options set Input/TargetPlatform x86_64
solution file add ./myproject.cpp
solution file set ./myproject.cpp -args "-I${MAIN_PATH}/hls4ml-catapult-template/catapult -I${PROJECT_PATH}/activation_tables"
if {$RUN_TEST == 1} { solution file add ./myproject_test.cpp -exclude true }
