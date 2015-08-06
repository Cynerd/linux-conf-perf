#!/usr/bin/env python3
import os
import sys

from conf import conf
from conf import sf
import initialize
import kernel
import boot
import database

def test():
	initialize.base()
	initialize.parse_kconfig()
	print("-- Make --")
	conf.kernel_make_output = True
	img = kernel.make('test')
	try:
		os.remove(sf(conf.jobfolder_linux_image))
	except FileNotFoundError:
		pass
	os.symlink(img, sf(conf.jobfolder_linux_image))
	conf.boot_output = True
	conf.parse_output = True
	print("-- Boot --")
	config = database.Config('0', 'test', img)
	boot.boot(config, False)

#################################################################################

if __name__ == "__main__":
	test()
