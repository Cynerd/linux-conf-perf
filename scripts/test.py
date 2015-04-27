#!/bin/env python3
import os
import sys

from conf import conf
import initialize
import kernel
import boot

def test():
	initialize.gen_requred() # Call this to check initial solution
	kernel.make()

#################################################################################

if __name__ == "__main__":
	test()
