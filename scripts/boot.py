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
import database

def boot(config, to_database = True):
	try:
		os.mkdir(sf(conf.output_folder))
	except FileExistsError:
			pass

	sprc = subprocess.Popen(conf.boot_command, stdout = subprocess.PIPE)
	with open(os.path.join(sf(conf.output_folder), config.cfile), "a") as f:
		for linen in sprc.stdout:
			line = linen.decode('utf-8')
			if conf.boot_output:
				print(line, end="")
			f.write(line)

	# Let user script parse double value

	value = None
	try:
		out = utils.callsubprocess('parse_command', conf.parse_command +
                                [os.path.join(conf.output_folder, config.cfile)],
				conf.parse_output, True)
		value = float(out[0])
	except Exception as e:
		print("W: parse exception: " + e.__str__())

	if to_database:
			dtb = database.database()
			dtb.add_measure(config.cfile, config.id, value)

	return config.cfile
