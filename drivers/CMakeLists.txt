# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

zephyr_include_directories(include)

add_subdirectory_ifdef(CONFIG_CLOCK_CONTROL clock_control)
add_subdirectory_ifdef(CONFIG_RESET reset)
add_subdirectory_ifdef(CONFIG_GPIO gpio)
add_subdirectory_ifdef(CONFIG_PINCTRL pinctrl)
add_subdirectory_ifdef(CONFIG_SERIAL_HAS_DRIVER serial)
add_subdirectory_ifdef(CONFIG_I2C i2c)

add_subdirectory(interrupt_controller)
