# SPDX-License-Identifier: Apache-2.0

zephyr_library_amend()

if (CONFIG_SOC_FAMILY_NSING_N32)

zephyr_library_sources_ifdef(CONFIG_EXTI_N32 exti_n32.c)

endif()