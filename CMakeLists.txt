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

add_library(_varikey
    src/varikey/varikey_gadget_usb.cpp
    src/pulp/macros.cpp
)

add_executable(wizard
    src/wizard/wizard.cpp
    src/wizard/wizard_args.cpp
)

add_executable(report-example 
    src/example/report-example.cpp
)

execute_process (COMMAND bash -c "git rev-parse --short=4 HEAD | tr -d '\n'" OUTPUT_VARIABLE GIT_HASH)
configure_file(${PROJECT_SOURCE_DIR}/build/wizard_revision.h.in ${PROJECT_SOURCE_DIR}/src/wizard_revision.h @ONLY)

target_sources(wizard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_args.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_config.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard_usb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/wizard.cpp
)

target_include_directories(wizard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/build/
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/example/
    ${CMAKE_CURRENT_LIST_DIR}/src/pulp/
    ${CMAKE_CURRENT_LIST_DIR}/src/varikey/
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/
)

target_include_directories(_varikey PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/build/
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/example/
    ${CMAKE_CURRENT_LIST_DIR}/src/pulp/
    ${CMAKE_CURRENT_LIST_DIR}/src/varikey/
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard/
)
    
target_include_directories(_varikey SYSTEM PRIVATE
    ${RAPIDJSON_INCLUDE_DIR}
)

target_include_directories(wizard SYSTEM PRIVATE
    ${RAPIDJSON_INCLUDE_DIR}
)

target_link_libraries(wizard PRIVATE _varikey)
target_link_libraries(report-example PRIVATE _varikey)

add_dependencies(wizard rapidjson)