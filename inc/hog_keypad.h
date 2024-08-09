#ifndef HOG_KEYPAD_H
#define HOG_KEYPAD_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define BTSTACK_FILE__ "hog_keypad.c"

#include "btstack.h"
#include "ble/gatt-service/battery_service_server.h"
#include "ble/gatt-service/device_information_service_server.h"
#include "ble/gatt-service/hids_device.h"

#include "hog_keyboard.h"
#include "hid_config.h"
#include "bt.h"
#include "bootsel.h"
#include "logger.h"

#define F13_KEYCODE      104

static int send_keycode;
static int send_modifier;

static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;


/**
 * @brief   Packet Handler
 * @details Handles all packets/events.
 * 
 * @param   packet_type 
 * @param   channel 
 * @param   packet 
 * @param   size 
 */
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

/**
 * @brief   HID Event Handler
 * @details Handles all HID events.
 * 
 * @param   packet 
 * @param   ts 
 */
static void hids_event_handler(uint8_t *packet, btstack_timer_source_t *ts);

/**
 * @brief   BLE Event Handler
 * @details Handles all BT LE events.
 * 
 * @param packet 
 */
static void bt_le_event_handler(uint8_t *packet);

/**
 * @brief   HCI Event Handler
 * @details Handles all HCI events.
 * 
 * @param packet 
 * @param event_type 
 * @param ts 
 * @return true 
 * @return false 
 */
static bool hci_event_handler(uint8_t *packet, uint8_t event_type, btstack_timer_source_t *ts);

/**
 * @brief   SM Event Handler
 * @details Handles all security events.
 * 
 * @param packet 
 * @param event_type 
 * @return true 
 * @return false 
 */
static bool sm_event_handler(uint8_t *packet, uint8_t event_type);

/**
 * @brief Sets Connection Handle
 * 
 * @param new_handle New connection handler.
 */
static inline void set_con_handle(hci_con_handle_t new_handle) {
    con_handle = new_handle;
}

/**
 * @brief Gets Connection Handle
 * 
 * @return hci_con_handle_t Connection handler.
 */
static inline hci_con_handle_t get_con_handle() {
    return con_handle;
}

#endif
