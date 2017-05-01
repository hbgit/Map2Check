# This script is intended to execute regression tests with expected results
import os
import subprocess

path = "../test/regression/"
freeDir = path + "free/"
memtrackDir = path + "memtrack/"
trueDir =  path + "true/"

successList = []
errorList = []

falseFree = os.listdir(freeDir)
falseMemtrack = os.listdir(memtrackDir)
trueValid = os.listdir(trueDir)

for program in falseFree: 
    command = "./map2check -e FALSE-FREE "
    command += freeDir + program
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        errorList.append(freeDir + program)
    else:                                                                                                   
        successList.append(freeDir + program)     

for program in falseMemtrack:
    command = "./map2check -e FALSE-MEMTRACK "
    command += memtrackDir + program
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        errorList.append(memtrackDir + program)
    else:                                                                                                   
        successList.append(memtrackDir + program)     

for program in trueValid:
    command = "./map2check -e TRUE "
    command += trueDir + program
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        errorList.append(trueDir + program)
    else:                                                                                                   
        successList.append(trueDir + program)     


print("################################\n\n")

print("SUCCESS:\n") 
for program in successList:
    print("\t" + program + "\n")       

print("\nFAILED:\n") 
for program in errorList:
    print("\t" + program + "\n")    