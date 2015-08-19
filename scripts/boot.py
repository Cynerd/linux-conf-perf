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
		res = utils.callsubprocess('parse_command', conf.parse_command,
				conf.parse_output, True, stdin = out)
		value = float(res[0])
	except Exception as e:
		print("W: parse exception: " + e.__str__())

	if to_database:
			dtb = database.database()
			txt = ''
			for ln in out:
				txt += ln + '\n'
			dtb.add_measure(txt, config.id, value)
