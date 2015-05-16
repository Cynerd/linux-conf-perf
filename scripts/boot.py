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

	wd = os.getcwd()

	sprc = subprocess.Popen(conf.boot_command,
			stdout = subprocess.PIPE)
	with open(os.path.join(sf(conf.output_folder), utils.get_last_configuration()), "a") as f:
		for linen in sprc.stdout:
			line = linen.decode('utf-8')
			if conf.boot_output:
				print(line, end="")
			f.write(line)

	os.chdir(wd)
