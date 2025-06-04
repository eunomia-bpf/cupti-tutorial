#include <Eval.h>
#include <Parser.h>
#include <nvperf_host.h>
#include <nvperf_cuda_host.h>
#include <nvperf_target.h>
#include <iostream>
#include <vector>
#include <string>

#define RETURN_IF_NVPW_ERROR(retval, actual) \
    do { \
        if (NVPA_STATUS_SUCCESS != actual) { \
            fprintf(stderr, "FAILED: %s\n", #actual); \
            return retval; \
        } \
    } while (0)

namespace NV {
    namespace Metric {
        namespace Eval {
            std::string GetHwUnit(const std::string& metricName)
            {
                return metricName.substr(0, metricName.find("__", 0));
            }

            bool GetMetricGpuValue(std::string chipName, std::vector<uint8_t> counterDataImage, std::vector<std::string> metricNames, std::vector<MetricNameValue>& metricNameValueMap) {
                if (!counterDataImage.size()) {
                    std::cout << "Counter Data Image is empty!\n";
                    return false;
                }

                // Initialize NVPERF
                NVPW_InitializeHost_Params initParams = { NVPW_InitializeHost_Params_STRUCT_SIZE };
                RETURN_IF_NVPW_ERROR(false, NVPW_InitializeHost(&initParams));

                // Create RawMetricsConfig
                NVPW_CUDA_RawMetricsConfig_Create_Params metricsContextCreateParams = { NVPW_CUDA_RawMetricsConfig_Create_Params_STRUCT_SIZE };
                metricsContextCreateParams.activityKind = NVPA_ACTIVITY_KIND_PROFILER;
                metricsContextCreateParams.pChipName = chipName.c_str();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_RawMetricsConfig_Create(&metricsContextCreateParams));

                // Set up cleanup for RawMetricsConfig
                NVPA_RawMetricsConfig* pRawMetricsConfig = metricsContextCreateParams.pRawMetricsConfig;
                
                // Create a scope exit handler to clean up resources
                auto cleanupRawMetricsConfig = [&]() {
                    NVPW_RawMetricsConfig_Destroy_Params params = { NVPW_RawMetricsConfig_Destroy_Params_STRUCT_SIZE };
                    params.pRawMetricsConfig = pRawMetricsConfig;
                    NVPW_RawMetricsConfig_Destroy(&params);
                };

                // Get number of ranges
                NVPW_CounterData_GetNumRanges_Params getNumRangesParams = { NVPW_CounterData_GetNumRanges_Params_STRUCT_SIZE };
                getNumRangesParams.pCounterDataImage = &counterDataImage[0];
                RETURN_IF_NVPW_ERROR(false, NVPW_CounterData_GetNumRanges(&getNumRangesParams));

                // Prepare metrics
                std::vector<std::string> reqName;
                reqName.resize(metricNames.size());

                bool isolated = true;
                bool keepInstances = true;
                std::vector<const char*> metricNamePtrs;
                metricNameValueMap.resize(metricNames.size());

                for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                    NV::Metric::Parser::ParseMetricNameString(metricNames[metricIndex], &reqName[metricIndex], &isolated, &keepInstances);
                    metricNamePtrs.push_back(reqName[metricIndex].c_str());
                    metricNameValueMap[metricIndex].metricName = metricNames[metricIndex];
                    metricNameValueMap[metricIndex].numRanges = getNumRangesParams.numRanges;
                }

                // Calculate scratch buffer size for the metrics evaluator
                NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize_Params scratchBufferSizeParams = { NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize_Params_STRUCT_SIZE };
                scratchBufferSizeParams.pChipName = chipName.c_str();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize(&scratchBufferSizeParams));

                // Allocate scratch buffer for the metrics evaluator
                std::vector<uint8_t> scratchBuffer(scratchBufferSizeParams.scratchBufferSize);

                // Create metrics evaluator
                NVPW_CUDA_MetricsEvaluator_Initialize_Params evalInitParams = { NVPW_CUDA_MetricsEvaluator_Initialize_Params_STRUCT_SIZE };
                evalInitParams.pScratchBuffer = scratchBuffer.data();
                evalInitParams.scratchBufferSize = scratchBufferSizeParams.scratchBufferSize;
                evalInitParams.pChipName = chipName.c_str();
                evalInitParams.pCounterDataImage = &counterDataImage[0];
                evalInitParams.counterDataImageSize = counterDataImage.size();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_MetricsEvaluator_Initialize(&evalInitParams));

                // Set up cleanup for MetricsEvaluator
                NVPW_MetricsEvaluator* pMetricsEvaluator = evalInitParams.pMetricsEvaluator;
                
                // Create a scope exit handler to clean up resources
                auto cleanupMetricsEvaluator = [&]() {
                    NVPW_MetricsEvaluator_Destroy_Params params = { NVPW_MetricsEvaluator_Destroy_Params_STRUCT_SIZE };
                    params.pMetricsEvaluator = pMetricsEvaluator;
                    NVPW_MetricsEvaluator_Destroy(&params);
                };

                for (size_t rangeIndex = 0; rangeIndex < getNumRangesParams.numRanges; ++rangeIndex) {
                    std::vector<const char*> descriptionPtrs;

                    NVPW_Profiler_CounterData_GetRangeDescriptions_Params getRangeDescParams = { NVPW_Profiler_CounterData_GetRangeDescriptions_Params_STRUCT_SIZE };
                    getRangeDescParams.pCounterDataImage = &counterDataImage[0];
                    getRangeDescParams.rangeIndex = rangeIndex;
                    RETURN_IF_NVPW_ERROR(false, NVPW_Profiler_CounterData_GetRangeDescriptions(&getRangeDescParams));
                    descriptionPtrs.resize(getRangeDescParams.numDescriptions);

                    getRangeDescParams.ppDescriptions = &descriptionPtrs[0];
                    RETURN_IF_NVPW_ERROR(false, NVPW_Profiler_CounterData_GetRangeDescriptions(&getRangeDescParams));

                    std::string rangeName;
                    for (size_t descriptionIndex = 0; descriptionIndex < getRangeDescParams.numDescriptions; ++descriptionIndex)
                    {
                        if (descriptionIndex)
                        {
                            rangeName += "/";
                        }
                        rangeName += descriptionPtrs[descriptionIndex];
                    }

                    std::vector<double> gpuValues;
                    gpuValues.resize(metricNames.size());
                    
                    // Create metric evaluation requests for each metric
                    std::vector<NVPW_MetricEvalRequest> metricEvalRequests;
                    for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                        NVPW_MetricsEvaluator_GetMetricTypeAndIndex_Params getMetricTypeParams = { NVPW_MetricsEvaluator_GetMetricTypeAndIndex_Params_STRUCT_SIZE };
                        getMetricTypeParams.pMetricsEvaluator = pMetricsEvaluator;
                        getMetricTypeParams.pMetricName = reqName[metricIndex].c_str();
                        RETURN_IF_NVPW_ERROR(false, NVPW_MetricsEvaluator_GetMetricTypeAndIndex(&getMetricTypeParams));
                        
                        NVPW_MetricEvalRequest metricEvalRequest;
                        metricEvalRequest.metricIndex = getMetricTypeParams.metricIndex;
                        metricEvalRequest.metricType = getMetricTypeParams.metricType;
                        // For counter metrics, use sum rollup
                        metricEvalRequest.rollupOp = NVPW_ROLLUP_OP_SUM;
                        // For ratio metrics, use the None submetric
                        metricEvalRequest.submetric = NVPW_SUBMETRIC_NONE;
                        
                        metricEvalRequests.push_back(metricEvalRequest);
                    }

                    // Evaluate metrics for this range
                    NVPW_MetricsEvaluator_EvaluateToGpuValues_Params evalToGpuParams = { NVPW_MetricsEvaluator_EvaluateToGpuValues_Params_STRUCT_SIZE };
                    evalToGpuParams.pMetricsEvaluator = pMetricsEvaluator;
                    evalToGpuParams.pMetricEvalRequests = metricEvalRequests.data();
                    evalToGpuParams.numMetricEvalRequests = metricEvalRequests.size();
                    evalToGpuParams.metricEvalRequestStructSize = sizeof(NVPW_MetricEvalRequest);
                    evalToGpuParams.metricEvalRequestStrideSize = sizeof(NVPW_MetricEvalRequest);
                    evalToGpuParams.pCounterDataImage = &counterDataImage[0];
                    evalToGpuParams.counterDataImageSize = counterDataImage.size();
                    evalToGpuParams.rangeIndex = rangeIndex;
                    evalToGpuParams.isolated = isolated;
                    evalToGpuParams.pMetricValues = &gpuValues[0];
                    RETURN_IF_NVPW_ERROR(false, NVPW_MetricsEvaluator_EvaluateToGpuValues(&evalToGpuParams));

                    for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                        metricNameValueMap[metricIndex].rangeNameMetricValueMap.push_back(std::make_pair(rangeName, gpuValues[metricIndex]));
                    }
                }

                // Clean up resources
                cleanupMetricsEvaluator();
                cleanupRawMetricsConfig();

                return true;
            }

            bool PrintMetricValues(std::string chipName, std::vector<uint8_t> counterDataImage, std::vector<std::string> metricNames) {
                if (!counterDataImage.size()) {
                    std::cout << "Counter Data Image is empty!\n";
                    return false;
                }

                // Initialize NVPERF
                NVPW_InitializeHost_Params initParams = { NVPW_InitializeHost_Params_STRUCT_SIZE };
                RETURN_IF_NVPW_ERROR(false, NVPW_InitializeHost(&initParams));

                // Create RawMetricsConfig
                NVPW_CUDA_RawMetricsConfig_Create_Params metricsContextCreateParams = { NVPW_CUDA_RawMetricsConfig_Create_Params_STRUCT_SIZE };
                metricsContextCreateParams.activityKind = NVPA_ACTIVITY_KIND_PROFILER;
                metricsContextCreateParams.pChipName = chipName.c_str();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_RawMetricsConfig_Create(&metricsContextCreateParams));

                // Set up cleanup for RawMetricsConfig
                NVPA_RawMetricsConfig* pRawMetricsConfig = metricsContextCreateParams.pRawMetricsConfig;
                
                // Create a scope exit handler to clean up resources
                auto cleanupRawMetricsConfig = [&]() {
                    NVPW_RawMetricsConfig_Destroy_Params params = { NVPW_RawMetricsConfig_Destroy_Params_STRUCT_SIZE };
                    params.pRawMetricsConfig = pRawMetricsConfig;
                    NVPW_RawMetricsConfig_Destroy(&params);
                };

                // Get number of ranges
                NVPW_CounterData_GetNumRanges_Params getNumRangesParams = { NVPW_CounterData_GetNumRanges_Params_STRUCT_SIZE };
                getNumRangesParams.pCounterDataImage = &counterDataImage[0];
                RETURN_IF_NVPW_ERROR(false, NVPW_CounterData_GetNumRanges(&getNumRangesParams));

                std::string reqName;
                bool isolated = true;
                bool keepInstances = true;
                std::vector<const char*> metricNamePtrs;
                for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                    NV::Metric::Parser::ParseMetricNameString(metricNames[metricIndex], &reqName, &isolated, &keepInstances);
                    metricNamePtrs.push_back(reqName.c_str());
                }

                // Calculate scratch buffer size for the metrics evaluator
                NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize_Params scratchBufferSizeParams = { NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize_Params_STRUCT_SIZE };
                scratchBufferSizeParams.pChipName = chipName.c_str();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_MetricsEvaluator_CalculateScratchBufferSize(&scratchBufferSizeParams));

                // Allocate scratch buffer for the metrics evaluator
                std::vector<uint8_t> scratchBuffer(scratchBufferSizeParams.scratchBufferSize);

                // Create metrics evaluator
                NVPW_CUDA_MetricsEvaluator_Initialize_Params evalInitParams = { NVPW_CUDA_MetricsEvaluator_Initialize_Params_STRUCT_SIZE };
                evalInitParams.pScratchBuffer = scratchBuffer.data();
                evalInitParams.scratchBufferSize = scratchBufferSizeParams.scratchBufferSize;
                evalInitParams.pChipName = chipName.c_str();
                evalInitParams.pCounterDataImage = &counterDataImage[0];
                evalInitParams.counterDataImageSize = counterDataImage.size();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_MetricsEvaluator_Initialize(&evalInitParams));

                // Set up cleanup for MetricsEvaluator
                NVPW_MetricsEvaluator* pMetricsEvaluator = evalInitParams.pMetricsEvaluator;
                
                // Create a scope exit handler to clean up resources
                auto cleanupMetricsEvaluator = [&]() {
                    NVPW_MetricsEvaluator_Destroy_Params params = { NVPW_MetricsEvaluator_Destroy_Params_STRUCT_SIZE };
                    params.pMetricsEvaluator = pMetricsEvaluator;
                    NVPW_MetricsEvaluator_Destroy(&params);
                };

                for (size_t rangeIndex = 0; rangeIndex < getNumRangesParams.numRanges; ++rangeIndex) {
                    std::vector<const char*> descriptionPtrs;

                    NVPW_Profiler_CounterData_GetRangeDescriptions_Params getRangeDescParams = { NVPW_Profiler_CounterData_GetRangeDescriptions_Params_STRUCT_SIZE };
                    getRangeDescParams.pCounterDataImage = &counterDataImage[0];
                    getRangeDescParams.rangeIndex = rangeIndex;
                    RETURN_IF_NVPW_ERROR(false, NVPW_Profiler_CounterData_GetRangeDescriptions(&getRangeDescParams));
                    
                    descriptionPtrs.resize(getRangeDescParams.numDescriptions);
                    
                    getRangeDescParams.ppDescriptions = &descriptionPtrs[0];
                    RETURN_IF_NVPW_ERROR(false, NVPW_Profiler_CounterData_GetRangeDescriptions(&getRangeDescParams));

                    std::string rangeName;
                    for (size_t descriptionIndex = 0; descriptionIndex < getRangeDescParams.numDescriptions; ++descriptionIndex)
                    {
                        if (descriptionIndex)
                        {
                            rangeName += "/";
                        }
                        rangeName += descriptionPtrs[descriptionIndex];
                    }

                    std::vector<double> gpuValues;
                    gpuValues.resize(metricNames.size());
                    
                    // Create metric evaluation requests for each metric
                    std::vector<NVPW_MetricEvalRequest> metricEvalRequests;
                    for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                        NVPW_MetricsEvaluator_GetMetricTypeAndIndex_Params getMetricTypeParams = { NVPW_MetricsEvaluator_GetMetricTypeAndIndex_Params_STRUCT_SIZE };
                        getMetricTypeParams.pMetricsEvaluator = pMetricsEvaluator;
                        getMetricTypeParams.pMetricName = metricNamePtrs[metricIndex];
                        RETURN_IF_NVPW_ERROR(false, NVPW_MetricsEvaluator_GetMetricTypeAndIndex(&getMetricTypeParams));
                        
                        NVPW_MetricEvalRequest metricEvalRequest;
                        metricEvalRequest.metricIndex = getMetricTypeParams.metricIndex;
                        metricEvalRequest.metricType = getMetricTypeParams.metricType;
                        // For counter metrics, use sum rollup
                        metricEvalRequest.rollupOp = NVPW_ROLLUP_OP_SUM;
                        // For ratio metrics, use the None submetric
                        metricEvalRequest.submetric = NVPW_SUBMETRIC_NONE;
                        
                        metricEvalRequests.push_back(metricEvalRequest);
                    }

                    // Evaluate metrics for this range
                    NVPW_MetricsEvaluator_EvaluateToGpuValues_Params evalToGpuParams = { NVPW_MetricsEvaluator_EvaluateToGpuValues_Params_STRUCT_SIZE };
                    evalToGpuParams.pMetricsEvaluator = pMetricsEvaluator;
                    evalToGpuParams.pMetricEvalRequests = metricEvalRequests.data();
                    evalToGpuParams.numMetricEvalRequests = metricEvalRequests.size();
                    evalToGpuParams.metricEvalRequestStructSize = sizeof(NVPW_MetricEvalRequest);
                    evalToGpuParams.metricEvalRequestStrideSize = sizeof(NVPW_MetricEvalRequest);
                    evalToGpuParams.pCounterDataImage = &counterDataImage[0];
                    evalToGpuParams.counterDataImageSize = counterDataImage.size();
                    evalToGpuParams.rangeIndex = rangeIndex;
                    evalToGpuParams.isolated = isolated;
                    evalToGpuParams.pMetricValues = &gpuValues[0];
                    RETURN_IF_NVPW_ERROR(false, NVPW_MetricsEvaluator_EvaluateToGpuValues(&evalToGpuParams));

                    std::cout << "Range: " << rangeName << ":" << std::endl;
                    for (size_t metricIndex = 0; metricIndex < metricNames.size(); ++metricIndex) {
                        std::cout << "\t" << metricNames[metricIndex] << ": " << gpuValues[metricIndex] << std::endl;
                    }
                }

                // Clean up resources
                cleanupMetricsEvaluator();
                cleanupRawMetricsConfig();

                return true;
            }
        }
    }
}
