# Autorange Profiling Sample

This sample demonstrates how to use the CUPTI Profiling API to automatically detect and profile CUDA kernel launches using the auto-range mode.

## Overview

The CUPTI Profiling API enables the collection of performance metrics for CUDA applications. This sample shows how to:

1. Set up autorange profiling for CUDA kernels
2. Configure and collect metrics automatically for kernel launches
3. Use the NVPW (NVIDIA Performance Works) and CUPTI Profiler APIs
4. Process and display collected metrics for multiple kernels

## Key Concepts

The sample demonstrates these key profiling concepts:

- **Autorange Profiling**: Automatically detecting kernel launches without manual instrumentation
- **Metric Collection**: Collecting performance metrics for CUDA kernels
- **Multiple Kernel Analysis**: Profiling multiple kernels in a single application run
- **CounterDataImage**: Setting up and using counter data images for metric collection
- **Performance Metrics**: Working with the CUPTI Profiler API to collect advanced metrics

## Implementation Details

The code is organized into these main components:

1. **Setup and Initialization**:
   - Creates counter data image and configuration for metrics
   - Initializes the CUPTI profiler for autorange collection
   - Sets up metric collection for "smsp__warps_launched.avg+" by default

2. **Vector Operations**:
   - Implements two simple CUDA kernels: `VecAdd` and `VecSub`
   - Demonstrates profiling multiple kernels in sequence

3. **Autorange Profiling**:
   - Enables automatic detection of kernel launches
   - Collects metrics for each kernel without manual instrumentation

4. **Results Processing**:
   - Processes the collected counter data
   - Evaluates metrics for each detected kernel
   - Displays the metric values with kernel identifiers

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./autoRangeSample
```

## Dependencies

This sample requires:
- CUDA Toolkit with CUPTI support
- NVPERF libraries (`libnvperf_host.so`, `libnvperf_target.so`)
- The profiler host utilities library (built from the extensions directory)

Make sure to run the installation script in the root directory to build and link the required dependencies:

```bash
cd ../  # Go to cupti_samples root
./install.sh
```

## Output Example

```
Launching kernel: blocks 196, thread/block 256
Range 0 : VecAdd
  smsp__warps_launched.avg: 196.0
Range 1 : VecSub
  smsp__warps_launched.avg: 196.0
```

The output shows:
- The kernel configuration (blocks and threads per block)
- Automatically detected ranges for each kernel launch
- The collected metric value for each kernel

## Implementation Notes

- The sample uses the advanced CUPTI Profiler API, which is the newer interface for collecting metrics.
- Autorange profiling simplifies performance analysis by automatically identifying kernel launches.
- The metrics collected can be changed by modifying the `METRIC_NAME` definition.
- This sample requires additional libraries compared to the basic CUPTI samples.
- The implementation follows NVIDIA's recommended practices for performance analysis.

## See Also

- [CUPTI Profiler API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__PROFILER__API)
- [NVPERF API Documentation](https://docs.nvidia.com/cupti/Cupti/modules.html#group__NVPERF__API)
- Other profiling-related samples: userrange_profiling, activity_trace 