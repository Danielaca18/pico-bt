#ifndef BT_H
#include "btstack.h"
#include "logger.h"

void register_bt_event_handlers(btstack_packet_handler_t packet_handler);
void init_gap_advertisements(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type, uint8_t adv_data_len, uint8_t *adv_data);
void sm_setup(io_capability_t capabilities, uint8_t auth_req);
void register_hid_bt_handlers(btstack_packet_handler_t callback);
void init_ble_services(const uint8_t* profile, uint8_t battery_level, const uint8_t* hid_descriptor, size_t hid_descriptor_size);
void register_timer(btstack_timer_source_t *ts, uint32_t timeout, char* timer_name, void (handler)(btstack_timer_source_t *ts));
#endif