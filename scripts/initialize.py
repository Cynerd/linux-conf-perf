#!/bin/env python3
import os
import sys
import subprocess
import shutil

import utils
from conf import conf
from conf import sf
import exceptions
import loop

def all():
	base()
	parse_kconfig()
	gen_requred()
	gen_nbscript()

def base():
	try: os.mkdir(conf.build_folder)
	except FileExistsError:
		pass

	if os.path.isfile(sf(conf.phase_file)):
		print("Warning: file " + conf.phase_file + " already exists. Not overwritten.")
	else:
		loop.phase_set(1)

	if os.path.isfile(sf(conf.iteration_file)):
		print("Warning: file " + conf.iteration_file + " already exists. Not overwritten.")
	else:
		loop.iteration_reset()


def parse_kconfig():
	"Execute parse_kconfig in linux_sources directory."
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.parse_kconfig_output:
		subprocess.call([sf(conf.parse_kconfig), sf(conf.linux_kconfig_head), sf(conf.build_folder), "-v", "-v"], env=utils.get_kernel_env())
	else:
		subprocess.call([sf(conf.parse_kconfig), sf(conf.linux_kconfig_head), sf(conf.build_folder)], env=utils.get_kernel_env())

	os.chdir(wd)


def gen_requred():
	"Generates required depenpency from .config file in linux source tree."

	if not os.path.isfile(sf(conf.linux_dot_config)):
		raise exceptions.MissingFile(sf(conf.linux_dot_config),
				'Generate initial configuration. Execute make defconfig in linux folder. Or use make menuconfig and change configuration.')

	utils.build_symbol_map() # Ensure smap existence
	srmap = {value:key for key, value in utils.smap.items()}

	try:
		os.remove(sf(conf.required_file))
		os.remove(sf(conf.dot_config_fragment_file))
	except OSError:
		pass

	shutil.copy(sf(conf.linux_dot_config), sf(conf.dot_config_back_file))

	with open(sf(conf.linux_dot_config), 'r') as f:
		with open(sf(conf.required_file), 'w') as freq:
			with open(sf(conf.dot_config_fragment_file), 'w') as fconf:
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


def gen_nbscript():
	if os.path.isfile(sf(conf.nbscript)):
		print("Warning: file " + conf.nbscript +
				" already exists. Generation skipped.")
		return

	with open(sf(conf.nbscript), 'w') as f:
		f.write('# generated novaboot script. Please don\'t edit.\n')
		f.write('load ' + sf(conf.linux_image) + ' console=ttyS0,115200\n')
		f.write('load ' + sf(conf.buildroot_initram) + '\n')

#################################################################################

if __name__ == '__main__':
	all()
