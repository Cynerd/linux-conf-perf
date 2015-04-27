#!/bin/python3
import os
import sys
from conf import conf

def gen_confmk():
	try:
		os.remove(conf.dot_confmk)
	except OSError:
		pass

	with open(conf.dot_confmk, 'w') as f:
		f.write("# This file is generated. Please don't edit this file.\n")
		f.write("SRCARCH := " + conf.SRCARCH + "\n")
		f.write("\n")
		f.write("BUILDROOT_INITRAM := " + conf.buildroot_initram + "\n")
		f.write("BUILDROOT_INITTAB_DIRECTIVE := " + conf.buildroot_inittab_directive + "\n")
		f.write("BUILDROOT_INITSCRIPT := " + conf.buildroot_initscript + "\n")
		f.write("INITRAM := " + conf.initram + "\n")
		f.write("BUILDROOT_DEF_CONFIG := " + conf.buildroot_def_config + "\n")
		f.write("\n")
		f.write("INITRAM := " + conf.initram + "\n")
		f.write("NBSCRIPTS := " + conf.nbscript + "\n")

#################################################################################

if __name__ == '__main__':
	gen_confmk()
