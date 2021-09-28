#ifndef SHUTTERS_EVENTS_H
#define SHUTTERS_EVENTS_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "event_groups.h"

#define FIRST_STATUS_INFO_SENT_FLAG             (1 << 0)
#define UPDATE_FIRMWARE_FLAG                    (1 << 1)
#define REQUEST_ERROR_OCCURRED_FLAG             (1 << 2)
#define STATUS_INFO_IS_BEING_SENT_FLAG          (1 << 3)
#define WIFI_CONNECTED_FLAG                     (1 << 4)
#define MANUALLY_IGNORE_ALARMS_FLAG             (1 << 5)

#define ACCESS_POINT_IS_BEING_SCANNED_FLAG      (1 << 7)
#define MOTION_DETECTOR_IS_BEING_IGNORED_FLAG   (1 << 8)
#define PIN_INTERRUPT_WAS_INITIALIZED_FLAG      (1 << 9)
#define ESP_EVENT_LOOP_INITIALIZED_FLAG         (1 << 10)
#define SEND_ALARM_1_EVENT                      (1 << 11)
#define SEND_ALARM_2_EVENT                      (1 << 12)
#define SEND_ALARM_3_EVENT                      (1 << 13)
#define SEND_STATUS_INFO_EVENT                  (1 << 14)
#define SCAN_ACCESS_POINT_EVENT                 (1 << 15)

void init_events();
void save_being_updated_event();
bool is_being_updated();
void save_request_error_occurred_event();
void clear_request_error_occurred_event();
bool is_request_error_occurred();
void save_sending_status_info_event();
void clear_sending_status_info_event();
bool is_status_info_being_sent();
bool is_first_status_info_sent();
void save_first_status_info_sent_event();
void save_connected_to_wifi_event();
void clear_connected_to_wifi_event();
bool is_connected_to_wifi();
void save_manually_ignored_alarms_flag();
void clear_manually_ignored_alarms_flag();
bool is_manually_ignored_alarms();
void save_sending_alarm_event(EventBits_t uxBitsToSet);
void clear_sending_alarm_event(EventBits_t uxBitsToSet);
bool is_alarm_being_sent(EventBits_t uxBitsToSet);
void save_access_point_scanning_event();
void clear_access_point_scanning_event();
bool is_access_point_is_being_scanned();
void save_motion_detector_is_being_ignored_event();
void clear_motion_detector_is_being_ignored_event();
bool is_motion_detector_being_ignored();
void save_pin_interrupt_was_initialized_event();
bool was_pin_interrupt_initialized();
void save_esp_event_loop_initialized_event();
bool is_esp_event_loop_initialized();

#endif