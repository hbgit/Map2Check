#!/usr/bin/env python3

import os
import sys

print("Test....")
os.system("pwd")
os.chdir("build/dependencies/")

if not os.path.isdir("clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04"):
	os.system("wget http://releases.llvm.org/3.8.0/clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz")
	os.system("tar -xf clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz")

llvm_dir=os.path.dirname(os.path.abspath("clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04"))
#print(llvm_dir)
#os.system("export LLVM_DIR="+llvm_dir+"/clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04/share/llvm/cmake")
#os.system("echo $LLVM_DIR")
llvm_dir = llvm_dir + "/clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-16.04/share/llvm/cmake"
print(llvm_dir)

if not os.path.isdir("crab-llvm"):
	os.system("git clone https://github.com/seahorn/crab-llvm.git")

os.chdir("crab-llvm")
# TODO define commit to build

if not os.path.isdir("build"):
	os.system("mkdir build")

os.chdir("build")
os.system("cmake -DCMAKE_INSTALL_PREFIX=run -DCMAKE_BUILD_TYPE=Release -DUSE_LDD=ON -DUSE_APRON=ON -DLLVM_DIR="+llvm_dir+" ../")
# cmake --build . --target extra 
os.system("cmake --build . --target crab && cmake ..")
os.system("cmake --build . --target ldd && cmake ..")
os.system("cmake --build . --target apron && cmake ..")
os.system("cmake --build . --target install")
os.system("")
os.system("pwd")

