# SPDX-License-Identifier: Apache-2.0

zephyr_library_amend()

if (CONFIG_SOC_FAMILY_NSING_N32)

zephyr_library_sources_ifdef(CONFIG_I2C_N32 i2c_n32.c)

endif()