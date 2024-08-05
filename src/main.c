#include "btstack_run_loop.h"
#include "btstack_run_loop_embedded.h"
#include "pico/stdlib.h"
#include "btstack.h"

int btstack_main();

int main() {
    stdio_init_all();

    btstack_main();

    btstack_run_loop_execute();
    btstack_run_loop_embedded_execute_once();
}