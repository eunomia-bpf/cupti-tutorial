# Activity Trace Sample

This sample demonstrates how to use the CUPTI Activity API to collect a trace of CUDA API and GPU activities.

## Overview

The CUPTI Activity API enables the collection of CUDA API and GPU activity records. This sample shows how to:

1. Initialize and finalize CUPTI activity tracing
2. Set up activity record buffers
3. Process activity records as they are completed
4. Print detailed information about various activities:
   - Device information
   - Memory copies (memcpy)
   - Memory sets (memset)
   - Kernel executions
   - CUDA API calls
   - CUDA runtime events
   - And more

## Key Concepts

The sample demonstrates these key concepts:

- **Activity Record Buffers**: Requesting and handling activity buffers
- **Buffer Callbacks**: Setting up buffer request and completion callbacks
- **Record Processing**: Parsing and interpreting activity records
- **Timestamp Normalization**: Normalizing timestamps for clearer output

## Implementation Details

The code includes:

1. **Buffer Management**:
   - `bufferRequested()`: Callback for when CUPTI requests a buffer
   - `bufferCompleted()`: Callback for when CUPTI completes a buffer

2. **Activity Processing**:
   - `printActivity()`: Processes and prints different types of activity records
   - Handles various record types like device information, memory operations, kernel launches, etc.

3. **Tracing Control**:
   - `initTrace()`: Sets up activity tracing
   - `finiTrace()`: Cleans up and processes any remaining activity records

4. **Sample Kernel**:
   - Uses a simple vector addition kernel (`vec.cu`) to demonstrate activity tracing

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./activity_trace
```

## Output

The sample produces output similar to the following:

```
DEVICE Device Name (0), capability 7.0, global memory (bandwidth 900 GB/s, size 16000 MB), multiprocessors 80, clock 1530 MHz
CONTEXT 1, device 0, compute API CUDA, NULL stream 1
DRIVER_API cuCtxCreate [ 10223 - 15637 ] 
MEMCPY HtoD [ 22500 - 23012 ] device 0, context 1, stream 7, correlation 1/1
KERNEL "vecAdd" [ 32058 - 35224 ] device 0, context 1, stream 7, correlation 2
MEMCPY DtoH [ 40388 - 41002 ] device 0, context 1, stream 7, correlation 3/3
...
```

Each line represents a different activity record with details about the activity type, timing, and context.

## See Also

- [CUPTI Activity API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__ACTIVITY__API)
- Other activity-related samples: activity_trace_async, autorange_profiling, userrange_profiling 