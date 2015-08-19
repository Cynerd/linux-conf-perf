import os
import sys
import tempfile
import shutil
import subprocess
import time
import hashlib

import utils
from conf import conf
from conf import sf
import exceptions
import database

def __buildtempcnf__(variable_count, files, strlines):
	""" Builds temporally file for cnf formulas
	  variable_count - number of variables in formulas
	  files          - list of files with formulas
	  strlines       - list of string lines with formulas"""
	lines = set()
	# Copy strlines
	for ln in strlines:
		lines.add(ln)
	# Files
	for file in files:
		with open(file, 'r') as f:
			for ln in f:
				lines.add(ln.rstrip())

	first_line = "p cnf " + str(variable_count) + " " + str(len(lines))

	wfile = tempfile.NamedTemporaryFile(delete=False)
	wfile.write(bytes(first_line + '\n', 'UTF-8'))
	for ln in lines:
		wfile.write(bytes(ln + ' 0\n', 'UTF-8'))
	wfile.close()
	return wfile.name

def __exec_sat__(file, args, conf_num):
	"""Executes SAT solver and returns configuration."""
	picosat_cmd = [sf(conf.picosat), file]
	picosat_cmd += conf.picosat_args
	stdout = utils.callsubprocess('picosat', picosat_cmd, conf.picosat_output,
			True, allow_all_exit_codes = True)

	rtn = []
	solut = []
	for line in stdout:
		if line[0] == 's':
			try:
				solut.remove(0)
				rtn.append(solut)
			except ValueError:
				pass
			solut = []
			if not line.rstrip() == 's SATISFIABLE':
				raise exceptions.NoSolution()
		elif line[0] == 'v':
			for sl in line[2:].split():
				solut.append(int(sl))
	try:
		solut.remove(0)
		rtn.append(solut)
	except ValueError:
		pass
	# Ensure smap existence
	utils.build_symbol_map()
	# Translate to dict
	con = list()
	for rt in rtn:
		cond = dict()
		for r in rt:
			if r < 0:
				val = False
				r *= -1
			else:
				val = True
			if r > int(conf_num):
				break;
			if 'NONAMEGEN' in utils.smap[r]: # ignore generated names
				continue
			cond[utils.smap[r]] = val
		con.append(cond)
	return con

def __txt_config__(con):
	config = []
	for key, val in con.items():
		txt = 'CONFIG_' + key + '='
		if val:
			txt += 'y'
		else:
			txt += 'n'
		config.append(txt)
	return config

def __write_temp_config_file__(con):
	wfile = tempfile.NamedTemporaryFile(delete=False)
	txt = __txt_config__(con)
	for ln in txt:
		wfile.write(bytes(ln + '\n', sys.getdefaultencoding()))
	wfile.close()
	return wfile.name

def __load_config_text__(txt):
	rtn = dict()
	for ln in txt:
		if not ln:
			continue
		if ln[0] == '#' or not '=' in ln:
			continue
		indx = ln.index('=')
		if (ln[indx + 1] == 'y'):
			rtn[ln[7:indx]] = True
		else:
			rtn[ln[7:indx]] = False
	return rtn


def __load_config_file__(file):
	f = open(file, 'r')
	rtn = __load_config_text__(f)
	f.close()
	return rtn

def __calchash__(con):
	dt = database.database()
	csort = dt.get_configsort()

	cstr = ""
	for c in csort:
		try:
			if con[c]:
				cstr += c
		except KeyError:
			pass

	# Add missing
	for key, val in con.items():
		try:
			csort.index(key)
		except ValueError:
			indx = len(csort)
			csort.append(key)
			dt.add_configsort(key)
			if val:
				cstr += key

	hsh = hashlib.md5(bytes(cstr, 'UTF-8'))
	return hsh.hexdigest()


def __calchash_file__(file):
	"""Calculates hash from configuration file"""
	con = __load_config_file__(file)
	return __calchash__(con)

def __register_conf__(con, conf_num, generator):
	dtb = database.database()
	# Solution to configuration
	txtconfig = __txt_config__(con)
	hsh = __calchash__(con)
	cconf = dtb.get_configration(hsh)
	for cc in cconf:
		print('hash: ' + hsh)
		if compare_text(cc.config, txtconfig):
			print("I: Generated existing configuration.")
			return False
		else:
			print("W: Generated configuration with collision hash.")
			# TODO this might have to be tweaked
			raise Exception()
	dtb.add_configuration(hsh, txtconfig, generator)
	return True

def __generate_single__(var_num, conf_num):
	measure_list = set()
	if not os.path.isfile(sf(conf.single_generated_file)):
		with open(sf(conf.measure_file), 'r') as fi:
			for ln in fi:
				measure_list.add(int(ln))
	else:
		with open(sf(conf.single_generated_file), 'r') as f:
			for ln in f:
				measure_list.add(int(ln))
	if not measure_list:
		return False
	tfile = __buildtempcnf__(var_num, (sf(conf.rules_file),
		sf(conf.fixed_file)), [str(measure_list.pop())])
	with open(sf(conf.single_generated_file), 'w') as fo:
		for ln in measure_list:
			fo.write(str(ln) + '\n')
	try:
		confs = __exec_sat__(tfile, ['-i', '0'], conf_num)
		for con in confs:
			__register_conf__(con, conf_num, 'single-sat')
	except exceptions.NoSolution:
		return __generate_single__(var_num, conf_num)
	finally:
		os.remove(tfile)
	return True

def __generate_random__(var_num, conf_num):
	tfile = __buildtempcnf__(var_num, (sf(conf.rules_file), sf(conf.fixed_file)), set())
	try:
		confs = __exec_sat__(tfile, ['-i', '3'], conf_num)
		for con in confs:
			if not __register_conf__(con, conf_num, 'random-sat'):
				__generate_random__(var_num, conf_num)
	finally:
		os.remove(tfile)
	return True

def generate():
	"""Collect boolean equations from files rules and required
	And get solution with picosat
	"""
	# Check if rules_file exist. If it was generated.
	if not os.path.isfile(sf(conf.rules_file)):
		raise exceptions.MissingFile(conf.rules_file,"Run parse_kconfig.")
	if not os.path.isfile(sf(conf.fixed_file)):
		raise exceptions.MissingFile(conf.required_file,"Run allconfig and initialization process.")

	# Load variable count
	with open(sf(conf.variable_count_file)) as f:
		var_num = f.readline().rstrip()
		conf_num = f.readline().rstrip()

	if __generate_single__(var_num, conf_num):
		return
	elif __generate_random__(var_num, conf_num):
		return

	raise exceptions.NoNewConfiguration()

def compare(conf1, conf2):
	# This is not exactly best comparison method
	for key, val in conf1.items():
		try:
			if conf2[key] != val:
				return False
		except ValueError:
			return False
	for key, val in conf2.items():
		try:
			if conf1[key] != val:
				return False
		except ValueError:
			return False
	return True

def compare_text(text1, text2):
	conf1 = __load_config_text__(text1)
	conf2 = __load_config_text__(text2)
	return compare_file(conf1, conf2)

def compare_file(file1, file2):
	"""Compared two configuration"""
	conf1 = __load_config_file__(file1)
	conf2 = __load_config_file__(file2)
	return compare_file(conf1, conf2)
