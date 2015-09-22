import os
import sys
import re
import importlib.machinery

## Global configs
# kernel_arch
# This defines environment variable ARCH for linux kernel.
# Change this to change target architecture
kernel_arch = 'x86'

# kernle_env
# Enviroment variables for Linux
kernel_env = {'SRCARCH': kernel_arch, 'ARCH': kernel_arch, 'KERNELVERSION': kernel_arch}
# build_command
# Command executed for kernel build in linux folder.
build_command = ['make']

# boot_command
# Command executed for booting. Output of this command is saved to output folder.
boot_command = ['echo', 'bootit']
# boot_timeout
# Set timeout of boot process if no output is generated for selected seconds
boot_timeout = 120

# parse_command
# Command to parse double value from boot output
parse_command = ['echo', '0']

# measurement_identifier
# Identifier of measurement can consist of measure tool name and version
measure_identifier = 'cyclictest-v0.92'

# picosat_args
# Additional arguments passed to PicoSAT.
picosat_args = []

# db_database
# Database in PostgreSQL to be used for this tools
db_database = 'linux-conf-perf'
# db_user
# Define PostgreSQL user
db_user = 'user'
# db_password
# Define PostrgreSQL user password
db_password = 'password'
# db_host
# Address of PostgreSQL database server
db_host = 'localhost'
# db_port
# Port of PotgreSQL database server
db_port = 5432

# multithread
# Define if measurement and kernel build should be executed in parallel.
multithread = False
# multithread_buffer
# Defines maximal number of buffered configurations before generating is suspended.
multithread_buffer = 32

# git_describe_cmd
# Command used for getting tools version and status from git
git_describe_cmd = ['git', 'describe', '--always', '--tags', '--dirty']
# git_commit_cmd
# Command used for getting commit hash from git
git_commit_cmd = ['git', 'rev-parse', '--verify', 'HEAD']

## Programs output show/hide
# These options hides output of launched programs from terminal.
# If variable is True, output is printed. Otherwise is hidden.
# What ever are these settings, output is always written to files in folder log.
parse_kconfig_output = False
picosat_output = False
kernel_config_output = True
kernel_make_output = True
boot_output = True
parse_output = False

## Configs for debugging
single_loop = False # Executes only one loop and exits.
only_config = False # Executes only to configuration phase. Building and booting phases are skipped.
ignore_misconfig = False # Ignore if configuration wasn't applied correctly.
#######################################
# Most probably you don't want touch rest of these.
## Path settings
dot_confmk = '.conf.mk'
dot_config = 'dot_config'

linux_sources = 'linux/'
linux_kconfig_head = linux_sources + 'Kconfig'
linux_dot_config = linux_sources + '.config'
linux_image = linux_sources + 'arch/' + kernel_arch + '/boot/bzImage'

build_folder = 'jobfiles/'
jobfolder_linux_image = build_folder + 'linuxImage'
symbol_map_file = build_folder + 'symbol_map' # Also defined in parse_kconfig
rules_file = build_folder + 'rules' # Also defined in parse_kconfig
variable_count_file = build_folder + 'variable_count' # Also defined in parse_kconfig
fixed_file = build_folder + 'fixed'
measure_file = build_folder + 'measure'
dot_measure_file = build_folder + 'dot_measure'
dot_config_back_file = build_folder + 'dot_config_back'
single_generated_file = build_folder + 'single_generated'
measurechecked_file = build_folder + 'measurechecked'

result_folder = 'result/'
log_folder = 'log/'

## Programs paths
parse_kconfig = 'scripts/parse_kconfig/parse'
write_config = 'scripts/write_config/write_config'
picosat = 'scripts/picosat-959/picosat'
allconfig = 'scripts/allconfig/allconfig'

#######################################
absroot = os.path.dirname(os.path.realpath(__file__))

#######################################
# Overlap configuration for specified target
if os.path.isfile(os.path.join(absroot, '.target')):
	target = None
	with open(os.path.join(absroot, '.target'), 'r') as f:
		target = f.readline().rstrip()
	conffile = os.path.join(absroot, 'targets', target, 'conf.py')
	if os.path.isfile(conffile):
		ovconf = importlib.machinery.SourceFileLoader("module.name", conffile).load_module()
		for name in dir(ovconf):
			if not re.match('__*__', name):
				vars()[name] = vars(ovconf)[name]
	else:
		print("E: Invalid target specifier. Write valid target to .target file.")
		sys.exit(-99)
else:
	print("E: No target specifier. Write target to .target file.")
	sys.exit(-99)
