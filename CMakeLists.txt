
idf_component_register(
    SRCS button.c
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "."
    REQUIRES esp_event
    PRIV_REQUIRES driver logger_common esp_timer
)
