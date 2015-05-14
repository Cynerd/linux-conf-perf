import os
import sys
import tempfile
import subprocess
import time

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

	if os.path.isfile(sf(conf.solution_file)) and conf.gen_all_solution_oninit:
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
	try:
		os.mkdir(sf(conf.log_folder))
	except OSError:
		pass

	picosat_cmd = [sf(conf.picosat), w_file.name]
	if (conf.gen_all_solution_oninit):
		picosat_cmd += ['--all']

	satprc = subprocess.Popen(picosat_cmd, stdout = subprocess.PIPE)
	with open(os.path.join(sf(conf.log_folder), "picosat.log"), 'a') as f:
		f.write("::" + time.strftime("%y-%m-%d-%H-%M-%S") + "::\n")
		solut = []
		for linen in satprc.stdout:
			line = linen.decode(sys.getdefaultencoding())
			f.write(line)
			if conf.picosat_output:
				print(line, end="")
			if line[0] == 's':
				try:
					solut.remove(0)
					with open(sf(conf.config_map_file), 'a') as fm:
						fm.write(str(utils.hash_config(solut)) + ':')
						for sl in solut:
							fm.write(str(sl) + ' ')
						fm.write('\n')
					with open(sf(conf.solved_file), 'a') as fs:
						for sl in solut:
							fs.write(str(-1 * sl) + ' ')
						fs.write('\n')
				except ValueError:
					pass
				solut = []
				if not line.rstrip() == 's SATISFIABLE':
					os.remove(w_file.name)
					raise exceptions.NoSolution()
			elif line[0] == 'v':
				for sl in line[2:].split():
					solut.append(int(sl))

	os.remove(w_file.name)

def apply():
	"""Apply generated solution to kernel source.
	"""
	utils.build_symbol_map() # Ensure smap existence

	solved = set()
	solution = []
	# Load set of solved solutions
	if os.path.isfile(sf(conf.config_solved_file)):
		with open(sf(conf.config_solved_file)) as f:
			for ln in f:
				solved.add(ln.strip())

	# Load one solution if it is not in solved
	hash = ''
	with open(sf(conf.config_map_file)) as f:
			while True:
				w = f.readline().split(sep=':')
				if not w[0]:
					break
				if not w[0] in solved:
					solution = utils.config_strtoint(w[1])
					hash = w[0]
					break
	if not solution:
		raise exceptions.NoApplicableSolution()

	# Write hash to config_solved
	with open(sf(conf.config_solved_file), 'a') as f:
		f.write(hash)
		f.write('\n')

	# Load variable count
	with open(sf(conf.symbol_map_file)) as f:
		for var_num, l in enumerate(f):
			pass
		var_num += 1
	# Write solution to .config file in linux source folder
	with open(sf(conf.linux_dot_config), 'w') as f:
		for s in solution:
			if s < 0:
				nt = True
				s *= -1
			else:
				nt = False
			if s >= var_num:
				break;
			if 'NONAMEGEN' in utils.smap[s]: # ignore generated names
				continue

			f.write('CONFIG_' + utils.smap[s] + '=')
			if not nt:
				f.write('y')
			else:
				f.write('n')
			f.write('\n')
