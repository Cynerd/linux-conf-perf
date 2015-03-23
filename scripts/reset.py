#!/bin/python3
import os
import sys

def rm_rf(d):
	for path in (os.path.join(d,f) for f in os.listdir(d)):
		if os.path.isdir(path):
			rm_rf(path)
		else:
			os.unlink(path)
		os.rmdir(d)

# TODO repair, broken
def reset():
	rm_rf(conf.build_folder)
	os.chdir(conf.linux_sources)
	subprocess.call(['make','clean'])
	os.rm('.config') # remove linux config file


#################################################################################

if __name__ == '__main__':
	reset()
