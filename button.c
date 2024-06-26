#include "button.h"
#include "button_private.h"
#include "iot_button.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_pm.h"

static const char *TAG = "button";

#define BUTTON_ACTIVE_LEVEL     0

l_button_t btns[L_BUTTONS_NUM] = {0};    // button array

#define BUTTON_EVRNT_TABLE_LEN  (BUTTON_EVENT_MAX + 1)
const char *button_event_table[] = {
    "BUTTON_PRESS_DOWN",
    "BUTTON_PRESS_UP",
    "BUTTON_PRESS_REPEAT",
    "BUTTON_PRESS_REPEAT_DONE",
    "BUTTON_SINGLE_CLICK",
    "BUTTON_DOUBLE_CLICK",
    "BUTTON_MULTIPLE_CLICK",
    "BUTTON_LONG_PRESS_START",
    "BUTTON_LONG_PRESS_HOLD",
    "BUTTON_LONG_PRESS_UP",
    "BUTTON_EVENT_MAX",
    "BUTTON_NONE_PRESS",
};

static void button_event_press_down_cb(void *arg, void *data) {
    int btn_num = (int)data;
    ESP_LOGI(TAG, "Button event press down %d", btn_num);
    if(btn_num < L_BUTTONS_NUM) {
        btns[btn_num].button_down = true;
        btns[btn_num].press_start = esp_timer_get_time();
        if(btns[btn_num].cb)
            (btns[btn_num].cb)(btn_num, L_BUTTON_DOWN, btns[btn_num].press_time);
    }
}

static void button_event_press_up_cb(void *arg, void *data) {
    int btn_num = (int)data;
    ESP_LOGI(TAG, "Button event press up %d", btn_num);
    if(btn_num < L_BUTTONS_NUM) {
        btns[btn_num].button_down = false;
        btns[btn_num].press_time = esp_timer_get_time() - btns[btn_num].press_start;
        ESP_LOGI(TAG, "Button %d press time %lldms", btn_num, btns[btn_num].press_time/1000);
        if(btns[btn_num].cb)
            (btns[btn_num].cb)(btn_num, L_BUTTON_UP, btns[btn_num].press_time);
    }
}

static void button_event_long_press_start_cb(void *arg, void *data) {
    int btn_num = (int)data;
    // ESP_LOGI(TAG, "Button event long press start %d", btn_num);
    if(btn_num < L_BUTTONS_NUM) {
        btns[btn_num].press_time = esp_timer_get_time() - btns[btn_num].press_start;
        ESP_LOGI(TAG, "Button %d [long] press start on time %lldms", btn_num, btns[btn_num].press_time/1000);
        if(btns[btn_num].cb)
            (btns[btn_num].cb)(btn_num, L_BUTTON_LONG_PRESS_START, btns[btn_num].press_time);
    }
}

static void button_event_long_press_start_2_cb(void *arg, void *data) {
    int btn_num = (int)data;
    // ESP_LOGI(TAG, "Button event long long press start %d", btn_num);
    if(btn_num < L_BUTTONS_NUM) {
        btns[btn_num].press_time = esp_timer_get_time() - btns[btn_num].press_start;
        ESP_LOGI(TAG, "Button %d [long long] press start on time %lldms", btn_num, btns[btn_num].press_time/1000);
        if(btns[btn_num].cb)
            (btns[btn_num].cb)(btn_num, L_BUTTON_LONG_LONG_PRESS_START, btns[btn_num].press_time);
    }
}

static void button_event_long_press_start_3_cb(void *arg, void *data) {
    int btn_num = (int)data;
    // ESP_LOGI(TAG, "Button event long long long press start %d", btn_num);
    if(btn_num < L_BUTTONS_NUM) {
        btns[btn_num].press_time = esp_timer_get_time() - btns[btn_num].press_start;
        ESP_LOGI(TAG, "Button %d [long long long] press start on time %lldms", btn_num, btns[btn_num].press_time/1000);
        if(btns[btn_num].cb)
            (btns[btn_num].cb)(btn_num, L_BUTTON_LONG_LONG_PRESS_START, btns[btn_num].press_time);
    }
}

