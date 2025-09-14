#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0


cmake_minimum_required(VERSION 3.20.0)
# list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include($ENV{ZEPHYR_BASE}/cmake/cfb.cmake NO_POLICY_SCOPE)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gaz)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
generate_cfb_font_for_target(app fonts/MxPlus_IBM_CGAthin.ttf ${ZEPHYR_BINARY_DIR}/include/generated/cfb_font_dice.h 8 8 -s 8)