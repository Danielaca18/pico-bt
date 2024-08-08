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

void sm_setup(io_capability_t capabilities, uint8_t auth_req) {
    sm_init();
    sm_set_io_capabilities(capabilities);
    sm_set_authentication_requirements(auth_req);
    logger(DEBUG, "Initialized Security Manager.\n");
}

void register_hid_bt_handlers(btstack_packet_handler_t callback) {
    register_bt_event_handlers(callback);
    hids_device_register_packet_handler(callback);
    logger(DEBUG, "Registered HID and BT event handlers.\n");
}

void init_ble_services(const uint8_t* profile, uint8_t battery_level, const uint8_t* hid_descriptor, size_t hid_descriptor_size) {
    // setup ATT server
    att_server_init(profile, NULL, NULL);
    battery_service_server_init(battery_level);
    device_information_service_server_init();
    hids_device_init(0, hid_descriptor, hid_descriptor_size);
    logger(DEBUG, "Finished setting up device services.\n");
}

void register_timer(btstack_timer_source_t *ts, uint32_t timeout, char* timer_name, void (handler)(btstack_timer_source_t *ts)) {
    if (handler != NULL) ts->process = handler;
    btstack_run_loop_base_remove_timer(ts);
    btstack_run_loop_set_timer(ts, timeout);
    btstack_run_loop_add_timer(ts);
    if (timer_name) logger(DEBUG, "Registered timer: %s\n", timer_name);
}