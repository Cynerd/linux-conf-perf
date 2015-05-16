#!/usr/bin/env python3
import os
import sys

import numpy.linalg as nplag

from conf import conf
from conf import sf
import utils

def reduce_matrix_remove_symbol(A, symrow, indx):
	del symrow[indx]
	for i in range(0, len(A)):
		del A[i][indx]

def reduce_matrix(A, symrow):
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


def evaluate():
	print("Collect data...")
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

	print('Build matrix...')
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

	# Reduce matrix A
	print('Simplify matrix...')
	reduce_matrix(A, symrow)

	for x in range(0, len(A)):
		A[x].append(1)
	symrow.append(0)

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
			for s in symrow[i]:
				print(utils.smap[s], end=' ')
		print("=", end=' ')
		print(str(R[0][i]))


#################################################################################

if __name__ == '__main__':
	evaluate()