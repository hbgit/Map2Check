LLVM_VERSION     := 3.8.1
LLVM_BUILD_DIR   := dependencies/llvm-build-cmake
LLVM_SRC_DIR     := dependencies/llvm-$(LLVM_VERSION)


CC               := $(LLVM_BUILD_DIR)/bin/clang
CXX              := g++

INC              := -I $(LLVM_SRC_DIR)/include
CXXFLAGS         := -fno-rtti -O0 -g $(INC)
CXXFLAGS_NORTTI  := -O0 -g
LDFLAGS          := -static -lboost_program_options -lboost_filesystem -lboost_system
PLUGIN_CXXFLAGS  := -fpic

LLVM_CXXFLAGS    := `$(LLVM_BUILD_DIR)/bin/llvm-config --cxxflags`
LLVM_LDFLAGS     := `$(LLVM_BUILD_DIR)/bin/llvm-config --ldflags --libs --system-libs`


LLVM_LINK        := $(LLVM_BUILD_DIR)/llvm-link

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_MODULES_DIR := modules
SRC_LLVM_DIR    := modules/src_llvm
BUILDDIR        := build
OBJECTS_I       := $(BUILDDIR)/map2check.o $(BUILDDIR)/caller.o $(BUILDDIR)/funcpass.o $(BUILDDIR)/allocapass.o $(BUILDDIR)/storepass.o $(BUILDDIR)/assertpass.o
SAMPLES         := $(wildcard samples/*.c)


.PHONY: all
all: make_builddir \
	  $(BUILDDIR)/funcpass \
		$(BUILDDIR)/assertpass \
		$(BUILDDIR)/storepass \
		$(BUILDDIR)/allocapass \
		$(BUILDDIR)/utils \
		$(BUILDDIR)/caller \
		$(BUILDDIR)/memoryutils \
		$(BUILDDIR)/map2check


.PHONY: release
release: all
	@test -d release || mkdir release
	cp $(BUILDDIR)/map2check release/
	cp $(BUILDDIR)/utils.bc release/lib/
	cp $(BUILDDIR)/memoryutils.bc release/lib/

.PHONY: pass
pass: $(BUILDDIR)/funcpass
	$(CXX) -shared -o $@.so $^.o

.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/funcpass: $(SRC_LLVM_DIR)/pass/FuncPass.cpp
			$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) $(PLUGIN_CXXFLAGS) -o $@.o

$(BUILDDIR)/assertpass: $(SRC_LLVM_DIR)/pass/AssertsPass.cpp
			$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) $(PLUGIN_CXXFLAGS) -o $@.o

$(BUILDDIR)/storepass: $(SRC_LLVM_DIR)/pass/StorePass.cpp
			$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) $(PLUGIN_CXXFLAGS) -o $@.o


$(BUILDDIR)/utils: $(SRC_LLVM_DIR)/utils/Utils.c
			$(CC) -I./dependencies/klee/include/ -c -emit-llvm  $^ -o $@.bc

$(BUILDDIR)/memoryutils: $(SRC_LLVM_DIR)/utils/MemoryUtils.c
			$(CC) -c -emit-llvm  $^ -o $@.bc

$(BUILDDIR)/allocapass: $(SRC_LLVM_DIR)/pass/AllocaPass.cpp
			$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) $(PLUGIN_CXXFLAGS) -o $@.o


$(BUILDDIR)/caller: $(SRC_MODULES_DIR)/main/caller.cpp
			$(CXX) -c $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) $(PLUGIN_CXXFLAGS) -o $@.o

$(BUILDDIR)/map2check: $(SRC_MODULES_DIR)/main/map2check.cpp
			$(CXX) -c $^ -o $@.o
			$(CXX) $(OBJECTS_I) $(LDFLAGS) $(LLVM_LDFLAGS) -o $(BUILDDIR)/map2check


.PHONY: samples
samples: $(SAMPLES)
		clang -emit-llvm -c $^ -o $^.bc


.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*
