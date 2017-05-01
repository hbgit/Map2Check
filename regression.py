# This script is intended to execute regression tests with expected results
import os
import subprocess

path = "../test/regression/"
freeDir = path + "free/"
targetDir = path + "target/"
memtrackDir = path + "memtrack/"
trueDir =  path + "true/"

successList = []
errorList = []

falseFree = os.listdir(freeDir)
falseMemtrack = os.listdir(memtrackDir)
targetReached = os.listdir(targetDir)
trueValid = os.listdir(trueDir)

for program in falseFree: 
    
    command = "./map2check -e FALSE-FREE "
    command += freeDir + program
    print("Running: " + command + "\n")
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        print("FAIL\n")
        errorList.append(freeDir + program)
    else:              
        print("SUCCESS\n")                                                                                     
        successList.append(freeDir + program)     

for program in targetReached: 
    command = "./map2check -f __VERIFIER_error -e TARGET-REACHED "
    command += targetDir + program
    print("Running: " + command + "\n")
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        print("FAIL\n")
        errorList.append(targetDir + program)
    else:          
        print("SUCCESS\n")                                                                                         
        successList.append(targetDir + program)   


for program in falseMemtrack:
    command = "./map2check -e FALSE-MEMTRACK "
    command += memtrackDir + program
    print("Running: " + command + "\n")
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        print("FAIL\n")
        errorList.append(memtrackDir + program)
    else:          
        print("SUCCESS\n")                                                                                         
        successList.append(memtrackDir + program)     

for program in trueValid:
    command = "./map2check -e TRUE "
    command += trueDir + program
    print("Running: " + command + "\n")
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        print("FAIL\n")
        errorList.append(trueDir + program)
    else:               
        print("SUCCESS\n")                                                                                      
        successList.append(trueDir + program)     


print("################################\n\n")

print("OK:\n") 
for program in successList:
    print("\t" + program + "\n")       

print("\nFAILED:\n") 
for program in errorList:
    print("\t" + program + "\n")    