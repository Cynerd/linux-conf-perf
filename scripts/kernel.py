import os
import sys
import subprocess
import shutil
import tempfile

from conf import conf
from conf import sf
import exceptions
import utils

def config(txtconfig):
	"Apply text configuration to kernel folder"
	infile = tempfile.NamedTemporaryFile()
	infile.write(bytes(txtconfig, sys.getdefaultencoding()))
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	try:
		utils.callsubprocess('write_config', [sf(conf.write_config), infile.name],
			conf.kernel_config_output, env=utils.get_kernel_env())
	except exceptions.ProcessFailed:
		raise exceptions.ConfigurationError("some configs mismatch")
	infile.close()
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

def make(confhash):
	wd = os.getcwd()
	os.chdir(sf(conf.linux_sources))
	if conf.kernel_make_output:
		subprocess.call(conf.build_command, env=utils.get_kernel_env())
	else:
		subprocess.call(conf.build_command, stdout=subprocess.DEVNULL,
				env=utils.get_kernel_env())
	jobimage = os.path.join(sf(conf.build_folder), confhash + '_linux.img')
	shutil.move(sf(conf.linux_image), jobimage)
	os.chdir(wd)
	return confhash + '_linux.img'
