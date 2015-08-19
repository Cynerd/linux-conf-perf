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

def all():
	base()
	parse_kconfig()
	gen_fixed()
	# check if database is initialized
	database.database()

def base():
	print('Initialize base...')
	try:
		os.mkdir(sf(conf.build_folder))
	except FileExistsError:
		pass
	try:
		os.mkdir(sf(conf.log_folder))
	except FileExistsError:
		pass

def parse_kconfig():
	"Execute parse_kconfig in linux_sources directory."
	if os.path.isfile(sf(conf.symbol_map_file)) and \
			os.path.isfile(sf(conf.rules_file)) and \
			os.path.isfile(sf(conf.variable_count_file)):
		print('Warning: parse_kconfig not executed. Files already exists.')
		return
	print('Executing parse_kconfig...')
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	parse_kconfig_cmd = [sf(conf.parse_kconfig)]
	parse_kconfig_cmd += [sf(conf.linux_kconfig_head), sf(conf.build_folder)]
	parse_kconfig_cmd += ['-v', '-v']
	utils.callsubprocess("parse_kconfig", parse_kconfig_cmd,
			conf.parse_kconfig_output, env=utils.get_kernel_env())
	os.chdir(wd)


def __gen_allconfig_fixed__():
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	allconfig_cmd = [sf(conf.allconfig)]
	allconfig_cmd += ['Kconfig', sf(conf.dot_config), sf(conf.dot_measure_file)]
	allconfig_cmd += ['--inv']
	utils.callsubprocess("allconfig_fixed", allconfig_cmd, False,
			env = utils.get_kernel_env())
	os.chdir(wd)

def gen_fixed():
	"Generates fixed depenpency from dot_config file."
	print('Generating required configuration...')

	if not os.path.isfile(sf(conf.dot_config)):
		raise exceptions.MissingFile(sf(conf.dot_config),
				'Generate fixed configuration. Use make dot_config.')

	utils.build_symbol_map() # Ensure smap existence
	srmap = {value:key for key, value in utils.smap.items()} # swap dictionary

	shutil.copy(sf(conf.dot_config), sf(conf.dot_config_back_file))
	__gen_allconfig_fixed__()

	with open(sf(conf.dot_config), 'r') as f:
		with open(sf(conf.fixed_file), 'w') as ffix:
			for line in f:
				if (line[0] == '#') or (not '=' in line):
					continue
				indx = line.index('=')
				if (line[indx + 1] == 'y'):
					if line[7:indx] == "MODULES": # exception if modules set
						raise exceptions.ConfigurationError("Fixed kernel configuration must have MODULES disabled.")
					ffix.write(str(srmap[line[7:indx]]) + "\n")
				elif (line[indx + 1] == 'n' or line[indx + 1] == 'm'):
					ffix.write("-" + str(srmap[line[7:indx]]) + "\n")
	with open(sf(conf.dot_measure_file), 'r') as f:
		with open(sf(conf.measure_file), 'w') as fmes:
			for line in f:
				if (line[0] == '#') or (not '=' in line):
					continue
				indx = line.index('=')
				if line[7:indx] == "MODULES":
					raise exceptions.ConfigurationError("Can't measure configuraion option MODULES. Not supported.")
				fmes.write(str(srmap[line[7:indx]]) + "\n")


#################################################################################

if __name__ == '__main__':
	all()
