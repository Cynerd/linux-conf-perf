import os
import sys
import subprocess
import time
import hashlib
import signal
import re
from threading import Thread
from conf import conf
from conf import sf
import exceptions

def build_symbol_map():
	"""Generates global variable smap from symbol_map_file.
	When file not exists, MissingFile exception is raised.
	"""
	global smap
	try:
		smap
	except NameError:
		# Check if symbol_map_file exist
		if not os.path.isfile(sf(conf.symbol_map_file)):
			raise exceptions.MissingFile(sf(conf.symbol_map_file),
					"Run parse_kconfig to generate it.")

		smap = dict()
		with open(sf(conf.symbol_map_file)) as f:
			for lnn in f:
				w = lnn.rstrip().split(sep=':')
				smap[int(w[0])] = w[1]

class __subprocess_timer__(Thread):
	def __init__(self, sprc, timeout):
		Thread.__init__(self, name='subprocess_timer')
		self.sprc = sprc
		self.last = time.time()
		self.exitit = False
		self.timeout = timeout
		self.timeouted = False
		if timeout > 0:
			self.start()
	def output(self):
		self.last = time.time()
	def exit(self):
		self.exitit = True
		return self.timeouted
	def run(self):
		while not self.exitit:
			now = time.time()
			if (now - self.last) >= self.timeout:
				self.timeouted = True
				os.kill(self.sprc.pid, signal.SIGTERM)
				return
			time.sleep(1)

def callsubprocess(process_name, process, show_output = True,
		return_output = False, env=os.environ, allowed_exit_codes = [0],
		allow_all_exit_codes = False, stdin = None, timeout = -1):
	sprc = subprocess.Popen(process, stdout = subprocess.PIPE,
			stderr = subprocess.STDOUT, stdin = subprocess.PIPE, env = env)


	try:
		os.mkdir(os.path.join(sf(conf.log_folder), process_name))
	except OSError:
		pass

	if stdin != None:
		for ln in stdin:
			sprc.stdin.write(bytes(ln + '\n', sys.getdefaultencoding()))
			sprc.stdin.flush()
		sprc.stdin.close()

	rtn = []
	timerout = __subprocess_timer__(sprc, timeout)
	with open(os.path.join(sf(conf.log_folder),
			process_name, time.strftime("%y-%m-%d-%H-%M-%S") + ".log"),
			"a") as f:
		f.write('::' + time.strftime("%y-%m-%d-%H-%M-%S-%f") + '::\n')
		for linen in sprc.stdout:
			timerout.output()
			try:
				line = linen.decode(sys.getdefaultencoding())
				f.write(line)
				if show_output:
					print(line, end="")
				if return_output:
					rtn.append(line.rstrip())
			except UnicodeDecodeError:
				if return_output:
					rtn.append('DecodeError')
	if timerout.exit():
		raise exceptions.ProcessTimeout(process_name, rtn)
	rtncode = sprc.wait()
	if rtncode not in allowed_exit_codes and not allow_all_exit_codes:
		raise exceptions.ProcessFailed(process, rtncode, rtn)
	return rtn

def get_kernel_env():
	env = dict(os.environ)
	env.update(conf.kernel_env)
	return env

def __dirty_repo__(path):
	cwd = os.getcwd()
	os.chdir(conf.absroot)
	out = subprocess.check_output(conf.git_describe_cmd)
	if re.search('dirty', out.decode(sys.getdefaultencoding())):
		raise exceptions.DirtyRepository(path)

def dirtycheck():
	__dirty_repo__(conf.absroot)
	__dirty_repo__(conf.linux_sources)
