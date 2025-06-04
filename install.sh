#!/bin/bash
#
# Installation script for CUPTI samples
#

set -e  # Exit on error

echo "CUPTI Samples Installation Script"
echo "=================================="

# Check if CUDA is installed
if [ -z "$CUDA_HOME" ]; then
    if [ -d "/usr/local/cuda" ]; then
        export CUDA_HOME="/usr/local/cuda"
    else
        echo "CUDA_HOME environment variable is not set and CUDA is not found in /usr/local/cuda"
        echo "Please install CUDA or set CUDA_HOME environment variable"
        exit 1
    fi
fi

echo "Using CUDA installation at: $CUDA_HOME"

# Create lib64 directory if it doesn't exist
if [ ! -d "lib64" ]; then
    mkdir -p lib64
    echo "Created lib64 directory"
fi

# Link CUPTI libraries
echo "Creating symlinks to CUPTI libraries..."
ln -sf $CUDA_HOME/lib64/libcupti.so* lib64/
ln -sf $CUDA_HOME/lib64/libnvperf_host.so* lib64/
ln -sf $CUDA_HOME/lib64/libnvperf_target.so* lib64/

# Create a dummy profilerHostUtil library for samples that need it
# This works around compatibility issues with the extensions library
echo "Creating placeholder profilerHostUtil library..."
mkdir -p lib64
cat > dummy_profilerHostUtil.c << EOF
// Dummy implementation for profilerHostUtil
// This provides a minimal implementation to allow samples to link
#include <stdio.h>

void* profilerHostUtil_placeholder() {
    return NULL;
}
EOF

gcc -shared -fPIC -o lib64/libprofilerHostUtil.so dummy_profilerHostUtil.c
rm -f dummy_profilerHostUtil.c

echo "Setting up environment variables..."
# Export library path for running samples
export LD_LIBRARY_PATH=$(pwd)/lib64:$LD_LIBRARY_PATH
echo "export LD_LIBRARY_PATH=$(pwd)/lib64:\$LD_LIBRARY_PATH" > setup_env.sh

echo "Installation completed successfully!"
echo "To set up the environment for running samples, run:"
echo "source setup_env.sh"
echo ""
echo "Note: Some advanced samples that require profilerHostUtil functionality"
echo "may have limited functionality due to compatibility issues."
echo "Basic samples should work correctly." 