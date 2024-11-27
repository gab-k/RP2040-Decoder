# Hardware Description

## Overview

![Block Diagram](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Block_Diagram.svg)

The above block diagram gives a general overview regarding the operation of components. For better illustration it has been divided into four functional blocks. Furthermore these functional blocks can be subdivided and will be explained below.

* Input Stage
  - DCC Signal Level Shift
  - Bridge Rectifier & Voltage Regulator
* Motor Driver
  - Motor Driver
  - Back EMF measurement (ADC)
* General Purpose
  - GPIO
  - Auxillary output
* Microcontroller
  - Flash memory
  - Crystal oscillator

## Characteristics


| Parameter                                             | Symbol               | Minimum | Maximum  | Unit |
| ----------------------------------------------------- | --------------       | ------- | -------- | ---- |
| Absolute max. DCC Input Voltage                       | V<sub>DCC</sub>      | -25     | +25      | V    |
| Absolute max. GPIO Voltage<sup>(0)</sup>              | V<sub>PIN</sub>      | -0.5    | 3.3+0.5  | V    |
| Input Voltage High<sup>(0)</sup>                      | V<sub>IH</sub>       | 2.145   | 3.6      | V    |
| Input Voltage Low<sup>(0)</sup>                       | V<sub>IL</sub>       | 0       | 1.155    | V    |
| Total GPIO Current<sup>(0)</sup>                      | I<sub>IOVDD_MAX</sub>|         | 50       | mA   |
| Absolute max. motor output current<sup>(1)</sup>      | I<sub>MOTOR</sub>    |         | 2.8      | A    |
| Absolute max. auxillary output current<sup>(2)</sup>  | I<sub>AUX</sub>      |         | 400      | mA   |

(0): According to: [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) (assuming V<sub>DD</sub>  = 3.3V)

(1): According to: [BDR6133 Datasheet](https://datasheet.lcsc.com/lcsc/2101211904_Bardeen-Micro--BDR6133_C2687793.pdf)

(2): According to: [T2N7002BK Datasheet](https://datasheet.lcsc.com/lcsc/1810271831_TOSHIBA-T2N7002BK-LM_C146372.pdf)

## Input Stage

![Input Stage](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Input_Stage.svg)

D1 Rectifies the DCC square-wave into +V, the amplitude is not critical as long as its within the 25V limit. The rectified voltage +V is then provided to the 3.3V voltage regulator (U1) and to the motor driver (U2).

The MOSFET on the right effectively translates the rather high DCC voltage to a suitable level for the microcontroller to process. When Q1 is turned off GPIO21 is pulled high by the RP2040 internal pull-up resistor, as soon as Q1 gets turned on (V<sub>GS</sub> > V<sub>GS,th</sub>), GPIO21 gets pulled to GND potential. R1 and R2 ensure that V<sub>GS</sub> doesnt exceed ±20V.

## Motor Driver

![Motor Driver](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Motor_Driver.svg)

The Motor Driver is a complete H-Bridge IC specifically made for DC-Motors.

In order to control the motor, it is necessary to measure the back-EMF. This is done by the ADC which measures at Points ADC_EMF_A and ADC_EMF_B respectively. The two resistors are used as a voltage divider.

## Auxillary Output

![Auxillary Output](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Aux_Outputs.svg)

Switching of loads up to a maximum of about ~400mA can be done using four N-Channel MOSFETS. They are switching on the low-side which means the load effectively gets connected to GND potential as the transistor starts to conduct. GPIO24 - GPIO27 are used.

## GPIO

To switch lighter loads six soldering pads on the back side of the PCB are connected to GPIO0 - GPIO5 pins . Although not implemented yet they could theoretically be used as inputs as well (e.g. Reed switch).

## Microcontroller

![Microcontroller](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/Microcontroller.svg)

The Microcontroller needs a crystal oscillator to function properly. External flash memory is also needed as it holds the configuration variables and the program itself. R3 & C14 are used to filter ADC_AVDD (ADC Reference Voltage).



## Complete Schematic

![Complete schematic](https://github.com/1nct/RP2040-Decoder/blob/main/docs/svg/RP2040-Decoder.svg)

BOM & Alternative Parts
--------------------------

| Ref | Quantity | Value | Component Type | Component Name | Footprint |
| ----------------------------------------------------- | -------------- | ------- | -------- | ---- |---- |
| C1, C2, C5, C6, C14 | 5 | 10u | Ceramic Capacitor | / | 0603_1608Metric |
| C3, C4, C7, C8, C9, C10, C11, C12, C13, C15 | 10 | 100n | Ceramic Capacitor | / | 0603_1608Metric |
| C16, C17 | 2 | 33p | Ceramic Capacitor | / | 0603_1608Metric |
| D1 | 1 | V<sub>RMS</sub> = 40V , I<sub>F</sub> = 2A | Bridge Rectifier | KMB24F | MBS (TO-269AA) |
| Q1, Q2, Q3, Q4, Q5 | 5 | V<sub>DS</sub> = 60V , I<sub>D</sub> = 0.4A  | N-Channel MOSFET |T2N7002BK | SOT-23 |
| R1, R2, R5, R6 | 4 | 6k8 | Resistor | / | 0603_1608Metric |
| R3 | 1 | 200 | Resistor | / | 0603_1608Metric |
| R7, R8, R9 | 3 | 1k | Resistor | / | 0603_1608Metric |
| U1 | 1 | V<sub>OUT</sub> = 3.3V | Low Dropout Voltage Regulator | HT7533S | SOT-23 |
| U2 | 1 | I = 2.8A | H-Bridge driver | BDR6133 | SOIC-8 |
| U3 | 1 | 64Mb | Serial Flash Memory | W25Q64JVSSIQ | SOIC-8 |
| U4 | 1 | / | Raspberry Pi RP2040 Microcontroller | RP2040 | QFN-56-1EP_7x7mm_P0.4mm |
| Y1 | 1 | f = 12MHz | Crystal Oscillator | X322512MSB4SI | Crystal_SMD_3225-4Pin_3.2x2.5mm |

NOTE: Keep capacitor V<sub>max</sub> in mind!

Alternatives for BDR6133:

[PT5126A-S-TP](https://lcsc.com/product-detail/Motor-Driver-ICs_PTC-Princeton-Tech-PT5126A-S-TP_C162093.html)

[PT2476](https://lcsc.com/product-detail/Motor-Driver-ICs_PTC-Princeton-Tech-PT2476_C481903.html)

Alternative for HT7533S:

[GS7533-33TR3](https://www.lcsc.com/product-detail/Linear-Voltage-Regulators-LDO_Gainsil-GS7533-33TR3_C7427372.html)
