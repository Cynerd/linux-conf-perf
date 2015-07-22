-- In this table are tracked versions of tools in git
CREATE TABLE toolsgit (
	id BIGSERIAL PRIMARY KEY, -- Id
	git_describe text NOT NULL, -- Git describe string (--always --tags --dirty)
	git_commit text NOT NULL -- Commit hash of version of tool used for generating
);

-- In this table are stored all generated configurations
CREATE TABLE configurations (
	id BIGSERIAL PRIMARY KEY, -- Id
	hash char(34) NOT NULL, -- Hash of configuration
	cfile text NOT NULL, -- File path with configuration
	gtime timestamp NOT NULL, -- Time and date of generation
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);

-- This table stores measured data
CREATE TABLE measure (
	id BIGSERIAL PRIMARY KEY, -- Id
	conf BIGINT REFERENCES configurations (id), -- Reference to configuration
	mfile text NOT NULL, -- File with measuring output
	value BIGINT DEFAULT null, -- Measured data value
	mtime timestamp NOT NULL, -- Time and date of measurement
	toolgit BIGINT REFERENCES toolsgit (id) -- Reference to git version of tools 
);
