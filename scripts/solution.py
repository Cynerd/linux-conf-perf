import os
import sys
import tempfile
import subprocess

import utils
from conf import conf
from conf import sf
import exceptions

def generate():
	"""Collect boolean equations from files: rules, solved and required
	And get solution with picosat
	"""
	# Check if rules_file exist. If it was generated.
	if not os.path.isfile(sf(conf.rules_file)):
		raise exceptions.MissingFile(conf.rules_file,"Run parse_kconfig.")

	if sys.path.isfile(sf(conf.solution_file)) and conf.gen_all_solution_oninit:
		raise exceptions.SolutionGenerated()

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
		for lnn in open(sf(conf.required_file), 'r'):
			ln = lnn.rstrip()
			if ln not in lines:
				lines.add(ln)

	with open(sf(conf.variable_count_file)) as f:
		var_num = f.readline()
	lines_count = len(lines)

	first_line = "p cnf " + var_num + " " + str(lines_count)
	w_file.write(bytes(first_line + '\n', 'UTF-8'))
	for ln in lines:
		w_file.write(bytes(ln + ' 0\n', 'UTF-8'))

	w_file.close()

	# Execute picosat
	picosat_cmd = [conf.picosat, w_file.name]
	picosat_cmd += ['-o', sf(conf.solution_file)]
	if (conf.gen_all_solution_oninit):
		picosat_cmd += ['--all']
	if conf.picosat_output:
		subprocess.call(picosat_cmd)
	else:
		subprocess.call(picosat_cmd, stdout=subprocess.DEVNULL)

	os.remove(w_file.name)

def apply():
	"""Apply generated solution to kernel source.
	"""
	# Check if solution_file exist
	if not os.path.isfile(sf(conf.solution_file)):
		raise Exception("Solution file is missing. Run sat_solution and check existence of " + sf(conf.solution_file))
	
	utils.build_symbol_map() # Ensure smap existence

	# Read solution if satisfiable
	solut = []
	with open(sf(conf.solution_file), 'r') as f:
		if not f.readline().rstrip() == 's SATISFIABLE':
			raise NoSolution()
		for line in f:
			if line[0] == 'v':
				solut += line[2:].split()
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

	# Load variable count
	with open(sf(conf.symbol_map_file)) as f:
		for var_num, l in enumerate(f):
			pass
		var_num += 1
	# Write solution to .config file in linux source folder
	with open(sf(conf.linux_dot_config), 'w') as f:
		for txt in solut:
			if txt[0] == '-':
				nt = True
				txt = txt[1:]
			else:
				nt = False
			if int(txt) >= var_num:
				break;
			if 'NONAMEGEN' in utils.smap[txt]: # ignore generated names
				continue

			f.write('CONFIG_' + utils.smap[txt] + '=')
			if not nt:
				f.write('y')
			else:
				f.write('n')
			f.write('\n')
