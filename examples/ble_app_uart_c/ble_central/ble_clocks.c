#include "ble_clocks.h"


#define COMPARE_COUNTERTIME  (1.25f)                                        /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */

uint8_t GYRO_TIMEOUT_FLAG = 0;
uint8_t BTN_TIMEOUT_FLAG = 0;
uint8_t LPOWER_TIMEOUT_FLAG = 0;
uint8_t HPOWER_TIMEOUT_FLAG = 0;

static uint64_t m_base_counter;
static uint64_t m_stamp64;
static drv_rtc_t m_rtc_inst = DRV_RTC_INSTANCE(1);

//const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
nrfx_wdt_channel_id m_channel_id;

//void clock_initialization()
//{
//    /* Start 16 MHz crystal oscillator */
//    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
//    NRF_CLOCK->TASKS_HFCLKSTART    = 1;

//    /* Wait for the external oscillator to start up */
//    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
//    {
//        // Do nothing.
//    }

//    /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
//    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
//    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
//    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

//    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
//    {
//        // Do nothing.
//    }
//}

void ble_hfclk_init( void )
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void ble_hfclk_deinit( void ){
    NRF_CLOCK->TASKS_HFCLKSTOP =1;   
}

/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
void ble_lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

///** @brief Function initialization and configuration of RTC driver instance.
// */
//void ble_rtc_config(void)
//{
//    uint32_t err_code;

//    //Initialize RTC instance
//    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
//    config.prescaler = 327;//or 40
//    err_code = nrf_drv_rtc_init(&rtc, &config, ble_rtc_handler);
//    APP_ERROR_CHECK(err_code);

//    //Enable tick event & interrupt
//    nrf_drv_rtc_tick_enable(&rtc,false);

//    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
//    err_code = nrf_drv_rtc_cc_set(&rtc,0,33,true); //1.25ms COMPARE_COUNTERTIME
//    APP_ERROR_CHECK(err_code);
//		
//    //Power on RTC instance
//    nrf_drv_rtc_enable(&rtc);
//}

void ble_wdt_init()
{
	nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    nrf_drv_wdt_init(&config, ble_wdt_handler);
    nrf_drv_wdt_channel_alloc(&m_channel_id);
    nrf_drv_wdt_enable();
}

/**
 * @brief WDT events handler.
 */
void ble_wdt_handler(void)
{
	
}


/**@brief Function for starting application timers.
 */
APP_TIMER_DEF(radio_timer_id);
APP_TIMER_DEF(gyro_timer_id);
APP_TIMER_DEF(btn_timer_id);
APP_TIMER_DEF(lpower_timer_id);

static void radio_timeout_handler(void * p_context)
{

}
void application_timers_start(uint32_t advInterval)
{
    uint32_t err_code;

    // Create timers.
    err_code = app_timer_create(&radio_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                radio_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start timers.
    err_code = app_timer_start(radio_timer_id, APP_TIMER_TICKS(advInterval), NULL);
    APP_ERROR_CHECK(err_code);
}
void application_timers_stop(){
		uint32_t err_code;
	
		err_code = app_timer_stop(radio_timer_id);
		APP_ERROR_CHECK(err_code);
}
/**@brief Function for starting gyro timers.
 */
static void gyro_timeout_handler(void * p_context)
{
		GYRO_TIMEOUT_FLAG = 1;
//		HPOWER_TIMEOUT_FLAG = 1;
}
void gyro_timers_start(uint32_t advInterval)
{
    uint32_t err_code;

    // Create timers.
    err_code = app_timer_create(&gyro_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                gyro_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start timers.
    err_code = app_timer_start(gyro_timer_id, APP_TIMER_TICKS(advInterval), NULL);
    APP_ERROR_CHECK(err_code);
}
void gyro_timers_stop(){
		uint32_t err_code;
	
		err_code = app_timer_stop(gyro_timer_id);
		APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting btn timers.
 */
static void btn_timeout_handler(void * p_context)
{
		BTN_TIMEOUT_FLAG = 1;
//		HPOWER_TIMEOUT_FLAG = 1;
}
void btn_timers_start(uint32_t advInterval)
{
    uint32_t err_code;

    // Create timers.
    err_code = app_timer_create(&btn_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                btn_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start timers.
    err_code = app_timer_start(btn_timer_id, APP_TIMER_TICKS(advInterval), NULL);
    APP_ERROR_CHECK(err_code);
}
void btn_timers_stop(){
		uint32_t err_code;

		err_code = app_timer_stop(btn_timer_id);
		APP_ERROR_CHECK(err_code);
}
/**@brief Function for starting lpower timers.
 */
static void lpower_timeout_handler(void * p_context)
{
		LPOWER_TIMEOUT_FLAG = 1;
}
void lpower_timers_start(uint32_t advInterval)
{
    uint32_t err_code;

    // Create timers.
    err_code = app_timer_create(&lpower_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                lpower_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start timers.
    err_code = app_timer_start(lpower_timer_id, APP_TIMER_TICKS(advInterval), NULL);
    APP_ERROR_CHECK(err_code);
}
void lpower_timers_stop(){
		uint32_t err_code;

		err_code = app_timer_stop(lpower_timer_id);
		APP_ERROR_CHECK(err_code);
}

/**
 * @brief Return current 64 bit timestamp
 */
uint64_t get_timestamps(void)
{
    uint64_t now = m_base_counter + drv_rtc_counter_get(&m_rtc_inst);

    /* it is possible that base was not updated and overflow occured, in that case 'now' will be
     * 24bit value behind. Additional timestamp updated on every 24 bit period is used to detect
     * that case. Apart from that 'now' should never be behind previously read timestamp.
     */
    if (now < m_stamp64) {
        now += (DRV_RTC_MAX_CNT + 1);
    }

    return now;
}
