# SPDX-License-Identifier: Apache-2.0

# board_runner_args(pyocd "--target=N32G4FRREL7" "--tool-opt=--pack=${ZEPHYR_GAP_MODULE_DIR}/hal_nationstech/${CONFIG_SOC_SERIES}/Nationstech.N32G4FR_DFP.1.0.5.pack")
# include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)

board_runner_args(jlink "--device=n32g430f8" "--speed=4000")
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
