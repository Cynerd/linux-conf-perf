# linux-conf-perf
This is tool for automatic performance analysis.
It calculates impact on performance per symbol.

TOOL IS IN DEVELOPMENT AND NOT WORKING PROPERLY

### Folders
* `scripts` In this folder are all python scripts
	* `pase_kconfig` Contains program for extracting dependences from Kconfig files to format for PicoSAT.
	* `write_config` Fixes configuration and compares it against generated one.
	* `permute_conf` Interactive program for setting measure base.
	* `shared` Shared files
	* `novaboot` Script Novaboot
	* `picosat-959` Program PicoSAT
* `jobfiles` In this folder are placed files generated with local scripts.
* `output` Contains output of executed boots.
* `result` In this folder are expected to be files with values from benchmarks.
* `log` To this folder are logged all program outputs.
* `linux` Submodule of linux kernel
* `buildroot` Submodule of Buildroot

### Configuration files
Main configuration is in file `conf.py` and has python syntax.
Important settings are commented inside file.
Configuration must be done before executing `make run` or `make init`.

### Usage
Before any other steps, create Linux configuration and setup boot target with benchmark.
You can run `make test` for testing if setup is correct.

For configuration of measured symbols, run `make mpermute_conf`.

To execute tests, run `make run`.

Output of benchmarks must be analyzed after execution of all different configurations.
Files in folder `output` contains benchmark outputs. You should ensure, that values for
benchmark are extracted and saved to folder `result` to file with same name as source.

Final evaluation is done by `make evaluate`.
