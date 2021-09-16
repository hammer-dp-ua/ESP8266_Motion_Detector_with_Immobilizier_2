Second version of the project using CMake

Wi-Fi update procedure:
1. Build the project. No need to build Motion_Detector_with_Immobilizier.bin for ota_0/ota_1 separately.
2. Copy cmake-build-debug/Motion_Detector_with_Immobilizier.bin into /home/servak/www/ESP8266_upgrade directory on the server server.
Set "ESP8266_upgrade" as "esp8266_fota" alias in Apache. I.e., Motion_Detector_with_Immobilizier.bin URL is http://192.168.0.2/esp8266_fota/Motion_Detector_with_Immobilizier.bin

