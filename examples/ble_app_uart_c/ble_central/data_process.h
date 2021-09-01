#ifndef _DATA_PROCESS_H_
#define _DATA_PROCESS_H_

#include <stdint.h>
#include "ble_gap.h"
#include "sdk_config.h"
#include "nrf_ble_scan.h"

//--------DEFINE--------/
#define OKAY                1
#define NOTOK               0
// esp32 <--> mcu communication type
#define APP_CONFIG          0x00
#define APP_SCAN_ON         0x01
#define APP_MSG_UPD         0x02
#define APP_SCAN_OFF        0x03
#define APP_RESET           0x04
#define APP_CONFIG_RSP      0x80
#define APP_SCAN_ON_RSP     0x81
#define APP_MSG_UDP_RSP     0x82
#define APP_SCAN_OFF_RSP    0x83
// rssi range
#define RSSI_RANGE_LOW		0x01
#define RSSI_RANGE_MEDI		0x02
#define RSSI_RANGE_HIGH		0x03
//duplication 
#define DUP_EST				0x01
// scan_evt_handle scan_rsl_msg
#define SCAN_RSL_MSG_CLR    0x00
#define SCAN_RSL_MSG_EST    0x01
// scan_evt_handle scan_time_msg
#define SCAN_TIMEOUT_CLR    0x00
#define SCAN_TIMEOUT_EST    0x01
// cmd_num(HEAD->CRC)
#define CONFIG_CMD_CNT      8


typedef struct {
	uint8_t 	interval;
	uint8_t		rssi_f;
	uint8_t		adv_f;
	uint8_t		dup_f;
}app_config_params_t;

typedef struct {
	uint16_t 				scan_rsl_msg;
	uint16_t				scan_time_msg;
	app_config_params_t		app_cfg;
}app_evt_tsk;


//---------Params---------/
extern ble_gap_scan_params_t m_scan_params;
extern app_evt_tsk m_tsk_params;
extern ble_gap_conn_params_t m_conn_params;

//--------FUNCTION---------/
uint8_t app_config(uint8_t *uart_cmd);
uint8_t app_checksum(uint8_t *buff);
uint8_t app_crc_8(uint8_t *buff, uint16_t len);

#endif
