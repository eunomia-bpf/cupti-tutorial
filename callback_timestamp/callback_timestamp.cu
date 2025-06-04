/*
 * Copyright 2010-2017 NVIDIA Corporation. All rights reserved
 *
 * Sample app to demonstrate use of CUPTI library to obtain device timestamps
 * using callbacks for CUDA runtime APIs
 *
 */

#include <stdio.h>
#include <cuda.h>
#include <cupti.h>

#define CHECK_CU_ERROR(err, cufunc)                                     \
  if (err != CUDA_SUCCESS)                                              \
    {                                                                   \
      printf ("%s:%d: error %d for CUDA Driver API function '%s'\n",    \
              __FILE__, __LINE__, err, cufunc);                         \
      exit(-1);                                                         \
    }

#define CHECK_CUPTI_ERROR(err, cuptifunc)                               \
  if (err != CUPTI_SUCCESS)                                             \
    {                                                                   \
      const char *errstr;                                               \
      cuptiGetResultString(err, &errstr);                               \
      printf ("%s:%d:Error %s for CUPTI API function '%s'.\n",          \
              __FILE__, __LINE__, errstr, cuptifunc);                   \
      exit(-1);                                                         \
    }

// Structure to hold data collected by callback
typedef struct RuntimeApiTrace_st {
  const char *functionName;
  uint64_t startTimestamp;
  uint64_t endTimestamp;
  size_t memcpy_bytes;
  enum cudaMemcpyKind memcpy_kind;
} RuntimeApiTrace_t;

enum launchOrder{ MEMCPY_H2D1, MEMCPY_H2D2, MEMCPY_D2H, KERNEL, THREAD_SYNC, LAUNCH_LAST};

// Vector addition kernel
__global__ void 
VecAdd(const int* A, const int* B, int* C, int N)
{
  int i = blockDim.x * blockIdx.x + threadIdx.x;
  if (i < N)
    C[i] = A[i] + B[i];
}

// Initialize a vector
static void 
initVec(int *vec, int n)
{
  for (int i = 0; i < n; i++)
    vec[i] = i;
}

void CUPTIAPI
getTimestampCallback(void *userdata, CUpti_CallbackDomain domain,
                     CUpti_CallbackId cbid, const CUpti_CallbackData *cbInfo)
{
  static int memTransCount = 0;
  uint64_t startTimestamp;
  uint64_t endTimestamp;
  RuntimeApiTrace_t *traceData = (RuntimeApiTrace_t*)userdata;
  CUptiResult cuptiErr;
      
  // Data is collected only for the following API
  if ((cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunch_v3020) ||
      (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunchKernel_v7000) ||
      (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaDeviceSynchronize_v3020) || 
      (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy_v3020))  { 
     
    // Set pointer depending on API
    if ((cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunch_v3020) ||
        (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunchKernel_v7000))
    {
      traceData = traceData + KERNEL;
    }
    else if (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaDeviceSynchronize_v3020) 
      traceData = traceData + THREAD_SYNC;
    else if (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy_v3020)
      traceData = traceData + MEMCPY_H2D1 + memTransCount;
                 
    if (cbInfo->callbackSite == CUPTI_API_ENTER) {
      // for a kernel launch report the kernel name, otherwise use the API
      // function name.
      if (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunch_v3020 ||
          cbid == CUPTI_RUNTIME_TRACE_CBID_cudaLaunchKernel_v7000)
      {
        traceData->functionName = cbInfo->symbolName;
      }
      else {
        traceData->functionName = cbInfo->functionName;
      }

      // Store parameters passed to cudaMemcpy
      if (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy_v3020) {
        traceData->memcpy_bytes = ((cudaMemcpy_v3020_params *)(cbInfo->functionParams))->count;
        traceData->memcpy_kind = ((cudaMemcpy_v3020_params *)(cbInfo->functionParams))->kind;
      }
        
      // Collect timestamp for API start
      cuptiErr = cuptiGetTimestamp(&startTimestamp);
      CHECK_CUPTI_ERROR(cuptiErr, "cuptiGetTimestamp");
            
      traceData->startTimestamp = startTimestamp;
    }

    if (cbInfo->callbackSite == CUPTI_API_EXIT) {
      // Collect timestamp for API exit
      cuptiErr = cuptiGetTimestamp(&endTimestamp);
      CHECK_CUPTI_ERROR(cuptiErr, "cuptiGetTimestamp");
            
      traceData->endTimestamp = endTimestamp;
     
      // Advance to the next memory transfer operation
      if (cbid == CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy_v3020) {
        memTransCount++;
      }
    } 
  }
}

static const char *
memcpyKindStr(enum cudaMemcpyKind kind)
{
  switch (kind) {
  case cudaMemcpyHostToDevice:
    return "HostToDevice";
  case cudaMemcpyDeviceToHost:
    return "DeviceToHost";
  default:
    break;
  }

  return "<unknown>";
}

