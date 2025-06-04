# Callback Metric Sample

This sample demonstrates how to use the CUPTI Callback API to collect CUDA performance metrics during kernel execution.

## Overview

The CUPTI Callback API allows applications to register callbacks that are invoked when CUDA runtime API functions are called. This sample shows how to:

1. Subscribe to CUDA runtime API callbacks
2. Set up CUPTI metric calculation for kernel launches
3. Collect the required events for computing metrics
4. Calculate and display metric values for kernel executions
5. Specifically focus on the "ipc" (instructions per cycle) metric

## Key Concepts

The sample demonstrates these key CUPTI metric concepts:

- **CUPTI Callbacks**: Setting up callbacks for CUDA runtime API calls
- **Event Groups**: Creating and managing event groups required for metrics
- **Multiple Passes**: Collecting events over multiple kernel executions if needed
- **Metric Calculation**: Converting raw event values into meaningful metrics
- **Event Normalization**: Accounting for different domain instances and normalization

## Implementation Details

The code is organized into these main components:

1. **Metric Setup**:
   - Obtains the metric ID for "ipc" (instructions per cycle)
   - Determines the events required for the metric
   - Creates event groups for collecting these events

2. **Callback Function**:
   - `getMetricValueCallback` is called on kernel launches
   - On API enter: Enables event groups and sets collection mode
   - On API exit: Reads event values and normalizes them

3. **Multiple Pass Collection**:
   - Some metrics require events that can't be collected simultaneously
   - The sample runs the kernel multiple times if needed for complete data collection

4. **Metric Calculation**:
   - After collecting all required events, calculates the metric value
   - Displays both raw event values and the final metric value

## Data Structures

- **MetricData_t**: Stores information for event collection:
  - Device being monitored
  - Event groups to collect for each pass
  - Arrays for event IDs and values
  - Current event index and total event count

## Building and Running

To build the sample:

```bash
make
```

To run the sample with the default metric ("ipc"):

```bash
./callback_metric
```

You can specify a different metric:

```bash
./callback_metric [metric_name]
```

## Output Example

```
Collecting events for metric ipc
Pass 1/1 collecting 2 events using 1 event group
        inst_executed = 51200 (51200)
        inst_executed (normalized) (51200 * 1) / 1 = 51200
        active_cycles = 25600 (25600)
        active_cycles (normalized) (25600 * 1) / 1 = 25600

Metric ipc
        Value: 2.000000
```

The output shows:
- The events collected for the metric
- Raw and normalized values for each event
- The calculated metric value (instructions per cycle)

## Implementation Notes

- The sample normalizes event values to account for all domain instances on the device.
- For metrics requiring multiple passes, the kernel is executed multiple times with different event groups enabled.
- The sample uses `cudaDeviceSynchronize()` to ensure all operations are completed before reading event values.
- If the requested metric isn't available on the device, the sample will report an error.

## See Also

- [CUPTI Callback API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__CALLBACK__API)
- [CUPTI Metric API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__METRIC__API)
- Other callback-related samples: callback_event, callback_timestamp 