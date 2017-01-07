CXX              := clang++
CXXFLAGS         := -fno-rtti -O0 -g
CXXFLAGS_NORTTI  := -O0 -g
LDFLAGS          := -lboost_program_options -lboost_filesystem -lboost_system
PLUGIN_CXXFLAGS  := -fpic

LLVM_CXXFLAGS    := `llvm-config --cxxflags`
LLVM_LDFLAGS     := `llvm-config --ldflags --libs`
# LLVM_LDFLAGS     := `llvm-config --ldflags --libs --system-libs`

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_MODULES_DIR := modules
SRC_LLVM_DIR    := modules/src_llvm
BUILDDIR        := build
OBJECTS_I       := $(BUILDDIR)/map2check.o $(BUILDDIR)/caller.o $(BUILDDIR)/funcpass.o
SAMPLES         := $(wildcard samples/*.c)


.PHONY: all
all: make_builddir \
	  $(BUILDDIR)/funcpass \
		$(BUILDDIR)/caller \
		$(BUILDDIR)/map2check


.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/funcpass: $(SRC_LLVM_DIR)/pass/FuncPass.cpp
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
