# Callback Event Sample

This sample demonstrates how to use the CUPTI Callback API to collect GPU performance events during the execution of CUDA kernels.

## Overview

The CUPTI Callback API allows applications to register callbacks that are invoked when certain CUDA activities occur. This sample shows how to:

1. Subscribe to CUDA runtime API callbacks
2. Set up CUPTI event groups and collect event data
3. Specifically track the "inst_executed" event (instructions executed)
4. Collect event data around kernel launches

## Key Concepts

The sample demonstrates these key CUPTI callback concepts:

- **CUPTI Callbacks**: Setting up callbacks for CUDA runtime API calls
- **Event Groups**: Creating and managing event groups for collecting performance data
- **Event Collection Mode**: Setting the event collection mode for kernel-level data
- **Reading Event Values**: Reading and aggregating event values across multiple instances

## Implementation Details

The code is organized into these main components:

1. **Callback Setup**:
   - Registers `getEventValueCallback` function to be called on CUDA runtime API calls
   - Specifically targets kernel launch callbacks (`cudaLaunch` and `cudaLaunchKernel`)

2. **Event Collection**:
   - On kernel launch (API enter):
     - Synchronizes the device
     - Sets the event collection mode to kernel mode
     - Enables the event group
   - On kernel completion (API exit):
     - Reads event values
     - Aggregates values across multiple instances
     - Disables the event group

3. **Sample Kernel**:
   - Uses a simple vector addition kernel to demonstrate event collection

## Data Structures

- **cupti_eventData**: Stores the event group and event ID
- **RuntimeApiTrace_t**: Stores event data and values collected by the callback

## Building and Running

To build the sample:

```bash
make
```

To run the sample with the default device (0) and event ("inst_executed"):

```bash
./callback_event
```

You can specify a different device or event:

```bash
./callback_event [device_num] [event_name]
```

## Output Example

```
CUDA Device Number: 0
CUDA Device Name: NVIDIA GeForce RTX 3080
Compute capability: 8.6
Event Name : inst_executed
Event Value : 2048000
```

The output shows the number of instructions executed by the vector addition kernel. This value will vary based on the kernel complexity and input size.

## Implementation Notes

- The sample synchronizes the device before enabling the event group and before reading event values to ensure all operations have completed.
- Event values are aggregated across all instances to get the total value.
- The event collection mode is set to `CUPTI_EVENT_COLLECTION_MODE_KERNEL` to collect events only for the kernel execution.

## See Also

- [CUPTI Callback API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__CALLBACK__API)
- [CUPTI Event API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__EVENT__API)
- Other callback-related samples: callback_timestamp, callback_metric 