#!/bin/python3
import os
import sys
import subprocess
import signal
from threading import Thread

from conf import conf
import initialize
import phase
import solution
import kernel
from exceptions import MissingFile
from exceptions import KernelConfigFailed
import iteration

def step():
	phs = phase.get()
	if phs == phase.phs("Not Initialized"):
		try:
			os.mkdir(conf.build_folder)
		except FileExistsError:
			pass
		phase.set(1)
	elif phs == phase.phs("Initializing"):
		print("-- Initializing ...")
		initialize.kconfig_parser()
		try:
			initialize.gen_requred()
		except MissingFile:
			pass
		iteration.reset()
		phase.set(2)
	elif phs == phase.phs("Initialized"):
		print("-- Initialized")
		phase.set(3)
	elif phs == phase.phs("Solution generating"):
		print("-- Generating solution ...")
		solution.generate()
		iteration.inc()
		phase.set(4)
	elif phs == phase.phs("Solution generated"):
		print("-- Solution generated")
		phase.set(5)
	elif phs == phase.phs("Solution applying"):
		print("-- Applying generated solution ...")
		solution.apply()
		phase.set(6)
	elif phs == phase.phs("Solution applied"):
		print("-- Generated solution applied")
		phase.set(7)
	elif phs == phase.phs("Kernel configuration"):
		print("-- Kernel configure ...")
		try:
			kernel.config()
			phase.set(8)
		except KernelConfigFailed:
			print("-- Kernel configuration failed")
			phase.set(3)
	elif phs == phase.phs("Kernel configured"):
		print("-- Kernel configured")
		phase.set(9)
	elif phs == phase.phs("Kernel build"):
		print("-- Build Linux ...")
		kernel.make()
		phase.set(10)
	elif phs == phase.phs("Kernel built"):
		print("-- Linux built")
		phase.set(2)

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


class mainThread(Thread):
	def __init__(self, name):
		Thread.__init__(self, name=name)
		self.term = False
	def run(self):
		while not self.term:
			step()
	
def loop_term():
	global thr
	thr.term = True

def sigterm_handler(_signo, _stack_frame):
	loop_term()

def main_loop():
	global thr
	thr = mainThread("thred")
	thr.start()
	try:
		thr.join()
	except KeyboardInterrupt:
		loop_term()

#################################################################################

if __name__ == '__main__':
	signal.signal(signal.SIGTERM, sigterm_handler)
	if sys.argv[1] == "reset":
		reset()
	else:
		main_loop()
