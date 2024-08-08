#include "hog_keypad.h"

#define DEBUG_PRINT
#define TYPING_PERIOD_MS 2

#define ADV_INT_MIN         0x0030
#define ADV_INT_MAX         0x0040

const uint8_t adv_data_len = sizeof(adv_data);

static int le_keyboard_setup(void){
    if (cyw43_arch_init()) {
        logger(ERROR, "Failed to initialize cyw43_arch\n");
        return 1;
    }
    l2cap_init();
    logger(DEBUG, "Initialized BT module.\n");

    sm_setup(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING);

    init_ble_services(profile_data, battery, hid_descriptor_keyboard_boot_mode, sizeof(hid_descriptor_keyboard_boot_mode));
    init_gap_advertisements(ADV_INT_MIN, ADV_INT_MAX, 0, adv_data_len, (uint8_t*) adv_data);
    register_hid_bt_handlers(packet_handler);

    logger(INFO, "Keyboard initialized\n");

    return 0;
}

static void sm_setup(io_capability_t capabilities, uint8_t auth_req) {
    sm_init();
    sm_set_io_capabilities(capabilities);
    sm_set_authentication_requirements(auth_req);
    logger(DEBUG, "Initialized Security Manager.\n");
}

static void register_hid_bt_handlers(btstack_packet_handler_t callback) {
    register_bt_event_handlers(packet_handler);
    hids_device_register_packet_handler(packet_handler);
    logger(DEBUG, "Registered HID and BT event handlers.\n");
}

static void init_ble_services(const uint8_t* profile, uint8_t battery_level, const uint8_t* hid_descriptor, size_t hid_descriptor_size) {
    // setup ATT server
    att_server_init(profile, NULL, NULL);
    battery_service_server_init(battery_level);
    device_information_service_server_init();
    hids_device_init(0, hid_descriptor, hid_descriptor_size);
    logger(DEBUG, "Finished setting up device services.\n");
}

// HID Report sending
static void send_report(int modifier, int keycode){
    uint8_t report[] = {  modifier, 0, keycode, 0, 0, 0, 0, 0};
    switch (protocol_mode){
        case 0:
            hids_device_send_boot_keyboard_input_report(con_handle, report, sizeof(report));
            break;
        case 1:
           hids_device_send_input_report(con_handle, report, sizeof(report));
           break;
        default:
            break;
    }
}


static void send_key(int modifier, int keycode){
    send_keycode = keycode;
    send_modifier = modifier;
    hids_device_request_can_send_now_event(con_handle);
}

static void typing_can_send_now(void){
   send_report(send_modifier, send_keycode);
}

static void typing_timer_handler(btstack_timer_source_t * ts){
    if (get_bootsel_button()) {
        send_key(0, F13_KEYCODE);
    } else {
        send_key(0, 0);
    }

    // set next timer
    btstack_run_loop_base_remove_timer(ts);
    btstack_run_loop_set_timer(ts, TYPING_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}

static void register_hid_timer(void){
    // set one-shot timer
    typing_timer.process = &typing_timer_handler;
    btstack_run_loop_base_remove_timer(&typing_timer);
    btstack_run_loop_set_timer(&typing_timer, TYPING_PERIOD_MS);
    btstack_run_loop_add_timer(&typing_timer);
    logger(DEBUG, "Registered HID timer.\n");
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    uint8_t event_type;
    event_type = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;
    switch (event_type) {
    //switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            logger(INFO, "Disconnected.\n");
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            logger(INFO, "Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
            logger(INFO, "Numeric Comparison Passkey: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
            sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
            break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            logger(INFO, "Display Passkey: %"PRIu32"\n",  sm_event_passkey_display_number_get_passkey(packet));
            break;
        case HCI_EVENT_HIDS_META:
            hids_event_handler(packet);
            break;
        case HCI_EVENT_LE_META:
            bt_le_event_handler(packet);
            break;
        default:
            break;
    }
}

static void hids_event_handler(uint8_t *packet) {
    switch (hci_event_hids_meta_get_subevent_code(packet)){
        case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
            con_handle = hids_subevent_input_report_enable_get_con_handle(packet);
            logger(DEBUG, "Input Report Characteristic Enabled.\n", hids_subevent_input_report_enable_get_enable(packet));
            register_hid_timer();
            break;
        case HIDS_SUBEVENT_BOOT_KEYBOARD_INPUT_REPORT_ENABLE:
            con_handle = hids_subevent_boot_keyboard_input_report_enable_get_con_handle(packet);
            logger(DEBUG, "Boot Keyboard Characteristic Enabled.\n", hids_subevent_boot_keyboard_input_report_enable_get_enable(packet));
            break;
        case HIDS_SUBEVENT_PROTOCOL_MODE:
            protocol_mode = hids_subevent_protocol_mode_get_protocol_mode(packet);
            logger(DEBUG, "Protocol Mode: %s mode\n", protocol_mode ? "Report" : "Boot");
            break;
        case HIDS_SUBEVENT_CAN_SEND_NOW:
            typing_can_send_now();
            break;
        default:
            break;
    }
}

static void bt_le_event_handler(uint8_t *packet) {
    static uint8_t status;
    switch (hci_event_le_meta_get_subevent_code(packet)) {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            status = hci_subevent_le_connection_complete_get_status(packet);
            if (!status) {
                // hci_subevent_le_connection_complete_get_peer_address(packet, &addr);
                con_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                logger(INFO, "LE Device connected.\n");
            } else {
                logger(INFO, "LE connection failed.\n");
            }
            break;
    }
}

int btstack_main(void)
{
    if (le_keyboard_setup()) return 1;

    // turn on!
    hci_power_control(HCI_POWER_ON);

    return 0;
}