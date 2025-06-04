#include <Metric.h>
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
        namespace Config {

            bool GetRawMetricRequests(NVPA_RawMetricsConfig* pRawMetricsConfig,
                                      std::vector<std::string> metricNames,
                                      std::vector<NVPA_RawMetricRequest>& rawMetricRequests,
                                      std::vector<std::string>& temp) {
                std::string reqName;
                bool isolated = true;
                bool keepInstances = true;
                int i = 0;

                for (auto& metricName : metricNames)
                {
                    NV::Metric::Parser::ParseMetricNameString(metricName, &reqName, &isolated, &keepInstances);
                    /* Bug in collection with collection of metrics without instances, keep it to true*/
                    keepInstances = true;
                    
                    // Use GetMetricProperties_V2 which is the newer API for metric properties
                    NVPW_RawMetricsConfig_GetMetricProperties_V2_Params getMetricPropertiesParams = { NVPW_RawMetricsConfig_GetMetricProperties_V2_Params_STRUCT_SIZE };
                    getMetricPropertiesParams.pRawMetricsConfig = pRawMetricsConfig;
                    getMetricPropertiesParams.metricIndex = 0; // We'll iterate through all metrics
                    
                    // Get the number of metrics available
                    NVPW_RawMetricsConfig_GetNumMetrics_Params getNumMetricsParams = { NVPW_RawMetricsConfig_GetNumMetrics_Params_STRUCT_SIZE };
                    getNumMetricsParams.pRawMetricsConfig = pRawMetricsConfig;
                    RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetNumMetrics(&getNumMetricsParams));
                    
                    // Find the metric dependencies by name
                    for (size_t metricIndex = 0; metricIndex < getNumMetricsParams.numMetrics; ++metricIndex) {
                        getMetricPropertiesParams.metricIndex = metricIndex;
                        RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetMetricProperties_V2(&getMetricPropertiesParams));
                        
                        if (reqName == getMetricPropertiesParams.pMetricName) {
                            // Found our metric, add it to the request
                            NVPA_RawMetricRequest metricRequest = { NVPA_RAW_METRIC_REQUEST_STRUCT_SIZE };
                            temp.push_back(getMetricPropertiesParams.pMetricName);
                            metricRequest.pMetricName = temp[i].c_str();
                            metricRequest.isolated = isolated;
                            metricRequest.keepInstances = keepInstances;
                            rawMetricRequests.push_back(metricRequest);
                            ++i;
                            break;
                        }
                    }
                }

                return true;
            }

            bool GetConfigImage(std::string chipName, std::vector<std::string> metricNames, std::vector<uint8_t>& configImage) 
            {
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
                auto cleanupMetricsConfig = [&]() {
                    NVPW_RawMetricsConfig_Destroy_Params params = { NVPW_RawMetricsConfig_Destroy_Params_STRUCT_SIZE };
                    params.pRawMetricsConfig = pRawMetricsConfig;
                    NVPW_RawMetricsConfig_Destroy(&params);
                };
                
                // Get raw metric requests
                std::vector<NVPA_RawMetricRequest> rawMetricRequests;
                std::vector<std::string> temp;
                GetRawMetricRequests(metricsContextCreateParams.pRawMetricsConfig, metricNames, rawMetricRequests, temp);

                // Begin pass group
                NVPW_RawMetricsConfig_BeginPassGroup_Params beginPassGroupParams = { NVPW_RawMetricsConfig_BeginPassGroup_Params_STRUCT_SIZE };
                beginPassGroupParams.pRawMetricsConfig = pRawMetricsConfig;
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_BeginPassGroup(&beginPassGroupParams));

                // Add metrics
                if (!rawMetricRequests.empty()) {
                    NVPW_RawMetricsConfig_AddMetrics_Params addMetricsParams = { NVPW_RawMetricsConfig_AddMetrics_Params_STRUCT_SIZE };
                    addMetricsParams.pRawMetricsConfig = pRawMetricsConfig;
                    addMetricsParams.pRawMetricRequests = &rawMetricRequests[0];
                    addMetricsParams.numMetricRequests = rawMetricRequests.size();
                    RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_AddMetrics(&addMetricsParams));
                }

                // End pass group
                NVPW_RawMetricsConfig_EndPassGroup_Params endPassGroupParams = { NVPW_RawMetricsConfig_EndPassGroup_Params_STRUCT_SIZE };
                endPassGroupParams.pRawMetricsConfig = pRawMetricsConfig;
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_EndPassGroup(&endPassGroupParams));

                // Generate config image
                NVPW_RawMetricsConfig_GenerateConfigImage_Params generateConfigImageParams = { NVPW_RawMetricsConfig_GenerateConfigImage_Params_STRUCT_SIZE };
                generateConfigImageParams.pRawMetricsConfig = pRawMetricsConfig;
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GenerateConfigImage(&generateConfigImageParams));

                // Get config image
                NVPW_RawMetricsConfig_GetConfigImage_Params getConfigImageParams = { NVPW_RawMetricsConfig_GetConfigImage_Params_STRUCT_SIZE };
                getConfigImageParams.pRawMetricsConfig = pRawMetricsConfig;
                getConfigImageParams.bytesAllocated = 0;
                getConfigImageParams.pBuffer = NULL;
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetConfigImage(&getConfigImageParams));

                configImage.resize(getConfigImageParams.bytesCopied);

                getConfigImageParams.bytesAllocated = configImage.size();
                getConfigImageParams.pBuffer = &configImage[0];
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetConfigImage(&getConfigImageParams));

                // Clean up resources
                cleanupMetricsConfig();

                return true;
            }

            bool GetCounterDataPrefixImage(std::string chipName, std::vector<std::string> metricNames, std::vector<uint8_t>& counterDataImagePrefix) 
            {
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
                auto cleanupMetricsConfig = [&]() {
                    NVPW_RawMetricsConfig_Destroy_Params params = { NVPW_RawMetricsConfig_Destroy_Params_STRUCT_SIZE };
                    params.pRawMetricsConfig = pRawMetricsConfig;
                    NVPW_RawMetricsConfig_Destroy(&params);
                };

                // Get raw metric requests
                std::vector<NVPA_RawMetricRequest> rawMetricRequests;
                std::vector<std::string> temp;
                GetRawMetricRequests(metricsContextCreateParams.pRawMetricsConfig, metricNames, rawMetricRequests, temp);

                // Create CounterDataBuilder
                NVPW_CUDA_CounterDataBuilder_Create_Params counterDataBuilderCreateParams = { NVPW_CUDA_CounterDataBuilder_Create_Params_STRUCT_SIZE };
                counterDataBuilderCreateParams.pChipName = chipName.c_str();
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_CounterDataBuilder_Create(&counterDataBuilderCreateParams));

                // Set up cleanup for CounterDataBuilder
                NVPA_CounterDataBuilder* pCounterDataBuilder = counterDataBuilderCreateParams.pCounterDataBuilder;
                
                // Create a scope exit handler to clean up resources
                auto cleanupCounterDataBuilder = [&]() {
                    NVPW_CounterDataBuilder_Destroy_Params params = { NVPW_CounterDataBuilder_Destroy_Params_STRUCT_SIZE };
                    params.pCounterDataBuilder = pCounterDataBuilder;
                    NVPW_CounterDataBuilder_Destroy(&params);
                };

                // Add metrics to CounterDataBuilder
                if (!rawMetricRequests.empty()) {
                    NVPW_CounterDataBuilder_AddMetrics_Params addMetricsParams = { NVPW_CounterDataBuilder_AddMetrics_Params_STRUCT_SIZE };
                    addMetricsParams.pCounterDataBuilder = counterDataBuilderCreateParams.pCounterDataBuilder;
                    addMetricsParams.pRawMetricRequests = &rawMetricRequests[0];
                    addMetricsParams.numMetricRequests = rawMetricRequests.size();
                    RETURN_IF_NVPW_ERROR(false, NVPW_CounterDataBuilder_AddMetrics(&addMetricsParams));
                }

                // Get counter data prefix
                NVPW_CounterDataBuilder_GetCounterDataPrefix_Params getCounterDataPrefixParams = { NVPW_CounterDataBuilder_GetCounterDataPrefix_Params_STRUCT_SIZE };
                getCounterDataPrefixParams.pCounterDataBuilder = counterDataBuilderCreateParams.pCounterDataBuilder;
                getCounterDataPrefixParams.bytesAllocated = 0;
                getCounterDataPrefixParams.pBuffer = NULL;
                RETURN_IF_NVPW_ERROR(false, NVPW_CounterDataBuilder_GetCounterDataPrefix(&getCounterDataPrefixParams));

                counterDataImagePrefix.resize(getCounterDataPrefixParams.bytesCopied);

                getCounterDataPrefixParams.bytesAllocated = counterDataImagePrefix.size();
                getCounterDataPrefixParams.pBuffer = &counterDataImagePrefix[0];
                RETURN_IF_NVPW_ERROR(false, NVPW_CounterDataBuilder_GetCounterDataPrefix(&getCounterDataPrefixParams));

                // Clean up resources
                cleanupCounterDataBuilder();
                cleanupMetricsConfig();

                return true;
            }
        }
    }
}
