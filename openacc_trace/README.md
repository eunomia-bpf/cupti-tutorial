# OpenACC Trace Sample

This sample demonstrates how to use the CUPTI API to trace OpenACC API calls and associated GPU activities.

## Overview

OpenACC is a directive-based programming model for parallel computing that simplifies GPU programming. This sample shows how to:

1. Intercept and trace OpenACC API calls
2. Track data transfers between host and device
3. Monitor kernel launches initiated by OpenACC
4. Correlate OpenACC directives with GPU activities

## Key Concepts

The sample demonstrates these key concepts:

- **OpenACC API Tracing**: Capturing OpenACC API calls and their parameters
- **Directive Correlation**: Mapping OpenACC directives to generated GPU operations
- **Performance Analysis**: Measuring the time spent in different OpenACC operations
- **Activity Tracking**: Monitoring data transfers and kernel executions
- **Profiling Integration**: Combining OpenACC API traces with GPU activity traces

## Implementation Details

The code is organized into these main components:

1. **OpenACC API Interception**:
   - Sets up callbacks for OpenACC API functions
   - Captures function parameters and return values
   - Tracks the OpenACC context and device information

2. **GPU Activity Correlation**:
   - Links OpenACC API calls with the resulting GPU activities
   - Matches data transfers with the corresponding API calls
   - Associates compute regions with kernel launches

3. **Sample Application**:
   - Implements a simple application using OpenACC directives
   - Demonstrates various OpenACC features (data directives, compute regions, etc.)
   - Shows typical OpenACC usage patterns

4. **Trace Analysis**:
   - Displays a timeline of OpenACC API calls and GPU activities
   - Shows performance statistics for OpenACC operations
   - Provides insights for optimization opportunities

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./openacc_trace
```

## Requirements

This sample requires:
- CUDA Toolkit with CUPTI support
- An OpenACC compiler (e.g., PGI/NVIDIA HPC SDK)
- OpenACC runtime library

## Output Example

```
OpenACC API Trace:
[00.000 ms] acc_init(acc_device_nvidia)
[00.245 ms] acc_get_num_devices(acc_device_nvidia) returned 1
[00.302 ms] acc_set_device_num(0, acc_device_nvidia)
[00.356 ms] acc_get_device_num(acc_device_nvidia) returned 0

[01.234 ms] acc_create(0x7fff5a1c3000, 16384) [4096 elements]
[01.456 ms] > CUDA Memory Allocation: 16384 bytes at 0xd00000
[01.789 ms] acc_create(0x7fff5a1c7000, 16384) [4096 elements]
[01.856 ms] > CUDA Memory Allocation: 16384 bytes at 0xd04000

[02.123 ms] acc_copyin(0x7fff5a1c3000, 16384) [4096 elements]
[02.234 ms] > CUDA H2D Transfer: 16384 bytes to 0xd00000
[02.345 ms] acc_copyin(0x7fff5a1c7000, 16384) [4096 elements]
[02.456 ms] > CUDA H2D Transfer: 16384 bytes to 0xd04000

[03.012 ms] acc_parallel: entering compute region
[03.123 ms] > CUDA Kernel Launch: Vector Addition [grid:16 block:256]
[03.456 ms] acc_parallel: exiting compute region

[04.123 ms] acc_copyout(0x7fff5a1c3000, 16384) [4096 elements]
[04.234 ms] > CUDA D2H Transfer: 16384 bytes from 0xd00000
[04.345 ms] acc_delete(0x7fff5a1c3000, 16384)
[04.456 ms] > CUDA Memory Free: 0xd00000
[04.567 ms] acc_delete(0x7fff5a1c7000, 16384)
[04.678 ms] > CUDA Memory Free: 0xd04000

OpenACC Performance Summary:
  Data Allocation Time: 0.623 ms
  Host-to-Device Transfer Time: 0.344 ms
  Kernel Execution Time: 0.444 ms
  Device-to-Host Transfer Time: 0.111 ms
  Data Deallocation Time: 0.233 ms
  Total OpenACC API Time: 1.755 ms
```

## Implementation Notes

- OpenACC API tracing requires intercepting functions from the OpenACC runtime library.
- The correlation between OpenACC directives and GPU activities is established through timestamps and context information.
- Different OpenACC implementations may have slight variations in their runtime API behavior.
- This sample focuses on the NVIDIA implementation of OpenACC.
- The trace output helps understand how OpenACC directives translate to GPU operations.

## See Also

- [OpenACC Programming Guide](https://www.openacc.org/sites/default/files/inline-files/OpenACC%20API%202.7.pdf)
- [CUPTI API Documentation](https://docs.nvidia.com/cuda/cupti/index.html)
- [NVIDIA HPC SDK Documentation](https://docs.nvidia.com/hpc-sdk/index.html) 