/*
	Flash storage for non-volatile data
		-2kB(KiB) of topmost Flash is set aside in STM32F103CBTx_FLASH.ld
*/

#include "flash_storage.h"

uint32_t* flash_storage_init(size_t size) {
	printf("uint32_t flash_storage_init(%d);\n", size);

	uint32_t *flash_storage_page0 = (uint32_t *)(0x8000000+(126*1024));
	uint32_t *flash_storage_page1 = (uint32_t *)(0x8000000+(127*1024));
	//printf("  -%p(%.8lx)\n", flash_storage_page0, *flash_storage_page0);
	//printf("  -%p(%.8lx)\n", flash_storage_page1, *flash_storage_page1);

	/*
		Check if both pages are erased
			- erased page gives checksum value of 0xd000a3e2
	*/
	uint32_t crc0 = HAL_CRC_Calculate(&hcrc, flash_storage_page0, 1024/sizeof(uint32_t));
	uint32_t crc1 = HAL_CRC_Calculate(&hcrc, flash_storage_page1, 1024/sizeof(uint32_t));

	if((crc0 == 0xd000a3e2)&&(crc1 == 0xd000a3e2)) {
		printf("both pages clear\n");
		//FLASH_ProgramWord(VarAddress, VarData);
		return(flash_storage_page0);
	}

	/*
		Either page has to be erased at any given time
	*/
	if((crc0 != 0xd000a3e2)&&(crc1 != 0xd000a3e2)) {
		printf("erase both pages\n");
		return(flash_storage_page0);
	}

	return(NULL);

}