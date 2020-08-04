MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
GEN_DIR=gen

#Math library for square root function
CFLAGS=-lm

TARGET=$(GEN_DIR)/$(PROJ_NAME).out
SOURCES=$(wildcard *.c)

all: printStart $(TARGET) printEnd

printStart:
	@echo ''
	@echo '************************************************************'
	@echo 'Building project: $(PROJ_NAME)'

printEnd:
	@echo ''
	@echo 'Output files can be found in the "$(GEN_DIR)" directory'
	@echo ''
	@echo 'Finished building project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Invokes the ARM x86 Cross Compiler to make the .out file
$(TARGET):
	@mkdir -p $(GEN_DIR)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: ARM x86 Cross Compiler'
	$(CC) -Wall $(SOURCES) $(CFLAGS) -o $(TARGET) -lncurses -lpthread
	@echo 'Finished building target: $@'

.PHONY: all clean

# Remove the $(GEN_DIR) directory
clean:
	@echo ''
	@echo '************************************************************'
	@echo 'Cleaning project: $(PROJ_NAME)'
	@echo ''
	@echo 'Removing files in the "$(GEN_DIR)" directory'
	@rm -rf $(GEN_DIR)
	@echo ''
	@echo 'Finished cleaning project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''
