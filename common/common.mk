# Common Makefile configuration for CUPTI samples
# Copyright NVIDIA Corporation

# Detect CUDA version to determine supported SM architectures
# CUDA 13+ removed support for compute capabilities below sm_75
CUDA_VERSION := $(shell $(CUDA_INSTALL_PATH)/bin/nvcc --version 2>/dev/null | grep release | sed 's/.*release //' | sed 's/,.*//' | cut -d. -f1)

# Set default SMS based on CUDA version if not already set
ifndef SMS
    ifeq ($(shell test $(CUDA_VERSION) -ge 13 2>/dev/null; echo $$?),0)
        # CUDA 13+ only supports sm_75 and above
        # Note: CUDA 13 uses sm_100, sm_103 instead of sm_101, sm_102
        SMS := 75 80 86 87 88 89 90 100 103 110 120
        DEFAULT_SM_ARCH := sm_75
    else
        # CUDA 12 and below support older architectures
        SMS := 50 52 53 60 61 62 70 72 75 80 86 87 89 90 100 101 120
        DEFAULT_SM_ARCH := sm_52
    endif
endif

# Generate SASS code for each SM architecture listed in $(SMS)
$(foreach sm,$(SMS),$(eval GENCODE_FLAGS += -gencode arch=compute_$(sm),code=sm_$(sm)))
