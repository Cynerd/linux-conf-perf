from conf import conf

def reset():
	with open(conf.iteration_file, 'w') as f:
		f.write('0')

def inc():
	with open(conf.iteration_file, 'r') as f:
		it = int(f.read())
	it += 1
	with open(conf.iteration_file, 'w') as f:
		f.write(str(it))
