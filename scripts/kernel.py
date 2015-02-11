import os
import sys
import subprocess

import utils
from conf import conf


def config():
	# Executing old linux config
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	sprc = subprocess.Popen(['make', 'oldconfig'], stdout=subprocess.PIPE, env=utils.get_kernel_env())
	while True:
		line = sprc.stdout.readline()
		if line != '':
			if b'Restart config' in line:
				print("Kernel config failed")
				sprc.terminate()
				break
			else:
				print(line.decode('utf-8'), end="")
		else:
			break
	os.chdir(wd)

def make():
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	subprocess.call(['make'] + conf.linux_make_args, env=utils.get_kernel_env())
	os.chdir(wd)
