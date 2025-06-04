# Callback Timestamp Sample

This sample demonstrates how to use the CUPTI Callback API to collect device timestamps during CUDA runtime API calls.

## Overview

The CUPTI Callback API allows applications to register callbacks that are invoked when CUDA runtime API functions are called. This sample shows how to:

1. Subscribe to CUDA runtime API callbacks
2. Collect device timestamps at the beginning and end of specific CUDA API calls
3. Calculate execution time for memory transfers and kernel executions
4. Display timing information for a complete CUDA workflow

## Key Concepts

The sample demonstrates these key CUPTI callback concepts:

- **CUPTI Callbacks**: Setting up callbacks for CUDA runtime API calls
- **Device Timestamps**: Collecting precise GPU timestamps using `cuptiDeviceGetTimestamp`
- **API Tracing**: Tracking time spent in different CUDA API calls
- **Memory Transfer Timing**: Measuring the duration of memory transfers with different directions
- **Kernel Execution Timing**: Measuring the duration of kernel execution

## Implementation Details

The code is organized into these main components:

1. **Callback Setup**:
   - Registers `getTimestampCallback` function to be called on CUDA runtime API calls
   - Specifically targets memory transfers, kernel launches, and synchronization

2. **Timestamp Collection**:
   - On API enter: Records function name, parameters (for memory transfers), and start timestamp
   - On API exit: Records end timestamp

3. **Sample Workflow**:
   - Executes a series of CUDA operations in a specific order:
     - Host-to-device memory transfers (two operations)
     - Kernel launch (vector addition)
     - Device-to-host memory transfer
     - Device synchronization

4. **Results Display**:
   - Shows function name, start timestamp, GPU time, bytes transferred, and transfer direction

## Data Structures

- **RuntimeApiTrace_t**: Stores information about each traced API call:
  - Function name
  - Start and end timestamps
  - Memory transfer size and direction (for memcpy operations)

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./callback_timestamp
```

## Output Example

```
startTimeStamp/gpuTime reported in nano-seconds

Name            Start Time              GPU Time        Bytes   Kind
cudaMemcpy      123456789012            5432           200000   HostToDevice
cudaMemcpy      123456794444            5432           200000   HostToDevice
VecAdd          123456799876            10864          NA       NA
cudaMemcpy      123456810740            5432           200000   DeviceToHost
```

The output shows:
- Function name
- Start timestamp (in nanoseconds)
- GPU time (duration in nanoseconds)
- Bytes transferred (for memory operations)
- Transfer direction (for memory operations)

## Implementation Notes

- The sample uses the `cudaDeviceSynchronize` call to ensure all operations have completed.
- Kernel end timestamps are calculated based on the `cudaDeviceSynchronize` timestamp, since kernels execute asynchronously.
- The sample tracks memory transfers in both directions (host-to-device and device-to-host).
- Device timestamps provide higher precision than host-based timing methods.

## See Also

- [CUPTI Callback API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__CALLBACK__API)
- [CUPTI Device Timestamp Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__TIMESTAMP__API)
- Other callback-related samples: callback_event, callback_metric 