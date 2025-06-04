# User-Range Profiling Sample

This sample demonstrates how to use the CUPTI Profiling API to collect performance metrics for user-defined ranges in a CUDA application.

## Overview

The CUPTI Profiling API enables the collection of performance metrics for specific ranges of code. This sample shows how to:

1. Define and instrument user-specified ranges in a CUDA application
2. Configure the CUPTI Profiler API for these user ranges
3. Collect performance metrics within the defined ranges
4. Process and display the collected metrics for each range

## Key Concepts

The sample demonstrates these key profiling concepts:

- **User-Range Profiling**: Manually marking specific regions of code for profiling
- **Range Creation**: Creating and naming ranges for focused profiling
- **Metric Collection**: Collecting performance metrics within defined ranges
- **Profiler Control**: Starting and stopping the profiler at specific points in the code
- **Performance Analysis**: Focusing analysis on specific parts of the application

## Implementation Details

The code is organized into these main components:

1. **Setup and Initialization**:
   - Creates counter data image and configuration for metrics
   - Initializes the CUPTI profiler for user-range collection
   - Sets up metric collection for "smsp__warps_launched.avg+" by default

2. **Vector Operations**:
   - Implements simple CUDA kernels: `VecAdd` and `VecSub`
   - Wraps specific operations in user-defined ranges

3. **User-Range Management**:
   - Starts a range before the operations of interest
   - Executes the GPU operations within the range
   - Ends the range after the operations complete
   - Uses `cuptiProfilerBeginRange` and `cuptiProfilerEndRange` for range control

4. **Results Processing**:
   - Processes the collected counter data
   - Evaluates metrics for each user-defined range
   - Displays the metric values with range identifiers

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./userRangeSample
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
Range 0 : Vector Add-Subtract
  smsp__warps_launched.avg: 392.0
```

The output shows:
- The kernel configuration (blocks and threads per block)
- The user-defined range name
- The collected metric value for the range

## Implementation Notes

- Unlike autorange profiling, user-range profiling requires explicit instrumentation.
- User ranges give precise control over which parts of the application are profiled.
- This approach is ideal when you want to focus on specific sections of code rather than individual kernel launches.
- Multiple nested ranges can be created for hierarchical profiling.
- This sample uses the advanced CUPTI Profiler API, which is the newer interface for collecting metrics.

## Key Differences from Autorange Profiling

- **Manual Control**: User explicitly defines where profiling starts and stops
- **Multiple Operations**: Can profile multiple GPU operations as a single logical unit
- **Custom Naming**: User provides meaningful names for profiled ranges
- **Selective Profiling**: Can focus on specific parts of an application without profiling everything

## See Also

- [CUPTI Profiler API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__PROFILER__API)
- [NVPERF API Documentation](https://docs.nvidia.com/cupti/Cupti/modules.html#group__NVPERF__API)
- Other profiling-related samples: autorange_profiling, activity_trace 