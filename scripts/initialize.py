import os
import sys
import subprocess

import utils
from conf import conf
from exceptions import MissingFile

def kconfig_parser():
	"Execute kconfig_parser in linux_sources directory and parsed output is placed to build_folder."
	env = dict(os.environ)
	env['SRCARCH'] = conf.SRCARCH
	env['ARCH'] = conf.ARCH
	env['KERNELVERSION'] = 'KERNELVERSION' # hides error
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	if conf.kconfig_parser_output:
		subprocess.call([conf.kconfig_parser, conf.linux_kconfig_head, conf.build_folder, "-v", "-v"], env=env)
	else:
		subprocess.call([conf.kconfig_parser, conf.linux_kconfig_head, conf.build_folder], env=env)

	os.chdir(wd)

def gen_requred():
	"Generates required depenpency from required file."
	utils.build_symbol_map()
	srmap = {value:key for key, value in utils.smap.items()}

	try:
		os.remove(conf.required_file)
		os.remove(conf.dot_config_file)
	except OSError:
		pass

	with open(conf.linux_sources + '/.config', 'r') as f:
		with open(conf.required_file, 'w') as freq:
			with open(conf.dot_config_file, 'w') as fconf:
				for line in f:
					if (line[0] == '#') or (not '=' in line):
						continue
					indx = line.index('=')
					if (line[indx + 1] == 'y' or line[indx + 1] == 'm'):
						freq.write(srmap[line[7:indx]] + "\n")
					elif (line[indx + 1] == 'n'):
						freq.write("-" + srmap[line[7:indx]] + "\n")
					else:
						fconf.write(line);
