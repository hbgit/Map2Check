#!/bin/bash
#
# Build Map2Check from scratch and setup environment
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.

# set -x

export PREFIX=`pwd`/install

export LD_LIBRARY_PATH="$PREFIX/lib:/lib/:$LD_LIBRARY_PATH"
export C_INCLUDE_PATH="$PREFIX/include:$C_INCLUDE_PATH"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:$PREFIX/share/pkgconfig:$PKG_CONFIG_PATH"


LLVM_VERSION=3.8.1
RUNDIR=`pwd`
SRCDIR=`dirname $0`
ABS_RUNDIR=`readlink -f $RUNDIR`
ABS_SRCDIR=`readlink -f $SRCDIR`

# create prefix directory
mkdir -p $PREFIX/bin
mkdir -p $PREFIX/lib
mkdir -p $PREFIX/lib32
mkdir -p $PREFIX/include

git_clone_or_pull()
{

	REPO="$1"
	FOLDER="$2"
	shift;shift

	if [ -d "$FOLDER" ]; then
		if [ "x$UPDATE" = "x1" ]; then
			cd $FOLDER && git pull && cd -
		fi
	else
		git clone $REPO $FOLDER $@
	fi
}


git_submodule_init()
{
	cd "$SRCDIR"

	git submodule init || exitmsg "submodule init failed"
	git submodule update || exitmsg "submodule update failed"

	cd -
}


exitmsg()
{
	echo "$1" >/dev/stderr
	exit 1
}

build()
{
	make -j4 CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" LDFLAGS="$LDFLAGS" $@ || exit 1
	return 0
}
check()
{
	if ! wget --version &>/dev/null; then
		if ! curl --version &>/dev/null; then
			echo "Need wget or curl to download files"
			exit 1
		fi

		# try replace wget with curl
		alias wget='curl -O'
	fi

	if ! python --version 2>&1 | grep -q 'Python 2'; then
		echo "llvm-3.2 needs python 2 to build"
		exit 1
	fi

	if ! bison --version &>/dev/null; then
		echo "STP needs bison program"
		exit 1
	fi

	if ! flex --version &>/dev/null; then
		echo "STP needs flex program"
		exit 1
	fi

	if [ "x$WITH_LLVM" != "x" ]; then
		if [ ! -d "$WITH_LLVM" ]; then
			exitmsg "Invalid LLVM directory given: $WITH_LLVM"
		fi
	fi
	if [ "x$WITH_LLVM_SRC" != "x" ]; then
		if [ ! -d "$WITH_LLVM_SRC" ]; then
			exitmsg "Invalid LLVM src directory given: $WITH_LLVM_SRC"
		fi
	fi
	if [ "x$WITH_LLVM_DIR" != "x" ]; then
		if [ ! -d "$WITH_LLVM_DIR" ]; then
			exitmsg "Invalid LLVM src directory given: $WITH_LLVM_DIR"
		fi
	fi
}

build_llvm()
{
	if [ ! -d "llvm-${LLVM_VERSION}" ]; then
		wget http://llvm.org/releases/${LLVM_VERSION}/llvm-${LLVM_VERSION}.src.tar.xz || exit 1
		wget http://llvm.org/releases/${LLVM_VERSION}/cfe-${LLVM_VERSION}.src.tar.xz || exit 1

		tar -xf llvm-${LLVM_VERSION}.src.tar.xz || exit 1
		tar -xf cfe-${LLVM_VERSION}.src.tar.xz || exit 1

                # rename llvm folder
                mv llvm-${LLVM_VERSION}.src llvm-${LLVM_VERSION}
		# move clang to llvm/tools and rename to clang
		mv cfe-${LLVM_VERSION}.src llvm-${LLVM_VERSION}/tools/clang


		rm -f llvm-${LLVM_VERSION}.src.tar.xz &>/dev/null || exit 1
		rm -f cfe-${LLVM_VERSION}.src.tar.xz &>/dev/null || exit 1
  else
    echo "LLVM folder already exists, proceding to compilation"
	fi

	mkdir -p llvm-build-cmake
	cd llvm-build-cmake || exitmsg "downloading failed"

	# configure llvm
	if [ ! -d CMakeFiles ]; then
		cmake ../llvm-${LLVM_VERSION} \
			-DCMAKE_BUILD_TYPE=Release\
			-DLLVM_INCLUDE_EXAMPLES=OFF \
			-DLLVM_INCLUDE_TESTS=OFF \
			-DLLVM_ENABLE_TIMESTAMPS=OFF \
			-DLLVM_TARGETS_TO_BUILD="X86" \
			-DLLVM_ENABLE_PIC=ON \
			-DCMAKE_C_FLAGS_DEBUG="-O0 -g" \
			-DCMAKE_CXX_FLAGS_DEBUG="-O0 -g" || clean_and_exit
	fi

	# build llvm
	ONLY_TOOLS='opt clang llvm-link llvm-dis llvm-nm' build
	cd -
}

