#!/bin/python3
import os
import sys
import tempfile
import subprocess
from conf import conf

if not os.path.isfile(conf.rules_file):
	print("Error: Rules are not generated yet, or wrong build_folder.\nCheck existence of " + rules_file, file=sys.stderr)
	sys.exit(1)


#w_file = tempfile.NamedTemporaryFile(delete=False)
w_file = open('bld', 'w')
# Join files to one single file
lines = set()
for ln in open(conf.rules_file, 'r'):
	if ln not in lines:
		lines.add(ln)
if os.path.isfile(conf.solved_file):
	for ln in open(conf.solved_file, 'r'):
		if ln not in lines:
			lines.add(ln)
if os.path.isfile(conf.required_file):
	for ln in open(conf.required_file, 'r'):
		if ln not in lines:
			lines.add(ln)

with open(conf.symbol_map_file) as f:
	for var_num, l in enumerate(f):
		pass
	var_num += 1
lines_count = len(lines)

first_line = "p cnf " + str(var_num) + " " + str(lines_count)
w_file.write(first_line + '\n')
for ln in lines:
	w_file.write(ln)

w_file.close()

print("temp file: " + w_file.name)
print("Output: " + conf.solution_file)
subprocess.call(['minisat', w_file.name, conf.solution_file])

#os.remove(w_file.name)
