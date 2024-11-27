# Configuration Variables (CV)

## Note
CV.h contains default values for every CV. When your board is still "factory new" these values are automatically written into flash. CV_65 == 255 indicates a factory condition and resets all CVs to default. Alternatively to reset all CVs to default a value of 8 can be written to CV8. Also note that index 0 in the CV array corresponds with CV_1, index 1 corresponds with CV_2 and so forth. This list only contains CVs that are in use.

## Speed step configuration
![Speed Table](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/speed_table.svg)
* V_min = CV_2
* V_mid = CV_6 * 16
* V_max = CV_5 * 16

The illustration above shows how the speed steps can be configured using the corresponding CVs. The red * represents V_min which is always at speed step 1, the green * shows V_mid at speed step 64 and the blue * shows V_max at speed step 126.

## CV List

### CV_1 - Basic address
Short DCC address


### CV_2 - V_min
Min motor voltage/speed


### CV_3 - Acceleration rate
CV_3*CV_175 = Time for one discrete speed step change in ms


### CV_4 - Deceleration rate
CV_4*CV_175 = Time for one discrete speed step change in ms


### CV_5 - V_max
Max motor voltage/speed - V_max = CV_5 * 16


### CV_6 - V_mid
Mid motor voltage/speed - V_max = CV_6 * 16


### CV_7 - Version No. (Read-Only)
Rev 0.3 = 1

Writing a value of 7 triggers am ADC offset adjustment.

### CV_8 - Manufacturer (Read-Only)
Default = "13" = Public Domain & Do-It-Yourself Decoders


Writing a value of 8 resets all CV values to the default values in CV.h.
This will also happen if CV_65 == "255" == "0xFF".


### CV_9 - Motor PWM Frequency
f in Hz = CV_9 * 100 + 10000


### CV_11 - Packet timeout
If no package is received for x seconds (x = CV_11) the decoder will automatically stop the motor (Emergency Stop).


### CV_12 - Permitted operating modes
Currently DCC Digital mode is the only supported mode.


### CV_17 & CV_18 - 14-Bit extended/long address
Address is only used if bit5 has a value of "1" in CV_29.

![Long Address](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/Long_address.svg)

Bit15 and bit14 which correspond to bit7 and bit6 of CV_17 are always "1". CV_17 = "192" & CV_18 = "0" is also not a valid address. Otherwise the  valid value range of CV_17 ranges from "192" to "231" and "0" to "255" for CV_18 respectively.  



### CV_29 - Decoder Configuration

Bit0 is used to reverse direction i.e. "0" = normal, "1" = reverse.
Bit5 switches between basic and extended addressing modes i.e. "0" = basic address, "1" = extended address.
The other 6 bits are currently not in use and therefore irrelevant.


### CV_31 & CV_32 - Extended CV pointer (Read-Only)
Not in use.

### CV_47 - Feed-forward gain k_ff
k_ff defines the correction behavior with regard to the y2 variable. High values for k_ff lead to a faster change in the variable. Values for k_ff that are too high can lead to oscillations.
k_ff = CV_47/10000


### CV_48 - Filter time constant (tau) in ms
The time constant tau characterizes the low-pass filter which is connected downstream in the D component of the PID controller


### CV_49 - PID Control sampling time t in ms
Every t seconds the pid_control() function gets called to compute the new control output


### CV_50 - PID Control I Factor
Integral gain factor of the PID-Controller


### CV_51 - PID Control D Factor
Derivative gain factor of the PID-Controller


### CV_52 & CV_53 - PID Integral Limits
Maximum and minimum integral part values


### CV_54 & CV_55 - Gain scheduling config k_p @ x0
CV_54 - High Byte -- CV_55 - Low Byte


### CV_56 & CV_57 - Gain scheduling config k_p @ x1
CV_56 - High Byte -- CV_57 - Low Byte


### CV_58 & CV_59 - Gain scheduling config k_p @ x2
CV_58 - High Byte -- CV_59 - Low Byte


### CV_60 - Gain scheduling config x1 shift percentage
Determines position of x1 in percent -> x1 shift in % ≙ CV_60/255 


### CV_61 - Measurement Iterations
Total amount of BEMF-Voltage measurement iterations


