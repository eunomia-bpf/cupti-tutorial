# NVLink Bandwidth Sample

This sample demonstrates how to use the CUPTI API to monitor NVLink bandwidth across multiple GPUs.

## Overview

NVLink is a high-bandwidth interconnect technology that enables fast communication between GPUs and between GPUs and other devices like CPUs. This sample shows how to:

1. Detect and identify NVLink connections in a multi-GPU system
2. Monitor data transfer rates over NVLink connections
3. Collect metrics for NVLink transmit and receive bandwidth
4. Display real-time bandwidth utilization statistics

## Key Concepts

The sample demonstrates these key concepts:

- **NVLink Detection**: Identifying NVLink connections between GPU devices
- **Bandwidth Monitoring**: Measuring the data transfer rates across NVLink
- **CUPTI Metrics**: Using CUPTI to collect NVLink-specific performance metrics
- **Multi-GPU Communication**: Demonstrating data transfer between multiple GPUs

## Implementation Details

The code is organized into these main components:

1. **NVLink Discovery**:
   - Detects the presence of NVLink connections
   - Identifies the topology of connected GPUs
   - Determines the number of NVLink connections and their properties

2. **Bandwidth Test**:
   - Executes memory transfers between connected GPUs
   - Uses CUDA memory copy operations to generate traffic over NVLink
   - Performs both unidirectional and bidirectional transfers

3. **Metric Collection**:
   - Collects CUPTI metrics for NVLink transmit and receive bandwidth
   - Monitors bandwidth utilization during transfers
   - Calculates achieved bandwidth compared to theoretical maximum

4. **Results Reporting**:
   - Displays detailed information about NVLink connections
   - Reports bandwidth achieved for each connection
   - Shows utilization statistics for the NVLink interconnect

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./nvlink_bandwidth
```

## Requirements

This sample requires:
- A system with multiple NVIDIA GPUs connected via NVLink
- CUDA Toolkit with CUPTI support
- GPUs that support NVLink technology (e.g., NVIDIA Pascal, Volta, Turing, Ampere, or newer architectures)

## Output Example

```
Detecting NVLink connections...
Found 4 GPUs with NVLink connections:
  GPU 0 <-> GPU 1: 2 NVLink connections
  GPU 0 <-> GPU 2: 2 NVLink connections
  GPU 1 <-> GPU 3: 2 NVLink connections
  GPU 2 <-> GPU 3: 2 NVLink connections

Running bandwidth test...
Transfer GPU 0 -> GPU 1:
  Data size: 1024 MB
  Time: 0.082 seconds
  Bandwidth: 12.5 GB/s (78.1% of theoretical max)

Transfer GPU 0 -> GPU 2:
  Data size: 1024 MB
  Time: 0.081 seconds
  Bandwidth: 12.6 GB/s (78.8% of theoretical max)

...
```

## Implementation Notes

- NVLink bandwidth varies depending on the GPU architecture and the number of NVLink connections.
- The sample measures both unidirectional and bidirectional bandwidth to demonstrate the full capabilities of NVLink.
- CUPTI metrics are used to provide detailed visibility into NVLink utilization.
- The performance achieved may vary based on system configuration and workload characteristics.

## See Also

- [NVIDIA NVLink Documentation](https://docs.nvidia.com/cuda/nvlink-user-guide/index.html)
- [CUPTI API Documentation](https://docs.nvidia.com/cuda/cupti/index.html)
- [CUDA Multi-GPU Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#multi-device-system) 