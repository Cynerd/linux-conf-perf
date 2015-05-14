#!/usr/bin/env python3
import os
import sys
import subprocess
import signal
from threading import Thread

from conf import conf
from conf import sf
import initialize
import solution
import kernel
import boot
import exceptions

def step():
	phs = phase_get()
	if phs == 0 or phs == 1:
		phase_message(1)
		initialize.all()
		phase_set(2)
	elif phs == 2:
		phase_message(2)
		phase_set(3)
	elif phs == 3:
		if not conf.gen_all_solution_oninit:
			phase_message(3)
			solution.generate()
		iteration_inc()
		phase_set(4)
	elif phs == 4:
		phase_message(4)
		phase_set(5)
	elif phs == 5:
		phase_message(5)
		solution.apply()
		phase_set(6)
	elif phs == 6:
		phase_message(6)
		phase_set(7)
	elif phs == 7:
		phase_message(7)
		try:
			kernel.config()
		except exceptions.ConfigurationError:
			if not conf.ignore_misconfig:
				print("Configuration mismatch. Exiting.")
				sys.exit(-2)
		phase_set(8)
	elif phs == 8:
		phase_message(8)
		if conf.only_config:
			phase_set(2)
		else:
			phase_set(9)
	elif phs == 9:
		phase_message(9)
		kernel.make()
		phase_set(10)
	elif phs == 10:
		phase_message(10)
		phase_set(11)
	elif phs == 11:
		phase_message(11)
		boot.boot()
		phase_set(12)
	elif phs == 12:
		phase_message(12)
		phase_set(2)

# Phase #
phases = ("Not Initialized",		#0
		  "Initializing",			#1
		  "Initialized",			#2
		  "Solution generating",	#3
		  "Solution generated",		#4
		  "Solution applying",		#5
		  "Solution applied",		#6
		  "Kernel configuration",	#7
		  "Kernel configured",		#8
		  "Kernel build",			#9
		  "Kernel built",			#10
		  "System boot",			#11
		  "Benchmark successful"	#12
		  )

def phase_get():
	try:
		with open(sf(conf.phase_file)) as f:
			txtPhase = f.readline().rstrip()
		if not txtPhase in phases:
			raise PhaseMismatch()
		return phases.index(txtPhase)
	except FileNotFoundError:
		return 0

def phase_set(phs):
	# TODO
	try:
		global thr
		if thr.term:
			return
	except NameError:
		pass
	with open(sf(conf.phase_file), 'w') as f:
		f.write(phases[phs])

def phase_message(phs):
	"Prints message signaling running phase_"
	print("-- " + phases[phs])

# Iteration #
def iteration_reset():
	with open(sf(conf.iteration_file), 'w') as f:
		f.write('0')

def iteration_inc():
	with open(sf(conf.iteration_file), 'r') as f:
		it = int(f.readline())
	it += 1
	with open(sf(conf.iteration_file), 'w') as f:
		f.write(str(it))

# Thread #
class mainThread(Thread):
	def __init__(self, name):
		Thread.__init__(self, name=name)
		self.term = False
	def run(self):
		if conf.step_by_step:
			step()
		elif conf.single_loop:
			while not phase_get() == 2:
				step()
			step()
			while not phase_get() == 2:
				step()
		else:
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
