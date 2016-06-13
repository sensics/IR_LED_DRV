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
# Compilation model: one of mods, modsl, modsl0, mods0
# Should match the application.
MODEL := mods0
#MODEL = mods
CC := "C:/Program Files (x86)/COSMIC/FSE_Compilers/CXSTM8/cxstm8.exe"
LIB := "C:/Program Files (x86)/COSMIC/FSE_Compilers/CXSTM8/clib.exe"

# for the STM8S003K3 on the reference and HDK IR boards
MCU := STM8S003

# These are the only modules we need for this firmware - no need to build extra,
# and not all modules are supported by the MCU used.
OBJ_BASES := clk exti gpio spi tim1 tim2 uart1


OUTPUT := StdPeriph_Driver
EXT := sm8

SRC_ROOT := STM8S_StdPeriph_Driver
# -oc causes the assembly peephole optimizer to comment-out the things it removes
# +split is basically gcc's -ffunction-sections
# +compact is rather like -Os while I imagine
# +fast is like -O3:
# the difference is that +compact adds essentially -of7 (assembly optimizer: limit depth to 7)
# to a "base" set of invocations while +fast adds -gi (code generator: inline
# machine library calls for long integers handling) to a base, so while the + flags themselves are exclusive,
# the things they enable are not.
# Leaving the default (neither one)seems reasonable, too.
CFLAGS = +split +debug -pc99 -oc
ALL_CFLAGS = +$(MODEL) \
             -v \
             -l \
             -d$(MCU) \
             -i$(SRC_ROOT)/inc \
             -iSTM8S_StdPeriph_Template \
             $(CFLAGS) \
             -cl$(OUTDIR)\ \
             -co$(OUTDIR)\  # -ppb

# Parse out compilation model to add suffix to our own library name.

# an l (lowercase L) if it's either modsl or modsl0, else it's empty
LIBNAME_SUFFIX_STACKSIZE := $(findstring l,$(MODEL))
# a 0 if it's either mods0 or modsl0, else it's empty
LIBNAME_SUFFIX_APPSIZE := $(findstring 0,$(MODEL))

# Suffix determined based on $(MODEL) to match standard libraries' convention
LIBNAME_SUFFIX := $(strip $(LIBNAME_SUFFIX_STACKSIZE))$(strip $(LIBNAME_SUFFIX_APPSIZE))

# Output library name.
LIBNAME := $(OUTPUT)$(strip $(LIBNAME_SUFFIX))
LIBNAME_FULL := $(LIBNAME).$(EXT)
# Main, default target.
all: $(LIBNAME_FULL)
.PHONY: all

# Dealing with Windows commands that get confused by forward slashes...
PATH_SEPARATOR = \
PATH_CONVERT = $(subst /,$(PATH_SEPARATOR),$1)

# Compute the full object filenames from the "base" names specified above
OBJECTS := $(patsubst %,$(OUTDIR)/stm8s_%.o,$(OBJ_BASES))

CLEAN_OUTPUTS := $(OBJECTS) \
                 $(OBJECTS:%.o:%.ls) \
                 $(LIBNAME_FULL) \
                 $(LIBNAME).lkf
DOS_CLEAN_OUTPUTS := $(subst /,\,$(CLEAN_OUTPUTS))

# Create output directory
$(OUTDIR):
	mkdir $(OUTDIR)

# Compile .c -> .o (and the .ls assembly listings with C interspersed)
$(OUTDIR)/%.o $(OUTDIR)/%.ls: $(SRC_ROOT)/src/%.c $(OUTDIR)
	$(CC) $(ALL_CFLAGS) $<

# Use library tool to generate the LIBNAME_FULL
$(LIBNAME_FULL): $(OBJECTS)
	$(LIB) -c $(LIBNAME_FULL) $(OBJECTS)

clean:
	cmd /c "del /q $(DOS_CLEAN_OUTPUTS)"
.PHONY: clean
