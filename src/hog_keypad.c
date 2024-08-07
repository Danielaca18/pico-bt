#include "hog_keypad.h"

#define DEBUG_PRINT
#define TYPING_PERIOD_MS 2

const uint8_t adv_data_len = sizeof(adv_data);

static int le_keyboard_setup(void){
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return 1;
    }
    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Initialized bt and wifi module.\n");
    #endif

    //btstack_memory_init();
    //btstack_run_loop_embedded_get_instance()->init();
    
    l2cap_init();
    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Initialized btstack run loop.\n");
    #endif

    // setup SM: No IO
    sm_init();

    #ifndef FIXED_PASSKEY
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING);
    #else // Allows bluetooth connection to be secured by a fixed passkey
    sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_ONLY);
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    sm_use_fixed_passkey_in_display_role(FIXED_PASSKEY);
    #endif
    
    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Initialized Security Manager.\n");
    #endif

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);

    // setup battery service
    battery_service_server_init(battery);

    // setup device information service
    device_information_service_server_init();
    
    // setup HID Device service
    hids_device_init(0, hid_descriptor_keyboard_boot_mode, sizeof(hid_descriptor_keyboard_boot_mode));

    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Finished setting up device services.\n");
    #endif

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0040;

    // init and enable GAP ads
    init_gap_advertisements(adv_int_min, adv_int_max, 0, adv_data_len, (uint8_t*) adv_data);

    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Initialized gap advertisements.\n");
    #endif

    // register packet handler with btstack
    register_bt_event_handlers(packet_handler);

    // register for HIDS
    hids_device_register_packet_handler(packet_handler);

    #ifdef DEBUG_PRINT
    printf("[Log] Debug: Registered event handlers\n");
    #endif

    return 0;
}

// HID Report sending
static void send_report(int modifier, int keycode){
    uint8_t report[] = {  modifier, 0, keycode, 0, 0, 0, 0, 0};
    switch (protocol_mode){
        case 0:
            printf("Sending boot report: %d\n", keycode);
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
        //printf("Keydown detected\n");
        send_key(0, F13_KEYCODE);
    } else {
        send_key(0, 0);
    }

    // set next timer
    btstack_run_loop_base_remove_timer(ts);
    btstack_run_loop_set_timer(ts, TYPING_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}

static void hid_embedded_start_typing(void){
    printf("Start typing..\n");

    // set one-shot timer
    typing_timer.process = &typing_timer_handler;
    btstack_run_loop_base_remove_timer(&typing_timer);
    btstack_run_loop_set_timer(&typing_timer, TYPING_PERIOD_MS);
    btstack_run_loop_add_timer(&typing_timer);
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            printf("Disconnected\n");
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            printf("Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
            printf("Confirming numeric comparison: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
            sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
            break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            printf("Display Passkey: %"PRIu32"\n", sm_event_passkey_display_number_get_passkey(packet));
            break;
        case HCI_EVENT_HIDS_META:
            switch (hci_event_hids_meta_get_subevent_code(packet)){
                case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_input_report_enable_get_con_handle(packet);
                    printf("Report Characteristic Subscribed %u\n", hids_subevent_input_report_enable_get_enable(packet));
#ifndef HAVE_BTSTACK_STDIN
                    hid_embedded_start_typing();
#endif
                    break;
                case HIDS_SUBEVENT_BOOT_KEYBOARD_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_boot_keyboard_input_report_enable_get_con_handle(packet);
                    printf("Boot Keyboard Characteristic Subscribed %u\n", hids_subevent_boot_keyboard_input_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_PROTOCOL_MODE:
                    protocol_mode = hids_subevent_protocol_mode_get_protocol_mode(packet);
                    printf("Protocol Mode: %s mode\n", hids_subevent_protocol_mode_get_protocol_mode(packet) ? "Report" : "Boot");
                    break;
                case HIDS_SUBEVENT_CAN_SEND_NOW:
                    //printf("Can Send Now\n");
                    typing_can_send_now();
                    break;
                default:
                    break;
            }
            break;
        
        default:
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