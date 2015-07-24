import os
import sys
import subprocess

from conf import conf
from conf import sf
import exceptions
import utils

def config(cfile):
	wd = os.getcwd()
	infile = os.path.join(sf(conf.configurations_folder), cfile)
	os.chdir(sf(conf.linux_sources))
	try:
		utils.callsubprocess('write_config', [sf(conf.write_config), infile],
			conf.kernel_config_output, env=utils.get_kernel_env())
	except exceptions.ProcessFailed:
		raise exceptions.ConfigurationError("some configs mismatch")
	os.chdir(wd)

#def config_noprogram():
#	# Executing old linux config
#	env = dict(os.environ)
#	wd = os.getcwd()
#	os.chdir(sf(conf.linux_sources))
#	if conf.kernel_config_output:
#		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
#			env=utils.get_kernel_env())
#	else:
#		sprc = subprocess.call('yes "" | make oldconfig', shell=True,
#			stdout=subprocess.DEVNULL, env=utils.get_kernel_env())
#	os.chdir(wd)

def make():
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.kernel_make_output:
		subprocess.call(conf.build_command, env=utils.get_kernel_env())
	else:
		subprocess.call(conf.build_command, stdout=subprocess.DEVNULL,
				env=utils.get_kernel_env())

	try:
		os.symlink(sf(conf.linux_image), sf(conf.jobfolder_linux_image))
	except FileExistsError:
		pass

	os.chdir(wd)
