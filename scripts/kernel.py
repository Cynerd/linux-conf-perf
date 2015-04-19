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
		sprc = subprocess.call([conf.write_config, conf.linux_kconfig_head,
			conf.build_folder], env=utils.get_kernel_env())
	else:
		sprc = subprocess.call([conf.write_config, conf.linux_kconfig_head,
			conf.build_folder], stdout=subprocess.DEVNULL,
			env=utils.get_kernel_env())
	os.chdir(wd)

def config_noprogram():
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
	if conf.kernel_make_output:
		subprocess.call(['make'] + conf.linux_make_args,
				env=utils.get_kernel_env())
	else:
		subprocess.call(['make'] + conf.linux_make_args,
				stdout=subprocess.DEVNULL, env=utils.get_kernel_env())
	os.chdir(wd)
