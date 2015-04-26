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

#################################################################################

if __name__ == '__main__':
	gen_confmk()
