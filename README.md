# hls4ml-catapult-framework
A framework for developing the hls4ml Catapult backend.

## Getting started
1. Clone this repository.
2. Put a Singularity image containing the Catapult software to the `bin` directory, naming the file `catapult.sif`.
3. Add the `bin` directory to your `PATH` environmental variable.
4. Done! You can now run `catapult`, `cconvert`, `compile` and `synthesize` commands from your terminal. If it's your first time with the framework, please continue reading this README.

## Directory structure
`hls4ml-catapult-framework` executes all of its operations inside the directory where you have cloned the repository (regardless of where you actually run the commands mentioned in "Getting started"). The folder is structured as follows:
* `bin`: all executable scripts/tools are stored here.
* `cpp-src`: the C++ code of the hls4ml Catapult backend is stored here.
* `projects`: all hls4ml Catapult projects are stored here.
* `templates`: all testbench and TCL templates used by the framework are stored here.
* `compile-logs`: the most recent logs produced by `compile` are stored here. This directory is created automatically by `compile`, you don't have to make it manually.
* `synthesize-logs`: the most recent logs produced by `synthesize` are stored here. This directory is created automatically by `synthesize`, you don't have to make it manually.

**You should not rename any of those directories if you want to be sure that everything works correctly.**

## hls4ml Catapult projects
The framework works with hls4ml Catapult projects stored inside the `projects` directory. Every project is a folder following strictly the rules described in `projects/README.md`. The examples are provided (which can also be used for the Catapult backend R&D).

Currently, the recommended way of making a new hls4ml Catapult project is converting its Vivado HLS equivalent by running `cconvert`. The following section describes how to use it.

## Tools
The framework has the following tools/scripts that can be used: `catapult`, `cconvert`, `compile` and `synthesize`.

### catapult
This is a simple Bash script starting the Catapult software. Any arguments provided to it are passed to Catapult.

`catapult` assumes that Vivado is installed in the `/opt/Xilinx` directory. If that's not the case, please update line 10 of the script accordingly.

### cconvert
This is a Python 3 tool converting an hls4ml Vivado HLS project to its Catapult equivalent, which can be then used by `hls4ml-catapult-framework`. During the conversion process, activation tables are pre-computed, random test inputs are generated and expected test outputs are produced.

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

For more details and available options, you can run either `cconvert -h` or `cconvert --help`.

### compile
This is a Python 3 tool running the first two steps of the Catapult flow (i.e. `go analyze` and `go compile`) along with running a C++ testbench where applicable. Use `compile` when you want to check the functional correctness of your project before HLS synthesis.

The basic usage is:
```
compile <SPACE-SEPARATED IDs OF THE PROJECTS TO BE PROCESSED>
```
A project ID is the `XXX` part of `DenseXXXy`/`SDenseXXXy` (unless the scripts/tools have been changed). Leading zeroes may be omitted. If no IDs are provided, all projects inside the `projects` directory will be processed.

If you're not sure what `DenseXXXy`/`SDenseXXXy` means, see `projects/README.md`.
