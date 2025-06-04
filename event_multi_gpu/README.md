# Event Multi-GPU Sample

This sample demonstrates how to use the CUPTI Event API to collect performance events on multiple GPUs simultaneously without serializing the work.

## Overview

The CUPTI Event API allows applications to collect GPU performance events. This sample shows how to:

1. Collect events across multiple GPU devices simultaneously
2. Handle multiple CUDA contexts, one for each GPU
3. Launch kernels on all GPUs without waiting for each to complete
4. Collect and display event values from all devices
5. Do all of this without serializing the execution of work across GPUs

## Key Concepts

The sample demonstrates these key CUPTI event concepts:

- **Multi-GPU Profiling**: Collecting events from multiple GPUs simultaneously
- **Context Management**: Creating and managing CUDA contexts for each GPU
- **Asynchronous Execution**: Launching kernels on all GPUs without waiting for each to complete
- **Synchronization Points**: Properly synchronizing contexts only when necessary
- **Event Collection**: Collecting event values independently for each device

## Implementation Details

The code is organized into these main stages:

1. **Device and Context Setup**:
   - Detects all available CUDA devices
   - Creates a separate CUDA context for each GPU
   - Sets up event collection for each context

2. **Kernel Execution**:
   - Launches a dummy kernel on each GPU without synchronizing between launches
   - This allows all GPUs to execute work concurrently

3. **Synchronization**:
   - After all kernels are launched, synchronizes each context
   - This ensures all work is complete before reading event values

4. **Event Reading**:
   - Reads event values from each GPU
   - Accumulates and displays the values for each device

## Building and Running

To build the sample:

```bash
make
```

To run the sample with the default event ("inst_executed"):

```bash
./event_multi_gpu
```

You can specify a different event:

```bash
./event_multi_gpu [event_name]
```

## Output Example

```
CUDA Device Name: NVIDIA GeForce RTX 3080
CUDA Device Name: NVIDIA GeForce RTX 3070
[0] inst_executed: 4194304
[1] inst_executed: 4194304
```

The output shows:
- The name of each detected CUDA device
- The value of the specified event for each device
- The numbers may vary based on the GPU model and the kernel execution

## Implementation Notes

- The sample uses a dummy kernel that performs a fixed number of iterations to generate measurable event values.
- This sample specifically demonstrates non-serialized execution across multiple GPUs, which is important for performance when collecting events on multi-GPU systems.
- The test automatically skips if only one GPU is available, as it's designed to demonstrate multi-GPU usage.
- The code demonstrates proper context handling, switching between devices using both CUDA Runtime and Driver APIs.
- Event collection is configured using the `CUPTI_EVENT_COLLECTION_MODE_KERNEL` mode, which collects events only during kernel execution.

## See Also

- [CUPTI Event API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__EVENT__API)
- [CUDA Multi-GPU Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#multi-device-system)
- Other event-related samples: event_sampling, cupti_query 