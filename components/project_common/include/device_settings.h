#ifndef DEVICE_SETTINGS
#define DEVICE_SETTINGS

// Only 1 is required
#define MOTION_SENSOR_WITH_IMMOBILIZER
//#define TESTING

// const are saved at 40200000h (flash memory)
#ifdef TESTING
static const char DEVICE_NAME[] = "Testing3";
#elif defined(MOTION_SENSOR_WITH_IMMOBILIZER)
static const char DEVICE_NAME[] = "Motion Sensor with Immobilizer";
static const char DEVICE_NAME_ENCODED_SPACES[] = "Motion%20Sensor%20with%20Immobilizer";
#endif

static const char ALARM_SOURCE_1[] = "MOTION_SENSOR_1";
static const char ALARM_SOURCE_2[] = "MOTION_SENSOR_2";
static const char ALARM_SOURCE_3[] = "MOTION_SENSOR_3";

static const char SERVER_IP_ADDRESS[] = "192.168.0.2";
static const char ACCESS_POINT_PASSWORD[] = "x>vZf8bqX]l-qg%";
static const char ACCESS_POINT_NAME[] = "SAMSUNG";

#define SERVER_PORT 80

#endif
