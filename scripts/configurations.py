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

def __exec_sat__(file, args):
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
	return rtn

def __write_temp_config_file__(con, conf_num):
	# Ensure smap existence
	utils.build_symbol_map()
	# Write temporally file
	wfile = tempfile.NamedTemporaryFile(delete=False)
	for s in con:
		if s < 0:
			nt = True
			s *= -1
		else:
			nt = False
		if s > int(conf_num):
			break;
		if 'NONAMEGEN' in utils.smap[s]: # ignore generated names
			continue
		wfile.write(bytes('CONFIG_' + utils.smap[s] + '=',
			sys.getdefaultencoding()))
		if not nt:
			wfile.write(bytes('y', sys.getdefaultencoding()))
		else:
			wfile.write(bytes('n', sys.getdefaultencoding()))
		wfile.write(bytes('\n', sys.getdefaultencoding()))
	wfile.close()
	return wfile.name

def __load_config_file__(file):
	rtn = dict()
	with open(file, 'r') as f:
		for ln in f:
			if ln[0] == '#' or not '=' in ln:
				continue
			indx = ln.index('=')
			if (ln[indx + 1] == 'y'):
				rtn[ln[7:indx]] = True
			else:
				rtn[ln[7:indx]] = False
	return rtn

def __calchash__(file):
	"""Calculates hash from configuration file"""
	# Build hashconfigsort
	csort = []
	try:
		with open(conf.hashconfigsort, 'r') as f:
			for ln in f:
				csort.append(ln.rstrip())
	except FileNotFoundError:
		pass

	con = __load_config_file__(file)
	cstr = ""
	for c in csort:
		try:
			if con[c]:
				cstr += c
		except ValueError:
			pass

	# Add missing
	csortfile = open(sf(conf.hashconfigsort), 'a');
	for key, val in con.items():
		try:
			csort.index(key)
		except ValueError:
			indx = len(csort)
			csort.append(key)
			csortfile.write(key + '\n')
			if val:
				cstr += key
	csortfile.close()

	hsh = hashlib.md5(bytes(cstr, 'UTF-8'))
	return hsh.hexdigest()

def __register_conf__(con, conf_num, generator):
	dtb = database.database()
	# Solution to configuration
	wfile = __write_temp_config_file__(con, conf_num)
	hsh = __calchash__(wfile)
	filen = os.path.join(sf(conf.configurations_folder), hsh)
	hshf = hsh
	if os.path.isfile(filen):
		if compare(filen, wfile):
			print("I: Generated existing configuration.")
			return False
		else:
			print("W: Generated configuration with collision hash.")
			# TODO this might have to be tweaked
			raise Exception()
	shutil.move(wfile, filen)
	dtb.add_configuration(hsh, hshf, generator)
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
				measure_list.append(int(ln))
	if measure_list:
		return False
	tfile = __buildtempcnf__(var_num, (sf(conf.rules_file),
		sf(conf.fixed_file)), (str(measure_list.pop())))
	try:
		confs = __exec_sat__(tfile, ['-i', '0'])
		for con in confs:
			__register_conf__(con, conf_num, 'single-sat')
	except exceptions.NoSolution:
		pass
	finally:
		os.remove(tfile)
	with open(sf(conf.single_generated_file), 'w') as fo:
		fo.writelines(measure_list)
	return True

def __generate_random__(var_num, conf_num):
	tfile = __buildtempcnf__(var_num, (sf(conf.rules_file), sf(conf.fixed_file)), ())
	try:
		confs = __exec_sat__(tfile, ['-i', '3'])
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

def compare(file1, file2):
	"""Compared two configuration"""
	conf1 = __load_config_file__(file1)
	conf2 = __load_config_file__(file2)

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
