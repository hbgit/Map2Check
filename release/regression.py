# This script is intended to execute regression tests with expected results
import os
import subprocess

path = "../test/regression/"
freeDir = path + "free/"
targetDir = path + "target/"
derefDir = path + "deref/"
memtrackDir = path + "memtrack/"
trueDir =  path + "true/"

successList = []
errorList = []

falseFree = os.listdir(freeDir)
falseDeref = os.listdir(derefDir)
falseMemtrack = os.listdir(memtrackDir)
targetReached = os.listdir(targetDir)
trueValid = os.listdir(trueDir)

for program in trueValid:
    command = "./map2check -e TRUE --assume-malloc-true "
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


for program in falseFree: 
    
    command = "./map2check -e FALSE-FREE --assume-malloc-true "
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

for program in falseDeref: 
    
    command = "./map2check -e FALSE-DEREF --assume-malloc-true "
    command += derefDir + program
    print("Running: " + command + "\n")
    try:
        cmnd_output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True);                         
    except subprocess.CalledProcessError as exc:                                                                                                   
        print("FAIL\n")
        errorList.append(derefDir + program)
    else:              
        print("SUCCESS\n")                                                                                     
        successList.append(derefDir + program)  

for program in targetReached: 
    command = "./map2check -f __VERIFIER_error -e TARGET-REACHED --assume-malloc-true "
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
    command = "./map2check -e FALSE-MEMTRACK --assume-malloc-true "
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



print("################################\n\n")

print("OK:\n") 
for program in successList:
    print("\t" + program + "\n")       

print("\nFAILED:\n") 
for program in errorList:
    print("\t" + program + "\n")    