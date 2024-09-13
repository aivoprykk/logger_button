#ifndef BTN_BUTTON_H
#define BTN_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <logger_common.h>

#define L_BUTTON_EV_LIST(l) l(L_BUTTON_DOWN, 0x00) l(L_BUTTON_UP, 0x01) l(L_BUTTON_LONG_PRESS_START, 0x02) l(L_BUTTON_LONG_LONG_PRESS_START, 0x03) l(L_BUTTON_CLICK, 0x04) l(L_BUTTON_DOUBLE_CLICK, 0x05) l(L_BUTTON_TRIPLE_CLICK, 0x06) l(L_BUTTON_NONE, 0x07)

typedef enum {
    L_BUTTON_EV_LIST(ENUM_V)
} l_button_ev_t;

extern const char * const l_button_ev_list[];

typedef void (*l_button_cb_t)(int, l_button_ev_t, uint64_t press_time);

typedef struct l_button_s {
    void * btn;
    int gpio_num;
    int64_t press_time;
    int64_t press_start;
    bool button_down;
    l_button_cb_t cb;
} l_button_t;

#if defined(CONFIG_LOGGER_BUTTON_GPIO_1)
#define L_BUTTONS_NUM             2
#else
#define L_BUTTONS_NUM             1
#endif

extern l_button_t btns[L_BUTTONS_NUM];    // button array

typedef l_button_t* l_button_handle_t;

void button_init();
void button_deinit();

#ifdef __cplusplus
}
#endif
#endif
