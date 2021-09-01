#ifndef __BLE_BEACON_C_H__
#define __BLE_BEACON_C_H__

#include <stdint.h>
#include "ble.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"

#define BLE_CHAR_BLE_OBSERVER_PRIO 2 //Priority with which BLE events are dispatched to the Specific Service.

#define BLE_CHAR_DEF(_name, _cnt)
static ble_beacon_c_t _name[_cnt];
NRF_SDH_BLE_OBSERVERS(
    _name ## _obs,
    BLE_CHAR_BLE_OBSERVER_PRIO,
    ble_charx_c_on_ble_evt,
    &_name,
    _cnt
)

/**@brief Copy from ckboy's file@ble_diy_inf.h */
// Simple Profile Service UUID
#define SIMPLEPROFILE_SERV_UUID             0xFFF0
// Key Pressed UUID
#define SIMPLEPROFILE_CHAR1_UUID            0xFFF1
#define SIMPLEPROFILE_CHAR2_UUID            0xFFF2
#define SIMPLEPROFILE_CHAR3_UUID            0xFFF3
#define SIMPLEPROFILE_CHAR4_UUID            0xFFF4
#define SIMPLEPROFILE_CHAR5_UUID            0xFFF5
#define SIMPLEPROFILE_CHAR6_UUID            0xFFF6
#define SIMPLEPROFILE_CHAR7_UUID            0xFFF7
#define SIMPLEPROFILE_CHAR60_UUID           0xFF60


/**@brief Structure containing the handles related to BEACON configuration.*/
typedef struct 
{
    // uint16_t beacon_cccd_handle;
    uint16_t char1_handle;
    uint16_t char2_handle;
    uint16_t char3_handle;
    uint16_t char4_handle;
    uint16_t char5_handle;
    uint16_t char6_handle;
    uint16_t char7_handle;
    uint16_t char60_handle;
}beacon_db_t;

/**@brief Client event type.*/
typedef enum
{
    BLE_BEACON_C_EVT_DISCOVERY_COMPLETE = 1,
    // BLE_BEACON_C_EVT_BUTTON_NOTIFICATION
}ble_beacon_c_evt_type_t;

/**@brief Structure containing the Beacon value received from the peer.*/
typedef struct 
{
    uint8_t char1_state;
    uint8_t char2_state;
    uint8_t char3_state;
    uint8_t char5_state;
    uint8_t char6_state;
    uint8_t char7_state;
}ble_beacon_cfg_t;


/**@brief BEACON Event structure.*/
typedef struct 
{
    ble_beacon_c_evt_type_t evt_type;
    uint16_t                conn_handle;
    union 
    {
        ble_write_t beacon_cfg;
        beacon_db_t peer_db;
    }params;
}ble_beacon_c_evt_t;

/**@brief Forward declaration of the @ble_beacon_c_t type.*/
typedef struct ble_beacon_c_s ble_beacon_c_t;
typedef (* ble_beacon_c_evt_handler_t) (ble_beacon_c_t * p_ble_lbs_c, ble_beacon_c_evt_t * p_evt);

struct ble_beacon_c_s
{
    uint16_t                conn_handle;
    beacon_db_t             peer_beacon_db;
    ble_beacon_c_evt_type_t evt_handler;
    ble_srv_error_handler_t error_handler;
    uint8_t                 uuid_type;
    nrf_ble_gq_t            *p_gatt_queue;
};

/**@brief Beacon Client initialization structure. */
typedef struct 
{
    ble_beacon_c_evt_handler_t  evt_handler;
    nrf_ble_gq_t                *p_gatt_queue;
    ble_srv_error_handler_t     error_handler;
}ble_beacon_c_init_t;



void ble_charx_c_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
uint32_t ble_lbs_c_init(ble_lbs_c_t * p_ble_lbs_c, ble_lbs_c_init_t * p_ble_lbs_c_init);


#endif

