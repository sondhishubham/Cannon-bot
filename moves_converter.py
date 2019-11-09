text = ''
infile = sys.args[1]
with open(infile) as f:
    for line in f.readlines():
	if "NORMAL" in line:
	    text+= line.split('"data": "')[1].split('"} Re')[0] + '\n'
with open(infile[:-4+'moves.log'],'w+') as f:
    f.write(text)
