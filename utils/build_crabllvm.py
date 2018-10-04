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

#if not os.path.isdir("clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04"):
#	os.system("wget http://releases.llvm.org/3.8.0/clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz")
#	os.system("tar -xf clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz")

llvm_dir=os.path.dirname(os.path.abspath("../../LLVM-3.8-Linux.tar.gz"))
##print(llvm_dir)
##os.system("export LLVM_DIR="+llvm_dir+"/clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04/share/llvm/cmake")
##os.system("echo $LLVM_DIR")
llvm_dir = llvm_dir + "/LLVM-3.8-Linux/share/llvm/cmake"
print(llvm_dir)

if not os.path.isdir("crab-llvm"):
	os.system("git clone https://github.com/seahorn/crab-llvm.git")

os.chdir("crab-llvm")
# TODO define commit to build

if not os.path.isdir("build"):
	os.system("mkdir build")

os.chdir("build")

print("cmake -DLLVM_DIR=" + llvm_dir + " -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-5 -DCMAKE_PROGRAM_PATH=/usr/bin -DCMAKE_INSTALL_PREFIX=run -DUSE_LDD=ON -DUSE_APRON=ON ../")

os.system("cmake -DLLVM_DIR=" + llvm_dir + " -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-5 -DCMAKE_PROGRAM_PATH=/usr/bin -DCMAKE_INSTALL_PREFIX=run -DUSE_LDD=ON -DUSE_APRON=ON ../")

#os.system("cmake -DCMAKE_INSTALL_PREFIX=run -DCMAKE_BUILD_TYPE= -DUSE_LDD=ON -DUSE_APRON=ON ../")

# cmake --build . --target extra 
os.system("cmake --build . --target crab && cmake ..")
os.system("cmake --build . --target ldd && cmake ..")
os.system("cmake --build . --target apron && cmake ..")
# os.system("cmake --build . --target llvm && cmake ..")
os.system("cmake --build . --target install")
os.system("")
os.system("pwd")

