#!/usr/bin/env python3
import os
import sys

from conf import conf
from conf import sf
import initialize
import iteration
import kernel
import boot

def test():
	initialize.parse_kconfig()
	initialize.gen_requred() # Call this to check initial solution
	iteration.reset() # Reset iteration
	conf.kernel_make_output = True
	kernel.make()
	conf.boot_output = True
	boot.boot()
	print('------------------------------')
	for nm in os.listdir(sf(conf.output_folder)):
		if os.path.isfile(os.path.join(sf(conf.output_folder), nm)):
			with open(os.path.join(sf(conf.output_folder), nm), 'r') as f:
				print(nm + ':')
				for line in f:
					print(line, end='')
			os.remove(os.path.join(sf(conf.output_folder), nm))
	# To be sure also try remove other file
	try:
		os.remove(sf(conf.solved_file))
	except OSError:
		pass


#################################################################################

if __name__ == "__main__":
	test()
