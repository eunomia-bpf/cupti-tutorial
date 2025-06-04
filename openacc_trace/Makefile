#
# Copyright 2015-2018 NVIDIA Corporation. All rights reserved
#

# CUPTI OpenACC support is only available on Linux x86_64 and ppc64le.

# This sample requires PGI compiler version 18.1 or later.
# Point to your PGI OpenACC installation's include directory.
# PGI compiler must be in PATH.
LIB_PATH ?= ../../lib64

OPENACC_INCLUDE_PATH =

HOST_ARCH := $(shell uname -m)

INCLUDES += -I../../include
INCLUDES += -I../../../../include
INCLUDES += -I$(OPENACC_INCLUDE_PATH)

PGCPP       = pgc++
PGCPP_FLAGS = -acc -ta=nvidia:cuda9.1 -Mcuda=nordc -pgf90libs
ifneq ($(HOST_ARCH),ppc64le)
        PGCPP_FLAGS += -tp=p7 -mp=nonuma
else
        PGCPP_FLAGS += -DHOST_ARCH_PPC=1
endif

export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(LIB_PATH)
TRACE_LIB = libopenacc_trace.so

# Check if PGI compiler is available
PGC_EXISTS := $(shell which $(PGCPP) > /dev/null 2>&1 && echo 1 || echo 0)

ifeq ($(PGC_EXISTS), 1)
all: openacc_app

$(TRACE_LIB): openacc_trace.cpp
	$(PGCPP) $(INCLUDES) -DCUPTI_DIRECTIVE_SUPPORT -fPIC -shared -o $@ $<

openacc_app: $(TRACE_LIB) openacc_app.cpp
	$(PGCPP) $(INCLUDES) $(PGCPP_FLAGS)         \
	-L. -L../../lib64  -lopenacc_trace -lcupti  \
	-o $@ $^

run: $(TRACE_LIB) openacc_app
	ACC_PROFLIB=$(TRACE_LIB) ./openacc_app

clean:
	rm -f $(TRACE_LIB) openacc_app
else
all:
	@echo "PGI compiler (pgc++) not found. Skipping openacc_trace build."
	@echo "To build this sample, install the NVIDIA HPC SDK which includes the PGI compiler."

clean:
	@echo "Nothing to clean"
endif

