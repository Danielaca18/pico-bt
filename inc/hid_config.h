#ifndef HID_CONFIG_H
#define HID_CONFIG_H

#include <stdint.h>
#include "btstack.h"



#define ADV_INT_MIN             0x0030
#define ADV_INT_MAX             0x0040
#define TYPING_PERIOD_MS        2
#define DEFAULT_BATTERY_LEVEL   100
#define REPORT_BYTES            16
#define DEBUG_PRINT

/**
 * @brief HID Descriptor
 * @details from USB HID Specification 1.1, Appendix B.1
 * 
 */
extern const uint8_t hid_descriptor_keyboard_boot_mode[];

/**
 * @brief   Advertisement Data
 * @details Specifies device attributes and types.
 */
extern const uint8_t adv_data[];

extern const uint8_t hid_descriptor_len;;
extern const uint8_t adv_data_len;


#endif