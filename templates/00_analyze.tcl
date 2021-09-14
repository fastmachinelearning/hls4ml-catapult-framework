# Put here the code to be executed immediately before running 'go analyze'
# in Catapult.

options set Input/CppStandard c++11
options set Input/TargetPlatform x86_64
solution file add ./myproject.cpp
solution file set ./myproject.cpp -args "-I${MAIN_PATH}/cpp-src/catapult -I${PROJECT_PATH}/activation_tables"
if {$RUN_TEST == 1 || $COMPILE_ONLY == 0} { solution file add ./myproject_test.cpp -exclude true }
