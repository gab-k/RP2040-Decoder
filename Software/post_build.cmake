# Check if the required arguments are provided
if(NOT DEFINED BOARD_HEADER)
    message(FATAL_ERROR "BOARD_HEADER argument is missing")
else()
    message(STATUS "BOARD_HEADER: ${BOARD_HEADER}")
endif()

if(NOT DEFINED SIZE_TOOL)
    message(FATAL_ERROR "SIZE_TOOL argument is missing")
else()
    message(STATUS "SIZE_TOOL: ${SIZE_TOOL}")
endif()

if(NOT DEFINED ELF_FILE)
    message(FATAL_ERROR "ELF_FILE argument is missing")
else()
    message(STATUS "ELF_FILE: ${ELF_FILE}")
endif()

# Post build checks
# Run the size tool and read the output
execute_process(
    COMMAND ${SIZE_TOOL} "${ELF_FILE}"
    OUTPUT_VARIABLE SIZE_OUTPUT
    RESULT_VARIABLE SIZE_RESULT
)

if(SIZE_RESULT)
    message(FATAL_ERROR "Failed to run arm-none-eabi-size on ${ELF_FILE}")
else()
    # Print the content of SIZE_OUTPUT
    message(STATUS "arm-none-eabi-size output:\n${SIZE_OUTPUT}")
endif()
# Use regex to match all numbers
string(REGEX MATCHALL "[0-9]+" MATCHED_VALUES "${SIZE_OUTPUT}")
# Get third element from list of matches, this corresponds with dec table value in "size_output.txt"
list(GET MATCHED_VALUES 3 PROGRAM_SIZE_DEC)
# Check for success of program size extraction from "size_output.txt"
if(PROGRAM_SIZE_DEC)
    message(STATUS "PROGRAM_SIZE_DEC: ${PROGRAM_SIZE_DEC} bytes")
else()
    message(FATAL_ERROR "Could not extract program size from size_output.txt")
endif()
# Path to the header file that defines PICO_FLASH_SIZE_BYTES 
set(FLASH_CONFIG_HEADER ${BOARD_HEADER})
# Read the contents of the header file into a variable
file(READ ${FLASH_CONFIG_HEADER} HEADER_CONTENT)
# Extract the value of PICO_FLASH_SIZE_BYTES using a regular expression
string(REGEX MATCHALL "#define PICO_FLASH_SIZE_BYTES [0-9]+" MACRO_MATCH "${HEADER_CONTENT}")
string(REGEX MATCHALL "[0-9]+" PICO_FLASH_SIZE_BYTES "${MACRO_MATCH}")
# Check for success of PICO_FLASH_SIZE_BYTES extraction from board header file
if(PICO_FLASH_SIZE_BYTES)
    message(STATUS "PICO_FLASH_SIZE_BYTES: ${PICO_FLASH_SIZE_BYTES} bytes")
else()
    message(FATAL_ERROR "Failed to find PICO_FLASH_SIZE_BYTES in the header file.")
endif()
# Sector size is always 4096 bytes
set(FLASH_SECTOR_SIZE 4096)
message(STATUS "FLASH_SECTOR_SIZE: ${FLASH_SECTOR_SIZE} bytes")
# Subtract flash sector size from flash size
math(EXPR FLASH_TARGET_OFFSET "${PICO_FLASH_SIZE_BYTES} - ${FLASH_SECTOR_SIZE}")
message(STATUS "FLASH_TARGET_OFFSET: ${FLASH_TARGET_OFFSET} bytes")
if(PROGRAM_SIZE_DEC GREATER FLASH_TARGET_OFFSET)
    message(FATAL_ERROR "Program size exceeds allowed flash size. Flash sector containing the CVs would be overwritten! Please reduce the size of the program.")
endif()