install_llvm()
{
  LLVM_LOCATION=llvm-build-cmake
  cp $LLVM_LOCATION/bin/clang $PREFIX/bin/clang || exit 1
	cp $LLVM_LOCATION/bin/opt $PREFIX/bin/opt || exit 1
	cp $LLVM_LOCATION/bin/llvm-link $PREFIX/bin/llvm-link || exit 1
	cp $LLVM_LOCATION/bin/llvm-nm $PREFIX/bin/llvm-nm || exit 1

  export LLVM_DIR=$ABS_RUNDIR/llvm-build-cmake/share/llvm/cmake/
	export LLVM_BUILD_PATH=$ABS_RUNDIR/llvm-build-cmake/
  export LLVM_SRC_PATH="$ABS_RUNDIR/llvm-${LLVM_VERSION}/"
}

clean_and_exit()
{
	CODE="$1"

	if [ "$2" = "git" ]; then
		git clean -xdf
	else
		rm -rf *
	fi

	exit $CODE
}

minisat()
{
  git_clone_or_pull https://github.com/stp/minisat.git minisat
	cd minisat
	export CPPFLAGS="$CPPFLAGS `pkg-config --cflags zlib`"
	(build lr && make prefix=$PREFIX install-headers) || exit 1
	cp build/release/lib/libminisat.a $PREFIX/lib/ || exit 1

cd -
}

stp()
{
  git_clone_or_pull https://github.com/stp/stp.git stp
	cd stp || exitmsg "Cloning failed"

  echo $PREFIX
	if [ ! -d CMakeFiles ]; then
		cmake . -DCMAKE_INSTALL_PREFIX=$PREFIX \
			-DSTP_TIMESTAMPS:BOOL="OFF" \
			-DCMAKE_CXX_FLAGS_RELEASE=-O2 \
			-DCMAKE_C_FLAGS_RELEASE=-O2 \
			-DCMAKE_BUILD_TYPE=Release \
			-DBUILD_SHARED_LIBS:BOOL=OFF \
			-DENABLE_PYTHON_INTERFACE:BOOL=OFF #|| clean_and_exit 1 "git"
	fi

	(build "OPTIMIZE=-O2 CFLAGS_M32=install" && make install) || exit 1
  cd -
}

klee()
{
  # build klee
	git_clone_or_pull "-b 3.0.8 https://github.com/rafaelsa94/klee.git" klee || exitmsg "Cloning failed"

	mkdir -p klee-build/
	cd klee-build/

	if [ ! -d CMakeFiles ]; then
		cmake ../klee -DCMAKE_INSTALL_PREFIX=$PREFIX \
			-DCMAKE_BUILD_TYPE=Release \
			-DKLEE_RUNTIME_BUILD_TYPE=Release+Asserts \
			-DENABLE_SOLVER_STP=ON \
			-DSTP_DIR=`pwd`/../stp \
			-DLLVM_CONFIG_BINARY=`pwd`/../llvm-build-cmake/bin/llvm-config \
			-DENABLE_UNIT_TESTS=OFF \
		|| clean_and_exit 1 "git"
	fi
	cd -
}

map2check() {
	make all
	make install

}

gtest() {
	cd /usr/src/gtest
	cmake CMakeLists.txt
	make

	# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
	cp *.a /usr/lib

	cd -
}


build_llvm
install_llvm
minisat
stp
klee
gtest
map2check

  # echo $LD_LIBRARY_PATH
