# Variables

PROGRAM_NAME := keyoverlord

COMPILER := clang++

SOURCE_TYPE := .cpp
HEADER_TYPE := .hpp
OBJECT_TYPE := .o

STANDARD := -std=c++20
OPTIMIZE := -O1

IGNORED := -Wno-c++20-extensions -Wno-c++23-extensions -Wno-c++98-compat-pedantic -Wno-covered-switch-default -Wno-padded -Wno-unsafe-buffer-usage

ifeq ($(COMPILER), clang++)
	WARNINGS := -Weverything $(IGNORED)
else
	WARNINGS := -Wall $(IGNORED)
endif

COMPILE_FLAGS := $(STANDARD) $(OPTIMIZE) $(WARNINGS)
LINK_FLAGS := -fuse-ld=lld -static

DEBUGGER := 0
PROFILER := 0
ifneq ($(DEBUGGER), 0)
	LINK_FLAGS := $(LINK_FLAGS) -g
	PRE_RUN_COMMAND := gdb -ex=r -ex=bt --batch --args
else ifneq ($(PROFILER), 0)
	LINK_FLAGS := $(LINK_FLAGS) -g
	PRE_RUN_COMMAND := perf record -g -q --user-callchains --
endif

DEBUG_PRINTING := 0
ifneq ($(DEBUG_PRINTING), 0)
	COMPILE_FLAGS := $(COMPILE_FLAGS) -DDEBUG
endif

SOURCE_FOLDER := src
OUTPUT_FOLDER := out

CLEAN_FOLDERS := $(OUTPUT_FOLDER)

ECHO_NEW_LINE := echo
CLEAN_COMMAND := rm -rf $(CLEAN_FOLDERS)

SOURCES := $(wildcard $(SOURCE_FOLDER)/*$(SOURCE_TYPE))
HEADERS := $(wildcard $(SOURCE_FOLDER)/*$(HEADER_TYPE))
OBJECTS := $(patsubst $(SOURCE_FOLDER)/%$(SOURCE_TYPE),$(OUTPUT_FOLDER)/%$(OBJECT_TYPE),$(SOURCES))
PROGRAM := $(OUTPUT_FOLDER)/$(PROGRAM_NAME)


# Targets


$(PROGRAM): $(OBJECTS) | $(OUTPUT_FOLDER)
	$(COMPILER) -o $(PROGRAM) $(OBJECTS) $(LINK_FLAGS)

$(OUTPUT_FOLDER)/%$(OBJECT_TYPE): $(SOURCE_FOLDER)/%$(SOURCE_TYPE) $(HEADERS) Makefile | $(OUTPUT_FOLDER)
	$(COMPILER) -o $@ -c $< $(COMPILE_FLAGS)

$(OUTPUT_FOLDER):
	mkdir -p $(OUTPUT_FOLDER)


# Commands


clean:
	$(CLEAN_COMMAND)

help:
	@$(ECHO_NEW_LINE)
	@echo make
	@echo     Compile the program
	@$(ECHO_NEW_LINE)
	@echo make clean
	@echo     Remove the obj and bin folders
	@$(ECHO_NEW_LINE)
	@echo make help
	@echo     Get help with the Makefile
	@$(ECHO_NEW_LINE)
	@echo make run
	@echo     Compile the program and run it
	@$(ECHO_NEW_LINE)

copy: $(PROGRAM)
	rsync $(PROGRAM) root@laptop:/usr/bin/

run: $(PROGRAM)
	rsync $(PROGRAM) root@laptop:/usr/bin/
	ssh root@laptop -t $(PRE_RUN_COMMAND) /usr/bin/$(PROGRAM_NAME)

kill:
	ssh root@laptop pkill -9 -f $(PROGRAM_NAME)
