#include "app_que.h"
#include "stdlib.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"
#include "data_process.h"

/**************************************************
 * @FUNCTION: 	
 * @PARAMS: 	
 * @RETURN: 	
 **/
int que_func(int (*func_ptr)(Qque*, ble_gap_evt_adv_report_t const*), Qque* que, ble_gap_evt_adv_report_t const*elem)
{
    return func_ptr(que, elem);
}


/**************************************************
 * @FUNCTION: Creation. 
 * @PARAMS: 	
 * @RETURN: 	
 **/
int que_init(Qque *que)
{
    que->base = (app_scan_rpt *)malloc(MAX_QSIZE * sizeof(app_scan_rpt));
    if (!que->base)
    {
        return -1;  
    }
    que->front = 0;
	que->rear = 0;
    return 1;
}

/**************************************************
 * @FUNCTION: Length.
 * @PARAMS: 	
 * @RETURN: 	
 **/
int qlen_get(Qque *que)
{
    return (que->rear - que->front + MAX_QSIZE) % MAX_QSIZE;
}

/**************************************************
 * @FUNCTION: Is full?
 * @PARAMS: 	
 * @RETURN: 	
 **/
int que_full(Qque *que)
{
    return (que->front == (que->rear+1) % MAX_QSIZE)?1:0;
}

/**************************************************
 * @FUNCTION: Is empty?
 * @PARAMS: 	
 * @RETURN: 	
 **/
uint8_t que_emp(Qque *que)
{
    return (que->front == que->rear)?1:0;
}

/**************************************************
 * @FUNCTION: Enter.
 * @PARAMS: 	
 * @RETURN: 	
 **/
int que_ent(Qque *que, ble_gap_evt_adv_report_t const *elem)
{
    if (que_full(que))
    {
        return -1;
    }
    
    memcpy(que->base[que->rear].addr, elem->peer_addr.addr, 6 * sizeof(uint8_t));
	
	que->base[que->rear].data.p_data = (uint8_t *)calloc(elem->data.len, sizeof(uint8_t));
	memcpy(que->base[que->rear].data.p_data, elem->data.p_data, elem->data.len);
	
	que->base[que->rear].data.len = elem->data.len;
	que->base[que->rear].rssi = elem->rssi;
	que->base[que->rear].type = elem->type.connectable|
								elem->type.scannable<<1|
								elem->type.directed<<2|
								elem->type.scan_response<<3|
								elem->type.extended_pdu<<4|
								elem->type.status<<5|
								elem->type.reserved<<9;
    que->rear = (que->rear+1) % MAX_QSIZE;
	
    return 1;
}

/**************************************************
 * @FUNCTION: Delete.
 * @PARAMS: 	
 * @RETURN: 	
 **/
int que_del(Qque *que, ble_gap_evt_adv_report_t elem)
{
    if (que->front == que->rear)    
    {
        return -1;
    }

	elem.type.connectable = que->base[que->front].type & 0x0001;
	elem.type.scannable = que->base[que->front].type & 0x0002;
	elem.type.directed = que->base[que->front].type & 0x0004;
	elem.type.scan_response = que->base[que->front].type & 0x0008;
	elem.type.extended_pdu = que->base[que->front].type & 0x0010;
	elem.type.status = que->base[que->front].type & 0x0060;
	elem.type.reserved = que->base[que->front].type & 0xff80;
    elem.data.p_data = que->base[que->front].data.p_data;
	elem.data.len = que->base[que->front].data.len;
	elem.rssi = que->base[que->front].rssi;
	*elem.peer_addr.addr = *que->base[que->front].addr;
	
    que->front = (que->front + 1) % MAX_QSIZE;

    return 1;
}

/**@brief 	clear
 * @param 	
 * @return 	
 **/
void que_clear(Qque *que)
{
	for(uint8_t i = que->front; i < que->rear; i++)
	{
		free(que->base[i].data.p_data);
		que->base[i].data.p_data = NULL;
	}
	que->front = que->rear = 0;
}

/**************************************************
 * @FUNCTION: print and tx.
 * @PARAMS: 	
 * @RETURN: 	
 **/
void que_print(Qque *que, void (*uart_handle)(uint8_t*, uint16_t))
{
	uint8_t app_scan_rpt_c;
	app_scan_rpt_c = sizeof(app_scan_rpt)-sizeof(ble_data_t);
	uint8_t num = 0; 	// len(device[i])
	uint16_t sum = 5; 	// len(buff_tx[0])
//	ble_gap_evt_adv_report_t elem;
	/* buff_tx type
		0	1	2	3	4 ...
	0	head:0xFE, len:??, type:0x82, sub_type:0x01, device_num:que->rear
	1	len1, device1_data
	2	len2, device2_data
	3	len3, device3_data
	.
	.
	.
	end crc:CRC_8
	*/
	uint8_t buff_tx[que->rear + 2][(app_scan_rpt_c + 26) + 2];
	memset(buff_tx, 0, sizeof(buff_tx));
	
	uint8_t *buff_txx;
	uint8_t *buff_txxx;
	buff_txx = (uint8_t *)malloc(5);
	memset(buff_txx, 0, 5);
	
    if (que_emp(que))
    {
        NRF_LOG_INFO("Empty.");
        
    }else{
		{
			buff_tx[0][0] = 0xFE;		//head
			buff_tx[0][1] = 0x00;		//length
			buff_tx[0][2] = 0x82; 		//type
			buff_tx[0][3] = 0x00;		//sub_type
			buff_tx[0][4] = que->rear;	//device_num
		}
        for(uint8_t i = que->front; i < que->rear; i++)
        {
			NRF_LOG_INFO("front_print: %d", i);
			NRF_LOG_INFO("peer_print:%02X %02X %02X %02X %02X %02X", 
													que->base[i].addr[0],
													que->base[i].addr[1],
													que->base[i].addr[2],
													que->base[i].addr[3],
													que->base[i].addr[4],
													que->base[i].addr[5]);
			
			/* Set buff that sent. */
			num = app_scan_rpt_c + que->base[i].data.len; //num(bytes) of buff_tx
			buff_tx[i+1][0] = num; //len[i]
			buff_txxx = (uint8_t *)realloc(buff_txx, num+1); // Apply for more memory dynamically.
			if(buff_txxx != NULL)
			{
				NRF_LOG_INFO("realloc ok.");
			}else{
				NRF_LOG_INFO("realloc error.");
			}
			
			memcpy(buff_txx, buff_tx, 5);
			buff_txx[sum] = num;
			memcpy(&buff_txx[sum+1], &que->base[i].type, app_scan_rpt_c);
			memcpy(&buff_txx[sum+app_scan_rpt_c+1], que->base[i].data.p_data, que->base[i].data.len);
			sum += (num + 1); //every payload count.
			
//			memcpy(&buff_tx[i+1][1], &que->base[i].type, app_scan_rpt_c);
//			memcpy(&buff_tx[i+1][app_scan_rpt_c+1], que->base[i].data.p_data, que->base[i].data.len);
//			
//			uart_handle(buff_tx[i+1], app_scan_rpt_c+que->base[i].data.len);
//			printf("0000"); //set separaror for python.
//			que_del(que, elem);
//			free(que->base[i].data.p_data);
        }
		//sum + 1: CRC
		buff_txx[sum] = app_crc_8(buff_txx, sum);
		
		// Send scan data to uart.
		uart_handle(buff_txx, sum+1);
		
		//destory memory.
		free(buff_txx);
		buff_txx = NULL;

		NRF_LOG_INFO("ok11.");
    }	    
}

