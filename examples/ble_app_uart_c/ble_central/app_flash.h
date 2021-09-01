#ifndef _APP_FLASH_H_
#define _APP_FLASH_H_

#include "stdint.h"
#include "nrf_fstorage_sd.h"


void flash_page_erase(uint32_t * page_address);
void flash_string_write(uint32_t address, uint8_t * src, uint32_t num_words);

void app_fstorage_init(nrf_fstorage_api_t * p_fs_api);
void app_fstorage_write(uint32_t dest, void const *p_src, uint32_t len);
void app_fstorage_read(uint32_t addr, void *p_src, uint32_t len);
void app_fstorage_erase(uint32_t page_addr, uint32_t len);


#endif





