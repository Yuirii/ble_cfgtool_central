#ifndef _BLE_CLOCKS_H_
#define _BLE_CLOCKS_H_

#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
//#include "ble_smlt.h"
#include "nrf_drv_wdt.h"
#include "app_timer.h"
#include "drv_rtc.h"

extern uint8_t GYRO_TIMEOUT_FLAG;
extern uint8_t BTN_TIMEOUT_FLAG;
extern uint8_t LPOWER_TIMEOUT_FLAG;
extern uint8_t HPOWER_TIMEOUT_FLAG;

void ble_hfclk_init( void );
void ble_hfclk_deinit( void );
void ble_lfclk_config(void);
void ble_rtc_config(void);
void ble_wdt_init(void);
void ble_rtc_handler(nrf_drv_rtc_int_type_t int_type);
void ble_wdt_handler(void);
void application_timers_start(uint32_t advInterval);
void application_timers_stop(void);
void gyro_timers_start(uint32_t advInterval);
void gyro_timers_stop(void);
void btn_timers_start(uint32_t advInterval);
void btn_timers_stop(void);
void lpower_timers_start(uint32_t advInterval);
void lpower_timers_stop(void);
uint64_t get_timestamps(void);

#endif /*_BLE_CLOCKS_H_ */

