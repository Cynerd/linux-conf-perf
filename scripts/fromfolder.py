#!/usr/bin/env python3
import os
import sys
import re
from conf import conf
from conf import sf

def loadfromKconfig(file):
	found = set()
	for ln in open(file):
		if re.search('^config', ln):
			name = ln[7:].rstrip()
			found.add(name)
	return found

def loadfromfolder(folder):
	found = set()
	for l in os.listdir(folder):
		if os.path.isdir(folder + '/' + l):
			found = found | loadfromfolder(folder + '/' + l)
		elif os.path.isfile(folder + '/' + l) and re.search('Kconfig', l):
			found =  found | loadfromKconfig(folder + '/' + l)
	return found

def removefrom(file, outfile, removelist):
	alllines = []
	for ln in open(file):
		alllines.append(ln.rstrip())
	
	for rl in removelist:
		for ln in alllines:
			if re.search('^CONFIG_' + rl + '=', ln):
				print('removing ' + ln)
				alllines.remove(ln)
	
	with open(outfile, 'w') as f:
		for ln in alllines:
			f.write(ln + '\n')
	
#################################################################################

# TODO propper argument parsing
if __name__ == '__main__':
	folder = sys.argv[1]
	inputfile = sys.argv[2]
	outputfile = sys.argv[3]
	if not len(sys.argv) < 5:
		exceptfile = sys.argv[4]

	exceptconf = set()
	if os.path.isfile(exceptfile):
		for ln in open(exceptfile):
			lns = ln.rstrip()
			if lns:
				exceptconf.add(lns)

	rem = loadfromfolder(sf(conf.linux_sources + '/' + folder))

	for ec in exceptconf:
		rrlist = []
		for r in rem:
			if re.match(ec, r):
				print('except ' + r)
				rrlist.append(r)
		for r in rrlist:
			try:
				rem.remove(r)
			except KeyError:
				pass

	removefrom(inputfile, outputfile, rem)
