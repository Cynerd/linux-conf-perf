import os
from exceptions import MissingFile

# Global configs
SRCARCH = 'x86' # Kernel architecture
ARCH = SRCARCH
linux_make_args = ['-j8']
novaboot_args = ['--qemu=qemu-system-x86_64']

minisat_args = ['-verb=2']
# Programs output show/hide
parse_kconfig_output = False
minisat_output = True
kernel_config_output = True
kernel_make_output = False
boot_output = False

step_by_step = True
#######################################
# Path settings
dot_confmk = '.conf.mk'
benchmark_python = 'benchmark.py'

linux_sources = 'linux/'
linux_kconfig_head = linux_sources + 'Kconfig'
linux_dot_config = linux_sources + '.config'
linux_image = linux_sources + 'arch/' + ARCH + '/boot/bzImage'

build_folder = 'build/'
phase_file = build_folder + 'phase'
symbol_map_file = build_folder + 'symbol_map' # Also defined in parse_kconfig
rules_file = build_folder + 'rules' # Also defined in parse_kconfig
variable_count_file = build_folder + 'variable_count' # Also defined in parse_kconfig
solved_file = build_folder + 'solved'
required_file = build_folder + 'required'
dot_config_fragment_file = build_folder + 'dot_config_fragment'
dot_config_back_file = build_folder + 'dot_config_back'
solution_file = build_folder + 'solution'
iteration_file = build_folder + 'iteration'
output_confs = build_folder + 'output_confs'

output_folder = build_folder + 'output/'

buildroot_def_config = 'scripts/buildroot_recipe/buildroot.def.config'
buildroot_inittab_directive = 'scripts/buildroot_recipe/inittab_directive'
buildroot_initscript = 'scripts/buildroot_recipe/linux-conf-perf'
buildroot_initram = 'scripts/buildroot/output/images/rootfs.cpio.gz'
initram = build_folder + 'initram.gz'

nbscript = 'scripts/nbscript'

# Programs paths
parse_kconfig = 'scripts/parse_kconfig/parse'
write_config = 'scripts/write_config/write'
novaboot = 'scripts/novaboot/novaboot'


absroot = os.path.dirname(os.path.realpath(__file__))
