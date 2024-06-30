#include <stdio.h>
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "pico/stdlib.h"

static btstack_packet_callback_registration_t hci_event_callback_registration;

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) return -1;

    l2cap_init();
    sm_init();
};