#include "events.h"

static EventGroupHandle_t flags_g = NULL;

void init_events() {
   if (flags_g == NULL) {
      flags_g = xEventGroupCreate();
   }
}

void save_being_updated_event() {
   xEventGroupSetBits(flags_g, UPDATE_FIRMWARE_FLAG);
}

bool is_being_updated() {
   return (xEventGroupGetBits(flags_g) & UPDATE_FIRMWARE_FLAG);
}

void save_request_error_occurred_event() {
   xEventGroupSetBits(flags_g, REQUEST_ERROR_OCCURRED_FLAG);
}

void clear_request_error_occurred_event() {
   xEventGroupClearBits(flags_g, REQUEST_ERROR_OCCURRED_FLAG);
}

bool is_request_error_occurred() {
   return xEventGroupGetBits(flags_g) & REQUEST_ERROR_OCCURRED_FLAG;
}

void save_sending_status_info_event() {
   xEventGroupSetBits(flags_g, STATUS_INFO_IS_BEING_SENT_FLAG);
}

void clear_sending_status_info_event() {
   xEventGroupClearBits(flags_g, STATUS_INFO_IS_BEING_SENT_FLAG);
}

bool is_status_info_being_sent() {
   return xEventGroupGetBits(flags_g) & STATUS_INFO_IS_BEING_SENT_FLAG;
}

void save_first_status_info_sent_event() {
   xEventGroupSetBits(flags_g, FIRST_STATUS_INFO_SENT_FLAG);
}

bool is_first_status_info_sent() {
   return xEventGroupGetBits(flags_g) & FIRST_STATUS_INFO_SENT_FLAG;
}

void save_connected_to_wifi_event() {
   xEventGroupSetBits(flags_g, WIFI_CONNECTED_FLAG);
}

void clear_connected_to_wifi_event() {
   xEventGroupClearBits(flags_g, WIFI_CONNECTED_FLAG);
}

bool is_connected_to_wifi() {
   return xEventGroupGetBits(flags_g) & WIFI_CONNECTED_FLAG;
}

void save_manually_ignored_alarms_flag() {
   xEventGroupSetBits(flags_g, MANUALLY_IGNORE_ALARMS_FLAG);
}

void clear_manually_ignored_alarms_flag() {
   xEventGroupClearBits(flags_g, MANUALLY_IGNORE_ALARMS_FLAG);
}

bool is_manually_ignored_alarms() {
   return xEventGroupGetBits(flags_g) & MANUALLY_IGNORE_ALARMS_FLAG;
}

void save_sending_alarm_event() {
   xEventGroupSetBits(flags_g, ALARM_IS_BEING_SENT_FLAG);
}

void clear_sending_alarm_event() {
   xEventGroupClearBits(flags_g, ALARM_IS_BEING_SENT_FLAG);
}

bool is_alarm_being_sent() {
   return xEventGroupGetBits(flags_g) & ALARM_IS_BEING_SENT_FLAG;
}

void save_access_point_scanning_event() {
   xEventGroupSetBits(flags_g, ACCESS_POINT_IS_BEING_SCANNED_FLAG);
}

void clear_access_point_scanning_event() {
   xEventGroupClearBits(flags_g, ACCESS_POINT_IS_BEING_SCANNED_FLAG);
}

bool is_access_point_is_being_scanned() {
   return xEventGroupGetBits(flags_g) & ACCESS_POINT_IS_BEING_SCANNED_FLAG;
}

void save_motion_detector_is_being_ignored_event() {
   xEventGroupSetBits(flags_g, MOTION_DETECTOR_IS_BEING_IGNORED_FLAG);
}

void clear_motion_detector_is_being_ignored_event() {
   xEventGroupClearBits(flags_g, MOTION_DETECTOR_IS_BEING_IGNORED_FLAG);
}

bool is_motion_detector_being_ignored() {
   return xEventGroupGetBits(flags_g) & MOTION_DETECTOR_IS_BEING_IGNORED_FLAG;
}

void save_pin_interrupt_was_initialized_event() {
   xEventGroupSetBits(flags_g, PIN_INTERRUPT_WAS_INITIALIZED_FLAG);
}

bool was_pin_interrupt_initialized() {
   return xEventGroupGetBits(flags_g) & PIN_INTERRUPT_WAS_INITIALIZED_FLAG;
}

void save_esp_event_loop_initialized_event() {
   xEventGroupSetBits(flags_g, ESP_EVENT_LOOP_INITIALIZED_FLAG);
}

bool is_esp_event_loop_initialized() {
   return xEventGroupGetBits(flags_g) & ESP_EVENT_LOOP_INITIALIZED_FLAG;
}