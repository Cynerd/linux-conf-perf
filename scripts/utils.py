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


def callsubprocess(process_name, process, show_output = True,
		return_output = False, env=os.environ, allowed_exit_codes = [0],
		allow_all_exit_codes = False):
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
	if rtncode not in allowed_exit_codes and not allow_all_exit_codes:
		raise exceptions.ProcessFailed(process, rtncode)
	return rtn

def get_kernel_env():
	env = dict(os.environ)
	env.update(conf.kernel_env)
	return env
