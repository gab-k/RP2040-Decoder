
def print_speed_steps():
    """
    This script generates and prints speed step definitions for both reverse and forward directions.
    Functions:
        print_speed_steps(): Generates and prints speed step definitions in hexadecimal and binary formats used in shared.h.
            - Reverse speed steps range from 0x00 to 0x7F.
            - Forward speed steps range from 0x80 to 0xFF.

    * in NMRA Standard 9.2.1 section 2.3.2.1 or RCN-212 section 2.2.2.
    *
    * The following speed steps are defined:
    *
    * | DEC | BIN       | DIRECTION | DEFINITION           |
    * |-----|-----------|-----------|----------------------|
    * | 000 | 0000 0000 | Reverse   | STOP                 |
    * | 001 | 0000 0001 | Reverse   | EMERGENCY STOP       |
    * | 002 | 0000 0010 | Reverse   | Speed step 001       |
    * | 003 | 0000 0011 | Reverse   | Speed step 002       |
    * | 004 | 0000 0100 | Reverse   | Speed step 003       |
    * | ... | .... .... | Reverse   | ...                  |
    * | 126 | 0111 1110 | Reverse   | Speed step 125       |
    * | 127 | 0111 1111 | Reverse   | Speed step 126       |
    * | 128 | 1000 0000 | Forward   | STOP                 |
    * | 129 | 1000 0001 | Forward   | EMERGENCY STOP       |
    * | 130 | 1000 0010 | Forward   | Speed step 001       |
    * | 131 | 1000 0011 | Forward   | Speed step 002       |
    * | 132 | 1000 0100 | Forward   | Speed step 003       |
    * | ... | .... .... | Forward   | ...                  |
    * | 254 | 1111 1110 | Forward   | Speed step 125       |
    * | 255 | 1111 1111 | Forward   | Speed step 126       |
    """
    # Reverse speed steps
    print(f"SPEED_STEP_REVERSE_STOP = 0x{0:02X}, /**< {0:08b}bin {0:03d}dec */") 
    print(f"SPEED_STEP_REVERSE_EMERGENCY_STOP = 0x{1:02X}, /**< {1:08b}bin {1:03d}dec */")
    for i in range(1, 127):
        print(f"SPEED_STEP_REVERSE_{i:03d} = 0x{i+1:02X}, /**< {i+1:08b}bin {i+1:03d}dec */")
    
    print("\n\n")
    
    # Forward speed steps
    print(f"SPEED_STEP_FORWARD_STOP = 0x{128:02X}, /**< {128:08b}bin {128:03d}dec */")
    print(f"SPEED_STEP_FORWARD_EMERGENCY_STOP = 0x{129:02X}, /**< {129:08b}bin {129:03d}dec */")
    for i in range(1, 127):
        print(f"SPEED_STEP_FORWARD_{i:03d} = 0x{i+129:02X}, /**< {i+129:08b}bin {i+129:03d}dec */")

# Call the function
print_speed_steps()
