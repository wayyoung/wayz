# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

zephyr_include_directories(include)

add_subdirectory(drivers)
add_subdirectory(hal_nsing)