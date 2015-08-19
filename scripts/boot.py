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
	out = utils.callsubprocess('boot', conf.boot_command, conf.boot_output, True)

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
			dtb.add_measure(out, config.id, value)
