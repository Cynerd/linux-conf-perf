import os
import sys
from conf import conf
from exceptions import MissingFile

def build_symbol_map():
	"""Generates global variable smap from symbol_map_file.
	When file not exists, MissingFile exception is raised.
	"""
	global smap
	try:
		smap
	except NameError:
		# Check if symbol_map_file exist
		if not os.path.isfile(conf.symbol_map_file):
			raise MissingFile(conf.symbol_map_file, "Run parse_kconfig to generate it.")

		smap = dict()
		with open(conf.symbol_map_file) as f:
			for lnn in f:
				w = lnn.rstrip().split(sep=':')
				smap[w[0]] = w[1]
				
def get_kernel_env():
	env = dict(os.environ)
	env['SRCARCH'] = conf.SRCARCH
	env['ARCH'] = conf.ARCH
	env['KERNELVERSION'] = 'KERNELVERSION' # hides error
	return env
