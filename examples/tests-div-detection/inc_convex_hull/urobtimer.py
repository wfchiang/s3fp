#!/usr/bin/env python 

import os
import sys
import time 

ST = time.time() 

for i in range(0, 10): 
    os.system("make test-urob")

ET = time.time() 

print "ave. urob time: " + str((float(ET) - float(ST)) / 10.0) 
