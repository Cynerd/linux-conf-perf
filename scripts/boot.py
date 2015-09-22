import os
import sys
import subprocess
import shutil
import importlib
import traceback

import utils
import initialize
from conf import conf
from conf import sf
import exceptions
import database

def boot(config, to_database = True):
	try:
		out = utils.callsubprocess('boot', conf.boot_command, conf.boot_output, \
				True, timeout = conf.boot_timeout)
		result = 'nominal'
	except exceptions.ProcessFailed as e:
		result = 'failed'
		out = e.output
		traceback.print_exc()
	except exceptions.ProcessTimeout as e:
		result = 'timeout'
		out = e.output
		traceback.print_exc()

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
				for c in ln:
					if c == b'\0':
						c = ' '
				txt += ln + '\n'
			dtb.add_measure(txt, result, config.id, value)
