import os
import sys
import subprocess

from conf import conf
import utils

def config():
	# Executing old linux config
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	if conf.kernel_config_output:
		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
			env=utils.get_kernel_env())
	else:
		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
			stdout=subprocess.DEVNULL, env=utils.get_kernel_env())
	os.chdir(wd)

def make():
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	subprocess.call(['make'] + conf.linux_make_args, env=utils.get_kernel_env())
	os.chdir(wd)
