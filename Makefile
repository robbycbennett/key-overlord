# Variables

PROGRAM := keyoverlord

COMPILER := clang++

SOURCE_TYPE := .cpp
HEADER_TYPE := .hpp
OBJECT_TYPE := .o

STANDARD := -std=c++20
OPTIMIZE := -O1

IGNORED := -Wno-c++98-compat-pedantic

ifeq ($(COMPILER), clang++)
	WARNINGS := -Weverything $(IGNORED)
else
	WARNINGS := -Wall
endif

COMPILE_FLAGS := $(STANDARD) $(OPTIMIZE) $(WARNINGS)
LINK_FLAGS := -fuse-ld=lld

DEBUG := 1
ifneq ($(DEBUG), 0)
	LINK_FLAGS := $(LINK_FLAGS) -g
	COMPILE_FLAGS := -DDEBUG
endif

SOURCE_FOLDER := src
OUTPUT_FOLDER := out

CLEAN_FOLDERS := $(OUTPUT_FOLDER)

ECHO_NEW_LINE := echo
CLEAN_COMMAND := rm -rf $(CLEAN_FOLDERS)

SOURCES := $(wildcard $(SOURCE_FOLDER)/*$(SOURCE_TYPE))
HEADERS := $(wildcard $(SOURCE_FOLDER)/*$(HEADER_TYPE))
OBJECTS := $(patsubst $(SOURCE_FOLDER)/%$(SOURCE_TYPE),$(OUTPUT_FOLDER)/%$(OBJECT_TYPE),$(SOURCES))
PROGRAM := $(OUTPUT_FOLDER)/$(PROGRAM)


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

run: $(PROGRAM)
# 	@$(ECHO_NEW_LINE)
# 	@echo $(PROGRAM)
# 	@$(ECHO_NEW_LINE)
# 	@$(PROGRAM)
	rsync $(PROGRAM) laptop:
