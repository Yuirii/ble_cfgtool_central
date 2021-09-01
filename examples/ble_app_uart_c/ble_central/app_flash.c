#include "app_flash.h"
#include "nrf_nvmc.h"
#include "data_process.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x4e000,
    .end_addr   = 0x4ffff,
};

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

static uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}

static void print_flash_info(nrf_fstorage_t * p_fstorage)
{
    NRF_LOG_INFO("========| flash info |========");
    NRF_LOG_INFO("erase unit: \t%d bytes",      p_fstorage->p_flash_info->erase_unit);
    NRF_LOG_INFO("program unit: \t%d bytes",    p_fstorage->p_flash_info->program_unit);
    NRF_LOG_INFO("==============================");
}

/**@brief 	
 * @param[in] 	
 * @param[in] 	
 * @return 	
 **/
void app_fstorage_init(nrf_fstorage_api_t * p_fs_api)
{
	ret_code_t rc;
	
	rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
	APP_ERROR_CHECK(rc);
	
	print_flash_info(&fstorage);
	
	(void) nrf5_flash_end_addr_get();
}

/**@brief 	write flash.
 * @param[in] 	
 * @param[in] 	
 * @return 	
 **/
void app_fstorage_write(uint32_t dest, void const *p_src, uint32_t len)
{
	ret_code_t rc;
	
	rc = nrf_fstorage_write(&fstorage, dest, p_src, len, NULL);
    APP_ERROR_CHECK(rc);
	
	while (nrf_fstorage_is_busy(&fstorage))
    {
        (void) sd_app_evt_wait();
    }
	
	
}

/**@brief 	read flash.
 * @param[in] 	
 * @param[in] 	
 * @return 	
 **/
void app_fstorage_read(uint32_t addr, void *p_src, uint32_t len)
{
	ret_code_t rc;
	
	rc = nrf_fstorage_read(&fstorage, addr, p_src, len);
	APP_ERROR_CHECK(rc);
}


/**@brief 	erase flash.
 * @param[in] 	page_addr   flash address.
 * @param[in]   len page    size.
 * @return 	
 **/
void app_fstorage_erase(uint32_t page_addr, uint32_t len)
{
	ret_code_t rc;
	
	rc = nrf_fstorage_erase(&fstorage, page_addr, len, NULL);
	APP_ERROR_CHECK(rc);
}


/** @brief Function for erasing a page in flash.

 *

 * @param page_address Address of the first word in the page to be erased.

 */
void flash_page_erase(uint32_t * page_address)
{
    // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

void flash_string_write(uint32_t address, uint8_t * src, uint32_t num_words)
{
    uint32_t i;

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
    }

    for (i = 0; i < num_words; i++)
    {
        /* Only full 32-bit words can be written to Flash. */
        ((uint32_t*)address)[i] = 0x000000FFUL & (uint32_t)((uint8_t)src[i]);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
        {
        }
    }

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
    }
}






