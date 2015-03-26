import os

def pf(rfile):
	"Relative patch of file is decoded to absolute acording to working tree."
	return os.path.dirname(os.path.realpath(__file__)) + '/' + rfile

def checkXf(f, message):
	"Check if file is executable. If not, raise MissingFile exception."
	if os.path.isfile(f) and os.access(f, os.X_OK):
		return f
	else:
		raise MissingFile(f, message)

# Global configs
SRCARCH = 'x86' # Kernel architecture
ARCH = SRCARCH
linux_make_args = ['-j8']

# Path settings
linux_sources = pf('linux')
linux_kconfig_head = 'Kconfig'

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

# Programs paths
kconfig_parser = checkXf(pf('programs/kconfig_parser'), 'You must build programs first.')

# Programs output hide
kconfig_parser_output = False
minisat_output = False
kernel_config_output = False
