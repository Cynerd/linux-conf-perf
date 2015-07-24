#!/usr/bin/env python3
import os
import sys
import subprocess
import shutil

import utils
import database
from conf import conf
from conf import sf
import exceptions
import loop
import configurations

def all():
	base()
	parse_kconfig()
	gen_requred()
	try:
		configurations.generate()
	except exceptions.NoSolution:
		pass
	# check if database is initialized
	database.database()

def base():
	print('Initialize base...')
	try:
		os.mkdir(sf(conf.build_folder))
	except FileExistsError:
		pass
	try:
		os.mkdir(sf(conf.configurations_folder))
	except FileExistsError:
		pass
	try:
		os.mkdir(sf(conf.log_folder))
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
	if os.path.isfile(sf(conf.symbol_map_file)) and \
			os.path.isfile(sf(conf.rules_file)) and \
			os.path.isfile(sf(conf.variable_count_file)):
		print('Warning: parse_kconfig not executed. Files already exists.')
		return
	print('Executing parse_kconfig...')
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	parse_kconfig_cmd = [sf(conf.parse_kconfig)]
	parse_kconfig_cmd += [sf(conf.linux_kconfig_head), sf(conf.build_folder)]
	parse_kconfig_cmd += ['-v', '-v']
	utils.callsubprocess("parse_kconfig", parse_kconfig_cmd,
			conf.parse_kconfig_output, env=utils.get_kernel_env())
	os.chdir(wd)


def gen_requred():
	"Generates required depenpency from dot_config file."
	print('Generating required configuration...')

	if not os.path.isfile(sf(conf.dot_config)):
		raise exceptions.MissingFile(sf(conf.dot_config),
				'Generate fixed configuration. Use make dot_config.')

	utils.build_symbol_map() # Ensure smap existence
	srmap = {value:key for key, value in utils.smap.items()} # swap dictionary

	shutil.copy(sf(conf.dot_config), sf(conf.dot_config_back_file))

	with open(sf(conf.dot_config), 'r') as f:
		with open(sf(conf.required_file), 'w') as freq:
			for line in f:
				if (line[0] == '#') or (not '=' in line):
					continue
				indx = line.index('=')
				if (line[indx + 1] == 'y'):
					if line[7:indx] == "MODULES": # exception if modules set
						raise exceptions.ConfigurationError("Fixed kernel configuration must have MODULES disabled.")
					freq.write(str(srmap[line[7:indx]]) + "\n")
				elif (line[indx + 1] == 'n' or line[indx + 1] == 'm'):
					freq.write("-" + str(srmap[line[7:indx]]) + "\n")


#################################################################################

if __name__ == '__main__':
	all()
