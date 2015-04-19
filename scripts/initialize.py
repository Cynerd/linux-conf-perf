import os
import sys
import subprocess
import shutil

import utils
from conf import conf
from exceptions import MissingFile

def parse_kconfig():
	"Execute parse_kconfig in linux_sources directory and parsed output is placed to build_folder."
	env = dict(os.environ)
	env['SRCARCH'] = conf.SRCARCH
	env['ARCH'] = conf.ARCH
	env['KERNELVERSION'] = 'KERNELVERSION' # hides error
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	if conf.parse_kconfig_output:
		subprocess.call([conf.parse_kconfig, conf.linux_kconfig_head, conf.build_folder, "-v", "-v"], env=env)
	else:
		subprocess.call([conf.parse_kconfig, conf.linux_kconfig_head, conf.build_folder], env=env)

	os.chdir(wd)

def gen_requred():
	"Generates required depenpency from required file."

	if not os.path.isfile(conf.linux_sources + '/.config'):
		raise MissingFile(conf.linux_sources + '/.config',
				'Generate initial configuration. Execute make defconfig in linux folder. Or use make menuconfig and change configuration.')

	utils.build_symbol_map() # Ensure smap existence
	srmap = {value:key for key, value in utils.smap.items()}

	try:
		os.remove(conf.required_file)
		os.remove(conf.dot_config_fragment_file)
	except OSError:
		pass

	shutil.copy(conf.linux_dot_config, conf.dot_config_back_file)

	with open(conf.linux_sources + '/.config', 'r') as f:
		with open(conf.required_file, 'w') as freq:
			with open(conf.dot_config_fragment_file, 'w') as fconf:
				for line in f:
					if (line[0] == '#') or (not '=' in line):
						continue
					indx = line.index('=')
					if (line[7:indx] == "MODULES"): # skip if modules set
						raise exceptions.ConfigurationError("Initial kernel configuration must have MODULES disabled.")
					if (line[indx + 1] == 'y'):
						freq.write(srmap[line[7:indx]] + "\n")
					elif (line[indx + 1] == 'n' or line[indx + 1] == 'm'):
						freq.write("-" + srmap[line[7:indx]] + "\n")
					else:
						fconf.write(line);
			freq.write("-" + srmap["MODULES"] + "\n"); # force modules no
