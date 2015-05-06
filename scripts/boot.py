import os
import sys
import subprocess
import shutil
import importlib

import utils
import initialize
from conf import conf
from conf import sf
from exceptions import MissingFile

def boot():
	if not os.path.isfile((conf.nbscript)):
		initialize.gen_nbscript()
	try:
		os.mkdir(sf(conf.output_folder))
	except FileExistsError:
			pass

	bench = importlib.machinery.SourceFileLoader("module.name",
			sf(conf.benchmark_python)).load_module()

	sprc = subprocess.Popen([sf(conf.novaboot), sf(conf.nbscript)] + conf.novaboot_args,
			stdout = subprocess.PIPE)
	output = ''
	for linen in sprc.stdout:
		line = linen.decode('utf-8')
		if conf.boot_output:
			print(line, end="")
		if line.startswith('lcp-output: '):
			output += line[12:]
	print(output)

	# TODO change
	data = bench.stdoutput(output)

	iteration = 0
	with open(sf(conf.iteration_file), 'r') as f:
		iteration = int(f.readline())

	for key, val in data.items():
		with open(os.path.join(sf(conf.output_folder),key), 'w') as f:
			f.write(str(iteration) + ':' + str(val) + '\n')
