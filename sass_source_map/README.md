# SASS Source Map Sample

This sample demonstrates how to use the CUPTI API to map SASS (Shader Assembly) instructions to source code lines in CUDA applications.

## Overview

CUDA kernels are compiled to PTX (Parallel Thread Execution) intermediate code and then to SASS assembly instructions for execution on the GPU. This sample shows how to:

1. Obtain SASS code for CUDA kernels
2. Map SASS instructions back to the original source code
3. Correlate performance data with specific SASS instructions
4. Understand the relationship between source code and generated assembly

## Key Concepts

The sample demonstrates these key concepts:

- **SASS Disassembly**: Obtaining the assembly code for CUDA kernels
- **Source Mapping**: Correlating assembly instructions with source code lines
- **Instruction Analysis**: Understanding the compiled form of CUDA code
- **Performance Correlation**: Connecting performance data with specific instructions
- **Code Optimization**: Using SASS information to optimize kernel code

## Implementation Details

The code is organized into these main components:

1. **SASS Extraction**:
   - Loads a CUDA module and extracts SASS instructions
   - Uses CUPTI APIs to access the compiled kernel code
   - Disassembles the binary into human-readable SASS format

2. **Source Correlation**:
   - Maps SASS instructions to source line numbers
   - Creates a correlation between assembly and source code
   - Shows which source lines generated which instructions

3. **Sample Kernel**:
   - Provides a simple kernel with various CUDA operations
   - Demonstrates different code patterns and their SASS representation
   - Illustrates common CUDA programming idioms

4. **Performance Analysis**:
   - Connects performance metrics with specific SASS instructions
   - Shows how source code choices affect instruction generation
   - Provides insights for optimization opportunities

## Building and Running

To build the sample:

```bash
make
```

To run the sample:

```bash
./sass_source_map
```

## Requirements

This sample requires:
- CUDA Toolkit with CUPTI support
- CUDA compiled with debugging information (`-lineinfo` or `-G` flags)
- Compatible NVIDIA GPU

## Output Example

```
SASS Disassembly for kernel 'vectorAdd':

Source Line 42: for(int i = 0; i < n; i++)
    /*0008*/    MOV R1, c[0x0][0x44];      /* Source Line 42 */
    /*0010*/    S2R R0, SR_CTAID.X;        /* Source Line 42 */
    /*0018*/    S2R R3, SR_TID.X;          /* Source Line 42 */
    /*0020*/    IMAD R0, R0, c[0x0][0x28], R3; /* Source Line 42 */
    /*0028*/    ISETP.GE.AND P0, PT, R0, R1, PT; /* Source Line 42 */
    /*0030*/    @P0 EXIT;                  /* Source Line 42 */

Source Line 43: {
Source Line 44:     c[i] = a[i] + b[i];
    /*0038*/    IMUL R3, R0, 0x4;          /* Source Line 44 */
    /*0040*/    IMAD R2, R0, 0x4, c[0x0][0x140]; /* Source Line 44 */
    /*0048*/    IMAD R1, R0, 0x4, c[0x0][0x148]; /* Source Line 44 */
    /*0050*/    IMAD R0, R0, 0x4, c[0x0][0x150]; /* Source Line 44 */
    /*0058*/    LDG R2, [R2];              /* Source Line 44 */
    /*0060*/    LDG R1, [R1];              /* Source Line 44 */
    /*0068*/    IADD R1, R1, R2;           /* Source Line 44 */
    /*0070*/    STG [R0], R1;              /* Source Line 44 */

Source Line 45: }
    /*0078*/    EXIT;                      /* Source Line 45 */

Performance Analysis:
  Line 42 (Loop setup): 6 instructions (30% of kernel instructions)
  Line 44 (Array access and computation): 8 instructions (70% of kernel instructions)
  Line 45 (Kernel exit): 1 instruction

Optimization opportunities:
  - Memory coalescing for global memory accesses
  - Loop unrolling potential
  - Register usage optimization
```

## Implementation Notes

- SASS code varies between different GPU architectures and compute capabilities.
- To obtain source mapping information, compile CUDA code with line information (`-lineinfo` flag).
- For more detailed mapping, use the full debug information (`-G` flag), but this may affect optimization.
- Understanding SASS can help identify optimization opportunities in CUDA code.
- The sample demonstrates both how to obtain SASS code and how to interpret it for performance analysis.

## See Also

- [CUDA Binary Utilities](https://docs.nvidia.com/cuda/cuda-binary-utilities/index.html)
- [CUPTI API Documentation](https://docs.nvidia.com/cuda/cupti/index.html)
- [CUDA Compiler Documentation](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html) 