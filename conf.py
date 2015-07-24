import os

## Global configs
# kernel_arch
# This defines environment variable ARCH for linux kernel.
# Change this to change target architecture
kernel_arch = 'x86'

# kernle_env
# Enviroment variables for Linux
kernel_env = {'SRCARCH': kernel_arch, 'ARCH': kernel_arch, 'KERNELVERSION': kernel_arch}
# linux_make_args
# These are arguments passed to make when linux is build.
linux_make_args = ['-j8']
# build_command
# Command executed for kernel build in linux folder.
build_command = ['make'] + linux_make_args

# novaboot_args
# These are arguments passed to Novaboot,
# but only if you don't remove it from boot_command.
novaboot_args = ['--qemu=qemu-system-x86_64']
# nbscript
# This variable is path to Novaboot script.
nbscript = 'scripts/nbscript'
# boot_command
# Command executed for booting. Output of this command is saved to output folder.
boot_command = ['scripts/novaboot/novaboot', nbscript] + novaboot_args

# picosat_args
# Arguments passed to PicoSAT.
picosat_args = ['--all']

# db_database
# Database in PostgreSQL to be used for this tools
db_database = 'lcp'
# db_user
# Define PostgreSQL user
db_user = 'cynerd'
# db_password
# Define PostrgreSQL user password
db_password = 'cynerd'
# db_host
# Address of PostgreSQL database server
db_host = 'localhost'
# db_port
# Port of PotgreSQL database server
db_port = 5432

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

## Configs for debugging
step_by_step = False # Executes only single step and exits.
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

buildroot_def_config = 'buildroot_recipe/buildroot.def.config'
buildroot_inittab_directive = 'buildroot_recipe/inittab_directive'
buildroot_initscript = 'scripts/buildroot_recipe/linux-conf-perf'
buildroot_initram = 'buildroot/output/images/rootfs.cpio.gz'

build_folder = 'jobfiles/'
jobfolder_linux_image = build_folder + 'linuxImage'
phase_file = build_folder + 'phase'
symbol_map_file = build_folder + 'symbol_map' # Also defined in parse_kconfig
rules_file = build_folder + 'rules' # Also defined in parse_kconfig
variable_count_file = build_folder + 'variable_count' # Also defined in parse_kconfig
config_map_file = build_folder + 'config_map'
config_solved_file = build_folder + 'config_solved'
required_file = build_folder + 'required'
dot_config_back_file = build_folder + 'dot_config_back'
iteration_file = build_folder + 'iteration'

output_folder = 'output/'
result_folder = 'result/'
log_folder = 'log/'

## Programs paths
parse_kconfig = 'scripts/parse_kconfig/parse'
write_config = 'scripts/write_config/write'
picosat = 'scripts/picosat-959/picosat'


absroot = os.path.dirname(os.path.realpath(__file__))
