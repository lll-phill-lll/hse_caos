LLVM_CONFIG ?= llvm-config

CXX      := clang++
CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags) -std=c++20 -O2
LDFLAGS  := $(shell $(LLVM_CONFIG) --ldflags)
LIBS     := $(shell $(LLVM_CONFIG) --libs core orcjit native passes) \
            $(shell $(LLVM_CONFIG) --system-libs)

ifeq ($(shell uname -s),Linux)
  LIBS += -ldl
endif

SRC := main.cpp jit.cpp cppjit.cpp
BIN := jitdb
GENERATED := query_gen.cpp query_gen.so jit_query.o query.ll query.opt.ll query.s

ROWS ?= 30000000
Q    ?= (c0*c0 + c1*c1 < 5000) ? max(c0, c1) : c2 - c3

all: $(BIN)

$(BIN): $(SRC) expr.hpp interp.hpp jit.hpp cppjit.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDFLAGS) $(LIBS)

run: $(BIN)
	./$(BIN) --rows $(ROWS) "$(Q)"

clean:
	rm -f $(BIN) $(GENERATED)

.PHONY: all run clean
