-- In this table are tracked versions of tools in git
CREATE TABLE toolsgit (
	id BIGSERIAL PRIMARY KEY, -- Id
	git_describe TEXT NOT NULL, -- Git describe string (--always --tags --dirty)
	git_commit TEXT NOT NULL -- Commit hash of version of tool used for generating
);

-- In this table are tracked versions of measured Linux in git
CREATE TABLE linuxgit (
	id BIGSERIAL PRIMARY KEY, -- Id
	git_describe TEXT NOT NULL, -- Git describe scring (--always --tags --dirty)
	git_commit TEXT NOT NULL -- Commit hash of version of tool used for generating
);

-- In this table are stored all generated configurations
CREATE TABLE configurations (
	id BIGSERIAL PRIMARY KEY, -- Id
	hash char(32) NOT NULL, -- Hash of configuration
	generator TEXT NOT NULL, -- Text identificator of configure generation method
	config TEXT NOT NULL, -- Full configuration in text form
	gtime timestamp NOT NULL, -- Time and date of generation
	linuxgit BIGINT REFERENCES linuxgit (id), -- Reference to git version of Linux
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);

-- This table stores measured data
CREATE TABLE measure (
	id BIGSERIAL PRIMARY KEY, -- Id
	conf BIGINT REFERENCES configurations (id), -- Reference to configuration
	measurement TEXT NOT NULL, -- Text identifivator of measuring tool
	output TEXT NOT NULL, -- Output of boot
	result TEXT NOT NULL, -- Result of boot script, if exited normally
	value DOUBLE PRECISION DEFAULT null, -- Measured data value
	mtime timestamp NOT NULL, -- Time and date of measurement
	linuxgit BIGINT REFERENCES linuxgit (id), -- Reference to git version of Linux
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);

-- In this table are sorted all used configuration options
-- Order in this table is fundamental for configuration hash calculation
CREATE TABLE configopt (
	id BIGSERIAL PRIMARY KEY, -- Id
	configopt TEXT NOT NULL -- Name of configuration option
);
