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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define SIZE 128
bool bitarray[SIZE] = {false};
uint8_t bytearray[13] = {0};
uint8_t index_new = 0;
uint8_t index_old;

bool readBit()
{
    busy_wait_us_32(87);
    return !gpio_get(17);
}

void writeBit(bool bit){
    index_old = (index_new+SIZE-1)%SIZE;
    bitarray[index_old] =  bit;
}

uint8_t read8bit(int index){
    uint8_t retval = 0;
    for (int i =0 ; i < 8; i++){
        retval  <<= 1;
        retval |= bitarray[index%SIZE]; 
        index--;
    }
    return retval;
}

void convert_array (int8_t number_of_bytes){
    //start of transmission -> byte_n(addressbyte) -> ... -> byte_0(error detection byte) -> end of transmission
    for (int i = 0; i < number_of_bytes; i++)
    {
        bytearray[i] = read8bit(index_new + 8 + i*9);
    }
}

bool error_detection(int8_t number_of_bytes)
{
    //Bitwise XOR for all Bytes -> Successful result is: "0000 0000"
    uint8_t exor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++)
    {
        exor_byte = exor_byte^bytearray[i]; 
    }
    if(exor_byte == 0b00000000){
        return true;
    }
    return false;
}

// Returns true for long address 
bool long_address(uint8_t number_of_bytes){
    if (bitarray[(index_new-1+(9*number_of_bytes))%SIZE]) //most significant bit of first received byte
    {
        if (bitarray[(index_new-2+(9*number_of_bytes))%SIZE]) //second most significant bit of first received byte
        {
             return true;
        }    
    }
    return false;
}

bool address_evaluation(uint8_t number_of_bytes){
    uint16_t read_address;
    if(long_address(number_of_bytes))
    {
        //start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
        uint8_t address_byte_1 = (bytearray[number_of_bytes-1])-192; //remove long address identifier bits
        uint8_t address_byte_0 = (bytearray[number_of_bytes-2]);
        read_address = (address_byte_1<<8)+address_byte_0;
        printf("long address: %d ,was read. \n", read_address);
    }
    else
    {
        //start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (bytearray[number_of_bytes-1]); 
        printf("short address: %d ,was read. \n",read_address);
    }
    if (CV_1 == read_address) return true;
    return false;
}


void command_evaluation(uint8_t number_of_bytes){
    uint8_t command_byte_n;
     //start of transmission -> ... -> command_byte_n -> ... -> command_byte_0 -> ... -> end of transmission
     if (long_address(number_of_bytes))
     {
        command_byte_n = bytearray[number_of_bytes-3];
     }
     else
     {
        command_byte_n = bytearray[number_of_bytes-2];
     }
     if (command_byte_n>>5 == 0b00000001)           // 001X-XXXX (001 Advanced Operation Instructions)
     {
         switch (command_byte_n)
         {
         case 0b00111100:                           // 0011-1100 (Speed, Direction and Functions)
             /* code */
             printf("0011-1100 (Speed, Direction and Functions) \n");
             break;
         case 0b00111111:                           // 0011-1111 (128 Speed Step Control)
            /*... */
             printf("0011-1111 (128 Speed Step Control) \n");

         default:
             break;
         }
     }
     if (command_byte_n>>6 == 0b00000010)           // 10XX-XXXX (Function Group Instruction)
     {
         if (command_byte_n>>5 == 0b00000100)       // Functions F0-F4
         {
            /*...........*/
            printf("Functions F0-F4 \n");
         }
         else
         {
             switch (command_byte_n>>4)
             {
             case 0b00001011:                       // Functions F5-F8
                 /* code */
                printf("Functions F5-F8 \n");
                break;
             case 0b00001010:                       // Functions F9-F12 
                printf("Functions F9-F12 \n");
                /* code */  
                break;
             default:
                 break;
             }
         }
     }
     
     
     

}


void gpio_callback_rise(unsigned int gpio, long unsigned int events) {
    writeBit(readBit());                            
    bool * s = bitarray;                            
    int fsm_result = fsm_main(s, index_new, SIZE );         //fsm_main returns number of bytes if it finds valid bit sequence otherwise it returns -1
    if(fsm_result != -1){ 
        convert_array(fsm_result);                           //converts bitarray to bytearray for easier handling
        if(error_detection(fsm_result)){                     //Returns true if exor equals 0b00000000 matches with CV
            printf("no error :-)\n");         
            if(address_evaluation(fsm_result)){              //Returns true if address matches with CV
                printf("address matches :-)\n");         
                command_evaluation(fsm_result);
            }
            else  printf("ADDRESS DOES NOT MATCH!\n");
        }
        else  printf("ERROR DETECED!\n");
        printf("\n");        

        /*for (int i = 0; i < fsm_result; i++)
        {
            printf("\nBYTE_%d: ",i);
            for (int j = 0; j < 8; j++)
            {
            printf("%d",bitarray[(index_new-j+8+(9*i))%SIZE]);
            }    
        }
        printf("\nPaketgroesse: %d\n",fsm_result);
        printf("\n");
        printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(error_detection(fsm_result, index_new)));
        */   
    }    
    index_new = index_old;
}


 int main() {
    stdio_init_all();
    gpio_init(17);
    gpio_set_dir(17, GPIO_IN);
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);   
    while(1);
}




    