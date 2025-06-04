#
# Copyright 2011-2018 NVIDIA Corporation. All rights reserved
# 
INCLUDES=-I../../../../include -I../../include/profilerhost_util -I../../include/c_util

ifndef OS
 OS   := $(shell uname)
 HOST_ARCH := $(shell uname -m)
endif

ifeq ($(OS),Windows_NT)
    export PATH := $(PATH):../../../../lib64
    LIBS= -lcuda -L ../../../../lib64 -lnvperf_host -lnvperf_target
    OBJ = obj
    LIBEXT = lib
    LIBPREFIX =
else
	NVCC_FLAGS= --std=c++11
    ifeq ($(OS), Darwin)
        export DYLD_LIBRARY_PATH := $(DYLD_LIBRARY_PATH):../../../../lib64
        LIBS= -Xlinker -framework -Xlinker cuda -L ../../../../lib64 -lnvperf_host -lnvperf_target
    else
        export LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):../../../../lib64
        LIBS= -lcuda -L ../../../../lib64 -lnvperf_host -lnvperf_target
    endif
    OBJ = o
    LIBEXT = a
    LIBPREFIX = lib
endif

$(LIBPREFIX)profilerHostUtil.$(LIBEXT): List.$(OBJ) Metric.$(OBJ) Eval.$(OBJ)
	nvcc -o $@ -lib $^ $(LIBS)

List.$(OBJ): List.cpp
	nvcc -c $(NVCC_FLAGS) $(INCLUDES) $<

Metric.$(OBJ): Metric.cpp
	nvcc -c $(NVCC_FLAGS) $(INCLUDES) $<

Eval.$(OBJ): Eval.cpp
	nvcc -c $(NVCC_FLAGS) $(INCLUDES) $<

clean:
ifeq ($(OS),Windows_NT)
	del $(LIBPREFIX)profilerHostUtil.$(LIBEXT) List.$(OBJ) Metric.$(OBJ) Eval.$(OBJ)
else
	rm -f $(LIBPREFIX)profilerHostUtil.$(LIBEXT) List.$(OBJ) Metric.$(OBJ) Eval.$(OBJ)
endif
