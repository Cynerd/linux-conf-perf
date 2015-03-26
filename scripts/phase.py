import os
import sys

from conf import conf

phases = ("Not Initialized"		#0
		  "Initializing",			#1
		  "Initialized",			#2
		  "Solution generating",	#3
		  "Solution generated",		#4
		  "Solution applying",		#5
		  "Solution applied",		#6
		  "Kernel configuration",	#7
		  "Kernel configured",		#8
		  "Kernel build",			#9
		  "Kernel built"			#10
		  )

def get():
	try:
		with open(conf.phase_file) as f:
			txtPhase = f.readline().rstrip()
		if not txtPhase in phases:
			raise PhaseMismatch()
		return phases.index(txtPhase)
	except FileNotFoundError:
		return 0
	
def set(phs):
	with open(conf.phase_file, 'w') as f:
		f.write(phases[phs])

def pset(str):
	set(phase.index(str))

def phs(str):
	return phases.index(str)

def message(phs):
	"Prints message signaling running phase."
	print("-- " + phases[phs])
