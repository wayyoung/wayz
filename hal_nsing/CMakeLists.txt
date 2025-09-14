# SPDX-License-Identifier: Apache-2.0

if (CONFIG_SOC_FAMILY_NSING_N32)

    zephyr_library()

    string(TOUPPER ${SOC_SERIES} _MCU)

    zephyr_compile_definitions( -DUSE_STDPERIPH_DRIVER -D${_MCU} )

    if(CONFIG_N32_ASSERT)
        zephyr_compile_definitions( -DUSE_FULL_ASSERT )
    endif()

    zephyr_include_directories(
        ${SOC_SERIES}/std_periph_driver/inc
        ${SOC_SERIES}/CMSIS/device
        )
    
        
    add_subdirectory(${SOC_SERIES})

endif()