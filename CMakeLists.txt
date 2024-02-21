# Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.22)

include(ExternalProject)
include("${CMAKE_SOURCE_DIR}/build/rapidjson.cmake")

project(wizard 
VERSION 1.1.0.0
DESCRIPTION "Custom composed HID device controller"
LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_compile_options(-Wall -fPIC -g)

execute_process (COMMAND bash -c "git rev-parse --short=4 HEAD | tr -d '\n'" OUTPUT_VARIABLE GIT_HASH)
configure_file(${PROJECT_SOURCE_DIR}/build/wizard_revision.h.in ${PROJECT_SOURCE_DIR}/src/wizard_revision.h @ONLY)

set(EXECUTABLE_NAME "${PROJECT_NAME}")
string(TOLOWER ${EXECUTABLE_NAME} EXECUTABLE_NAME)

set(LIBRARY_NAME "${PROJECT_NAME}_lib")
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME)

include("${CMAKE_SOURCE_DIR}/build/graffiti.cmake")

add_library(${LIBRARY_NAME}
    src/pulp/macros.cpp
    src/varikey/varikey_command.cpp
    src/varikey/varikey_gadget_usb.cpp
)

add_executable(${EXECUTABLE_NAME}
    src/wizard/wizard_args.cpp
    src/wizard/wizard.cpp
)

add_executable(report-example 
    src/example/report-example.cpp
)

add_dependencies(${EXECUTABLE_NAME} ascii_art)

target_sources(${EXECUTABLE_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_args.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_config.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_usb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard.cpp
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/build/
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/example/
    ${CMAKE_CURRENT_LIST_DIR}/src/pulp/
    ${CMAKE_CURRENT_LIST_DIR}/src/varikey/
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/build/
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/example/
    ${CMAKE_CURRENT_LIST_DIR}/src/pulp/
    ${CMAKE_CURRENT_LIST_DIR}/src/varikey/
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/
)
    
target_include_directories(${LIBRARY_NAME} SYSTEM PRIVATE
    ${RAPIDJSON_INCLUDE_DIR}
)

target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE
    ${RAPIDJSON_INCLUDE_DIR}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})
target_link_libraries(report-example PRIVATE ${LIBRARY_NAME})

add_dependencies(${EXECUTABLE_NAME} rapidjson)
