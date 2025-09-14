/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/irq.h>


/**
 * @brief Perform basic hardware initialization at boot.
 *
 * This needs to be run from the very beginning.
 * So the init priority has to be 0 (zero).
 *
 * @return 0
 */
static int n32g430_init(void) {
  uint32_t key;

  key = irq_lock();

  System_Initializes();
  System_Clock_Frequency_Update();

  irq_unlock(key);

  return 0;
}

SYS_INIT(n32g430_init, PRE_KERNEL_1, 0);
