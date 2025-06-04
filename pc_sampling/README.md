# PC Sampling Sample

This sample demonstrates how to use the CUPTI PC Sampling API to collect program counter (PC) samples from executing CUDA kernels.

## Overview

The CUPTI PC Sampling API enables applications to sample the program counter during kernel execution to identify hot spots in the code. This sample shows how to:

1. Configure and enable PC sampling for CUDA kernels
2. Collect PC samples during kernel execution
3. Associate PC samples with source code and line numbers
4. Identify performance hotspots in kernel code

## Key Concepts

The sample demonstrates these key PC sampling concepts:

- **Program Counter Sampling**: Collecting PC samples during kernel execution
- **Source Correlation**: Mapping PC addresses to source code locations
- **Hotspot Identification**: Finding frequently executed code regions
- **Sampling Configuration**: Setting up the sampling period and attributes
- **Performance Analysis**: Using PC sampling to analyze kernel execution

## Implementation Details

The code is organized into these main components:

1. **Sampling Setup**:
   - Configures PC sampling parameters (sample period, etc.)
   - Enables PC sampling for the device
   - Sets up callbacks for collecting sample data

2. **Sample Collection**:
   - Collects PC samples during kernel execution
   - Processes sample records to extract PC information
   - Maps PC addresses to source locations using CUDA debugging information

3. **Sample Kernel**:
   - Implements a sample kernel with various code paths
   - Contains intentional hotspots to demonstrate sampling
   - Shows how different algorithms have different PC distributions

4. **Results Analysis**:
   - Displays PC sample distribution
   - Shows source correlation for frequently sampled PCs
   - Provides statistics on hotspot code regions

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./pc_sampling
```

## Requirements

This sample requires:
- CUDA Toolkit with CUPTI support
- CUDA compiled with debugging information (for source correlation)

## Output Example

```
PC Sampling Configuration:
  Sample period: 5
  Collection mode: Kernel
  Sample buffer size: 8192 samples

Running test kernel...
Kernel execution complete.

PC Sampling Results:
  Total samples collected: 5432
  Unique PC addresses: 128

Top 5 hotspots:
  1. PC: 0x1a40 (25% of samples) - matrixMul.cu:45 - Matrix multiplication inner loop
  2. PC: 0x1b00 (18% of samples) - matrixMul.cu:32 - Shared memory data loading
  3. PC: 0x1c80 (12% of samples) - matrixMul.cu:65 - Result computation
  4. PC: 0x1d20 (8% of samples)  - matrixMul.cu:28 - Thread ID calculation
  5. PC: 0x1e60 (5% of samples)  - matrixMul.cu:71 - Result writing

Performance insights:
  - 25% of execution time spent in the main computation loop
  - 18% of execution time spent loading data to shared memory
  - 12% of execution time spent on final result computation
```

## Implementation Notes

- PC sampling provides statistical information about kernel execution with minimal overhead.
- The sampling period affects the granularity of the results - smaller periods collect more samples but introduce more overhead.
- For accurate source correlation, compile CUDA code with debugging information (`-lineinfo` or `-G` compiler flags).
- PC sampling is particularly useful for identifying hotspots in large, complex kernels.
- The sample demonstrates both the collection of PC samples and their interpretation for performance analysis.

## See Also

- [CUPTI PC Sampling API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__SAMPLER__API)
- [NVIDIA Nsight Compute](https://docs.nvidia.com/nsight-compute/index.html) - A comprehensive profiling tool that includes PC sampling functionality
- Other CUPTI sampling-related samples: event_sampling 