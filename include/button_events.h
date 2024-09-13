#ifndef EBA12D5B_22F9_4295_B6AF_D0C073FDB789
#define EBA12D5B_22F9_4295_B6AF_D0C073FDB789

#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "logger_common.h"

// Declare an event base
ESP_EVENT_DECLARE_BASE(BUTTON_EVENT);        // declaration of the BUTTON_EVENT family

#define BUTTON_EVENT_LIST(l) \
    l(BUTTON_EVENT_INIT_DONE) \
    l(BUTTON_EVENT_DEINIT_DONE) \
    l(BUTTON_EVENT_DOWN) \
    l(BUTTON_EVENT_UP) \
    l(BUTTON_EVENT_HELD_SHORT) \
    l(BUTTON_EVENT_HELD_LONG) \
    l(BUTTON_EVENT_HELD_LONG_2) \
    l(BUTTON_EVENT_HELD_LONG_3)
// declaration of the specific events under the BUTTON_EVENT family
enum {                                       
    BUTTON_EVENT_LIST(ENUM)
};

extern const char * const button_event_table[];

#ifdef __cplusplus
}
#endif

#endif /* EBA12D5B_22F9_4295_B6AF_D0C073FDB789 */
