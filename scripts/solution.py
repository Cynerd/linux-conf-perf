import os
import sys
import tempfile
import subprocess

import utils
from conf import conf
from conf import sf
from exceptions import NoSolution

def generate():
	"""Collect boolean equations from files: rules, solved and required
	And get solution with minisat

	Relevant configurations
	   rules_file
	   solver_file
	   required_file
	   solution_file
	"""
	# Check if rules_file exist. If it was generated.
	if not os.path.isfile(sf(conf.rules_file)):
		raise Exception("Rules file missing. Run parse_kconfig and check ecistence of " + rules_file)

	w_file = tempfile.NamedTemporaryFile(delete=False)
	# Join files to one single temporary file
	lines = set()
	with open(sf(conf.rules_file), 'r') as f:
		for lnn in open(sf(conf.rules_file), 'r'):
			ln = lnn.rstrip()
			if ln not in lines:
				lines.add(ln)
	if os.path.isfile(sf(conf.solved_file)):
		for lnn in open(sf(conf.solved_file), 'r'):
			ln = lnn.rstrip()
			if ln not in lines:
				lines.add(ln)
	if os.path.isfile(sf(conf.required_file)):
		for lnn in open((conf.required_file), 'r'):
			ln = lnn.rstrip()
			if ln not in lines:
				lines.add(ln)

	with open(sf(conf.symbol_map_file)) as f:
		for var_num, l in enumerate(f):
			pass
		var_num += 1
	lines_count = len(lines)

	first_line = "p cnf " + str(var_num) + " " + str(lines_count)
	w_file.write(bytes(first_line + '\n', 'UTF-8'))
	for ln in lines:
		w_file.write(bytes(ln + ' 0\n', 'UTF-8'))

	w_file.close()

	# Execute minisat
	if conf.minisat_output:
		subprocess.call(['minisat', w_file.name, sf(conf.solution_file)])
	else:
		subprocess.call(['minisat', w_file.name, sf(conf.solution_file)], stdout=subprocess.DEVNULL)

	os.remove(w_file.name)

def apply():
	"""Apply generated solution to kernel source.
	"""
	# Check if solution_file exist
	if not os.path.isfile(sf(conf.solution_file)):
		raise Exception("Solution file is missing. Run sat_solution and check existence of " + sf(conf.solution_file))
	
	utils.build_symbol_map() # Ensure smap existence

	# Read solution if satisfiable
	with open(sf(conf.solution_file), 'r') as f:
		if not f.readline().rstrip() == 'SAT':
			raise NoSolution()
		solut = f.readline().split()
	solut.remove('0') # Remove 0 at the end 

	# Write solution to output_confs file
	with open(sf(conf.output_confs), 'a') as f:
		iteration = 0
		with open(sf(conf.iteration_file)) as ff:
			iteration = int(ff.readline())
		f.write(str(iteration) + ':')
		for txt in solut:
			f.write(txt + ' ')
		f.write('\n')


	# Write negotation solution to solver_file
	with open(sf(conf.solved_file), 'a') as f:
		for txt in solut:
			if txt[0] == '-':
				ntx = ""
				txt = txt[1:]
			else:
				ntx = "-"
			f.write( ntx + txt + " ")
		f.write("\n")

	# Write solution to .config file in linux source folder
	with open(sf(conf.linux_dot_config), 'w') as f:
		for txt in solut:
			if txt[0] == '-':
				nt = True
				txt = txt[1:]
			else:
				nt = False
			if 'NONAMEGEN' in utils.smap[txt]: # ignore generated names
				continue

			f.write('CONFIG_' + utils.smap[txt] + '=')
			if not nt:
				f.write('y')
			else:
				f.write('n')
			f.write('\n')
