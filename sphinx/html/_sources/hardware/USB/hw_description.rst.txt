Overview
==============================

.. figure:: ../../../../svg/hw/USB/Block_Diagram_Hardware.svg
   :align: center
   :alt: Block Diagram

   Harware block diagram

The above block diagram gives a general overview regarding the operation of components. For better illustration it has been divided into four functional blocks. Furthermore, these functional blocks can be subdivided and will be explained below.

- **Input Stage**
   - DCC Signal Level Shift
   - Bridge Rectifier & Voltage Regulator
- **Motor Driver**
   - Motor Driver
   - Back EMF measurement (ADC)
- **General Purpose**
   - GPIO
   - Auxiliary output
- **Microcontroller**
   - Flash memory
   - Crystal oscillator
   - USB Connector

Electrical Characteristics
==============================
.. table:: Electrical Characteristics & Absolute Maximum Ratings
   :widths: auto

   ========================================================  =============================  =========  =========  =====
   Parameter                                                 Symbol                         Minimum    Maximum    Unit
   ========================================================  =============================  =========  =========  =====
   Absolute max. DCC input voltage difference                V\ :sub:`DCC`\                            +24        V
   Absolute max. GPIO voltage\ :sup:`(0)`\                   V\ :sub:`PIN`\                 -0.5       3.3+0.5    V
   Input voltage high\ :sup:`(0)`\                           V\ :sub:`IH`\                  2.145      3.6        V
   Input voltage low\ :sup:`(0)`\                            V\ :sub:`IL`\                  0          1.155      V
   Total GPIO Current\ :sup:`(0)`\                           I\ :sub:`IOVDD`\                          50         mA
   Absolute max. motor output current\ :sup:`(1)`\           I\ :sub:`MOTOR`\                          2.8        A
   Absolute max. auxiliary output current \ :sup:`(2)(3)`\   I\ :sub:`AUX`\                            1.4        A
   ========================================================  =============================  =========  =========  =====


(0): According to: `RP2040 Datasheet <https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf>`_ (assuming V\ :sub:`DD`\ = 3.3V)

(1): According to: `BDR6133 Datasheet <https://www.lcsc.com/datasheet/lcsc_datasheet_2409291808_Bardeen-Micro--BDR6133_C2687793.pdf>`_

(2): According to: `Si1308EDL Datasheet <https://www.vishay.com/docs/63399/si1308edl.pdf>`_ 

(3): T\ :sub:`Case`\ = 25°C, keep power dissipation, thermal resistance the corresponding temperature rise ΔT and switching losses in PWM mode in mind!

.. note:: Keep in mind that realistically these maximum ratings can't be reached in practice due to thermal limitations. For example the motor current can probably not exceed 1.5A for continuous operation.


Input Stage
==============================

.. figure:: ../../../../svg/hw/USB/Input_Stage.svg
   :align: center
   :alt: Input Stage

   Input stage

D2 Rectifies the DCC square-wave into VDC, the amplitude is not critical as long as it's within the 25V limit. The rectified voltage VDC is then provided to the 3.3V voltage regulator and to the motor driver. The shottky diode D1 and the bridge rectifier D2 ensure a logical "or" linkage of the VBUS supply voltages and the DCC input voltage, meaning that the decoder can be powered either by the DCC track signal or by the VBUS supply voltage which is supplied via USB connector. 

The MOSFET on the right effectively translates the rather high DCC voltage to a suitable level for the microcontroller to process. When Q1 is turned off GPIO21 is pulled high by the RP2040 internal pull-up resistor, as soon as Q1 gets turned on (V\ :sub:`GS`\ > V\ :sub:`GS,th`\), GPIO21 gets pulled to GND potential. R1 and R2 ensure that V\ :sub:`GS`\ doesn't exceed ±12V.

Motor Driver
==============================

.. figure:: ../../../../svg/hw/USB/Motor_Driver.svg
   :align: center
   :alt: Motor Driver

   Motor driver

The Motor Driver is a special H-Bridge IC that is made for DC motors. To control the motor, it is necessary to measure the back-EMF voltage, which is directly related to the motor's speed. This is done by the ADC, which measures at Points ADC_EMF_A and ADC_EMF_B. The two resistors work together as a voltage divider.

Auxiliary Outputs
==============================

.. figure:: ../../../../svg/hw/USB/Aux_Outputs.svg
   :align: center
   :alt: Auxiliary Outputs

   Auxiliary Outputs

Switching of loads can be done using four N-Channel MOSFETs (Q2 to Q5). The amount of current is somewhat dependend on either static R\ :sub:`DS(on)`\  losses and/or switching losses using PWM. They are switching on the low-side, which means the load effectively gets connected to GND potential as the transistor starts to conduct. GPIO24 - GPIO27 are used.

GPIO
==============================

To switch lighter loads, six GPIO soldering pads are connected to GPIO0 to GPIO5 pins. Although not yet set up in the software, they could be used as inputs as well (e.g. reed switch). GPIO0 and GPIO1 can also be used to facilitate stdio logging via UART, which can easily be set up in CMakeLists.txt.

Microcontroller
==============================

.. figure:: ../../../../svg/hw/USB/Microcontroller.svg
   :align: center
   :alt: Microcontroller

   Microcontroller, flash memory, crystal oscillator, USB connector, ...

The Microcontroller needs a crystal oscillator for its clock. External flash memory holds the configuration variables and the program itself. The USB connector can be used to flash the software onto the decoder and also for logging via stdio (can be enabled via CMakeLists.txt). D3 is used as a status LED. R11 & C13 are used to filter ADC_AVDD (ADC Reference Voltage).

Complete Schematic
==============================

.. figure:: ../../../../svg/hw/USB/RP2040-Decoder.svg
   :align: center
   :alt: Complete Schematic

   Complete Schematic


.. _wiring_USB:

Wiring the Decoder
==============================

.. figure:: ../../../../svg/hw/USB/Wiring_diagram.svg
    :width: 700
    :alt: Wiring Diagram
    :align: center

    Wiring Diagram

The illustration above provides an example of a wiring configuration.
It supports the connection of up to 4 transistor-switched outputs and 6 GPIOs for lighter loads or custom applications.
When switching inductive loads, always include a free-wheeling diode to protect the circuit.