#ifndef HOG_KEYPAD_H
#define HOG_KEYPAD_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define BTSTACK_FILE__ "hog_keypad.c"

#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "ble/gatt-service/battery_service_server.h"
#include "ble/gatt-service/device_information_service_server.h"
#include "ble/gatt-service/hids_device.h"

#include "hog_keyboard.h"
#include "bt.h"
#include "bootsel.h"
#include "logger.h"

#ifndef DEMO_TEXT
#define DEMO_TEXT "\n\nHello World!\nThis is the Pico HOG\n\n"
#endif

#define F13_KEYCODE      104

//static btstack_timer_source_t typing_timer;
static int send_keycode;
static int send_modifier;

static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;
static uint8_t battery = 100;
static uint8_t protocol_mode = 1;

// from USB HID Specification 1.1, Appendix B.1
const uint8_t hid_descriptor_keyboard_boot_mode[] = {
    0x05, 0x01,                    // Usage Page (Generic Desktop)
    0x09, 0x06,                    // Usage (Keyboard)
    0xa1, 0x01,                    // Collection (Application)
    0x85,  0x01,                   // Report ID 1

    // Modifier byte

    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x08,                    //   Report Count (8)
    0x05, 0x07,                    //   Usage Page (Key codes)
    0x19, 0xe0,                    //   Usage Minimum (Keyboard LeftControl)
    0x29, 0xe7,                    //   Usage Maxium (Keyboard Right GUI)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0x01,                    //   Logical Maximum (1)
    0x81, 0x02,                    //   Input (Data, Variable, Absolute)

    // Reserved byte

    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x08,                    //   Report Count (8)
    0x81, 0x03,                    //   Input (Constant, Variable, Absolute)

    // LED report + padding

    0x95, 0x05,                    //   Report Count (5)
    0x75, 0x01,                    //   Report Size (1)
    0x05, 0x08,                    //   Usage Page (LEDs)
    0x19, 0x01,                    //   Usage Minimum (Num Lock)
    0x29, 0x05,                    //   Usage Maxium (Kana)
    0x91, 0x02,                    //   Output (Data, Variable, Absolute)

    0x95, 0x01,                    //   Report Count (1)
    0x75, 0x03,                    //   Report Size (3)
    0x91, 0x03,                    //   Output (Constant, Variable, Absolute)

    // Keycodes

    0x95, 0x06,                    //   Report Count (6)
    0x75, 0x08,                    //   Report Size (8)
    //0x75, 0x10,                    //   Report Size (16)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0xff,                    //   Logical Maximum (1)
    0x05, 0x07,                    //   Usage Page (Key codes)
    0x19, 0x00,                    //   Usage Minimum (Reserved (no event indicated))
    0x29, 0xff,                    //   Usage Maxium (Reserved)
    0x81, 0x00,                    //   Input (Data, Array)

    0xc0,                          // End collection
};

const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x09, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', ' ', 'H', 'O', 'G',
    // 16-bit Service UUIDs
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE & 0xff, ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE >> 8,
    // Appearance HID - Keyboard (Category 15, Sub-Category 1)
    0x03, BLUETOOTH_DATA_TYPE_APPEARANCE, 0xC1, 0x03,
};

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
//static void init_ble_services(const uint8_t* profile_data, uint8_t battery_level, const uint8_t* hid_descriptor, size_t hid_descriptor_size);
static void hids_event_handler(uint8_t *packet, btstack_timer_source_t *ts);
static void bt_le_event_handler(uint8_t *packet);
//static void register_hid_bt_handlers(btstack_packet_handler_t callback);
// static void register_hid_timer(btstack_timer_source_t *ts, uint32_t timeout, void (handler)(btstack_timer_source_t *ts));
// static void register_timer(btstack_timer_source_t *ts, uint32_t timeout, char* timer_name, void (handler)(btstack_timer_source_t *ts));
static bool hci_event_handler(uint8_t *packet, uint8_t event_type, btstack_timer_source_t *ts);
static bool sm_event_handler(uint8_t *packet, uint8_t event_type);
#endif