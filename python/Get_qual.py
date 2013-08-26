import sys
import re

if len(sys.argv)!=2:
	print 'Use: python ./Get_qual.py SAMfile'
	sys.exit('Error')

arg1 = sys.argv[1]
f=open(arg1,'r')
namefile = arg1 + '.qual'
qual = open(namefile, 'w')
b=0

print "parsing"
while(1):
	"""get each line"""
	line = f.readline()
	if not line:
		break
	if line[0]!='@':	
		a=line.split('\t')
		qual.write(a[10])
		if len(a)>11:
			qual.write('\n')
			b=12
			l = a[11]
			while 	b< len(a):
	 			l += '\t' + a[b]
				b = b+1	

