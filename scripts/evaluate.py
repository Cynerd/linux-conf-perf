#!/usr/bin/env python3
import os
import sys

import numpy.linalg as nplag

from conf import conf
from conf import sf
import utils

def reduce_matrix_search_for_base_recurse(wset, columns, contains, ignore):
	bases = []
	for x in range(0, len(columns)):
		if x in contains or x in ignore:
			continue
		colide = False
		for i in range(0, len(wset)):
			if wset[i] == 1 and columns[x][i] == 1:
				colide = True
				break
		if not colide:
			newset = list(wset)
			onecount = 0
			for i in range(0, len(newset)):
				newset[i] = newset[i] | columns[x][i]
				if (newset[i] == 1):
					onecount += 1
			contains.add(x)
			if onecount == len(newset):
				bases.append(set(contains))
			else:
				rbases = reduce_matrix_search_for_base_recurse(newset, columns, contains, ignore)
				for rbase in rbases:
					if not rbase in bases:
						bases.append(rbase)
			contains.remove(x)
	return bases
0
def reduce_matrix_search_for_base(columns):
	bases = []
	ignore = []
	for i in range(0, len(columns)):
		wset = list(columns[i])
		ignore.append(i)
		bases.extend(reduce_matrix_search_for_base_recurse(wset, columns, {i}, ignore))
	return bases

def reduce_matrix_remove_symbol(A, symrow, indx):
	del symrow[indx]
	for i in range(0, len(A)):
		del A[i][indx]

def reduce_matrix(A, symrow, bases):
	# Remove fixed symbols
	i = len(A[0]) - 1
	while i >= 0:
		strue = False
		sfalse = False
		for y in range(0, len(A)):
			if A[y][i] == 0:
				sfalse = True
			else:
				strue = True
		if (strue and not sfalse) or (sfalse and not strue):
			reduce_matrix_remove_symbol(A, symrow, i)
		i -= 1

	# Remove duplicate symbols
	i = len(A[0]) - 1
	columns = []
	while i >= 0:
		column = []
		for y in range(0, len(A)):
			column.append(A[y][i])
		if column in columns:
			reduce_matrix_remove_symbol(A, symrow,  i)
		else:
			columns.append(column)
		i -= 1

	# Search for Bases
	columnsr = []
	for i in range(len(columns) - 1, -1,-1):
		columnsr.append(columns[i])
	basesx = reduce_matrix_search_for_base(columnsr)
	if bases:
		for base in basesx:
			bases[0].append(base)

	# Generate new Base
	if bases == [[]]:
		for x in range(0, len(A)):
			A[x].append(1)
		symrow.append(0)

def collect_data():
	hashs = {}
	for fl in os.listdir(sf(conf.result_folder)):
		if os.path.isfile(os.path.join(sf(conf.result_folder), fl)):
			hashs[fl] = [[], []]
	try:
		hashs.pop('NoConfig')
	except KeyError:
		pass

	with open(sf(conf.config_map_file)) as f:
		for line in f:
			w = line.rstrip().split(sep=':')
			if not w[0] or not w[0] in hashs:
				continue
			sol = utils.config_strtoint(w[1], False)
			hashs[w[0]][0] = sol
	
	for hash, data in hashs.items():
		with open(os.path.join(sf(conf.result_folder), hash)) as f:
			vec = []
			for ln in f:
				vec.append(float(ln))
			hashs[hash][1] = vec
	return hashs

def build_matrix(hashs):
	A = []
	B = []
	for hash,data in hashs.items():
		A.append(data[0])
		B.append(data[1])
	symrow = []
	for y in range(0, len(A[0])):
		symrow.append([abs(A[0][y])])
	for x in range(0, len(A)):
		for y in range(0, len(A[0])):
			if A[x][y] < 0:
				A[x][y] = 0
			else:
				A[x][y] = 1
	return A, B, symrow

def evaluate():
	print("Collect data...")
	hashs = collect_data()

	print('Build matrix...')
	A, B, symrow = build_matrix(hashs)

	# Reduce matrix A
	print('Simplify matrix...')
	bases = []
	reduce_matrix(A, symrow, [bases])

	# Calculate value
	print('Figuring values...')
	R = nplag.lstsq(A, B)

	# Print result
	print('--------------------')
	utils.build_symbol_map()
	for i in range(0, len(R[0])):
		if symrow[i] == 0:
			print("Base", end=' ')
		else:
			if len(bases) > 0:
				if i in bases[0]:
					print("Base", end=' ')
			elif len(bases) > 1:
				for x in range(0, len(bases)):
					if i in bases[x]:
						print("Base" + x, end=' ')
			for s in symrow[i]:
				print(utils.smap[s], end=' ')
		print("=", end=' ')
		print(str(R[0][i]))


#################################################################################

if __name__ == '__main__':
	evaluate()
