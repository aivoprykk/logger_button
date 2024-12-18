#ifndef E16FE1_75E8_4E3C_B1E7_809AC7E76B61
#define E16FE1_75E8_4E3C_B1E7_809AC7E76B61

#ifdef __cplusplus
extern "C" {
#endif

#if (CONFIG_LOGGER_BUTTON_LOG_LEVEL <= 2)

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include "esp_timer.h"
#include "esp_log.h"

#ifndef LOG_INFO
#define LOG_INFO(a, b, ...) ESP_LOGI(a, b, __VA_ARGS__)
#endif
#ifndef MEAS_START
#define MEAS_START() uint64_t _start = (esp_timer_get_time())
#endif
#ifndef MEAS_END
#define MEAS_END(a, b, ...) \
    ESP_LOGI(a, b, __VA_ARGS__, (esp_timer_get_time() - _start))
#endif
#endif

#if defined(CONFIG_LOGGER_BUTTON_LOG_LEVEL_TRACE) // "A lot of logs to give detailed information"

#define DLOG LOG_INFO
#define DMEAS_START MEAS_START
#define DMEAS_END MEAS_END
#define ILOG LOG_INFO
#define IMEAS_START MEAS_START
#define IMEAS_END MEAS_END
#define WLOG LOG_INFO
#define WMEAS_START MEAS_START
#define WMEAS_END MEAS_END

#elif defined(CONFIG_LOGGER_BUTTON_LOG_LEVEL_INFO) // "Log important events"

#define DLOG(a, b, ...) ((void)0)
#define DMEAS_START() ((void)0)
#define DMEAS_END(a, b, ...) ((void)0)
#define ILOG LOG_INFO
#define IMEAS_START MEAS_START
#define IMEAS_END MEAS_END
#define WLOG LOG_INFO
#define WMEAS_START MEAS_START
#define WMEAS_END MEAS_END

#elif defined(CONFIG_LOGGER_BUTTON_LOG_LEVEL_WARN) // "Log if something unwanted happened but didn't cause a problem"

#define DLOG(a, b, ...) ((void)0)
#define DMEAS_START() ((void)0)
#define DMEAS_END(a, b, ...) ((void)0)
#define ILOG(a, b, ...) ((void)0)
#define IMEAS_START() ((void)0)
#define IMEAS_END(a, b, ...) ((void)0)
#define WLOG LOG_INFO
#define WMEAS_START MEAS_START
#define WMEAS_END MEAS_END

#else // "Do not log anything"

#define DLOG(a, b, ...) ((void)0)
#define DMEAS_START() ((void)0)
#define DMEAS_END(a, b, ...) ((void)0)
#define ILOG(a, b, ...) ((void)0)
#define IMEAS_START() ((void)0)
#define IMEAS_END(a, b, ...) ((void)0)
#define WLOG(a, b, ...) ((void)0)
#define WMEAS_START() ((void)0)
#define WMEAS_END(a, b, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* C68D7F37_A55C_4F0E_A19C_B0D2B1853F2A */
