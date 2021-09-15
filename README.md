# hls4ml-catapult-framework
A framework for developing the hls4ml Catapult backend.

## Getting started
1. Make sure that Singularity and Python 3.6+ are present in your setup.
2. Clone this repository.
3. Put a Singularity image containing **both** Catapult and QuestaSIM to the `bin` directory, naming the file `catapult.sif`.
4. Add the `bin` directory to your `PATH` environmental variable.
5. Done! You can start working with `hls4ml-catapult-framework`. If it's your first time with the framework, please continue reading this README and read `projects/README.md`.

## Directory structure
`hls4ml-catapult-framework` always executes all of its operations inside the directory where you have cloned the repository. The folder is structured as follows:
* `bin`: all executable scripts/tools are stored here.
* `cpp-src`: the C++ code of the hls4ml Catapult backend is stored here.
* `projects`: all hls4ml Catapult projects are stored here.
* `templates`: all testbench and TCL templates used by the framework are stored here.
* `compile_logs`: the most recent logs produced by `compile` are stored here. This directory is created automatically by `compile`, you don't have to make it manually.
* `synthesize_logs`: the most recent logs produced by `synthesize` are stored here. This directory is created automatically by `synthesize`, you don't have to make it manually.

**You should not rename any of those directories if you want to be sure that everything works correctly.**

## hls4ml Catapult projects
The framework works with hls4ml Catapult projects stored inside the `projects` directory. Every project is a folder following strictly the rules described in `projects/README.md`. The examples are provided (which can also be used for the Catapult backend R&D).

Currently, the recommended way of making a new hls4ml Catapult project is converting its Vivado HLS equivalent by running `cconvert`. The following section describes how to use it.

## Tools
The framework has the following tools/scripts that can be used: `catapult`, `cconvert`, `compile` and `synthesize`. If you have followed the "Getting started" instructions carefully, you will be able to run these commands anywhere in your terminal (the tools/scripts will still do their activities in the directory where you have cloned the repository).

### catapult
This is a simple Bash script starting the Catapult software. Any arguments provided to it are passed to Catapult.

`catapult` assumes that Vivado is installed in the `/opt/Xilinx` directory. If that's not the case, please update line 10 of the script accordingly.

### cconvert
This is a Python tool converting an hls4ml Vivado HLS project to its Catapult equivalent, which can be then used by `hls4ml-catapult-framework`. During the conversion process, activation tables are pre-computed, random test inputs are generated and expected test outputs are produced by running the Vivado HLS testbench.

The basic usage is:
```
cconvert <PATH TO hls4ml VIVADO HLS PROJECT DIRECTORY> <PATH TO hls4ml CATAPULT PROJECT DIRECTORY TO BE CREATED>
```
where an hls4ml Vivado HLS project directory is the one containing `hls4ml_config.yml`.

For example, running:
```
cconvert /home/abc/hls4ml/project1 /home/abc/hls4ml-catapult-framework/projects/Dense001
```
will convert the hls4ml Vivado HLS project stored in `/home/abc/hls4ml/project1` to its Catapult equivalent and save it to `/home/abc/hls4ml-catapult-framework/projects/Dense001`.

**`cconvert` doesn't create Catapult project files automatically. This means you need to run `compile` or `synthesize` after `cconvert` if you want to work with the new project directly in Catapult.**

For more details and available options, you can run either `cconvert -h` or `cconvert --help`.

### compile
This is a Python tool running the first two steps of the Catapult flow (i.e. `go analyze` and `go compile`) along with executing a C++ testbench where applicable. Use `compile` when you want to check the functional correctness of your project before HLS synthesis.

The basic usage is:
```
compile <SPACE-SEPARATED IDs OF THE PROJECTS TO BE PROCESSED>
```
A project ID is the `XXXy` part of `DenseXXXy`/`SDenseXXXy` (unless the scripts/tools have been changed). Leading zeroes may be omitted. If no IDs are provided, all projects inside the `projects` directory will be processed.

If you're not sure what `DenseXXXy`/`SDenseXXXy` means, see `projects/README.md`.

For example, running:
```
compile 1 2 3Q
```
will run the first two steps of the Catapult flow for `Dense001`, `Dense002` and `Dense003Q`.

Catapult outputs are caught by `compile` and saved to a log file in the `compile_logs` directory. `compile_logs` stores only logs from the most recent run of `compile`.

The tool automatically parses the results of running a C++ testbench and presents them in a way where a judgement can be made quickly (i.e. the largest relative error between expected and actual results). However, all testing results should be eventually checked manually by inspecting the produced log file.

For more details and available options, you can run either `compile -h` or `compile --help`.

### synthesize
This is a Python tool running the entire Catapult flow along with executing a QuestaSIM cosimulation and optionally running an RTL synthesis process. It doesn't run a C++ testbench and parse its results like `compile` does.

The basic usage is:
```
synthesize <SPACE-SEPARATED IDs OF THE PROJECTS TO BE PROCESSED>
```
Project IDs are parsed in the same way as in `compile`. However, at least 1 ID **must** be provided. If you want to process all projects inside the `projects` directory, put `all` instead of IDs.

If you want to run an RTL synthesis process after synthesising a project, run the command with the `--rtl-synthesize` flag.

For example, running:
```
synthesize 1 2 3Q --rtl-synthesize
```
will run the entire Catapult flow along with RTL synthesis for `Dense001`, `Dense002` and `Dense003Q`.

Catapult and RTL tool outputs are caught by `synthesize` and saved to a log file in the `synthesize_logs` directory. `synthesize_logs` stores only logs from the most recent run of `synthesize`.

The tool automatically parses the results of running the flow, cosimulation and optionally RTL synthesis and prints the following information:
* Predicted latency and throughput
* Cosimulated latency and throughput
* Overall cosimulation testing results (OK / problem likely)
* FPGA resource usage (if RTL synthesis is run)

The RTL tool used for RTL synthesis is determined automatically by inspecting `02_libraries.tcl` in a project. The choice can be overridden by running the command with the `--settings RTL_SYN_TOOL=<tool>` option. **Only Vivado is supported at the moment.** Moreover, `synthesize` assumes that the path to the RTL tool is already present in your `PATH` environmental variable.

You can quickly change variable values used by TCL files in a project by running the command with the `--settings K1=V1,K2=V2,K3=V3,...` option, where K1, K2, K3 etc. are variable names and V1, V2, V3 etc. are values of your choice. See `projects/README.md` to get to know how the variables work.

For more details and available options, you can run either `synthesize -h` or `synthesize --help`.

## Issues
If there is something you don't understand or you have a problem with using the framework, feel free to post an issue in "Issues".

## Contributing
All contributions are welcome! Please fork the repository, make your changes and create a pull request.

## License
(to be determined)
