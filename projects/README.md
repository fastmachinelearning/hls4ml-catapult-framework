## 'projects' directory
All hls4ml Catapult project directories should be put here. Unless the scripts have been changed, the name of every folder should follow one of the patterns below:
* `DenseXXXy`, where `XXX` is a number with **exactly** 3 digits (if the number has less than 3 digits, leading zeroes must be added) and `y` is any string. This corresponds to a standard (i.e. non-stress) model.
* `SDenseXXXy`, where `XXX` is a number with **exactly** 3 digits (if the number has less than 3 digits, leading zeroes must be added) and `y` is any string. This corresponds to a stress model (which is processed when `compile`/`synthesize` is run with the `--stress` flag).
* `EntireTemplate`. This corresponds to a special project designed for checking the compile-time correctness of the C++ part of the Catapult backend. The project ID here is always 0.

### Project structure
Every project directory must have the following contents:
* `activation_tables`: a folder with header files containing pre-computed activation tables (usually produced by `cconvert`).
* `myproject.cpp`: a C++ source code file with the main function.
* `myproject_test.cpp`: a C++ source code file calling a testbench.
* `tb_data`: a folder with `tb_input_features.dat` containing test inputs and `tb_output_predictions.dat` containing expected outputs (usually produced by `cconvert`).
* `tcl`: a folder with TCL files (see the next section for the details).

`EntireTemplate` (provided here) is an exception to this rule.

### TCL files
Every project must have the following TCL files in the `tcl` directory:
* `00_analyze.tcl`: executed immediately before `go analyze` in Catapult.
* `01_compile.tcl`: executed immediately before `go compile` in Catapult.
* `02_libraries.tcl`: executed immediately before `go libraries` in Catapult.
* `03_assembly.tcl`: executed immediately before `go assembly` in Catapult.
* `04_architect.tcl`: executed immediately before `go architect` in Catapult.
* `05_schedule.tcl`: executed immediately before `go schedule` in Catapult.

All TCL scripts can have Catapult-specific commands.

#### Variables
In any of the TCL files, you can use variables whose values may then be provided by the `--settings` option in `synthesize`.

For example, if you use variables named `TEST1`, `TEST2` and `TEST3`, you can specify their values later on by running `synthesize` with the `--settings TEST1=VALUE1,TEST2=VALUE2,TEST3=VALUE3` option, where VALUE1, VALUE2 and VALUE3 are values of your choice.

Always make sure that all variables have a default value in case they're not specified in the `synthesize` command. This can be done in the following way inside a TCL script:
```
if {![info exists <VARIABLE_NAME>]} {
   set <VARIABLE_NAME> <DEFAULT_VALUE>
}
```


You must pay special attention to `RTL_SYN_TOOL` in `02_libraries.tcl`. Its default value is used by `synthesize` for determining what RTL tool should be used for RTL synthesis. Therefore, you should not remove or rename that variable unless `synthesize` has been changed.
