#ifndef EBA12D5B_22F9_4295_B6AF_D0C073FDB789
#define EBA12D5B_22F9_4295_B6AF_D0C073FDB789

#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

// Declare an event base
ESP_EVENT_DECLARE_BASE(BUTTON_EVENTS);        // declaration of the BUTTON_EVENTS family

// declaration of the specific events under the BUTTON_EVENTS family
enum {                                       
    BUTTON_EVENT_INIT_DONE,
    BUTTON_EVENT_DEINIT_DONE,
    BUTTON_EVENT_HELD_SHORT,
    BUTTON_EVENT_HELD_LONG, 
    BUTTON_EVENT_HELD_LONG_2, 
    BUTTON_EVENT_HELD_LONG_3,
};

#ifdef __cplusplus
}
#endif

#endif /* EBA12D5B_22F9_4295_B6AF_D0C073FDB789 */
