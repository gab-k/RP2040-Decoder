# Software Description

## Overview

The Software is structured as follows:
* Core0.c
  - DCC package detection & decoding
  - Programming mode (Modify and read CVs on programming track)
  - ADC-Offset measurement
  - Feed-forward base PWM measurement

* Core1.c
  - PID motor controller
* CV.h
  - Default configuration variables

Conveniently The Raspberry Pi Foundation provides a well documented [SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf) for the RP2040. The SDK provides abstraction to a higher level and therefore there is no need to access any registers directly. Everything is written using the SDK so hopefully the code is easier to understand. In the following, the motor control and the decoding of the DCC signals will be explained.

## Motor control overview

![Block Diagram](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Block_Diagram_Control.svg)
***
**Block diagram - Motor control**

The above block diagram in essence describes the functionality of the control loop. While the central component is the PID controller, there  is an additional block called [Feed-forward](https://en.wikipedia.org/wiki/Feed_forward_(control)) that has an impact on the control output variable. The Feed-forward block adds an additional offset to the output depending on the setpoint, this is done to achieve better control. A more detailed explanation regarding Feed-forward can be found below. The speed helper block is corresponding to the speed_helper() function which delays changing the setpoint according to the configured deceleration/acceleration rates.


## PID Controller

![PID Controller](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/Block_Diagram_PID.svg)

The block diagram above shows the PID controller in the s-domain. The derivative part contains an additional digital low pass filter used to filter out high-frequency noise.

Output = E * (K<sub>P</sub> + K<sub>I</sub>/s + (K<sub>D</sub>*s)/(sτ+1)) = P + I + D

*  P = E * K<sub>P</sub>
*  I = E * K<sub>I</sub>/s
*  D = E * (K<sub>D</sub>*s)/(sτ+1)

The continuous-time system representation can now be transformed into a discrete-time representation using [bilinear transform](https://en.wikipedia.org/wiki/Bilinear_transform) [ s = 2*(z-1) / T<sub>s</sub>*(z+1) ].

This results in the following difference equations which are then implemented in software:

* p<sub>n</sub> = K<sub>P</sub> * e<sub>n</sub>
* i<sub>n</sub> = 0.5K<sub>I</sub> * (e<sub>n</sub> + e<sub>n-1</sub>) + i<sub>n-1</sub>
* d<sub>n</sub> = [ 2K<sub>D</sub> * (e<sub>n</sub> - e<sub>n-1</sub>) + (2τ - T<sub>s</sub>) * d<sub>n-1</sub> ] / [ 2τ + T<sub>s</sub>] 

=> output<sub>n</sub> = p<sub>n</sub> + i<sub>n</sub> + d<sub>n</sub>

Also note that instead of using derivative of error, derivative on measurement is used. This is done to prevent a high derivative part in the event of a large change in setpoint. 

### Gain scheduling

![K_P](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/adaptive_kp.svg)

Another aspect to consider is the implementation of [gain scheduling](https://en.wikipedia.org/wiki/Gain_scheduling). K<sub>P</sub> is a function of the current setpoint. Often it is favorable to have a higher proportional gain K<sub>P</sub> for slow speeds, achieving better control results. The illustration above shows the default setting for K<sub>P</sub>. CV_54 & CV_55 are used to set K<sub>P</sub> @ x<sub>0</sub>, CV_56 & CV_57 for K<sub>P</sub> @ x<sub>1</sub> and CV_58 & CV_59 for K<sub>P</sub> @ x<sub>2</sub>. Additionally CV_60 is used to shift x<sub>1</sub> from the leftmost point (0% = 0/255)  to the rightmost point (100% = 255/255).

### Feed-forward 

![Feed-forward](https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/feed_forward.svg)
To achieve better control [Feed-forward](https://en.wikipedia.org/wiki/Feed_forward_(control)) is used. On startup the decoder will check for a configuration regarding Feed-forward control - when no configuration is found the decoder automatically runs a calibration to establish a base pwm level (y<sub>1</sub>) for both directions. The base pwm level (y<sub>1</sub>) will vary depending on load, motor and voltage on the tracks and can be configured with CV_176 - CV_179. If the setpoint is greater than the threshold, y<sub>2</sub> is corrected automatically when i<sub>n</sub> reaches 0.5i<sub>min</sub> or 0.5i<sub>max</sub>. CV_47 can be used to set how sensitive this correction is.

### Back-EMF voltage measurement 

To provide a feedback signal which is proportional to the motor speed the ADC is used to measure the [Back-EMF voltage](https://en.wikipedia.org/wiki/Counter-electromotive_force). The measurement works by setting the PWM duty cycle to 0%, waiting for a certain delay time (CV_62) and then measuring x times (x = CV_63). While measuring the array with measurement values are being sorted using insertion sort. Afterwards y elements (y = CV_63) from the left (lowest values) and z elements (z = CV_64) from the right (highest values) will be dismissed to mitigate the impact of potential outliers in measurement. The average value of the remaining values will be computed and fed back into the control algorithm. Considering default settings (100us delay, 100 samples, ~2µs sampling time) the complete measurement run including averaging takes about 0.3ms to 0.35ms which effectively reduces the maximum possible duty cycle to about 93% to 94%.

## DCC signal decoding

The detection of the DCC signal works by looking at every rising and falling edge and calculating the time between them. When the time between rising and falling edge is greater than 87μs then this is equivalent to "0", else "1" - This value then gets shifted into a 64-Bit variable.

Decoding is done after every falling edge. It starts with an error detection which when not passed will dismiss the received command. Then the address will be decoded and compared to the address stored in the configuration. If the address matches the command/instruction will be decoded.

Only a few instructions are currently implemented, only 128 speed step instructions are supported.

**Implemented instructions:**
* 0011-1111 - (128 Speed Step Control) - 2 Byte length
* 10XX-XXXX - (Function Group Instruction) (F0 - F12)
* 110X-XXXX - Expansion Instruction  (F13 - F31)

All DCC instructions can be found in Section 9.2, 9.2.1 and 9.2.1.1 of the [NMRA Communications Standard  ](https://www.nmra.org/index-nmra-standards-and-recommended-practices)


