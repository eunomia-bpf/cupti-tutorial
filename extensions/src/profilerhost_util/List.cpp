#include <List.h>
#include <iostream>
#include <nvperf_host.h>
#include <nvperf_cuda_host.h>
#include <nvperf_target.h>
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
        namespace Enum {
            bool ListSupportedChips() {
                // Initialize NVPERF
                NVPW_InitializeHost_Params initParams = { NVPW_InitializeHost_Params_STRUCT_SIZE };
                RETURN_IF_NVPW_ERROR(false, NVPW_InitializeHost(&initParams));
                
                NVPW_GetSupportedChipNames_Params getSupportedChipNames = { NVPW_GetSupportedChipNames_Params_STRUCT_SIZE };
                RETURN_IF_NVPW_ERROR(false, NVPW_GetSupportedChipNames(&getSupportedChipNames));
                std::cout << "\n Number of supported chips : " << getSupportedChipNames.numChipNames;
                std::cout << "\n List of supported chips : \n";

                for (size_t i = 0; i < getSupportedChipNames.numChipNames; i++) {
                    std::cout << " " << getSupportedChipNames.ppChipNames[i] << "\n";
                }

                return true;
            }

            bool ListMetrics(const char* chip, bool listSubMetrics) {
                // Initialize NVPERF
                NVPW_InitializeHost_Params initParams = { NVPW_InitializeHost_Params_STRUCT_SIZE };
                RETURN_IF_NVPW_ERROR(false, NVPW_InitializeHost(&initParams));

                // Create RawMetricsConfig
                NVPW_CUDA_RawMetricsConfig_Create_Params metricsContextCreateParams = { NVPW_CUDA_RawMetricsConfig_Create_Params_STRUCT_SIZE };
                metricsContextCreateParams.activityKind = NVPA_ACTIVITY_KIND_PROFILER;
                metricsContextCreateParams.pChipName = chip;
                RETURN_IF_NVPW_ERROR(false, NVPW_CUDA_RawMetricsConfig_Create(&metricsContextCreateParams));

                // Set up cleanup for RawMetricsConfig
                NVPA_RawMetricsConfig* pRawMetricsConfig = metricsContextCreateParams.pRawMetricsConfig;
                
                // Create a scope exit handler to clean up resources
                auto cleanupMetricsConfig = [&]() {
                    NVPW_RawMetricsConfig_Destroy_Params params = { NVPW_RawMetricsConfig_Destroy_Params_STRUCT_SIZE };
                    params.pRawMetricsConfig = pRawMetricsConfig;
                    NVPW_RawMetricsConfig_Destroy(&params);
                };

                // Get number of metrics available
                NVPW_RawMetricsConfig_GetNumMetrics_Params getNumMetricsParams = { NVPW_RawMetricsConfig_GetNumMetrics_Params_STRUCT_SIZE };
                getNumMetricsParams.pRawMetricsConfig = pRawMetricsConfig;
                RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetNumMetrics(&getNumMetricsParams));
                
                std::cout << getNumMetricsParams.numMetrics << " metrics in total on the chip\n Metrics List : \n";
                
                // Iterate through all metrics
                NVPW_RawMetricsConfig_GetMetricProperties_V2_Params getMetricPropertiesParams = { NVPW_RawMetricsConfig_GetMetricProperties_V2_Params_STRUCT_SIZE };
                getMetricPropertiesParams.pRawMetricsConfig = pRawMetricsConfig;
                
                for (size_t i = 0; i < getNumMetricsParams.numMetrics; i++) {
                    getMetricPropertiesParams.metricIndex = i;
                    RETURN_IF_NVPW_ERROR(false, NVPW_RawMetricsConfig_GetMetricProperties_V2(&getMetricPropertiesParams));
                    
                    // Filter metrics based on submetric preferences
                    std::string metricName = getMetricPropertiesParams.pMetricName;
                    bool showMetric = true;
                    
                    if (!listSubMetrics) {
                        // Simple heuristic to filter out submetrics if requested
                        // Real implementation would need more sophisticated filtering
                        if (metricName.find("__") != std::string::npos &&
                            (metricName.find("_peak_") != std::string::npos ||
                             metricName.find("_per_cycle_") != std::string::npos ||
                             metricName.find("_pct_") != std::string::npos)) {
                            showMetric = false;
                        }
                    }
                    
                    if (showMetric) {
                        std::cout << metricName << "\n";
                    }
                }

                // Clean up resources
                cleanupMetricsConfig();

                return true;
            }
        }
    }
}
