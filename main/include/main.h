#include "freertos/FreeRTOS.h"
#include "task.h"
#include "nvs_flash.h"

#include "global_definitions.h"
#include "malloc_logger.h"
#include "ota.h"

#ifndef MAIN_HEADER
#define MAIN_HEADER

#ifdef TESTING
#define AP_CONNECTION_STATUS_LED_PIN         GPIO_NUM_13
#define SERVER_AVAILABILITY_STATUS_LED_PIN   GPIO_NUM_2
#define BUZZER_PIN                           GPIO_NUM_4
#define MOTION_DETECTOR_ENABLE_PIN           GPIO_NUM_14
#define MOTION_DETECTOR_INPUT_PIN            GPIO_NUM_12
#else
#define AP_CONNECTION_STATUS_LED_PIN         GPIO_NUM_5
#define SERVER_AVAILABILITY_STATUS_LED_PIN   GPIO_NUM_4
#define BUZZER_PIN                           GPIO_NUM_12
#define MOTION_DETECTOR_ENABLE_PIN           GPIO_NUM_14
#define MOTION_DETECTOR_INPUT_PIN            GPIO_NUM_2
#endif

#define ERRORS_CHECKER_INTERVAL_MS (10 * 1000)
#define STATUS_REQUESTS_SEND_INTERVAL_MS (30 * 1000)
#define SCAN_ACCESS_POINT_TASK_INTERVAL (10 * 60 * 1000) // 10 minutes
#define ALARM_RESEND_PAUSE (10 * 1000 / portTICK_PERIOD_MS) // 10 seconds
#define MOTION_DETECTOR_IGNORING_TIMEOUT_MS (60 * 1000)

#define MILLISECONDS_COUNTER_DIVIDER 10

#define MAX_REPETITIVE_ALLOWED_ERRORS_AMOUNT 15

#define SYSTEM_RESTART_REASON_TYPE_RTC_ADDRESS  64
#define CONNECTION_ERROR_CODE_RTC_ADDRESS       (SYSTEM_RESTART_REASON_TYPE_RTC_ADDRESS + 1)

typedef enum {
   ACCESS_POINT_CONNECTION_ERROR = 1,
   REQUEST_CONNECTION_ERROR,
   SOFTWARE_UPGRADE,
   TCP_SERVER_ERROR
} SYSTEM_RESTART_REASON_TYPE;

const char SEND_STATUS_INFO_TASK_NAME[] = "send_status_info_task";

const char RESPONSE_SERVER_SENT_OK[] = "\"statusCode\":\"OK\"";
const char STATUS_INFO_POST_REQUEST[] =
      "POST /server/esp8266/statusInfo HTTP/1.1\r\n"
      "Content-Length: <1>\r\n"
      "Host: <2>\r\n"
      "User-Agent: ESP8266\r\n"
      "Content-Type: application/json\r\n"
      "Connection: close\r\n"
      "Accept: application/json\r\n\r\n"
      "<3>\r\n";
const char STATUS_INFO_REQUEST_PAYLOAD_TEMPLATE[] =
      "{"
      "\"gain\":\"<1>\","
      "\"deviceName\":\"<2>\","
      "\"errors\":<3>,"
      "\"uptime\":<4>,"
      "\"buildTimestamp\":\"<5>\","
      "\"freeHeapSpace\":<6>,"
      "\"resetReason\":\"<7>\","
      "\"systemRestartReason\":\"<8>\""
      "}";

#ifdef TESTING
const char ALARM_GET_REQUEST_TEMPLATE[] =
      "GET /server/manager/testAlarm?alarmSource=<1>&deviceName=<2> HTTP/1.1\r\n"
      "Host: <3>\r\n"
      "User-Agent: ESP8266\r\n"
      "Connection: close\r\n"
      "Accept: application/json\r\n\r\n";
#else
const char ALARM_GET_REQUEST_TEMPLATE[] =
      "GET /server/esp8266/alarm?alarmSource=<1>&deviceName=<2> HTTP/1.1\r\n"
      "Host: <3>\r\n"
      "User-Agent: ESP8266\r\n"
      "Connection: close\r\n"
      "Accept: application/json\r\n\r\n";
#endif

const char UPDATE_FIRMWARE[] = "\"updateFirmware\":true";
const char MANUALLY_IGNORE_ALARMS[] = "\"ignoreAlarms\":true";

static void stop_both_leds_blinking();
static void start_both_leds_blinking(unsigned int ms);
static void beep();
static void stop_motion_detector_ignoring_cb();

#endif