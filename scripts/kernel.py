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
	sprc = subprocess.Popen(['make', 'oldconfig'], env=utils.get_kernel_env())
	for line in sprc.stdout:
		if line == "* Restart config...":
			print("Configuration failed")
			sprc.kill()
		else:
			print(line)
	os.chdir(wd)

def make():
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	subprocess.call(['make', '-j8'], env=utils.get_kernel_env())
	os.chdir(wd)
