#!/usr/bin/python

import sys

if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <file1> <file2>"
    sys.exit(1)

with open(sys.argv[1]) as f:
    array = f.readlines()
    
with open(sys.argv[2]) as f:
    array2 = f.readlines()

def lcs(s1, s2):
    m = [[0] * (1 + len(s2)) for i in range(1 + len(s1))]
    longest, x_longest = 0, 0
    for x in range(1, 1 + len(s1)):
        for y in range(1, 1 + len(s2)):
            if s1[x - 1] == s2[y - 1]:
                m[x][y] = m[x - 1][y - 1] + 1
                if m[x][y] > longest:
                    longest = m[x][y]
                    x_longest = x
            else:
                m[x][y] = 0
    return s1[x_longest - longest: x_longest]
    
for i in range(0, len(array)):
    for j in range(0, len(array2)):
        s = lcs(array[i], array2[j]).strip()
        if s.count(' ') >= 4 and len(s) > 15:
            print "Line", i, "in file1 and line", j, "in file2:"
            print s
            sys.stdout.write(array[i])
            sys.stdout.write(array2[j])
	    print
