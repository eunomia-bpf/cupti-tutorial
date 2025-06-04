# Unified Memory Sample

This sample demonstrates how to use the CUPTI API to profile and analyze CUDA Unified Memory operations.

## Overview

CUDA Unified Memory provides a single memory space accessible by both CPU and GPU, with automatic data migration between host and device. This sample shows how to:

1. Track Unified Memory events and operations
2. Monitor page faults and data migrations
3. Collect performance metrics related to Unified Memory
4. Analyze the impact of Unified Memory access patterns

## Key Concepts

The sample demonstrates these key Unified Memory profiling concepts:

- **Unified Memory Events**: Tracking page faults, migrations, and memory access patterns
- **Memory Advice**: Using cudaMemAdvise to optimize data placement
- **Page Fault Analysis**: Understanding when and why page faults occur
- **Migration Tracking**: Monitoring data movement between CPU and GPU
- **Performance Impact**: Analyzing the overhead of Unified Memory operations

## Implementation Details

The code is organized into these main components:

1. **Unified Memory Allocation**:
   - Allocates data using cudaMallocManaged for automatic migration
   - Demonstrates different allocation patterns and access strategies
   - Shows how to apply memory usage hints with cudaMemAdvise

2. **Event Collection**:
   - Sets up CUPTI to track Unified Memory related activities
   - Collects information about page faults and data migrations
   - Records timestamps for memory operations

3. **Sample Algorithms**:
   - Implements algorithms with different memory access patterns
   - Demonstrates how access patterns affect Unified Memory performance
   - Shows optimization techniques for Unified Memory applications

4. **Results Analysis**:
   - Displays statistics about Unified Memory operations
   - Shows the relationship between access patterns and page faults
   - Provides insights for optimizing Unified Memory usage

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./unified_memory
```

## Requirements

This sample requires:
- CUDA Toolkit with CUPTI support
- GPU with Unified Memory support (Compute Capability 3.0 or higher)
- For full functionality, a GPU with hardware page migration support (Pascal or newer)

## Output Example

```
Unified Memory Profiling Results:

Memory Allocation: 256MB

Access Pattern 1 (Sequential CPU-then-GPU):
  CPU-to-GPU Page Faults: 65536
  GPU-to-CPU Page Faults: 0
  Total Data Transferred: 256MB
  Migration Time: 25.4ms

Access Pattern 2 (Prefetched to GPU):
  CPU-to-GPU Page Faults: 0
  GPU-to-CPU Page Faults: 0
  Total Data Transferred: 256MB
  Migration Time: 15.2ms

Access Pattern 3 (Concurrent CPU-GPU access):
  CPU-to-GPU Page Faults: 32768
  GPU-to-CPU Page Faults: 8192
  Total Data Transferred: 384MB
  Migration Time: 42.8ms
```

## Implementation Notes

- Unified Memory behavior varies significantly between GPU architectures.
- Pascal and later GPUs provide hardware page fault support for more efficient memory management.
- Using cudaMemAdvise and cudaMemPrefetchAsync can significantly improve performance by reducing page faults.
- The sample demonstrates how to use CUPTI to gain insights into Unified Memory behavior that can help optimize applications.
- Access patterns have a major impact on Unified Memory performance.

## See Also

- [CUDA Unified Memory Documentation](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#unified-memory-programming)
- [CUPTI API Documentation](https://docs.nvidia.com/cuda/cupti/index.html)
- [Unified Memory Optimization Strategies](https://developer.nvidia.com/blog/maximizing-unified-memory-performance-cuda/) 