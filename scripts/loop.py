#!/usr/bin/env python3
import os
import sys
import subprocess
import signal
from threading import Thread
from threading import Lock
from threading import Event

from conf import conf
from conf import sf
import initialize
import configurations
import kernel
import boot
import exceptions
import database
import utils

__confs_unmeasured__ = []

def prepare():
	"""Prepare for measuring
	Outcome is Linux image for generated configuration."""
	print("Preparing new image.")
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
	kernel.config(con.config)
	img = kernel.make(con.hash)
	print("Prepared image: " + img)
	return img, con

def measure(kernelimg, con):
	print("Measuring " + con.hash)
	try:
		os.remove(sf(conf.jobfolder_linux_image))
	except FileNotFoundError:
		pass
	os.symlink(kernelimg, sf(conf.jobfolder_linux_image))
	boot.boot(con)
	print("Configuration '" + con.hash + "' measured.")

# Multithread #
__conflist__ = []
__listlock__ = Lock()
__preparethreadEvent__ = Event()
__measurethreadEvent__ = Event()

class prepareThread(Thread):
	global __preparethread__
	global __measurethread__
	def __init__(self, name='prepare'):
		Thread.__init__(self, name=name)
	def run(self):
		print('Prepare thread start')
		while not __terminate__:
			try:
				img, config = prepare()
			except exceptions.NoApplicableConfiguration:
				return
			__listlock__.acquire()
			__conflist__.append((img, config))
			__preparethreadEvent__.set()
			if len(__conflist__) > conf.multithread_buffer:
				__listlock__.release()
				print('Prepare thread suspended')
				__measurethreadEvent__.wait()
				print('Prepare thread waken')
			else:
				__listlock__.release()
			__measurethreadEvent__.clear()
		print('Prepare thread stop')

class measureThread(Thread):
	global __preparethread__
	global __measurethread__
	def __init__(self, name='measure'):
		Thread.__init__(self, name=name)
	def run(self):
		print('Measure thread start')
		while not __terminate__:
			__listlock__.acquire()
			if len(__conflist__) <= 0:
				__listlock__.release()
				print('Measure thread suspended')
				__preparethreadEvent__.wait()
				print('Measure thread waken')
				__listlock__.acquire()
			__preparethreadEvent__.clear()
			img, config = __conflist__.pop()
			__listlock__.release()
			__measurethreadEvent__.set()
			measure(img, config)
		print('Measure thread stop')

__preparethread__ = prepareThread()
__measurethread__ = measureThread()

# Start and sigterm handler #
def sigterm_handler(_signo, _stack_frame):
	global __terminate__
	__terminate__ = True
	if conf.multithread:
		__measurethreadEvent__.set()
		__preparethreadEvent__.set()

# Main loop and single thread #
__terminate__ = False
def loop():
	utils.dirtycheck()
	initialize.all()
	if conf.multithread:
		__preparethread__.start()
		__measurethread__.start()
	else:
		if conf.single_loop:
			img, config = prepare()
			measure(img, config)
		else:
			while not __terminate__:
				img, config = prepare()
				measure(img, config)

#################################################################################

if __name__ == '__main__':
	signal.signal(signal.SIGTERM, sigterm_handler)
	signal.signal(signal.SIGINT, sigterm_handler)
	loop()
