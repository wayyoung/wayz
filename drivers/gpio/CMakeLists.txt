# SPDX-License-Identifier: Apache-2.0

if (CONFIG_SOC_FAMILY_NSING_N32)

zephyr_library_amend()

zephyr_library_sources_ifdef(CONFIG_GPIO_N32 gpio_n32.c)

endif()