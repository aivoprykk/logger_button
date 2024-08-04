#ifndef BTN_BUTTON_H
#define BTN_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    L_BUTTON_DOWN = 0,
    L_BUTTON_UP,
    L_BUTTON_LONG_PRESS_START,
    L_BUTTON_LONG_LONG_PRESS_START,
    L_BUTTON_CLICK,
    L_BUTTON_DOUBLE_CLICK,
    L_BUTTON_TRIPLE_CLICK,
    L_BUTTON_NONE
} l_button_ev_t;

typedef void (*l_button_cb_t)(int, l_button_ev_t, uint64_t press_time);

typedef struct l_button_s {
    void * btn;
    int gpio_num;
    int64_t press_time;
    int64_t press_start;
    bool button_down;
    l_button_cb_t cb;
} l_button_t;

#if defined(BTN_GPIO_INPUT_1_ACTIVE)
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
