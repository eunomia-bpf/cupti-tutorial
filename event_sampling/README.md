# Event Sampling Sample

This sample demonstrates how to use the CUPTI Event API to sample GPU performance events while a CUDA kernel is running.

## Overview

The CUPTI Event API allows applications to collect GPU performance events. This sample demonstrates how to:

1. Set up continuous event collection mode
2. Sample an event (instruction count) at regular intervals
3. Run the sampling in a separate thread while a CUDA kernel executes
4. Accumulate and display event values during kernel execution

## Key Concepts

The sample demonstrates these key CUPTI event sampling concepts:

- **Continuous Event Collection**: Setting the collection mode to continuous rather than per-kernel
- **Sampling Thread**: Running event collection in a separate thread
- **Multi-threaded Profiling**: Sampling events while computations are running
- **Periodic Sampling**: Reading event values at fixed intervals

## Implementation Details

The code is organized into two main threads:

1. **Sampling Thread** (`sampling_func`):
   - Sets up continuous event collection mode
   - Creates an event group and adds the desired event ("inst_executed" by default)
   - Reads and displays event values at regular intervals (every 50ms by default)
   - Continues sampling until the computation thread signals completion

2. **Computation Thread** (main thread):
   - Allocates memory for the vector addition operation
   - Launches the vector addition kernel multiple times (2000 iterations by default)
   - Signals the sampling thread when computation is complete

## Building and Running

To build the sample:

```bash
make
```

To run the sample with the default event ("inst_executed"):

```bash
./event_sampling
```

You can specify a different event:

```bash
./event_sampling [event_name]
```

## Output Example

The output shows a series of samples of the "inst_executed" event taken during the execution of the vector addition kernel:

```
inst_executed: 0
inst_executed: 25600000
inst_executed: 51200000
inst_executed: 76800000
inst_executed: 102400000
...
inst_executed: 4582400000
inst_executed: 4608000000
```

Each line represents a snapshot of the cumulative instruction count at a specific time. The value increases as more instructions are executed by the kernel.

## Implementation Notes

- The sample uses a 50ms sampling interval, which can be adjusted by changing the `SAMPLE_PERIOD_MS` constant.
- The event values are accumulated across all instances to get the total value.
- The kernel is executed multiple times to ensure there's enough time to collect meaningful samples.
- The collection mode is set to `CUPTI_EVENT_COLLECTION_MODE_CONTINUOUS` to enable sampling while the kernel is running.
- Cross-platform support is provided for both Windows and Linux/Unix systems.

## See Also

- [CUPTI Event API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__EVENT__API)
- Other event-related samples: event_multi_gpu, cupti_query 