#include "button.h"
#include "button_private.h"

#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "sys/time.h"

#include "button_events.h"

static const char *TAG = "button";

ESP_EVENT_DEFINE_BASE(BUTTON_EVENTS);

#ifdef CONFIG_BTN_BUTTON_OLD_BEHAIVIOR
esp_err_t init_Button_push(struct Button_push *me, int GPIO_pin,
                           uint32_t push_time, uint32_t long_pulse_time,
                           uint32_t max_count) {
    if (!me)
        return ESP_FAIL;
    ESP_LOGI(TAG, "[%s] (pin %d, timeout %lu)", __FUNCTION__, GPIO_pin,
             push_time);
    gpio_set_direction((gpio_num_t)GPIO_pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)GPIO_pin, GPIO_PULLUP_ONLY);
    memset(me, 0, sizeof(struct Button_push));
    me->Input_pin = GPIO_pin;
    me->time_out_millis = push_time;
    me->max_pulse_time = long_pulse_time;
    me->max_button_count = max_count;
    return ESP_OK;
}

esp_err_t Button_pushed(struct Button_push *me) {
    if (!me)
        return ESP_FAIL;
    int32_t millis = get_millis();
    me->return_value = 0;
    me->button_status = gpio_get_level((gpio_num_t)me->Input_pin);
    if (me->button_status == 1) {
        me->push_millis = millis;
    }
    if (me->button_status == 1 &&
        ((millis - me->push_millis) > me->time_out_millis) &&
        (me->old_button_status == 0)) {
        ESP_LOGI(TAG, "Button (pin %d, timeout %lu)  pushed %lld ms",
                 me->Input_pin, me->time_out_millis, millis - me->push_millis);
        if (me->long_pulse)
            me->button_count++;
        if (me->button_count > me->max_button_count)
            me->button_count = 0;
        me->old_button_status = 1;
        me->millis_10s = millis;
        me->return_value = 1;
    } else {
        me->return_value = 0;
    }
    if (me->button_status == 1 &&
        (millis - me->millis_10s) < (1000 * me->max_pulse_time)) {
        me->long_pulse = 1;
    } else {
        me->long_pulse = 0;
    }
    if (gpio_get_level((gpio_num_t)me->Input_pin) == 1) {
        me->old_button_status = 0;
    }
    return me->return_value;
}
#else

#define GPIO_INPUT_IO_0 CONFIG_BTN_GPIO_INPUT_0
#ifdef CONFIG_BTN_GPIO_INPUT_1
#define GPIO_INPUT_IO_1 CONFIG_BTN_GPIO_INPUT_1
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0) | (1ULL << GPIO_INPUT_IO_1))
#else
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0))
#endif
/*
 * Let's say, GPIO_INPUT_IO_0=4, GPIO_INPUT_IO_1=5
 * In binary representation,
 * 1ULL<<GPIO_INPUT_IO_0 is equal to 0000000000000000000000000000000000010000 and
 * 1ULL<<GPIO_INPUT_IO_1 is equal to 0000000000000000000000000000000000100000
 * GPIO_INPUT_PIN_SEL                0000000000000000000000000000000000110000
 * */
#define ESP_INTR_FLAG_DEFAULT 0

struct io_button_s {
    bool inverted;
    //uint8_t _pad0;
    uint16_t history;
    uint32_t pin;
    uint32_t down_time;
    uint32_t next_short_time;
    uint32_t next_long_time;
    uint32_t next_llong_time;
    void (*handler)(button_event_t *ev);
};

#define IO_BUTTON_DEFAULTS {\
    .inverted = true,       \
    .history = 0xffff,      \
    .pin = 0,               \
    .down_time = 0,         \
    .next_short_time = 0,   \
    .next_long_time = 0,    \
    .next_llong_time = 0,   \
    .handler = NULL \
}

// static QueueHandle_t gpio_evt_queue = NULL;
static struct io_button_s *io_buttons;
int pin_count = -1;

static void update_button(struct io_button_s *d) {
    d->history = (d->history << 1) | gpio_get_level(d->pin);
}

#define MASK 0b1111000000111111
static bool button_rose(struct io_button_s *d) {
    if ((d->history & MASK) == 0b0000000000111111) {
        d->history = 0xffff;
        return 1;
    }
    return 0;
}
static bool button_fell(struct io_button_s *d) {
    if ((d->history & MASK) == 0b1111000000000000) {
        d->history = 0x0000;
        return 1;
    }
    return 0;
}

static bool button_down(struct io_button_s *d) {
    if (d->inverted)
        return button_fell(d);
    return button_rose(d);
}

static bool button_up(struct io_button_s *d) {
    if (d->inverted)
        return button_rose(d);
    return button_fell(d);
}

/* static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
} */

