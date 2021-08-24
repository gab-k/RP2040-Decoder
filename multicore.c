/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pwm.pio.h"
#include "automat.h"
#include "CV.h"




bool readBit()
{
    busy_wait_us_32(87);
    return !gpio_get(17);
}

#define SIZE 128
bool bitarray[SIZE] = {false};
int index_neu = 0;
int index_alt;
void writeBit(bool bit){
    index_alt = (index_neu+SIZE-1)%SIZE;
    bitarray[index_alt] =  bit;
}

// void printer(char buf[100]){
//     while (*buf) {
//         multicore_fifo_push_blocking(*buf);
//         buf++;
//     }
// }
// uint32_t errorcount = 0;

// #define FLAG_VALUE 123

// void pretty_print(char* s){
//     char addresse[9];
//     s+=11;
//     for (int i = 0; i < 8 ; i++) {
//         addresse[i] = *(s+i);
//     }
//     addresse[8] =0;
//     s+=9;

//     char befehl[9];
//     for (int i = 0; i < 8 ; i++) {
//         befehl[i] = *(s+i);
//     }
//     befehl[8] =0;
//     s+=9;
//     char pruef[9];
//     for (int i = 0; i < 8 ; i++) {
//         pruef[i] = *(s+i);
//     }
//     s+=8;
//     pruef[8] =0;
//     static int32_t counter = 1;
//     printf("%d address: %s befehl: %s pruef: %s %c\n", counter++, addresse, befehl, pruef, *s);
// }

// void core1_entry() {
//     multicore_fifo_push_blocking(FLAG_VALUE);

//     int index = 0;
//     char buf[100] = {0};
//     while (1) {
//         buf[index] = multicore_fifo_pop_blocking();
//         if (buf[index] == '\n' || index == 99) {
//             pretty_print(buf);
//             for(int i =0 ; i < 100; i++) {
//                 buf[i] = 0;
//             }
//             index = -1;
//         }
//         index++;
//     }
// }

// const int SIZE = 10 + 1  + 3 * 9 + 1 + 1;

// #define LEFTESTBIT 0x8000000000000000
// char* genCharArray(uint64_t last_read_bits, uint64_t mask);

void gpio_callback_rise(uint gpio, uint32_t events) {
    writeBit(readBit());
    bool * s = bitarray;
    int FSM_RESULT = fsm_main(s, index_neu, SIZE );
    if( -1 != FSM_RESULT){
        /*printf("Index: %d\n", index_neu);
        for (int i = 0; i < SIZE; i++)
        {
            printf("Bit %d: %d \n",i,bitarray[i]);
        }*/
        for (int i = 0; i < FSM_RESULT; i++)
        {
            printf("\nletzter-%d BYTE: ",i);
            for (int j = 0; j < 8; j++)
            {
            printf("%d",bitarray[(index_neu-j+8+(9*i))%SIZE]);
            }    
        }
        printf("\nPaketgroesse: %d\n",FSM_RESULT);
        printf("\n");
    }
    index_neu = index_alt;
}
    // static uint64_t last_read_Bits;
    // static uint64_t mask = LEFTESTBIT;
    // bool last_read_bit = readBit();

    // if (last_read_bit) {
    //     last_read_Bits = last_read_Bits | mask;
    // } else {
    //     last_read_Bits = last_read_Bits & ~mask;
    // }
    // if ( mask == 1) {
    //     mask = LEFTESTBIT;
    // } else {
    //     mask >>= 1;
    // }
    // char * s = genCharArray(last_read_Bits, mask);
    // if (fsm_main(bitarray) == 1) {
    //     s[SIZE-2 ] ='\n';
    //     s[SIZE-1 ] =0;
    //     printer(s);
    // }
    // free(s);


// char* genCharArray(uint64_t last_read_bits, uint64_t mask) {
//     char *s = malloc(SIZE);
//     for ( int i = SIZE-3; i >= 0 ; i-- ) {
//         if ( mask == LEFTESTBIT ){
//             mask = 1;
//         } else {
//             mask <<= 1;
//         }
//         s[i] = last_read_bits & mask ? '1': '0';
//     }
//     s[SIZE-2 ] =0;
//     return s;
// }

 int main() {
    stdio_init_all();
    // multicore_launch_core1(core1_entry);
    // Wait for it to start up
    // uint32_t g = multicore_fifo_pop_blocking();

    // if (g != FLAG_VALUE)
    //     printf("Hmm, that's not right on core 0!\n");
    // else {
    //     printf("It's all gone well on core 0!");
    // }

    gpio_init(17);
    gpio_set_dir(17, GPIO_IN);
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);

/*
    //preamble
    for (int i = 0; i < 14; i++) writeBit(1);
    
    //trenn bit
    writeBit(0);
    
    //byte 0
    for (int i = 0; i < 8; i++) writeBit(1);   
    
    //trenn bit
    writeBit(0);

    //byte 1
    for (int i = 0; i < 8; i++) writeBit(0);

    //trenn bit
    writeBit(0);
    
    //byte 2
    for (int i = 0; i < 8; i++) writeBit(1);
    
    //trenn bit
    writeBit(0);

    //byte 3
    for (int i = 0; i < 8; i++) writeBit(1);

    //trenn bit
    writeBit(0);

    //byte 4
    for (int i = 0; i < 8; i++) writeBit(1);   
    
    //trenn bit
    writeBit(0);

    //byte 5
    for (int i = 0; i < 8; i++) writeBit(1);   
    
    //end bit
    writeBit(1);
*/
    while(1);

    /// \end::setup_multicore[]
        // todo get free sm
    // PIO pio = pio0;
    // int sm = 0;
    // uint offset = pio_add_program(pio, &pwm_program);
    // printf("Loading program at %d\n", offset);

    // pwm_program_init(pio, sm, offset, 10);
    // pio_pwm_set_period(pio, sm, (1u<<16)-1000);

    // int level = 0;
    // while (true) {
    //     printf("Level = %d\n", level);
    //     pio_pwm_set_level(pio, sm, level * level);
    //     level = (level + 1) % 256;
    //     sleep_ms(100);
    }


