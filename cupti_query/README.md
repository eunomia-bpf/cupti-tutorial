# CUPTI Query Sample

This sample demonstrates how to query CUPTI for supported domains, events, and metrics on a CUDA device.

## Overview

The CUPTI Query API allows applications to discover the performance monitoring capabilities available on CUDA devices. This sample demonstrates how to:

1. Query available event domains on a device
2. List events supported by a specific domain
3. List metrics supported by a device
4. Display detailed information about each domain, event, and metric

## Key Concepts

The sample demonstrates these key CUPTI query concepts:

- **Event Domains**: Groups of related hardware counters
- **Events**: Raw hardware counters that can be queried
- **Metrics**: Derived measurements calculated from one or more events
- **Collection Methods**: How events are collected (PM, SM, Instrumented, NVLINK_TC)

## Implementation Details

The code is organized into these main functions:

1. **Domain Enumeration** (`enumEventDomains`):
   - Gets the number of domains on a device
   - Enumerates all domains on the device
   - For each domain, displays:
     - Domain ID and name
     - Number of profiled and total instances
     - Event collection method

2. **Event Enumeration** (`enumEvents`):
   - Gets the number of events in a domain
   - Enumerates all events in the domain
   - For each event, displays:
     - Event ID and name
     - Short and long descriptions
     - Event category

3. **Metric Enumeration** (`enumMetrics`):
   - Gets the number of metrics on a device
   - Enumerates all metrics on the device
   - For each metric, displays:
     - Metric ID and name
     - Short and long descriptions

## Command Line Options

The sample supports the following command line options:

```
-help                                  : displays help message
-device <dev_id> -getdomains           : displays supported domains for specified device
-device <dev_id> -getmetrics           : displays supported metrics for specified device
-device <dev_id> -domain <domain_id> -getevents : displays supported events for specified domain and device
```

Note: The default device is 0, and the default domain is the first domain for the device.

## Building and Running

To build the sample:

```bash
make
```

To list all domains on device 0:

```bash
./cupti_query -device 0 -getdomains
```

To list all events in domain 0 on device 0:

```bash
./cupti_query -device 0 -domain 0 -getevents
```

To list all metrics on device 0:

```bash
./cupti_query -device 0 -getmetrics
```

## Output Example

When listing domains:
```
Domain# 1
Id         = 0
Name       = CUPTI_DOMAIN_0
Profiled instance count = 1
Total instance count = 1
Event collection method = CUPTI_EVENT_COLLECTION_METHOD_PM
...
```

When listing events:
```
Event# 1
Id         = 1
Name       = active_warps
Description= Number of active warps per cycle
Category   = CUPTI_EVENT_CATEGORY_INSTRUCTION
...
```

When listing metrics:
```
Metric# 1
Id         = 1
Name       = achieved_occupancy
Description= Ratio of active warps to maximum supported warps per multiprocessor
...
```

## See Also

- [CUPTI Event API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__EVENT__API)
- [CUPTI Metric API Documentation](https://docs.nvidia.com/cuda/cupti/modules.html#group__CUPTI__METRIC__API)
- Other event-related samples: event_sampling, event_multi_gpu 