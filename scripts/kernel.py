import os
import sys
import subprocess

from conf import conf
from conf import sf
import exceptions
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
	if sprc > 0:
		raise exceptions.ConfigurationError("some configs mismatch")
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
		subprocess.call(build_command, env=utils.get_kernel_env())
	else:
		subprocess.call(build_command, stdout=subprocess.DEVNULL,
				env=utils.get_kernel_env())
	os.chdir(wd)
