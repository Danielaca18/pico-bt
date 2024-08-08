#include "bt.h"

void register_bt_event_handlers(btstack_packet_handler_t packet_handler) {
    // define event handler structs for sm and hci layers
    static btstack_packet_callback_registration_t hci_event_callback;
    static btstack_packet_callback_registration_t sm_event_callback;

    // init event handler start addresses
    hci_event_callback.callback = packet_handler;
    sm_event_callback.callback = packet_handler;

    // register event handlers with btstack
    hci_add_event_handler(&hci_event_callback);
    sm_add_event_handler(&sm_event_callback);
}

void init_gap_advertisements(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type, uint8_t adv_data_len, uint8_t *adv_data) {
    // Init null bt address
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);

    // Configure gap advertisement
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, adv_data);

    // Enable gap advertisements
    gap_advertisements_enable(1);

    logger(DEBUG, "Initialized gap advertisements.\n");
}