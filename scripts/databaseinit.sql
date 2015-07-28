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
	cfile TEXT NOT NULL, -- File path with configuration
	gtime timestamp NOT NULL, -- Time and date of generation
	linuxgit BIGINT REFERENCES linuxgit (id), -- Reference to git version of Linux
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);

-- This table stores measured data
CREATE TABLE measure (
	id BIGSERIAL PRIMARY KEY, -- Id
	conf BIGINT REFERENCES configurations (id), -- Reference to configuration
	measurement TEXT NOT NULL, -- Text identifivator of measuring tool
	mfile TEXT NOT NULL, -- File with measuring output
	value DOUBLE PRECISION DEFAULT null, -- Measured data value
	mtime timestamp NOT NULL, -- Time and date of measurement
	linuxgit BIGINT REFERENCES linuxgit (id), -- Reference to git version of Linux
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);
