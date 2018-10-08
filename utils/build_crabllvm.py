#!/usr/bin/env python3

import os
import sys
import shutil

FORCE_BUILD = False

if not FORCE_BUILD and os.path.isdir("build/dependencies/crab-llvm/build/run"):
	if not os.path.isdir("release/bin/crabllvm"):
		os.system("mkdir release/bin/crabllvm")
		os.system("cp -r build/dependencies/crab-llvm/build/run/* release/bin/crabllvm/")
	sys.exit(0)


os.chdir("build/dependencies/")

#llvm_dir=os.path.dirname(os.path.abspath("../../LLVM-3.8-Linux.tar.gz"))
#print(llvm_dir)
#llvm_dir = llvm_dir + "/LLVM-3.8-Linux/share/llvm/cmake"
llvm_dir = "/llvm/release/llvm600/lib/cmake/llvm/"


if not os.path.isdir("crab-llvm"):
	os.system("git clone https://github.com/hbgit/crab-llvm.git")

os.chdir("crab-llvm")
# Defining brach to build
os.system("git checkout dev-llvm-6.0")

if not os.path.isdir("build"):
	os.system("mkdir build")

os.chdir("build")

print("cmake -DLLVM_DIR=" + llvm_dir + " -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=_DIR_ -DUSE_LDD=ON -DUSE_APRON=ON ../")

# cmake --build . --target extra 
os.system("cmake --build . --target crab && cmake ..")
os.system("cmake --build . --target ldd && cmake ..")
os.system("cmake --build . --target apron && cmake ..")
# os.system("cmake --build . --target llvm && cmake ..")
os.system("cmake --build . --target install")
os.system("")
os.system("pwd")

