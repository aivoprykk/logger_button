#ifndef BTN_BUTTON_H
#define BTN_BUTTON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_BTN_BUTTON_OLD_BEHAIVIOR

struct io_button_s {
    uint8_t long_pulse;
    uint8_t button_status;
    uint8_t old_button_status;

    int return_value;
    int Input_pin;
    int millis_10s;
    uint32_t button_count;
    
    uint32_t push_millis;
    uint32_t time_out_millis;
    uint32_t max_pulse_time;
    uint32_t max_button_count;
};

int init_io_button_s(struct io_button_s*, int GPIO_pin, uint32_t push_time, uint32_t long_pulse_time, uint32_t max_count);  // constructor
int io_button_sed(struct io_button_s*);                                                                                     // return true if button is pushed longer then push_time

#else

#define PIN_BIT(x) (1ULL << x)

#define BUTTON_DOWN (1)
#define BUTTON_UP (2)
#define BUTTON_HELD_SHORT (3)
#define BUTTON_HELD_LONG (4)
#define BUTTON_HELD_LLONG (5)

typedef struct {
    uint8_t event;
    //uint8_t _pad0;
    //uint16_t _pad1;
    //uint32_t _pad2;
    uint32_t pin;
    uint32_t start_time;
} button_event_t;

void init_button_task();
void deinit_button_task();
void button_add_handler(uint32_t pin, void (*handler)(button_event_t *ev));

#endif

#ifdef __cplusplus
}
#endif
#endif
