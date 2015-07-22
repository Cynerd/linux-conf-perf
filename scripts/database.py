import datetime
import postgresql
import collections

import utils
from conf import conf

def __git_describe__():
	return utils.callsubprocess('git_describe',
			conf.git_describe_cmd, False).rstrip()

def __git_commit__():
	return utils.callsubprocess('git_rev_parse',
			conf.git_commit_cmd, False).rstrip()

def __timestamp__():
	return datetime.datetime.now().strftime('%y-%m-%d-%H-%M-%S')

Config = collections.namedtuple('Config', 'id hash cfile') # Named tuple for configuration
Measure = collections.namedtuple('Measure', 'id conf_id mfile value') # Named tuple for measurement

class database:
	"Class used for accessing PostgreSQL project database."
	def __init__(self):
		self.db = postgresql.open(database = conf.db_database,
				user = conf.db_user,
				password = conf.db_password,
				host = conf.db_host,
				port = conf.db_port
				)

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

	def add_configuration(self, hash, cfile):
		"Add configuration to database."
		ps = self.db.prepare("""INSERT INTO configurations
								(hash, cfile, gtime, toolgit)
								VALUES
								($1, $2, $3, $4);
								""")
		gt = self.check_toolsgit()
		tm = datetime.datetime.now()
		ps(hash, cfile, tm, gt)

	def get_configration(self, hash):
		"Return configration id for inserted hash."
		ps = self.db.prepare("""SELECT id, cfile FROM configurations
								WHERE hash = $1""")
		rtn = []
		for dt in ps(hash):
			rtn.append(Config(dt[0], hash, dt[1]))
		return rtn

	def add_measure(self, mfile, conf_id, value = None):
		"Add measurement."
		ps = self.db.prepare("""INSERT INTO measure
								(conf, mfile, value, mtime, toolgit)
								VALUES
								($1, $2, $3, $4, $5);
								""")
		gt = self.check_toolsgit()
		tm = datetime.datetime.now()
		ps(conf_id, mfile, value, tm, gt)

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
		ps = self.db.prepare("""SELECT id, mfile, value FROM measure
								WHERE conf = $1;
								""")
		rtn = []
		for dt in ps(conf_id):
			rtn.append(Measure(dt[0], conf_id, dt[1], dt[2]))
		return rtn

	def get_unmeasured(self):
		"Returns list of all unmeasured configurations."
		ps = self.db.prepare("""SELECT c.id, c.hash, c.cfile
								FROM configurations AS c, measure AS m
								WHERE c.id NOT IN m.conf;
								""")
		rtn = []
		for dt in ps():
			rtn.append(Config(dt[0], dt[1], dt[2]))
		return rtn