static void 
displayTimestamps(RuntimeApiTrace_t *trace)
{
  // Calculate timestamp of kernel based on timestamp from
  // cudaDeviceSynchronize() call
  trace[KERNEL].endTimestamp = trace[THREAD_SYNC].endTimestamp;

  printf("startTimeStamp/gpuTime reported in nano-seconds\n\n");
  printf("Name\t\tStart Time\t\tGPU Time\tBytes\tKind\n");
  printf("%s\t%llu\t%llu\t\t%llu\t%s\n", trace[MEMCPY_H2D1].functionName,
         (unsigned long long)trace[MEMCPY_H2D1].startTimestamp, 
         (unsigned long long)trace[MEMCPY_H2D1].endTimestamp - trace[MEMCPY_H2D1].startTimestamp,
         (unsigned long long)trace[MEMCPY_H2D1].memcpy_bytes,
         memcpyKindStr(trace[MEMCPY_H2D1].memcpy_kind));
  printf("%s\t%llu\t%llu\t\t%llu\t%s\n", trace[MEMCPY_H2D2].functionName,
         (unsigned long long)trace[MEMCPY_H2D2].startTimestamp,
         (unsigned long long)trace[MEMCPY_H2D2].endTimestamp - trace[MEMCPY_H2D2].startTimestamp, 
         (unsigned long long)trace[MEMCPY_H2D2].memcpy_bytes,
         memcpyKindStr(trace[MEMCPY_H2D2].memcpy_kind)); 
  printf("%s\t%llu\t%llu\t\tNA\tNA\n", trace[KERNEL].functionName,
         (unsigned long long)trace[KERNEL].startTimestamp,
         (unsigned long long)trace[KERNEL].endTimestamp - trace[KERNEL].startTimestamp);
  printf("%s\t%llu\t%llu\t\t%llu\t%s\n", trace[MEMCPY_D2H].functionName,
         (unsigned long long)trace[MEMCPY_D2H].startTimestamp,
         (unsigned long long)trace[MEMCPY_D2H].endTimestamp - trace[MEMCPY_D2H].startTimestamp, 
         (unsigned long long)trace[MEMCPY_D2H].memcpy_bytes,
         memcpyKindStr(trace[MEMCPY_D2H].memcpy_kind)); 
}

static void 
cleanUp(int *h_A, int *h_B, int *h_C, int *d_A, int *d_B, int *d_C)
{
  if (d_A)
    cudaFree(d_A);
  if (d_B)
    cudaFree(d_B);
  if (d_C)
    cudaFree(d_C);

  // Free host memory
  if (h_A)
    free(h_A);
  if (h_B)
    free(h_B);
  if (h_C)
    free(h_C);
}

int
main()
{
  CUcontext context = 0;
  CUdevice device = 0;
  CUresult cuerr;
  CUptiResult cuptierr;
  int N = 50000;
  size_t size = N * sizeof(int);
  int threadsPerBlock = 0;
  int blocksPerGrid = 0;
  int sum, i;
  int *h_A, *h_B, *h_C;
  int *d_A, *d_B, *d_C;

  CUpti_SubscriberHandle subscriber;
  RuntimeApiTrace_t trace[LAUNCH_LAST];
    
  cuerr = cuInit(0);
  CHECK_CU_ERROR(cuerr, "cuInit");

  cuerr = cuCtxCreate(&context, 0, device);
  CHECK_CU_ERROR(cuerr, "cuCtxCreate");

  cuptierr = cuptiSubscribe(&subscriber, (CUpti_CallbackFunc)getTimestampCallback , &trace);
  CHECK_CUPTI_ERROR(cuptierr, "cuptiSubscribe");

  cuptierr = cuptiEnableDomain(1, subscriber, CUPTI_CB_DOMAIN_RUNTIME_API);
  CHECK_CUPTI_ERROR(cuptierr, "cuptiEnableDomain");

  // Allocate input vectors h_A and h_B in host memory
  h_A = (int*)malloc(size);
  h_B = (int*)malloc(size);
  h_C = (int*)malloc(size);

  // Initialize input vectors
  initVec(h_A, N);
  initVec(h_B, N);
  memset(h_C, 0, size);

  // Allocate vectors in device memory
  cudaMalloc((void**)&d_A, size);
  cudaMalloc((void**)&d_B, size);
  cudaMalloc((void**)&d_C, size);

  // Copy vectors from host memory to device memory
  cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

  // Invoke kernel
  threadsPerBlock = 256;
  blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

  VecAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);
  cudaDeviceSynchronize();
    
  // Copy result from device memory to host memory
  // h_C contains the result in host memory
  cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
    
  // Verify result
  for (i = 0; i < N; ++i) {
    sum = h_A[i] + h_B[i];
    if (h_C[i] != sum) {
      printf("kernel execution FAILED\n");
      goto Error;
    }
  }
 
  displayTimestamps(trace);

  cuptierr = cuptiUnsubscribe(subscriber);
  CHECK_CUPTI_ERROR(cuptierr, "cuptiUnsubscribe");

  cleanUp(h_A, h_B, h_C, d_A, d_B, d_C);
  cudaDeviceSynchronize();
  return 0;

 Error:
  cleanUp(h_A, h_B, h_C, d_A, d_B, d_C);
  cudaDeviceSynchronize();
  return -1;
}

