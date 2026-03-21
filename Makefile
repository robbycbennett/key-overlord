# Variables

PROGRAM_NAME = keyoverlord

CXX = clang++
CXXFLAGS = -O2 -Wall -flto -fno-exceptions -fno-ident -std=c++20

LD = clang++
LDFLAGS = -fuse-ld=lld -s -static

DEBUG = 0
ifneq ($(DEBUG), 0)
	CXXFLAGS += -DDEBUG
	LDFLAGS += -g
endif

RUN_DEBUGGER = 0
RUN_PROFILER = 0
ifneq ($(RUN_DEBUGGER), 0)
	ifeq ($(DEBUG), 0)
		LDFLAGS += -g
	endif
	PRE_RUN_COMMAND = gdb -ex=r -ex=bt --batch --args
else ifneq ($(RUN_PROFILER), 0)
	LDFLAGS += -g
	PRE_RUN_COMMAND = perf record -g -q --user-callchains --
endif

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.hpp)
OBJECTS = $(patsubst src/%.cpp,out/%.o,$(SOURCES))
PROGRAM = out/$(PROGRAM_NAME)


# Targets

$(PROGRAM): $(OBJECTS) | out
	$(LD) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS)

out/%.o: src/%.cpp $(HEADERS) Makefile | out
	$(CXX) -o $@ -c $< $(CXXFLAGS)

out:
	mkdir -p out


# Commands

.PHONY: clean copy run kill

clean:
	rm -rf out

copy: $(PROGRAM)
	rsync $(PROGRAM) root@laptop:/usr/local/bin/

run: $(PROGRAM)
	rsync $(PROGRAM) root@laptop:/usr/local/bin/
	ssh root@laptop -t $(PRE_RUN_COMMAND) /usr/local/bin/$(PROGRAM_NAME)

kill:
	ssh root@laptop pkill -9 -f /usr/local/bin/$(PROGRAM_NAME)
