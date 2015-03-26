import os
import sys

from conf import conf
import utils


def check():
	"""Check if .config file in kernel tree is consistent with generated solution.
	This containst code fragments from solution.py (apply)
	"""
	# Check if solution_file exist
	if not os.path.isfile(conf.solution_file):
		raise Exception("Solution file is missing. Run sat_solution and check existence of " + conf.solution_file)

	utils.build_symbol_map() # Ensure smap existence
	srmap = {value:key for key, value in utils.smap.items()}

	# Read solution if satisfiable
	with open(conf.solution_file, 'r') as f:
		if not f.readline().rstrip() == 'SAT':
			raise NoSolution()
		solut = f.readline().split()
	solut.remove('0') # Remove 0 at the end 
	solutb = []
	for sl in solut: # This is using that sat. solver output is sorted
		if sl[0] == '-':
			solutb.append(False)
		else:
			solutb.append(True)

	mismatch = False
	with open(conf.linux_sources + '/.config', 'r') as f:
		for line in f:
			if (line[0] == '#') or (not '=' in line):
				continue
			indx = line.index('=')
			if (line[indx + 1] == 'y'):
				if (solutb[int(srmap[line[7:indx]]) - 1] == False):
					print("W: Setting mismatch: " + line, end='')
					mismatch = True
			if (line[indx + 1] == 'm'):
				print("W: module setting find: " + line, end='')
			elif (line[indx + 1] == 'n'):
				if (solutb[int(srmap[line[7:indx]]) - 1] == True):
					print("W: Setting mismatch: " + line, end='')
					mismatch = True
	return mismatch
