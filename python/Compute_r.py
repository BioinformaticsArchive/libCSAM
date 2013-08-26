import sys
import re
import math


if len(sys.argv)!=3:
        print 'Use: python ./Compute_r.py qualityFile.qual referenceFile.qual'
        sys.exit('Error')

arg1 = sys.argv[1]
arg2 = sys.argv[2]
f=open(arg1,'r')
Realqual = open(arg2, 'r')


values = []
rmax = 0
cont = 0
print arg1
while(1):
	"""get each line"""
	line = f.readline()
	lineReal = Realqual.readline()
	if not line:
		break
	i = 0
	max_local = 0
	while i < (len(line) - 1):
		val = (max(ord(lineReal[i]), ord(line[i])) - 33)* 1.0  / (min(ord(lineReal[i]), ord(line[i])) - 33.0)
		max_local = max(max_local, val)
		i += 1
	values.append(max_local)
	rmax += max_local
	cont += 1

values.sort()
print (rmax*1.0/cont), values[long(0.025*cont)],  values[long(0.975*cont)]

