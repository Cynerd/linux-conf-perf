from conf import conf
from conf import sf

def reset():
	with open(sf(conf.iteration_file), 'w') as f:
		f.write('0')

def inc():
	with open(sf(conf.iteration_file), 'r') as f:
		it = int(f.read())
	it += 1
	with open(sf(conf.iteration_file), 'w') as f:
		f.write(str(it))
