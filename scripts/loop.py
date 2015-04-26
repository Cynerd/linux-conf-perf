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
import iteration

def step():
	phs = phase.get()
	if phs == 0:
		try:
			os.mkdir(conf.build_folder)
		except FileExistsError:
			pass
		phase.set(1)
	elif phs == 1:
		phase.message(1)
		initialize.parse_kconfig()
		initialize.gen_requred()
		iteration.reset()
		phase.set(2)
	elif phs == 2:
		phase.message(2)
		phase.set(3)
	elif phs == 3:
		phase.message(3)
		solution.generate()
		iteration.inc()
		phase.set(4)
	elif phs == 4:
		phase.message(4)
		phase.set(5)
	elif phs == 5:
		phase.message(5)
		solution.apply()
		phase.set(6)
	elif phs == 6:
		phase.message(6)
		phase.set(7)
	elif phs == 7:
		phase.message(7)
		kernel.config()
		phase.set(8)
	elif phs == 8:
		phase.message(8)
		phase.set(9)
	elif phs == 9:
		phase.message(9)
		kernel.make()
		phase.set(10)
	elif phs == 10:
		phase.message(10)
		phase.set(2)

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

def loop():
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
	loop()
