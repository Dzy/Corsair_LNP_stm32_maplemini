/*
	Flash storage for non-volatile data
		-2kB(KiB) of topmost Flash is set aside in STM32F103CBTx_FLASH.ld
*/

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdio.h>

extern CRC_HandleTypeDef hcrc;

uint32_t* flash_storage_init(size_t size);