/**
 * @file bt.h
 * @author Daniel Castro (dcastro@ualberta.ca)
 * @brief 
 * @version 0.1
 * @date 2024-08-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef BT_H
#define BT_H

#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "logger.h"

/**
 * @brief   Register BT Event Handler
 * @details Registers event handler with hci and sm layers.
 * 
 * @param packet_handler Handler function to be registered.
 */
void register_bt_event_handlers(btstack_packet_handler_t packet_handler);

/**
 * @brief   Initializes GAP Ads
 * @details Initializes, configures and enables GAP advertisements.
 * 
 * @param adv_int_min   Minimum values of advertising interval.
 * @param adv_int_max   Maximum values of advertising interval.
 * @param adv_type      Advertisement type.
 * @param adv_data_len  Advertisement data size.
 * @param adv_data      Advertisement data.
 */
void init_gap_advertisements(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type, uint8_t adv_data_len, uint8_t *adv_data);

/**
 * @brief   Setup Security Manager
 * @details Initializes and configures security manager.
 * 
 * @param capabilities  Device capabilities.
 * @param auth_req      Authorization requirements.
 */
void sm_setup(io_capability_t capabilities, uint8_t auth_req);

/**
 * @brief   Registers BT and HID Handler
 * @details Registers callback function with both both HID and BT layers.
 * 
 * @param callback  Callback function to be registered.
 */
void register_hid_bt_handlers(btstack_packet_handler_t callback);

/**
 * @brief   Initializes BLE Services
 * @details Initializes ATT server and related services.
 * 
 * @param profile               ATT profile.         
 * @param battery_level         Battery level.
 * @param hid_descriptor        HID descriptor.
 * @param hid_descriptor_size   HID descriptor size.
 */
void init_ble_services(const uint8_t* profile, uint8_t battery_level, const uint8_t* hid_descriptor, size_t hid_descriptor_size);

/**
 * @brief   Registers Timer
 * @details Adds timer source to bt run loop.
 * 
 * @param ts            Timer source.   
 * @param timeout       Timer timeout value.
 * @param timer_name    Timer name.
 * @param handler       Handler function.
 */
void register_timer(btstack_timer_source_t *ts, uint32_t timeout, char* timer_name, void (handler)(btstack_timer_source_t *ts));

/**
 * 
 * @brief   Initialize BT Hardware
 * @details Initializes CYW43 Arch and L2CAP layer.
 * 
 * @return true if initialization successful.
 * @return false if initialization failed.
 */
bool bt_hardware_init();

#endif