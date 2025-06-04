# Activity Trace Async Sample

This sample demonstrates how to use the CUPTI Activity API to asynchronously collect a trace of CUDA API and GPU activities.

## Overview

The CUPTI Activity API enables the collection of CUDA API and GPU activity records. This sample shows how to:

1. Initialize CUPTI activity tracing with asynchronous buffer handling
2. Set up activity record buffers
3. Process activity records as they are completed
4. Print detailed information about various activities while the application continues to run
5. Handle buffer requests and completions in a non-blocking manner

## Key Concepts

The sample demonstrates these key concepts:

- **Asynchronous Activity Buffers**: Handling activity buffers asynchronously to avoid application stalls
- **Buffer Callbacks**: Setting up buffer request and completion callbacks
- **Activity Record Processing**: Parsing and interpreting activity records in a separate thread
- **Timestamp Normalization**: Normalizing timestamps for clearer output
- **Continuous Profiling**: Collecting profiling data without blocking the main application

## Implementation Details

The code includes:

1. **Buffer Management**:
   - `bufferRequested()`: Callback for when CUPTI requests a buffer
   - `bufferCompleted()`: Callback for when CUPTI completes a buffer
   - Asynchronous handling of buffer completions

2. **Activity Processing**:
   - `printActivity()`: Processes and prints different types of activity records
   - Handles various record types like device information, memory operations, kernel launches, etc.

3. **Tracing Control**:
   - `initTrace()`: Sets up activity tracing with asynchronous buffer handling
   - `finiTrace()`: Cleans up and processes any remaining activity records

4. **Sample Kernel**:
   - Uses a simple vector addition kernel (`vec.cu`) to demonstrate activity tracing

## Asynchronous vs. Synchronous Tracing

Unlike the regular `activity_trace` sample, this sample demonstrates asynchronous handling of activity buffers, which:

- Reduces application impact by processing buffers in a non-blocking manner
- Allows the application to continue execution while profiling data is processed
- Is better suited for production environments or long-running applications
- Provides the same detailed tracing information but with lower overhead

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./activity_trace_async
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

## Implementation Notes

- The sample uses CUPTI's asynchronous buffer handling mechanism, where buffer completion callbacks are invoked in a separate thread.
- The buffer completion callback must return quickly to avoid stalling the CUPTI activity buffer management.
- Additional synchronization may be needed in more complex applications to handle buffer data safely across threads.

## See Also

- [CUPTI Activity API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__ACTIVITY__API)
- The synchronous version: activity_trace
- Other activity-related samples: autorange_profiling, userrange_profiling 