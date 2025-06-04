# NVIDIA CUPTI Samples

This repository contains sample applications demonstrating the use of the CUDA Profiling Tools Interface (CUPTI). CUPTI provides performance analysis tools with detailed information about how applications are using the GPU through a standardized interface.

## Overview

CUPTI enables the creation of profiling and tracing tools that target CUDA applications. It provides the following capabilities:
- Activity API for asynchronous collection of CUDA runtime and driver API routines and GPU activities
- Callback API for intercepting CUDA runtime and driver API calls
- Event API for accessing hardware performance counters
- Metric API for accessing computed metrics
- Profiling API for controlling the profiler from within the application

## Requirements

- CUDA Toolkit (compatible with the samples)
- GPU with compute capability 2.0 or higher
- Appropriate GPU drivers

## Installation

To set up the required dependencies and environment for building the samples, run the included installation script:

```bash
chmod +x install.sh  # Make the script executable (if not already)
./install.sh         # Run the installation script
source setup_env.sh  # Set up the environment variables
```

The install script will:
1. Check for a valid CUDA installation
2. Create a local lib64 directory with necessary symlinks
3. Build required extension libraries
4. Set up environment variables for running the samples

## Building the Samples

Each sample includes its own Makefile. You can build all samples at once using the provided top-level Makefile:

```bash
make
```

To build a specific sample:

```bash
cd <sample_directory>
make
```

## Sample Descriptions

### Activity API Samples
- **activity_trace**: Demonstrates tracing CUDA API and GPU activities
- **activity_trace_async**: Shows asynchronous activity tracing 
- **userrange_profiling**: Illustrates user-range profiling capabilities
- **autorange_profiling**: Shows automatic range profiling

### Callback API Samples
- **callback_event**: Demonstrates callbacks for CUDA events
- **callback_timestamp**: Shows collecting timestamps for CUDA API calls
- **callback_metric**: Illustrates collecting metrics during CUDA API calls

### Event and Metric API Samples
- **cupti_query**: Shows querying available CUPTI domains, events, and metrics
- **event_sampling**: Demonstrates sampling of CUPTI events
- **event_multi_gpu**: Shows event collection on multiple GPUs

### Advanced Samples
- **nvlink_bandwidth**: Demonstrates monitoring NVLink bandwidth
- **openacc_trace**: Shows tracing OpenACC API calls
- **pc_sampling**: Demonstrates program counter sampling
- **sass_source_map**: Shows mapping between SASS and source code
- **unified_memory**: Demonstrates profiling unified memory operations

## Directory Structure

Each sample directory contains:
- Source code files (.cpp, .cu)
- Makefile for building the sample
- README.md with detailed explanations

## Troubleshooting

If you encounter build errors:
1. Ensure CUDA is properly installed and CUDA_HOME is set
2. Run the install.sh script to set up dependencies
3. Check that library paths are correctly set in setup_env.sh
4. For samples requiring additional libraries (like profilerHostUtil), make sure they are built correctly

## License

These samples are provided under the NVIDIA Corporation license. 