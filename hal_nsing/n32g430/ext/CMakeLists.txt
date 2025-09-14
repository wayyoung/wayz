# SPDX-License-Identifier: Apache-2.0

zephyr_library_sources_ifdef(CONFIG_N32_HAL_DMA   src/stm32f1xx_ll_dma.c)

zephyr_include_directories(inc)