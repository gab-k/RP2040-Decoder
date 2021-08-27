#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
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

#define SIZE_BIT_ARRAY 128
#define SIZE_BYTE_ARRAY 13
#define SIZE_FUNCTION_BIT_ARRAY 68
bool bit_array[SIZE_BIT_ARRAY] = {false};
bool function_bit_array[SIZE_FUNCTION_BIT_ARRAY] = {false};
uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
uint8_t index_new = 0;
uint8_t index_old;


bool get_current_direction(){
    return false;
}

void emergency_break(){
    printf("EMERGENCY BREAK");
}

void set_speed(uint8_t speed, bool direction)
{
    printf("Speed: %d\nDirection: %d", speed, direction);
}

void set_outputs()
{
    for (uint8_t i = 0; i < SIZE_FUNCTION_BIT_ARRAY; i++)
    {
        if (function_bit_array[i])
        {
            uint8_t mask = 0b00000001;
            for (uint8_t j = 0; j < 8; j++)
            {
                uint8_t bit_value = (mask&CV_FUNCTION_ARRAY[i])>>j;
              //gpio_put(j,bit_value);
                printf("GPIO: %d set to: %d\n",j,bit_value);
                mask = mask << 1;
            }
            
        }
        // bool current_bit = function_bit_array[i];
        // uint8_t current_CV = CV_FUNCTION_ARRAY[i];
        // printf("GPIO: %d - %d ; ",i,function_bit_array[i]);
    }
    printf("\n");
}

bool readBit()
{
    busy_wait_us_32(87);
    return !gpio_get(17);
}

void writeBit(bool bit){
    index_old = (index_new+SIZE_BIT_ARRAY-1)%SIZE_BIT_ARRAY;
    bit_array[index_old] =  bit;
}

uint8_t read8bit(int index){
    uint8_t retval = 0;
    for (int i =0 ; i < 8; i++){
        retval  <<= 1;
        retval |= bit_array[index%SIZE_BIT_ARRAY]; 
        index--;
    }
    return retval;
}

void parse_to_function_bit_array(uint8_t bit_index_offset,uint8_t input_byte,uint8_t count)
{
    uint8_t mask = 0b00000001;
    for (uint8_t i = 0; i < count; i++)
        {
            function_bit_array[i+bit_index_offset] = (input_byte&mask)>>i;
            mask = mask<<1;
        }
}

void convert_array (int8_t number_of_bytes){
    //start of transmission -> byte_n(addressbyte) -> ... -> byte_0(error detection byte) -> end of transmission
    for (int i = 0; i < number_of_bytes; i++)
    {
        byte_array[i] = read8bit(index_new + 8 + i*9);
    }
}

bool error_detection(int8_t number_of_bytes)
{
    //Bitwise XOR for all Bytes -> Successful result is: "0000 0000"
    uint8_t exor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++)
    {
        exor_byte = exor_byte^byte_array[i]; 
    }
    if(exor_byte == 0b00000000){
        return true;
    }
    return false;
}

// Returns true for long address 
bool long_address(uint8_t number_of_bytes){
    if (bit_array[(index_new-1+(9*number_of_bytes))%SIZE_BIT_ARRAY]) //most significant bit of first received byte
    {
        if (bit_array[(index_new-2+(9*number_of_bytes))%SIZE_BIT_ARRAY]) //second most significant bit of first received byte
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
        uint8_t address_byte_1 = (byte_array[number_of_bytes-1])-192; //remove long address identifier bits
        uint8_t address_byte_0 = (byte_array[number_of_bytes-2]);
        read_address = (address_byte_1<<8)+address_byte_0;
        printf("long address: %d, was read. \n", read_address);
    }
    else
    {
        //start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (byte_array[number_of_bytes-1]); 
        printf("short address: %d, was read. \n",read_address);
    }
    if (CV_1 == read_address) return true;
    return false;
}

