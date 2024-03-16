#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    while (1) {
        adc_select_input(0);
        uint adc_x_raw = adc_read();
        adc_select_input(1);
        uint adc_y_raw = adc_read();


        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        printf("\rX: [");
        for (int i = 0; i < bar_width; ++i)
            putchar(i == bar_x_pos ? 'o' : ' ');
        printf("]  Y: [");
        for (int i = 0; i < bar_width; ++i)
            putchar(i == bar_y_pos ? 'o' : ' ');
        printf("]");
        sleep_ms(50);
    }
}