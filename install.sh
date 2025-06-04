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
echo "Creating placeholder profilerHostUtil library with stub functions..."
mkdir -p lib64
mkdir -p extensions/src/profilerhost_util

cat > dummy_profilerHostUtil.cpp << EOF
// Dummy implementation for profilerHostUtil
// This provides stub implementations of required functions
#include <stdio.h>
#include <string>
#include <vector>
#include <utility>

namespace NV {
    namespace Metric {
        namespace Config {
            bool GetConfigImage(std::string chipName, std::vector<std::string> metricNames, std::vector<uint8_t>& configImage) {
                printf("STUB: GetConfigImage called\\n");
                // Just create some dummy data in the vector
                configImage.resize(100, 0);
                return true;
            }

            bool GetCounterDataPrefixImage(std::string chipName, std::vector<std::string> metricNames, std::vector<uint8_t>& counterDataImagePrefix) {
                printf("STUB: GetCounterDataPrefixImage called\\n");
                // Just create some dummy data in the vector
                counterDataImagePrefix.resize(100, 0);
                return true;
            }
        }

        namespace Eval {
            struct MetricNameValue {
                std::string metricName;
                int numRanges;
                // <rangeName, metricValue> pair
                std::vector<std::pair<std::string, double>> rangeNameMetricValueMap;
            };

            bool GetMetricGpuValue(std::string chipName, std::vector<uint8_t> counterDataImage, std::vector<std::string> metricNames, std::vector<MetricNameValue>& metricNameValueMap) {
                printf("STUB: GetMetricGpuValue called\\n");
                return true;
            }

            bool PrintMetricValues(std::string chipName, std::vector<uint8_t> counterDataImage, std::vector<std::string> metricNames) {
                printf("STUB: PrintMetricValues called\\n");
                printf("Metric values (STUB):\\n");
                for (const auto& metricName : metricNames) {
                    printf("  %s: 0.0\\n", metricName.c_str());
                }
                return true;
            }
        }

        namespace Enum {
            bool ListMetrics(const char* chipName, bool hideStagingMetrics) {
                printf("STUB: ListMetrics called\\n");
                return true;
            }
        }
    }
}

extern "C" {
    void* profilerHostUtil_placeholder() {
        return NULL;
    }
}
EOF

# Compile the dummy library with C++
g++ -std=c++11 -shared -fPIC -o lib64/libprofilerHostUtil.so dummy_profilerHostUtil.cpp

# Copy the library to the extensions directory
cp lib64/libprofilerHostUtil.so extensions/src/profilerhost_util/

# Create a symlink in the extensions directory
ln -sf $(pwd)/lib64/libprofilerHostUtil.so extensions/src/profilerhost_util/libprofilerHostUtil.so

# Clean up
rm -f dummy_profilerHostUtil.cpp

echo "Setting up environment variables..."
# Export library path for running samples
export LD_LIBRARY_PATH=$(pwd)/lib64:$LD_LIBRARY_PATH
echo "export LD_LIBRARY_PATH=$(pwd)/lib64:\$LD_LIBRARY_PATH" > setup_env.sh

echo "Installation completed successfully!"
echo "To set up the environment for running samples, run:"
echo "source setup_env.sh"
echo ""
echo "Note: Some advanced samples that require profilerHostUtil functionality"
echo "will run with stub implementations that return dummy values."
echo "Basic samples should work correctly." 