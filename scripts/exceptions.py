class MissingFile(Exception):
	def __init__(self, f, advice):
		self.f = f
		self.advice = advice
	def __str__(self):
		if advice == None:
			return "No required file: " + f 
		else:
			return "No required file: " + f + "\n" + advice

class NoSolution(Exception):
	def __init__(self):
		pass
	def __str__(self):
		return "SAT solver found no solution. Statement is not satisfiable."

class ConfigurationError(Exception):
	def __init__(self, message):
		self.message = message;
	def __str__(self):
		return "Configuration error: " + message

class NoApplicableConfiguration(Exception):
	def __init__(self):
		pass
	def __str__(self):
		return "No applicable configuration find. All generated configurations were already applied."

class ProcessFailed(Exception):
	def __init__(self, process, returncode):
		self.process = process
		self.returncode = returncode
	def __str__(self):
		return "Process failed: " + str(self.process) + \
			" with return code: " + str(self.returncode)

class DatabaseUninitialized(Exception):
	def __str__(self):
		return "Database seems to be uninitialized."
