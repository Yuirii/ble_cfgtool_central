#include "data_process.h"
#include "string.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

ble_gap_scan_params_t m_scan_params = 
{
    .active        = 0,                            		// 1: active
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,   		// x * 0.625ms
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,     		// x * 0.625ms
    .timeout       = (NRF_BLE_SCAN_SCAN_INTERVAL/16)+1, // 10ms * X
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,   		// no
    .scan_phys     = BLE_GAP_PHY_AUTO,            		// auto, default is 1M.
};

app_evt_tsk m_tsk_params = 
{
    .scan_rsl_msg   =   SCAN_RSL_MSG_CLR,
	.scan_time_msg	= 	SCAN_TIMEOUT_CLR,
	.app_cfg		= 	{0}
};

ble_gap_conn_params_t m_conn_params = 
{
    .min_conn_interval  = MSEC_TO_UNITS(NRF_BLE_SCAN_MIN_CONNECTION_INTERVAL, UNIT_1_25_MS),  // MIN 7.5ms
    .max_conn_interval  = MSEC_TO_UNITS(NRF_BLE_SCAN_MAX_CONNECTION_INTERVAL, UNIT_1_25_MS),  // MAX 30ms  
    .slave_latency      = NRF_BLE_SCAN_SLAVE_LATENCY,                                         //	
    .conn_sup_timeout   = MSEC_TO_UNITS(NRF_BLE_SCAN_SUPERVISION_TIMEOUT, UNIT_10_MS),        // TIMEOUT 4000ms 
};


uint8_t app_crc_8(uint8_t *buff, uint16_t len)
{
	uint8_t sum = 0;
	
	
	for (uint16_t i = 0; i < len; i++)
    {
        sum += buff[i];
    }
	
	return sum;
}

/**************************************************
 * @FUNCTION: checksum 8-bits.
 * @PARAMS: 	
 * @RETURN: 	1:success  0:fail
 **/
uint8_t app_checksum(uint8_t *buff)
{
	uint8_t sum = 0;
    uint8_t len_crc = 0;

    len_crc = buff[1] + 1;

    for (uint16_t i = 0; i < len_crc; i++)
    {
        sum += buff[i];
    }
	
	if(sum == buff[len_crc])
	{
		return 1;
	}else{
		return 0;
	}
}



/**************************************************
 * @FUNCTION: 
 * @PARAMS: 	
 * @RETURN: 	
 **/
void scan_interval_set(uint8_t *uart_cmd)
{

}

/**************************************************
 * @FUNCTION: get msg from uart to settings.
 * @PARAMS: 	
 * @RETURN: 	null
 **/
uint8_t app_config(uint8_t *uart_cmd)
{
    uint8_t count;
    count = (uart_cmd[1]-1);
	uint8_t cmd[count];
	memset(cmd, 0, count);

    if (uart_cmd[2] == 0x00 && app_checksum(uart_cmd) == 1) //0x00 filter configuration
    {
        memcpy(cmd, (uart_cmd+3), count*sizeof(uint8_t));      

        if (cmd[0] != 0x00)
        {
            /* interval */
            m_tsk_params.app_cfg.interval = cmd[0];
        }else{
            m_tsk_params.app_cfg.interval = 0;
        }
        
        if (cmd[1] != 0x00)
        {
            /* rssi */
            m_tsk_params.app_cfg.rssi_f = cmd[1];
        }else{
            m_tsk_params.app_cfg.rssi_f = 0;
        }

        if (cmd[2] == 0x01)
        {
            /* advertising */ 
            m_tsk_params.app_cfg.adv_f = cmd[2];
        }else{
            m_tsk_params.app_cfg.adv_f = 0;
        }

        if (cmd[3] == 0x01)
        {
            /* dupilcate */ 
            m_tsk_params.app_cfg.dup_f = cmd[3];
        }else{
            m_tsk_params.app_cfg.dup_f = 0;
        }
        return OKAY;
	}else{
        return NOTOK;
    }
	
}


