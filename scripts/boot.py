import os
import sys
import subprocess
import shutil

import utils
from conf import conf
from exceptions import MissingFile

def gen_nbscript():
	try:
		os.remove(conf.nbscript)
	except OSError:
		pass

	with open(conf.nbscript, 'w') as f:
		f.write('load ' + conf.linux_image + ' console=ttyS0,115200\n')
		f.write('load ' + conf.initram + '\n')

def boot():
	if not os.path.isfile(conf.nbscript):
		gen_nbscript()

	sprc = subprocess.Popen([conf.novaboot, conf.nbscript] + conf.novaboot_args,
			stdout = subprocess.PIPE)

