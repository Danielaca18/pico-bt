#include "btstack_run_loop.h"
#include "pico/stdlib.h"

int btstack_main();

int main() {
    stdio_init_all();

    btstack_main();

    btstack_run_loop_execute();
}