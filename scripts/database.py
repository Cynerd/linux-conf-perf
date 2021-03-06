import os
import datetime
import postgresql
import collections

import utils
import exceptions
from conf import conf
from conf import sf

def __git_describe__():
	return utils.callsubprocess('git_describe',
			conf.git_describe_cmd, False, True)[0]

def __git_commit__():
	return utils.callsubprocess('git_rev_parse',
			conf.git_commit_cmd, False, True)[0]

def __timestamp__():
	return datetime.datetime.now().strftime('%y-%m-%d-%H-%M-%S')

Config = collections.namedtuple('Config', 'id hash config') # Named tuple for configuration
Measure = collections.namedtuple('Measure', 'id conf_id output value') # Named tuple for measurement

class database:
	"Class used for accessing PostgreSQL project database."
	def __init__(self):
		self.db = postgresql.open(database = conf.db_database,
				user = conf.db_user,
				password = conf.db_password,
				host = conf.db_host,
				port = conf.db_port
				)
		# check if tables are present
		tables = ('toolsgit', 'configurations', 'measure')
		for tab in tables:
			val = self.db.prepare("""SELECT COUNT(*) FROM pg_class
							   WHERE relname = $1""")(tab)[0][0]
			if val < 1:
				raise exceptions.DatabaseUninitialized()

	def check_toolsgit(self):
		"Return id of toolsgit row. If missing, it is inserted"
		ds = __git_describe__()
		cm = __git_commit__()
		ps = self.db.prepare("""SELECT id FROM toolsgit
							  WHERE git_describe = $1 AND git_commit = $2
							  """)
		id = ps(ds, cm)
		if id:
			return id[0][0]
		ps = self.db.prepare("""INSERT INTO toolsgit
						   (git_describe, git_commit)
						   VALUES
						   ($1, $2);
						   """)
		ps(ds, cm)
		return self.check_toolsgit()

	def check_linuxgit(self):
		"Return id of linuxgit row. If missing, it is inserted."
		wd = os.getcwd()
		os.chdir(sf(conf.linux_sources))
		ds = __git_describe__()
		cm = __git_commit__()
		os.chdir(wd)
		ps = self.db.prepare("""SELECT id FROM linuxgit
							  WHERE git_describe = $1 AND git_commit = $2
							  """)
		id = ps(ds, cm)
		if id:
			return id[0][0]
		ps = self.db.prepare("""INSERT INTO linuxgit
						   (git_describe, git_commit)
						   VALUES
						   ($1, $2);
						   """)
		ps(ds, cm)
		return self.check_linuxgit()

	def add_configuration(self, hash, txtconfig, generator):
		"Add configuration to database."
		ps = self.db.prepare("""INSERT INTO configurations
								(hash, config, gtime, toolgit, linuxgit, generator)
								VALUES
								($1, $2, $3, $4, $5, $6);
								""")
		gt = self.check_toolsgit()
		lgt = self.check_linuxgit()
		tm = datetime.datetime.now()
		ps(hash, '\n'.join(txtconfig), tm, gt, lgt, generator)

	def get_configration(self, hash):
		"Return configration id for inserted hash."
		ps = self.db.prepare("""SELECT id, config FROM configurations
								WHERE hash = $1""")
		rtn = []
		for dt in ps(hash):
			rtn.append(Config(dt[0], hash, dt[1].split('\n')))
		return rtn

	def add_measure(self, output, result, conf_id, value = None):
		"Add measurement."
		ps = self.db.prepare("""INSERT INTO measure
								(conf, output, value, mtime, toolgit,
								linuxgit, measurement, result)
								VALUES
								($1, $2, $3, $4, $5, $6, $7, $8);
								""")
		gt = self.check_toolsgit()
		lgt = self.check_linuxgit()
		tm = datetime.datetime.now()
		ps(conf_id, output, value, tm, gt, lgt, conf.measure_identifier, result)

	def update_measure(self, measure_id, value):
		"Update measured value"
		ps = self.db.prepare("""UPDATE measure SET
								(value) = ($2)
								WHERE
								id = $1;
								""")
		ps(measure_id, value)

	def get_measures(self, conf_id):
		"Get measures for configuration with conf_id id"
		ps = self.db.prepare("""SELECT id, output, value FROM measure
								WHERE conf = $1;
								""")
		rtn = []
		for dt in ps(conf_id):
			rtn.append(Measure(dt[0], conf_id, dt[1], dt[2]))
		return rtn

	def get_unmeasured(self):
		"Returns list of all unmeasured configurations."
		ps = self.db.prepare("""SELECT id, hash, config FROM configurations
								WHERE id NOT IN
								(SELECT conf FROM measure)
								""")
		rtn = []
		for dt in ps():
			rtn.append(Config(dt[0], dt[1], dt[2].split('\n')))
		return rtn

	def add_configsort(self, configopt):
		"Add configuration option to sorted list"
		ps = self.db.prepare("""INSERT INTO configopt
								(configopt) VALUES ($1)
								""")
		ps(configopt)

	def get_configsort(self):
		"Returns sorted list of all configuration options"
		ps = self.db.prepare("""SELECT id, configopt FROM configopt
								ORDER BY id ASC
								""")
		rtn = []
		itms = ps()
		for id, config in itms:
			rtn.append(config)
		return rtn