### CV_62 - Measurement Delay
BEMF-Voltage measurement delay


### CV_63 & CV_64 - Measurement array cutoffs 
CV_63 determines how many elements from the left side (beginning with index 0) of the array will be discarded while CV_64 determines how many elements from the right side (beginning from highest index) will be discarded.


### CV_65 - New flash variable
Each Byte of the factory new flash chip should have a inital state of "0xFF". This will be checked for in the beginning of core0.c (see cv_setup_check()). When CV_65 == "255" == "0xFF" the default config (CV.h) will be written to flash. Alternatively a reset to default can be done by either writing "8" to CV_8 or writing "255" to CV_65 which then triggers a reset on startup.


### CV_112, CV_113, CV_114, CV_115 - GPIO PWM Configuration 
PWM enabled "1" - PWM disabled "0"


### CV_116 to CV_171 - PWM Slice configuration
![GPIO Slices](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/GPIO_slices.svg)


Every slice controls two independent output channels, duty cycle from 0% to 100% inclusive and can be configured with several parameters:


* Wrap (16Bit): Sets the slice time period of x cycles - effectively determining the frequency
* Clock Divider: Additional clock divider
* Level Channel A / Level Channel B - (16Bit each): Determining how many of the x cycles the signal is at a high level - effectively determining the duty cycle

To see which CVs correspond to which slice and channel, it is advisable to use CV.h as a reference

The PWM-frequency is calculated as follows: f<sub>PWM</sub> = 125 MHz / [ (CV<sub>clk_div</sub> + 1) * (CV<sub>wrap</sub> + 1) ]

The duty-cycle can be calculated as follows: D = CV<sub>Level</sub> / CV<sub>wrap</sub> + 1

**Example configuration - (AUX2: PWM-Frequency f<sub>PWM</sub> = 10kHz; Duty-cycle D = 20%):**

AUX1 => GPIO25 => Slice 4 / Channel B => Wrap: CV_144 & CV_145; Clock-divider: CV_146; Level(B): CV_149 & CV_150 

=> f<sub>PWM</sub> = 10kHz = 125 MHz / [ (0 + 1) * (12499 + 1) ];

=> CV_144 = 110000, CV_145 = 11010011, CV_146 = 0;   ---   (Note: "12499<sub>dec</sub>" = "11000011010011<sub>bin</sub>")

=> CV<sub>Level</sub> = CV_149 & CV_150 = D * (CV<sub>wrap</sub> + 1) = 0.2 * 12500 = 2500

=> CV_149 = 1001, CV_150 = 11000100;   ---   (Note: "2500<sub>dec</sub>" = "100111000100<sub>bin</sub>") 

Another configuration which also achieves 10kHz and D = 20% would be CV<sub>clk_div</sub> = 10, CV<sub>wrap</sub> = 1249, CV<sub>Level</sub> = 250.


A more detailed explanation regarding PWM can be found in the [RP2040-Datasheet - Chapter 4.5](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf).

### CV_172 - ADC Offset
If CV_172 = "0xFF" = "255" there will be a measurement procedure on startup


### CV_173 - ADC offset measurement cycles
Determines how many ADC offset measurement iterations are done.


### CV_174 - Additional motor-PWM clock divider.
Additional Clock Divider - works the same as any GPIO clock divider


### CV_175 - speed_helper timer delay
This timer delay can be used to change the time between two speed steps. So basically an additional option to adjust the deceleration and acceleration rate.


### CV_176, CV_177, CV_178, CV_179 - Base PWM Level used in feed-forward control
CV_176 - High Byte forward  -- CV_177 - Low Byte forward -- CV_178 - High Byte reverse  -- CV_179 - Low Byte reverse

These values are equivalent to y1(forward) and y1(reverse) mentioned [here](https://github.com/gab-k/RP2040-Decoder/wiki/Software-description#feed-forward). Writing "0" to all four CVs schedules a new calibration on startup.


### CV_257 to CV_512 - Function Mapping CVs
The switching status of the outputs depends both on the function instruction received and on the current direction of travel. That means you can, for example, use a function to switch two or more outputs or, as is usual with headlights, switch them depending on the direction of travel. For configuration, 32 bits (corresponding to the GPIOs) are available for each function in each direction of travel.
