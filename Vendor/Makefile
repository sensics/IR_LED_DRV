# Makefile for building a subset of the STM8 StdPeriph_Driver into a sm8 library with Cosmic
# for use with STVD

# Author: Ryan Pavlik

# Copyright 2016 Sensics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#        http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


OUTDIR := Release
OUTPUT := StdPeriph_Driver
EXT := sm8
# Compilation model: one of mods, modsl, modsl0, mods0
MODEL := mods0
#MODEL = mods
# for the STM8S003K3 on the reference and HDK IR boards
MCU := STM8S003
CC := "C:/Program Files (x86)/COSMIC/FSE_Compilers/CXSTM8/cxstm8.exe"
LIB := "C:/Program Files (x86)/COSMIC/FSE_Compilers/CXSTM8/clib.exe"

SRC_ROOT := STM8S_StdPeriph_Driver
CFLAGS =  +split +debug
ALL_CFLAGS = +$(MODEL) \
             +compact \
             -pc99 \
             -l \
             -d$(MCU) \
             -i$(SRC_ROOT)/inc \
             -iSTM8S_StdPeriph_Template \
             $(CFLAGS) \
             -cl$(OUTDIR)\ \
             -co$(OUTDIR)\  # -ppb

# Parse out compilation model to add suffix to our own library name.
ifneq (,$(findstring l,$(MODEL)))
# either modsl or modsl0
LIBNAME_SUFFIX_STACKSIZE := l
else
LIBNAME_SUFFIX_STACKSIZE :=
endif

ifneq (,$(findstring 0,$(MODEL)))
# either mods0 or modsl0
LIBNAME_SUFFIX_APPSIZE := 0
else
LIBNAME_SUFFIX_APPSIZE :=
endif

# Suffix determined based on $(MODEL) to match standard libraries' convention
LIBNAME_SUFFIX := $(LIBNAME_SUFFIX_STACKSIZE)$(LIBNAME_SUFFIX_APPSIZE)

# Output library name.
LIBNAME := $(OUTPUT)$(LIBNAME_SUFFIX)

# Main, default target.
all: $(LIBNAME).$(EXT)
.PHONY: all

# Dealing with Windows commands that get confused by forward slashes...
PATH_SEPARATOR = \
PATH_CONVERT = $(subst /,$(PATH_SEPARATOR),$1)

# These are the only modules we need for this firmware - no need to build extra,
# and not all modules are supported by the MCU used.
OBJ_BASES := clk exti gpio spi tim1 tim2 uart1

OBJECTS := $(patsubst %,$(OUTDIR)/stm8s_%.o,$(OBJ_BASES))

CLEAN_OUTPUTS := $(OBJECTS) \
                 $(patsubst %.o,%.ls,$(OBJECTS)) \
                 $(LIBNAME).$(EXT) \
                 $(LIBNAME).lkf
DOS_CLEAN_OUTPUTS := $(subst /,\,$(CLEAN_OUTPUTS))

# Create output directory
$(OUTDIR):
	mkdir $(OUTDIR)

# Compile .c -> .o
$(OUTDIR)/%.o: $(SRC_ROOT)/src/%.c $(OUTDIR)
	$(CC) $(ALL_CFLAGS) $<

# Use library tool
$(LIBNAME).$(EXT): $(OBJECTS)
	$(LIB) -c $(LIBNAME).$(EXT) $(OBJECTS)

clean:
	cmd /c "del /q $(DOS_CLEAN_OUTPUTS)"
.PHONY: clean