void instruction_evaluation(uint8_t number_of_bytes){
    uint8_t command_byte_n;
    uint8_t command_byte_index;
    // start of transmission -> ... -> command_byte_n -> ... -> command_byte_0 -> ... -> end of transmission
    if (long_address(number_of_bytes))
    {   
        command_byte_index = number_of_bytes-3;
    }
    else
    {
        command_byte_index = number_of_bytes-2;
    }
    command_byte_n = byte_array[command_byte_index];

    if (command_byte_n>>6 == 0b00000001)    // 01XX-XXXX (Basic Speed and Direction Instruction)
    {
        if (command_byte_n<<4>>4 == 0b00000001)     // 01XX-0001 (Emergency Break)
        {
            emergency_break();
        }
        if (command_byte_n<<4>>4 == 0b00000000)     // 01XX-0000 (Normal Break)
        {
            set_speed(0 , 1);
        }
        else
        {
            printf("01XX-XXXX - Basic Speed \n");   //01XX-XXXX - Basic Speed (26 Steps)
            uint8_t mask = 0b01111111; 
            uint8_t speed = ((command_byte_n & mask)-1)*4;      //Max speed step is equivalent to step 104 of 128 speed step 
            mask = 0b00100000;
            uint8_t direction = (command_byte_n&mask)>>5;  
            set_speed(speed, direction);
        }
    }
    if (command_byte_n>>5 == 0b00000001)    // 001X-XXXX (Advanced Operation Instructions)
    {
        switch (command_byte_n)
        {
            case 0b00111100:                            // 0011-1100 (Speed, Direction and Functions) - 3 to 6 Byte length
                printf("0011-1100 (Speed, Direction and Functions) \n");
                switch (command_byte_index)             //command_byte_index == Instruction Byte Length  
                {                                       //Switch case could be replaced with for loop...
                case 6:
                    //F0 - F7
                    parse_to_function_bit_array(0,byte_array[command_byte_index-2],8);
                    //F8-F15
                    parse_to_function_bit_array(8,byte_array[command_byte_index-3],8);
                    //F16-F23
                    parse_to_function_bit_array(16,byte_array[command_byte_index-4],8);
                    //F24-F31
                    parse_to_function_bit_array(24,byte_array[command_byte_index-5],8);
                    //...
                    goto speed_128;
                    break;
                case 5: 
                    //F0 - F7
                    parse_to_function_bit_array(0,byte_array[command_byte_index-2],8);
                    //F8-F15
                    parse_to_function_bit_array(8,byte_array[command_byte_index-3],8);
                    //F16-F23
                    parse_to_function_bit_array(16,byte_array[command_byte_index-4],8);
                    //...
                    goto speed_128;
                    break;
                case 4:
                    //F0 - F7
                    parse_to_function_bit_array(0,byte_array[command_byte_index-2],8);
                    //F8-F15
                    parse_to_function_bit_array(8,byte_array[command_byte_index-3],8);
                    //...
                    goto speed_128;
                    break;
                case 3: 
                    //F0 - F7
                    parse_to_function_bit_array(0,byte_array[command_byte_index-2],8);
                    //...
                    goto speed_128;
                    break;
                default:
                    break;
                }
            break;

            case 0b00111111:                            // 0011-1111 (128 Speed Step Control) - 2 Byte length
                speed_128:
                printf("0011-1111 (128 Speed Step Control) \n");
                uint8_t command_byte_n_minus1 = byte_array[command_byte_index-1];
                uint8_t mask = 0b01111111; 
                uint8_t speed = (command_byte_n_minus1 & mask)-1;
                uint8_t direction = command_byte_n_minus1>>7;
                set_speed(speed , direction);
                break;
            default:
                break;
        }
    }
    if (command_byte_n>>6 == 0b00000010)            // 10XX-XXXX (Function Group Instruction)
    {
        if (command_byte_n>>5 == 0b00000100)        // Functions F0-F4
        {
            parse_to_function_bit_array(0,command_byte_n>>4,1);    //F0
            parse_to_function_bit_array(1,command_byte_n,4);    //F1-F4
            printf("Functions F0-F4 \n");
        }
        else
        {
            switch (command_byte_n>>4)
            {
            case 0b00001011:                        // Functions F5-F8
                printf("Functions F5-F8 \n");
                parse_to_function_bit_array(5,command_byte_n,4);
                break;
            case 0b00001010:                        // Functions F9-F12
                printf("Functions F9-F12 \n");
                parse_to_function_bit_array(9,command_byte_n,4);
                break;
            default:
                break;
            }
        }
    }
    if (command_byte_n>>5 == 0b00000110)        //Feature Expansion Instruction 110X-XXXX
    {
        switch (command_byte_n)
        {
        case 0b11011110:                        // F13-F20
            parse_to_function_bit_array(13,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011111:                        // F21-F28
            parse_to_function_bit_array(21,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011000:                        // F29-F36
            parse_to_function_bit_array(29,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011001:                        // F37-F44
            parse_to_function_bit_array(37,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011010:                        // F45-F52
            parse_to_function_bit_array(45,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011011:                        // F53-F60
            parse_to_function_bit_array(53,byte_array[command_byte_index-1],8);        
            break;
        case 0b11011100:                        // F61-F68
            parse_to_function_bit_array(61,byte_array[command_byte_index-1],8);        
            break;
        default:
            break;
        }
    }
    
    set_outputs();
}


void gpio_callback_rise(unsigned int gpio, long unsigned int events) {
    writeBit(readBit());                            
    bool * s = bit_array;                            
    int fsm_result = fsm_main(s, index_new, SIZE_BIT_ARRAY );       //fsm_main returns number of bytes if it finds valid bit sequence otherwise it returns -1
    if(fsm_result != -1){ 
        convert_array(fsm_result);                                  //converts bit_array to byte_array for easier handling
        if(error_detection(fsm_result)){                            //Returns true if the XOR verification was error-free
            printf("no error :-)\n");         
            if(address_evaluation(fsm_result)){                     //Returns true if address matches with CV
                printf("address matches :-)\n");         
                instruction_evaluation(fsm_result);                 //
            }
            else  printf("ADDRESS DOES NOT MATCH!\n");
        }
        else  printf("ERROR DETECED!\n");
        printf("\n");        

        for (int i = 0; i < fsm_result; i++)
        {
            printf("\nBYTE_%d: ",i);
            for (int j = 0; j < 8; j++)
            {
            printf("%d",bit_array[(index_new-j+8+(9*i))%SIZE_BIT_ARRAY]);
            }    
        }
        printf("\nPaketgroesse: %d\n",fsm_result);
        printf("\n");
       // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(error_detection(fsm_result, index_new)));
           
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




    