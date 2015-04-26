import os
from exceptions import MissingFile

def pf(rfile):
	"Relative patch of file is decoded to absolute acording to working tree."
	return os.path.dirname(os.path.realpath(__file__)) + '/' + rfile

# Global configs
SRCARCH = 'x86' # Kernel architecture
ARCH = SRCARCH
linux_make_args = ['-j8']

# Path settings
dot_confmk = pf('.conf.mk')

linux_sources = pf('linux')
linux_kconfig_head = 'Kconfig'
linux_dot_config = linux_sources + '/.config'
linux_image = linux_sources + '/arch/' + ARCH + '/boot/bzImage'

build_folder = pf('build/')
phase_file = build_folder + '/phase'
symbol_map_file = build_folder + '/symbol_map' # Also defined in kconfig_parser
rules_file = build_folder + '/rules' # Also defined in kconfig_parser
solved_file = build_folder + '/solved'
required_file = build_folder + '/required'
dot_config_fragment_file = build_folder + '/dot_config_fragment'
dot_config_back_file = build_folder + '/dot_config_back'
solution_file = build_folder + '/solution'
iteration_file = build_folder + '/iteration'

buildroot_def_config = pf('scripts/buildroot.def.config')
buildroot_initram = pf('scripts/buildroot/output/images/rootfs.cpio.gz')
initram = build_folder + '/initram.gz'

# Programs paths
parse_kconfig = pf('scripts/parse_kconfig/parse')
write_config = pf('scripts/write_config/write')

# Programs output show/hide
parse_kconfig_output = False
minisat_output = False
kernel_config_output = True
kernel_make_output = True
