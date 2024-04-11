#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "pico/stdlib.h"
#include <stdio.h>


#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen) {
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize) {
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if (bufsize < 1)
                return;
        }
    }
}


static void send_hid_report(uint8_t report_id, uint32_t btn) {
    if (!tud_hid_ready())
        return;

    switch (report_id) {
        case REPORT_ID_GAMEPAD: {
            hid_gamepad_report_t report = {.x = 0,
                                           .y = 0,
                                           .z = 0,
                                           .rz = 0,
                                           .rx = 0,
                                           .ry = 0,
                                           .hat = 0,
                                           .buttons = 0};
            adc_select_input(0);
            uint adc_y_raw = adc_read();
            adc_select_input(1);
            uint adc_x_raw = adc_read();

            if (adc_y_raw < 1024) {
                if (adc_x_raw < 1536) {
                    report.buttons = GAMEPAD_BUTTON_1;
                } else if (adc_x_raw > 1536 && adc_x_raw < 2560) {
                    report.buttons = GAMEPAD_BUTTON_3;
                } else {
                    if (gpio_get(7)) { // if for reverse gear
                        report.buttons = GAMEPAD_BUTTON_6;
                    } else {
                        report.buttons = GAMEPAD_BUTTON_5;
                    }
                }
            } else if (adc_y_raw > 2560) {
                if (adc_x_raw < 1536) {
                    report.buttons = GAMEPAD_BUTTON_0;
                } else if (adc_x_raw > 1536 && adc_x_raw < 2560) {
                    report.buttons = GAMEPAD_BUTTON_2;
                } else {
                    report.buttons = GAMEPAD_BUTTON_4;
                }
            }

            tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

        } break;

        default:
            break;
    }
}

void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms)
        return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by
        // tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_GAMEPAD, btn);
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    // gpio_pull_down(7);
    board_init();
    tusb_init();


    while (1) {
        tud_task();
        hid_task();
    }
}