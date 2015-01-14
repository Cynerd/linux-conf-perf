#!/bin/python3
import os
import sys
import subprocess
from conf import conf

def kconfig_parser():
	"Execute kconfig_parser in linux_sources directory and parsed output is placed to build_folder."
	env = dict(os.environ)
	env['SRCARCH'] = conf.SRCARCH
	env['ARCH'] = conf.ARCH
	env['KERNELVERSION'] = 'KERNELVERSION'
	wd = os.getcwd()
	os.chdir(conf.linux_sources)
	subprocess.call([conf.kconfig_parser, conf.linux_kconfig_head, conf.build_folder],
			env=env)

def main():
	kconfig_parser()

if __name__ == "__main__":
	main()
