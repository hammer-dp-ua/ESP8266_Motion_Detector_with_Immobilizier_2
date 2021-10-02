cmake_minimum_required(VERSION 3.5)

message("~~~ PATH: $ENV{PATH}")
message("~~~ ESPPORT: $ENV{ESPPORT}")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(Motion_Detector_with_Immobilizier)