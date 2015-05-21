import os

## Global configs
# ARCH
# This defines environment variable for linux kernel.
# Change this to change target architecture
ARCH = 'x86'
# gen_all_solutions_oninit
# If True, all solutions are generated at initalization.
# If False, every loop is generated one solution.
gen_all_solution_oninit = True # If True, all solutions are generated at initialization.

# kernle_env
# Enviroment variables for Linux
kernel_env = {'SRCARCH': ARCH, 'ARCH': ARCH, 'KERNELVERSION': ARCH}
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
picosat_args = []

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
linux_image = linux_sources + 'arch/' + ARCH + '/boot/bzImage'

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
solved_file = build_folder + 'solved'
required_file = build_folder + 'required'
dot_config_fragment_file = build_folder + 'dot_config_fragment'
dot_config_back_file = build_folder + 'dot_config_back'
solution_file = build_folder + 'solution'
iteration_file = build_folder + 'iteration'
output_confs = build_folder + 'output_confs'

output_folder = 'output/'
result_folder = 'result/'
log_folder = 'log/'

## Programs paths
parse_kconfig = 'scripts/parse_kconfig/parse'
write_config = 'scripts/write_config/write'
picosat = 'scripts/picosat-959/picosat'


absroot = os.path.dirname(os.path.realpath(__file__))
