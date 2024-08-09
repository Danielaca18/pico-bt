#include "hog_keypad.h"

#define DEBUG_PRINT
#define TYPING_PERIOD_MS        2

#define DEFAULT_BATTERY_LEVEL   100

#define ADV_INT_MIN             0x0030
#define ADV_INT_MAX             0x0040

static int le_keyboard_setup(void){
    if (bt_hardware_init()) return 1;

    sm_setup(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING);
    init_ble_services(profile_data, DEFAULT_BATTERY_LEVEL, hid_descriptor_keyboard_boot_mode, sizeof(hid_descriptor_keyboard_boot_mode));
    init_gap_advertisements(ADV_INT_MIN, ADV_INT_MAX, 0, sizeof(adv_data), (uint8_t*) adv_data);
    register_hid_bt_handlers(packet_handler);

    logger(INFO, "Keyboard initialized\n");

    return 0;
}

static void send_report(int modifier, int keycode, uint8_t protocol_mode) {
    hci_con_handle_t handle = get_con_handle();
    uint8_t report[] = {  modifier, 0, keycode, 0, 0, 0, 0, 0};

    if (protocol_mode) {
        hids_device_send_input_report(handle, report, sizeof(report));
    } else {
        hids_device_send_boot_keyboard_input_report(handle, report, sizeof(report));
    }
}

static void send_key(int modifier, int keycode){
    send_keycode = keycode;
    send_modifier = modifier;
    hci_con_handle_t handle = get_con_handle();

    hids_device_request_can_send_now_event(handle);
}

static void typing_timer_handler(btstack_timer_source_t * ts){
    if (get_bootsel_button()) {
        send_key(0, F13_KEYCODE);
    } else {
        send_key(0, 0);
    }
    register_timer(ts, TYPING_PERIOD_MS, NULL, NULL);
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    if (packet_type != HCI_EVENT_PACKET) return;

    static btstack_timer_source_t typing_timer;
    uint8_t event_type = hci_event_packet_get_type(packet);

    if (sm_event_handler(packet, event_type) != 1) {
        hci_event_handler(packet, event_type, &typing_timer);
    }
}

static bool hci_event_handler(uint8_t *packet, uint8_t event_type, btstack_timer_source_t *ts) {
    switch (event_type) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            set_con_handle(HCI_CON_HANDLE_INVALID);
            logger(INFO, "Disconnected.\n");
            return true;
        case HCI_EVENT_HIDS_META:
            hids_event_handler(packet, ts);
            return true;
        case HCI_EVENT_LE_META:
            bt_le_event_handler(packet);
            return true;
        default:
            return false;
    }
}

static bool sm_event_handler(uint8_t *packet, uint8_t event_type) {
    switch (event_type) {
        case SM_EVENT_JUST_WORKS_REQUEST:
            logger(INFO, "Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            return true;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
            logger(INFO, "Numeric Comparison Passkey: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
            sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
            return true;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            logger(INFO, "Display Passkey: %"PRIu32"\n",  sm_event_passkey_display_number_get_passkey(packet));
            return true;
        default:
            return false;
    }
}

static void hids_event_handler(uint8_t *packet, btstack_timer_source_t *ts) {
    static uint8_t protocol_mode = 1;

    switch (hci_event_hids_meta_get_subevent_code(packet)){
        case HIDS_SUBEVENT_INPUT_REPORT_ENABLE: {
            hci_con_handle_t handle = hids_subevent_input_report_enable_get_con_handle(packet);
            set_con_handle(handle);
            logger(DEBUG, "Report Characteristic Enabled.\n", hids_subevent_input_report_enable_get_enable(packet));
            register_timer(ts, TYPING_PERIOD_MS, "Poll Inputs", &typing_timer_handler);
            break;
        }
        case HIDS_SUBEVENT_BOOT_KEYBOARD_INPUT_REPORT_ENABLE: {
            hci_con_handle_t handle = hids_subevent_boot_keyboard_input_report_enable_get_con_handle(packet);
            set_con_handle(handle);
            logger(DEBUG, "Boot Characteristic Enabled.\n");
            break;
        }
        case HIDS_SUBEVENT_PROTOCOL_MODE:
            protocol_mode = hids_subevent_protocol_mode_get_protocol_mode(packet);
            logger(DEBUG, "Protocol Mode: %s mode\n", protocol_mode ? "Report" : "Boot");
            break;
        case HIDS_SUBEVENT_CAN_SEND_NOW:
            send_report(send_modifier, send_keycode, protocol_mode);
            break;
        default:
            break;
    }
}

static void bt_le_event_handler(uint8_t *packet) {
    uint8_t status;
    switch (hci_event_le_meta_get_subevent_code(packet)) {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE: {
            status = hci_subevent_le_connection_complete_get_status(packet);
            if (!status) {
                hci_con_handle_t handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                set_con_handle(handle);
                logger(INFO, "LE Device connected.\n");
            } else {
                logger(INFO, "LE connection failed.\n");
            }
            break;
        }
    }
}

int btstack_main(void)
{
    if (le_keyboard_setup()) return 1;
    hci_power_control(HCI_POWER_ON);
    return 0;
}