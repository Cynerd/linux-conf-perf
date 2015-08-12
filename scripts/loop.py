#!/usr/bin/env python3
import os
import sys
import subprocess
import signal
from threading import Thread
from threading import Lock

from conf import conf
from conf import sf
import initialize
import configurations
import kernel
import boot
import exceptions
import database

__confs_unmeasured__ = []

def prepare():
	"""Prepare for measuring
	Outcome is Linux image for generated configuration."""
	global __confs_unmeasured__
	if len(__confs_unmeasured__) == 0:
		dtb = database.database()
		confs = dtb.get_unmeasured()
		if len(confs) == 0:
			configurations.generate()
			confs = dtb.get_unmeasured()
			if len(confs) == 0:
				raise exceptions.NoApplicableConfiguration()
		__confs_unmeasured__ = list(confs)
	con = __confs_unmeasured__.pop()
	kernel.config(con.cfile)
	img = kernel.make(con.hash)
	print("Prepared image: " + img)
	return img, con

def measure(kernelimg, con):
	try:
		os.remove(sf(conf.jobfolder_linux_image))
	except FileNotFoundError:
		pass
	os.symlink(kernelimg, sf(conf.jobfolder_linux_image))
	boot.boot(con)
	print("Configuration '" + con.hash + "' measured.")

# Threads #
__terminate__ = False
class mainThread(Thread):
	def run(self):
		if conf.single_loop:
			img, config = prepare()
			measure(img, config)
		else:
			while not __terminate__:
				img, config = prepare()
				measure(img, config)

# Multithread section #
__conflist__ = []
__listlock__ = Lock()

class prepareThread(Thread):
	def __init__(self, name='prepare'):
		Thread.__init__(self, name=name)
	def run(self):
		__listlock__.aquire()
		while not __terminate__ and len(__conflist__) <= conf.multithread_buffer:
			__listlock__.release()
			try:
				config = prepare()
			except exceptions.NoApplicableConfiguration:
				return
			__listlock__.aquire()
			__conflist__.append(config)
			if not __measurethread__.isActive():
				__measurethread__.start()
		__listlock__.release()

class measureThread(Thread):
	def __init__(self, name='measure'):
		Thread.__init__(self, name=name)
	def run(self):
		__listlock__.aquire()
		while not __terminate__ and len(__conflist__) > 0:
			config = __conflist__[0]
			del __conflist__[0]
			__listlock__.release()
			if not __preparethread__.isActive():
				__preparethread__.start()
			measure(config)
			__listlock__.aquire()
		__listlock__.release()

__preparethread__ = prepareThread()
__measurethread__ = measureThread()

# Start and sigterm handler #
def sigterm_handler(_signo, _stack_frame):
	__terminate__ = True

def loop():
	initialize.all()
	global thr
	thr = mainThread()
	thr.start()
	try:
		thr.join()
	except KeyboardInterrupt:
		__terminate__ = True

#################################################################################

if __name__ == '__main__':
	signal.signal(signal.SIGTERM, sigterm_handler)
	loop()
