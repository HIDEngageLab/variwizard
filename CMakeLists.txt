# Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.22)

project(wizard 
VERSION 1.0.0.0
DESCRIPTION "Custom composed HID device controller"
LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_compile_options(-Wall -fPIC -g)

add_library(_varikey
    src/varikey_gadget_usb.cpp
)

add_executable(wizard
    src/wizard.cpp
    src/wizard_args.cpp
)

execute_process (COMMAND bash -c "git rev-parse --short=4 HEAD | tr -d '\n'" OUTPUT_VARIABLE GIT_HASH)
configure_file(${PROJECT_SOURCE_DIR}/src/wizard_revision.h.in ${PROJECT_SOURCE_DIR}/src/wizard_revision.h @ONLY)

target_sources(wizard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard_usb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wizard_args.cpp
)

target_include_directories(wizard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src/
)

target_link_libraries(wizard PRIVATE _varikey)
