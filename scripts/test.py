#!/usr/bin/env python3
import os
import sys

from conf import conf
from conf import sf
import initialize
import kernel
import boot

def test():
	initialize.base()
	initialize.parse_kconfig()
	initialize.gen_requred() # Call this to check initial solution
	conf.kernel_make_output = True
	kernel.make()
	conf.boot_output = True
	boot.boot()

#################################################################################

if __name__ == "__main__":
	test()
