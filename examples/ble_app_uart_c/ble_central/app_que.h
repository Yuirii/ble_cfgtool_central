#ifndef _APP_QUE_H_
#define _APP_QUE_H_

#include <stdint.h>
#include "ble_gap.h"


/*DEFINE*/
#define MAX_QSIZE   100
typedef struct 
{
	uint8_t					type;
	uint8_t addr[BLE_GAP_ADDR_LEN];
	int8_t                    	rssi;
	ble_data_t                	data;
}app_scan_rpt;

typedef struct  
{
	app_scan_rpt				*base;   //    inherit @ble_gap_evt_adv_report_t
    uint8_t    					front;
    uint8_t    					rear;
}Qque;




/*FUNCTION*/
int que_func(int (*func_ptr)(Qque*, ble_gap_evt_adv_report_t const*), Qque* que, ble_gap_evt_adv_report_t const *elem);
int que_init(Qque *que);
int qlen_get(Qque *que);
uint8_t que_emp(Qque *que);
int que_full(Qque *que);
void que_clear(Qque *que);
int que_ent(Qque *que, ble_gap_evt_adv_report_t const *elem);
int que_del(Qque *que, ble_gap_evt_adv_report_t elem);
void que_print(Qque *que, void (*uart_handle)(uint8_t*, uint16_t));


#endif