static void button_event_single_click_cb(void *arg, void *data) {
    int btn_num = (int)data;
    ESP_LOGI(TAG, "Button event single click %d", btn_num);
}

static void button_event_double_click_cb(void *arg, void *data) {
    int btn_num = (int)data;
    ESP_LOGI(TAG, "Button event double click %d", btn_num);
}

void power_save_init(void) {
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
//     esp_pm_config_t pm_config = {
//         .max_freq_mhz = 240,
//         .min_freq_mhz = 40,
// #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
//         .light_sleep_enable = true
// #endif
//     };
//     ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
// #else
// #if CONFIG_IDF_TARGET_ESP32
//     esp_pm_config_esp32_t pm_config = {
// #elif CONFIG_IDF_TARGET_ESP32S2
//     esp_pm_config_esp32s2_t pm_config = {
// #elif CONFIG_IDF_TARGET_ESP32C3
//     esp_pm_config_esp32c3_t pm_config = {
// #elif CONFIG_IDF_TARGET_ESP32S3
//     esp_pm_config_esp32s3_t pm_config = {
// #elif CONFIG_IDF_TARGET_ESP32C2
//     esp_pm_config_esp32c2_t pm_config = {
// #endif
//         .max_freq_mhz = 240,
//         .min_freq_mhz = 40,
// #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
//         .light_sleep_enable = true
// #endif
//     };
//     ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
// #endif
}

void button_init() {
     power_save_init();
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = 2000,
        .short_press_time = 200,
        .gpio_button_config = {
        .active_level = BUTTON_ACTIVE_LEVEL,
        // #if CONFIG_GPIO_BUTTON_SUPPORT_POWER_SAVE
        //             .enable_power_save = true,
        // #endif
        },
    };
    for(int i = 0; i < L_BUTTONS_NUM; i++){
#if defined(BTN_GPIO_INPUT_1_ACTIVE)
        btns[i].gpio_num = (i==0) ? CONFIG_BTN_GPIO_INPUT_0 : CONFIG_BTN_GPIO_INPUT_1;
#else
        btns[i].gpio_num = CONFIG_BTN_GPIO_INPUT_0;
#endif
        btn_cfg.gpio_button_config.gpio_num = btns[i].gpio_num;
        btns[i].btn = iot_button_create(&btn_cfg);
        assert(btns[i].btn);
        button_event_config_t cfg = {0}, cfg1 = {0};
        esp_err_t err = ESP_OK;
        cfg1.event = BUTTON_SINGLE_CLICK;
        cfg1.event_data.multiple_clicks.clicks = 1;
        err |= iot_button_register_event_cb(btns[i].btn, cfg, button_event_single_click_cb, (void *)i);
        cfg1.event = BUTTON_DOUBLE_CLICK;
        cfg1.event_data.multiple_clicks.clicks = 2;
        err |= iot_button_register_event_cb(btns[i].btn, cfg1, button_event_double_click_cb, (void *)i);

        cfg.event = BUTTON_PRESS_DOWN;
        err = iot_button_register_event_cb(btns[i].btn, cfg, button_event_press_down_cb, (void *)i);
        cfg.event = BUTTON_PRESS_UP;
        err |= iot_button_register_event_cb(btns[i].btn, cfg, button_event_press_up_cb, (void *)i);
        cfg.event = BUTTON_LONG_PRESS_START;
        cfg.event_data.long_press.press_time = 2000;
        err |= iot_button_register_event_cb(btns[i].btn, cfg, button_event_long_press_start_cb, (void *)i);
        cfg.event_data.long_press.press_time = 5000;
        err |= iot_button_register_event_cb(btns[i].btn, cfg, button_event_long_press_start_2_cb, (void *)i);
        cfg.event_data.long_press.press_time = 10000;
        err |= iot_button_register_event_cb(btns[i].btn, cfg, button_event_long_press_start_3_cb, (void *)i);

        ESP_ERROR_CHECK(err);
    }
}

void button_deinit() {
}