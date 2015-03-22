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
	
	if not os.path.isfile(conf.required):
		raise MissingFile(conf.required, None)

	try:
		os.remove(conf.required_file)
	except OSError:
		pass

	with open(conf.required_file, 'w') as fout:
		with open(conf.required, 'r') as f:
			for line in f:
				for word in line.rstrip().split():
					if word[0] == '-':
						fout.write('-')
						word = word[1:]
					fout.write(srmap[word] + " ")
				fout.write("\n")

