#ifndef E16FE1_75E8_4E3C_B1E7_809AC7E76B61
#define E16FE1_75E8_4E3C_B1E7_809AC7E76B61

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#if (defined(CONFIG_LOGGER_USE_GLOBAL_LOG_LEVEL) && CONFIG_LOGGER_GLOBAL_LOG_LEVEL < CONFIG_LOGGER_BUTTON_LOG_LEVEL)
#define C_LOG_LEVEL CONFIG_LOGGER_GLOBAL_LOG_LEVEL
#else
#define C_LOG_LEVEL CONFIG_LOGGER_BUTTON_LOG_LEVEL
#endif
#include "common_log.h"

#ifdef __cplusplus
}
#endif

#endif /* E16FE1_75E8_4E3C_B1E7_809AC7E76B61 */