static void gpio_task(void *arg) {
    //uint32_t io_num;
    uint32_t t;
    button_event_t event;
    for (;;) {
        for (int pin = 0; pin < pin_count; ++pin) {
            // if (xQueueReceive(gpio_evt_queue, &io_num, 1000/portTICK_PERIOD_MS)) {
            struct io_button_s *b = 0;
            /* for (uint32_t pin = 0; pin < pin_count; ++pin) {
                if (io_buttons[pin].pin == io_num) { */
            b = &io_buttons[pin];
            /*         break;
                }
            }
            if (!b)
                continue; */

            // ESP_LOGW(TAG, " Button Event for pin %" PRIu32 , io_num);
            update_button(b);
            t = get_millis();
            event.pin = b->pin;
            event.start_time = b->down_time;
            if (button_up(b)) {
                b->down_time = 0;
                ESP_LOGW(TAG, "pin %" PRIu32 " UP %" PRIu32 "ms", b->pin, t-event.start_time);
                event.event = BUTTON_UP;
                if (b->handler)
                    b->handler(&event);
            } else if (button_down(b) && b->down_time == 0) {
                b->down_time = t;
                event.start_time = t;
                ESP_LOGW(TAG, "pin %" PRIu32 " DOWN", b->pin);
                b->next_short_time = b->down_time + CONFIG_BTN_GPIO_INPUT_SHORT_PRESS_DURATION_MS;
                b->next_long_time = b->down_time + CONFIG_BTN_GPIO_INPUT_LONG_PRESS_DURATION_MS;
                b->next_llong_time = b->down_time + CONFIG_BTN_GPIO_INPUT_LONG_LONG_PRESS_DURATION_MS;
                event.event = BUTTON_DOWN;
                if (b->handler)
                    b->handler(&event);
            }

            if (b->down_time && t >= b->next_llong_time) {
                //ESP_LOGW(TAG, "pin %" PRIu32 " LONG LONG %" PRIu32 "ms", b->pin, t-event.start_time);
                //b->next_llong_time = b->next_llong_time + CONFIG_BTN_GPIO_INPUT_LONG_LONG_PRESS_REPEAT_MS;
                event.event = BUTTON_HELD_LLONG;
                ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENTS, BUTTON_EVENT_HELD_LONG_2, NULL,0, portMAX_DELAY));
                if (b->handler)
                    b->handler(&event);
            } else if (b->down_time && t >= b->next_long_time) {
                //ESP_LOGW(TAG, "pin %" PRIu32 " LONG %" PRIu32 "ms", b->pin, t-event.start_time);
                //b->next_long_time = b->next_long_time + CONFIG_BTN_GPIO_INPUT_LONG_PRESS_REPEAT_MS;
                event.event = BUTTON_HELD_LONG;
                ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENTS, BUTTON_EVENT_HELD_LONG, NULL,0, portMAX_DELAY));
                if (b->handler)
                    b->handler(&event);
            } else if (b->down_time && t >= b->next_short_time) {
                //ESP_LOGW(TAG, "pin %" PRIu32 " SHORT %" PRIu32 "ms", b->pin, t-event.start_time);
                //b->next_short_time = b->next_short_time + CONFIG_BTN_GPIO_INPUT_SHORT_PRESS_REPEAT_MS;
                event.event = BUTTON_HELD_SHORT;
                ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENTS, BUTTON_EVENT_HELD_SHORT, NULL,0, portMAX_DELAY));
                if (b->handler)
                    b->handler(&event);
            }
        }
        delay_ms(10);
    }
}

static TaskHandle_t t1;
void init_button_task() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // Scan the pin map to determine number of pins
    pin_count = 0;
    for (int pin = 0; pin <= 39; pin++) {
        if ((1ULL << pin) & GPIO_INPUT_PIN_SEL) {
            pin_count++;
        }
    }
    io_buttons = calloc(pin_count, sizeof(struct io_button_s));
    // create a queue to handle gpio event from isr
    // gpio_evt_queue = xQueueCreate(12, sizeof(uint32_t));

    uint32_t idx = 0;
    for (uint32_t pin = 0; pin <= 39; ++pin) {
        if ((1ULL << pin) & GPIO_INPUT_PIN_SEL) {
            ESP_LOGI(TAG, "Registering button input: %" PRIu32, pin);
            io_buttons[idx].pin = pin;
            io_buttons[idx].down_time = 0;
            io_buttons[idx].inverted = true;
            if (io_buttons[idx].inverted)
                io_buttons[idx].history = 0xffff;
            idx++;
        }
    }
    // start gpio task
    xTaskCreate(gpio_task, "gpio_task", CONFIG_BTN_TASK_STACK_SIZE, NULL, 10, &t1);
    ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENTS, BUTTON_EVENT_INIT_DONE, NULL,0, portMAX_DELAY));
    // install gpio isr service
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    /* for (uint32_t pin = 0; pin < pin_count; ++pin) {
        gpio_isr_handler_add(io_buttons[pin].pin, gpio_isr_handler, (void *)io_buttons[pin].pin);
    } */
    #ifdef DEBUG
    printf("[%s] Minimum free heap size: %" PRIu32 " bytes\n", __FUNCTION__, esp_get_minimum_free_heap_size());
    #endif
}

void deinit_button_task() {
    vTaskDelete(t1);
    ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENTS, BUTTON_EVENT_DEINIT_DONE, NULL,0, portMAX_DELAY));
    free(io_buttons);
}

void button_add_handler(uint32_t io_pin, void (*handler)(button_event_t *ev)) {
    for (uint32_t pin = 0; pin < pin_count; ++pin) {
        if (io_buttons[pin].pin == io_pin) {
            ESP_LOGI(TAG, "Registering button handler: %" PRIu32, io_pin);
            io_buttons[pin].handler = handler;
            break;
        }
    }
}

#endif
