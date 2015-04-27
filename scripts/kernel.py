import os
import sys
import subprocess

from conf import conf
from conf import sf
import utils

def config():
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.kernel_config_output:
		sprc = subprocess.call([sf(conf.write_config), sf(conf.linux_kconfig_head),
			sf(conf.build_folder)], env=utils.get_kernel_env())
	else:
		sprc = subprocess.call([sf(conf.write_config), sf(conf.linux_kconfig_head),
			sf(conf.build_folder)], stdout=subprocess.DEVNULL,
			env=utils.get_kernel_env())
	os.chdir(wd)

def config_noprogram():
	# Executing old linux config
	env = dict(os.environ)
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.kernel_config_output:
		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
			env=utils.get_kernel_env())
	else:
		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
			stdout=subprocess.DEVNULL, env=utils.get_kernel_env())
	os.chdir(wd)

def make():
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.kernel_make_output:
		subprocess.call(['make'] + conf.linux_make_args,
				env=utils.get_kernel_env())
	else:
		subprocess.call(['make'] + conf.linux_make_args,
				stdout=subprocess.DEVNULL, env=utils.get_kernel_env())
	os.chdir(wd)
