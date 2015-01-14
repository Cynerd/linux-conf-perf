import os

def pf(rfile):
	"Relative patch of file is decoded to absolute acording to working tree."
	return os.path.dirname(os.path.realpath(__file__)) + '/' + rfile

def checkXf(f, message):
	if os.path.isfile(f) and os.access(f, os.X_OK):
		return f
	else:
		print('Error: Missing executable file "' + f + '"\n' + message,
				file=sys.stderr)
		return None

# Global configs
SRCARCH = 'x86' # Kernel architecture
ARCH = SRCARCH

# Path settings
linux_sources = pf('linux')
linux_kconfig_head = 'Kconfig'

build_folder = pf('build/')
symbol_map_file = build_folder + '/symbol_map' # Also defined in kconfig_parser
rules_file = build_folder + '/rules' # Also defined in kconfig_parser
solved_file = build_folder + '/solved'
required_file = build_folder + '/required'
solution_file = build_folder + '/solution'


kconfig_parser = checkXf(pf('programs/kconfig_parser'),'You must build programs first.')
