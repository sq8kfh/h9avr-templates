/*
 * h9??
 * ??
 *
 * Created by SQ8KFH on 2017-??-??.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include "can/can.h"

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PB1


int main(void) {
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;
    DDRE = 0xff;

    CAN_init();
    sei();

    _delay_ms(100);
    CAN_send_turned_on_broadcast();

    uint32_t led_counter = 0x1000;

    while (1) {
        if (led_counter == 0) {
            LED_PORT ^= (1<<LED_PIN);
            if (LED_PORT & (1<<LED_PIN)) {
                led_counter = 0x1000;
            }
            else {
                led_counter = 0x80000;
            }
        }
        --led_counter;

        h9msg_t cm;
        if (CAN_get_msg(&cm)) {
            LED_PORT |= (1<<LED_PIN);
            led_counter = 0x10000;
            if (cm.type == H9MSG_TYPE_GET_REG) {
                h9msg_t cm_res;
                CAN_init_response_msg(&cm, &cm_res);
                cm_res.dlc = 2;
                cm_res.data[0] = cm.data[0];

                uint32_t tmp;
                uint16_t adc;
                switch (cm.data[0]) {
                    case 10:
                        cm_res.data[1] = output;
                        CAN_put_msg(&cm_res);
                        break;
                }
            }
            else if (cm.type == H9MSG_TYPE_SET_REG) {
                h9msg_t cm_res;
                CAN_init_response_msg(&cm, &cm_res);
                cm_res.dlc = 0;
                cm_res.data[0] = cm.data[0];
                switch (cm.data[0]) {
                    case 10:
                        cm_res.data[1] = output;
                        cm_res.dlc = 2;
                        CAN_put_msg(&cm_res);
                        break;
                }
            }
        }
    }
}
