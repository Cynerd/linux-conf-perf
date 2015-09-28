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
import configurations

def all():
	try:
		utils.dirtycheck()
	except exceptions.DirtyRepository as e:
		print("Warning: " + str(e))
	base()
	parse_kconfig()
	gen_fixed()
	checkmeasure()
	database.database() # check if database is initialized

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
	parse_kconfig_cmd += ['Kconfig', sf(conf.build_folder)]
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

def checkmeasure():
	if os.path.isfile(sf(conf.measurechecked_file)):
		print("Checking if all configurations can be measured skiped.")
		print("  For new check remove file " + sf(conf.measurechecked_file))
		return
	print("Checking if all configurations can be measured...")
	utils.build_symbol_map()
	measure_list = set()
	with open(sf(conf.variable_count_file)) as f:
		var_num = f.readline().rstrip()
		conf_num = f.readline().rstrip()
	with open(sf(conf.measure_file), 'r') as fi:
		for ln in fi:
			measure_list.add(int(ln))
	for measure in measure_list:
		tfile1 = configurations.__buildtempcnf__(var_num, (sf(conf.rules_file),
			sf(conf.fixed_file)), [str(measure)])
		tfile2 = configurations.__buildtempcnf__(var_num, (sf(conf.rules_file),
			sf(conf.fixed_file)), [str(-1 * measure)])
		try:
			configurations.__exec_sat__(tfile1, [], conf_num)
		except exceptions.NoSolution:
			print("W: " + utils.smap[measure] + " won't be measured! Can't select.")
		try:
			configurations.__exec_sat__(tfile2, [], conf_num)
		except exceptions.NoSolution:
			print("W: " + utils.smap[measure] + " won't be measured! Can't unselect.")
	with open(sf(conf.measurechecked_file), 'w') as f:
		f.write("Remove this file if you wanna execute check if all configurations can be measured once again.\n")

#################################################################################

if __name__ == '__main__':
	all()
