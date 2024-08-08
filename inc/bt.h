#include "btstack.h"
#include "logger.h"
// #include "ble/gatt-service/device_information_service_server.h"

void register_bt_event_handlers(btstack_packet_handler_t packet_handler);
void init_gap_advertisements(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type, uint8_t adv_data_len, uint8_t *adv_data);