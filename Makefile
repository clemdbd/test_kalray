####################################################################################################
# \file Makefile
# \brief  makefile to build png_transform programm
# \author C. Dubourdieu
#
####################################################################################################

# Variables
CC=gcc
CFLAGS=-Wall -O
LDFLAGS=-lpng
EXEC=png_transform
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

# Compiler output directories and libs
BUILD_OUTPUT_DIR := build


.PHONY: CREATE_OUT_DIRS

# rules definition
all: CREATE_OUT_DIRS $(EXEC)

CREATE_OUT_DIRS:
	@echo "Create build folder"
	mkdir -p $(BUILD_OUTPUT_DIR)

png_transform: png_transform.o
	@echo $(EXEC) "built"
	@$(CC) -o $(BUILD_OUTPUT_DIR)/$@ $(BUILD_OUTPUT_DIR)/$^ $(LDFLAGS)
	
%.o: %.c
	$(CC) -o $(BUILD_OUTPUT_DIR)/$@ -c $< $(CFLAGS)

clean:
	rm -f $(BUILD_OUTPUT_DIR)/*.o
	rm -f $(BUILD_OUTPUT_DIR)/$(EXEC)
	rm -d $(BUILD_OUTPUT_DIR)
