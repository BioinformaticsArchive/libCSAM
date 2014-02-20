import sys
import re

if len(sys.argv)!=4:
	print 'Use: python ./Change_qual_letter.py file.sam letter name_output.sam'
	sys.exit('Error')

arg1 = sys.argv[1]
print arg1
f=open(arg1,'r')
letter = sys.argv[2]
print letter
arg2 = sys.argv[3]
print arg2

newSAM = open(arg2, 'w')


print "parsing"
while(1):
	"""get each line"""
	line = f.readline()
	if not line:
		break
	if line[0]!='@':
		a=line.split('\t')
		newLine = a[0]+'\t'+a[1]+'\t'+a[2]+'\t'+a[3]+'\t'+a[4]+'\t'+a[5]+'\t'
		newLine += a[6]+'\t'+a[7]+'\t'+a[8] +'\t'+a[9]+'\t'
		qual = letter*len(a[9])
		newLine += qual
		if len(a)>11:
			b=12
			l = a[11]
			while 	b< len(a):
	 			l += '\t' + a[b]
				b = b+1	
			newLine += '\t'+l
		else:
			newLine += '\n'
		newSAM.write(newLine)
	else:	
		newSAM.write(line)

