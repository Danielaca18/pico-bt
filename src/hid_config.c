#include "hid_config.h"

const uint8_t hid_descriptor_keyboard_boot_mode[] = {
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x06,                     // Usage (Keyboard)
    0xa1, 0x01,                     // Collection (Application)

    // Report ID

    0x85, 0x01,                     // Report ID

    // Modifier byte (input)

    0x75, 0x01,                     //   Report Size (1)
    0x95, 0x08,                     //   Report Count (8)
    0x05, 0x07,                     //   Usage Page (Key codes)
    0x19, 0xe0,                     //   Usage Minimum (Keyboard LeftControl)
    0x29, 0xe7,                     //   Usage Maximum (Keyboard Right GUI)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x25, 0x01,                     //   Logical Maximum (1)
    0x81, 0x02,                     //   Input (Data, Variable, Absolute)

    // Reserved byte (input)

    0x75, 0x01,                     //   Report Size (1)
    0x95, 0x08,                     //   Report Count (8)
    0x81, 0x03,                     //   Input (Constant, Variable, Absolute)

    // LED report + padding (output)

    0x95, 0x05,                     //   Report Count (5)
    0x75, 0x01,                     //   Report Size (1)
    0x05, 0x08,                     //   Usage Page (LEDs)
    0x19, 0x01,                     //   Usage Minimum (Num Lock)
    0x29, 0x05,                     //   Usage Maximum (Kana)
    0x91, 0x02,                     //   Output (Data, Variable, Absolute)

    0x95, 0x01,                     //   Report Count (1)
    0x75, 0x03,                     //   Report Size (3)
    0x91, 0x03,                     //   Output (Constant, Variable, Absolute)

    // Keycodes (input)

    0x95, 0x06,                     //   Report Count (6)
    0x75, 0x08,                     //   Report Size (8)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x25, 0xff,                     //   Logical Maximum (1)
    0x05, 0x07,                     //   Usage Page (Key codes)
    0x19, 0x00,                     //   Usage Minimum (Reserved (no event indicated))
    0x29, 0xff,                     //   Usage Maximum (Reserved)
    0x81, 0x00,                     //   Input (Data, Array)

    0xc0,                           // End collection
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

const uint8_t hid_descriptor_len = sizeof(hid_descriptor_keyboard_boot_mode);
const uint8_t adv_data_len = sizeof(adv_data);