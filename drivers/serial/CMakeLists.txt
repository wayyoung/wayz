# SPDX-License-Identifier: Apache-2.0
zephyr_library_amend()

if (CONFIG_SOC_FAMILY_NSING_N32)

zephyr_library_sources_ifdef(CONFIG_UART_N32 uart_n32.c)

endif()


zephyr_include_directories(
    .
    ${ZEPHYR_BASE}/subsys/usb/device_next/
    )
