import os
import sys
import subprocess
import time
import hashlib
import re
from conf import conf
from conf import sf
import exceptions

def build_symbol_map():
	"""Generates global variable smap from symbol_map_file.
	When file not exists, MissingFile exception is raised.
	"""
	global smap
	try:
		smap
	except NameError:
		# Check if symbol_map_file exist
		if not os.path.isfile(sf(conf.symbol_map_file)):
			raise exceptions.MissingFile(sf(conf.symbol_map_file),
					"Run parse_kconfig to generate it.")

		smap = dict()
		with open(sf(conf.symbol_map_file)) as f:
			for lnn in f:
				w = lnn.rstrip().split(sep=':')
				smap[int(w[0])] = w[1]


def build_conf_map():
	"""Generates global variable cmap from config_map_file and config_solved_file.
	cmap is dictionary containing list ([configuration], bool solved)
	cmap is rebuild every time this function is called.
	"""
	global cmap
	cmap = dict()
	if os.path.isfile(sf(conf.config_map_file)):
		with open(sf(conf.config_map_file)) as f:
			for ln in f:
				w = ln.rstrip().split(sep=':')
				cf = list()
				for vr in w[1].split(sep=" "):
					if vf[0] == '-':
						cf.append(-1 * int(vf[1:]))
					cf.append(int(vf))
				cmap[w[0]] = [w[1], False]

		if os.path.isfile(sf(conf.config_solved_file)):
			with open(sf(conf.config_solved_file)) as f:
				for ln in f:
					try:
						cmap[ln.rstrip()][1] = True
					except KeyError:
						pass

def callsubprocess(process_name, process, show_output = True,
		return_output = False, env=os.environ, allowed_exit_codes = [0]):
	sprc = subprocess.Popen(process, stdout = subprocess.PIPE, env = env)

	try:
		os.mkdir(os.path.join(sf(conf.log_folder), process_name))
	except OSError:
		pass

	rtn = []
	with open(os.path.join(sf(conf.log_folder),
			process_name, time.strftime("%y-%m-%d-%H-%M-%S") + ".log"),
			"a") as f:
		f.write('::' + time.strftime("%y-%m-%d-%H-%M-%S-%f") + '::\n')
		for linen in sprc.stdout:
			line = linen.decode(sys.getdefaultencoding())
			f.write(line)
			if show_output:
				print(line, end="")
			if return_output:
				rtn.append(line.rstrip())

	rtncode = sprc.wait()
	if rtncode not in allowed_exit_codes:
		raise exceptions.ProcessFailed(process, rtncode)
	return rtn

def get_kernel_env():
	env = dict(os.environ)
	env.update(conf.kernel_env)
	return env


def hash_config(cf):
	"""Hashes configuration using MD5 hash.
	"""
	try:
		cf.remove(0)
	except ValueError:
		pass
	str = ""
	for c in cf:
		if c < 0:
			str += '-'
		else:
			str += '+'
	hsh = hashlib.md5(bytes(str, sys.getdefaultencoding()))
	return hsh.hexdigest()

def config_strtoint(str, full):
	"""Reads list of configured symbols from string
	"""
	rtn = []
	if full:
		for s in str.rstrip().split(sep=' '):
			rtn.append(int(s))
	else:
		count = 0
		with open(sf(conf.variable_count_file)) as f:
			f.readline()
			count = int(f.readline())
		for s in str.rstrip().split(sep=' '):
			val = int(s)
			if abs(val) <= count:
				rtn.append(val)
			else:
				break;
	try:
		rtn.remove(0)
	except ValueError:
		pass
	return rtn

def get_config_from_hash(hash):
	with open(sf(conf.config_map_file), "r") as f:
		for line in f:
			w = line.rstrip().split(sep=':')
			if w[0] == hash:
				return config_strtoint(w[1], True)
	return None

def get_last_configuration():
	hsh = ""
	try:
		with open(sf(conf.config_solved_file), "r") as f:
			for line in f:
				sline = line.rstrip()
				if sline != '':
					hsh = sline
	except FileNotFoundError:
		try:
			with open(sf(conf.config_map_file), "r") as f:
				w = f.readline().split(sep=':')
				hsh = w[0]
		except FileNotFoundError:
			pass

	if hsh != '':
		return hsh
	else:
		return 'NoConfig'
