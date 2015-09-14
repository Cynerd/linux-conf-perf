#!/usr/bin/env python3
import os
import sys
import re
from conf import conf

def gen_confmk():
	try:
		os.remove(conf.dot_confmk)
	except OSError:
		pass

	with open(conf.dot_confmk, 'w') as f:
		f.write("# This file is generated. Please don't edit this file.\n")
		for var in dir(conf):
			if not re.match('__.*__', var):
				val = eval('conf.' + var)
				if type(val) is str:
					f.write("CONF_" + var.upper() + " := ")
					f.write(val + '\n')
				elif type(val) is int:
					f.write("CONF_" + var.upper() + " := ")
					f.write(str(val) + '\n')

#################################################################################

if __name__ == '__main__':
	gen_confmk()
