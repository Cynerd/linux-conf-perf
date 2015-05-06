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

class PhaseMismatch(Exception):
	def __init__(self):
		pass
	def __str__(self):
		return "Phase in " + conf.phase_file + " is unknown."

class ConfigurationError(Exception):
	def __init__(self, message):
		self.message = message;
	def __str__(self):
		return "Configuration error: " + message

class SolutionGenerated(Exception):
	def __init__(self):
		pass
	def __str__(self):
		return "Solution already generated."
