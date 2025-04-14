Software Description
==============================

Overview
------------------------------

Software files high level overview:

- **core0.c / core0.h**

   - DCC package detection & decoding
   - Programming mode (Modify and read CVs on programming track)
   - ADC-Offset measurement

- **core1.c / core0.h**

   - PID motor controller
   - Back-EMF voltage measurement

- **shared.c / shared.h** (used by both cores)

   - Error handling
   - Helper functions for retrieving CV's

- **CV.h**
  
   - Default configuration variables

- **CMakeLists.txt**

   - CMake version, SDK version, C standard version
   - Board definitions
   - Compile/Build options
   - Sources files, header files
   - Debug logging configuration (via stdio using UART/USB)
   - Linked libraries

- **RP2040-Decoder-board-USB.h / RP2040-Decoder-board-Legacy.h**
   
   - Flash Size
   - Pin definitions

      - LED pin when available
      - UART_TX/UART_RX pins when used for logging
      - DCC input pin
      - Motor Control PWM pins
      - ADC pins

- **post_build.cmake**
   
   - Post build script for checking whether the size of the binary is within the limits of the flash size minus one sector (used for storing CVs).

- **pico_sdk_import.cmake**
   
   - Helps to locate and import the Pico SDK

The `Raspberry Pi Pico SDK <https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf>`_ is a dependency of the decoder software. The SDK provides abstraction to a higher level, so hopefully, in combination with the comments included in header and source files, the code is easy enough to understand.

Control Loop & Digital Controller
------------------------------------

.. figure:: ../../../svg/sw/Block_Diagram_Control_Loop.svg
   :alt: Block Diagram - Control Loop
   :align: center

   Control Loop - Block diagram

The block diagram above shows the (simplified) control loop. The actual digital controller block is explained below.

The *Speed Helper* block effectively sets the setpoint of the controller according to the CV configured acceleration and deceleration rates and actual target speedstep.

For feedback the back EMF voltage *V_EMF* is measured via voltage divider and ADC. *V_EMF* is proportional to the motor speed.


While the central component is the PID controller, there is an additional block called `Feed-forward <https://en.wikipedia.org/wiki/Feed_forward_(control)>`_ that has an impact on the control output variable. The Feed-forward block adds an additional offset to the output depending on the setpoint; this is done to achieve better control. A more detailed explanation regarding Feed-forward can be found below. The speed helper block corresponds to the ``speed_helper()`` function, which delays changing the setpoint according to the configured deceleration/acceleration rates.

.. figure:: ../../../svg/sw/Block_Diagram_Digital_Controller.svg
   :alt: Block Diagram - Digital Controller
   :align: center

   Digital Controller - Block diagram

The controller is divided into two parts, depending on whether the motor is stationary or in motion.
At all times either the startup controller or the PID controller is active.

The startup controller handles overcoming friction of the motor, and finding a base PWM level for overcoming friction.
The base PWM level is equivalent to a certain PWM duty cycle.
The base level gets saved and added to the output of the PID controller which handles motor control when the motor is in motion.
This means the actual output of the PID controller is the sum of the PID output and the base PWM level. This is referred to as the :ref:`feed forward <feed_forward>` block.
The benefit of doing this is that the PID controller does not have to work in the non linear friction region of the motor, improving control performance.


Startup Controller
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Used on startup, meaning when the motor is not moving. First the startup controller retrieves the last `BASE_PWM_ARR_LEN` base PWM levels.
The base pwm levels are not retained on a power cycle. That means when there is no previous base PWM level saved, the startup controller will start ramping up from 0.
When there is less than `BASE_PWM_ARR_LEN` base PWM levels saved, the startup controller will ramp up from the average of the saved base PWM levels.

After the startup controller retrieved the average of the last base pwm levels :math:`\bar{b}` it sets a PWM duty cycle level of :math:`\frac{2}{3} \cdot \bar{b}` and then ramps up the duty cycle/PWM level until the motor starts moving.
When no previous startup PWM levels are saved the startup controller begins ramping up from 0.
The actual value at which the motor started moving is multiplied by `K_FF` and used as a reference for the :ref:`feed forward <feed_forward>` block.



PID Controller
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: ../../../svg/sw/Block_Diagram_PID.svg
   :alt: PID Controller figure
   :align: center

   PID Controller

The block diagram above shows the PID controller in the s-domain. The derivative part contains an additional digital low-pass filter used to filter out high-frequency noise.

.. math::

   \begin{align}
   \text{Output} &= E \cdot \left(K_P + \frac{K_I}{s} + \frac{K_D \cdot s}{s \tau + 1}\right) = P + I + D \\
   P &= E \cdot K_P \\
   I &= E \cdot \frac{K_I}{s} \\
   D &= E \cdot \frac{K_D \cdot s}{s \tau + 1}
   \end{align}

With :math:`T_s` being the sampling time / controller update rate / time step of the discrete system.
And :math:`\tau` being the filter time constant of the digital filter

The continuous-time system representation can now be transformed into a discrete-time representation using `bilinear transform <https://en.wikipedia.org/wiki/Bilinear_transform>`_ :math:`s = \frac{2 \cdot (z - 1)}{T_s \cdot (z + 1)}`.

This results in the following difference equations which are then implemented in software:

.. math::
   
   \begin{align}
   p_n &= K_P \cdot e_n \\
   i_n &= 0.5 \cdot K_I \cdot \left(e_n + e_{n-1}\right) + i_{n-1} \\
   d_n &= \frac{2 K_D \cdot \left(e_n - e_{n-1}\right) + \left(2 \tau - T_s\right) \cdot d_{n-1}}{2 \tau + T_s} \\
   \text{output}_n &= p_n + i_n + d_n
   \end{align}


.. note::
   - Instead of using the derivative of error, the derivative on measurement is used. This is done to prevent a high derivative part in the event of a large change in setpoint.
   - The previously explained base PWM is added to the actual output as well, meaning the formula above only shows the actual PID control part without feed forward. 


Gain Scheduling
^^^^^^^^^^^^^^^^^^^^

.. figure:: ../../../svg/sw/adaptive_kp.svg
   :alt: Gain Scheduling figure
   :align: center
   
   Gain Scheduling

Another aspect to consider is the implementation of `gain scheduling <https://en.wikipedia.org/wiki/Gain_scheduling>`_. K\ :sub:`P` is a function of the current setpoint. Often it is favorable to have a higher proportional gain K\ :sub:`P` for slow speeds, achieving better control results. The illustration above shows the default setting for K\ :sub:`P`. CV_54 & CV_55 are used to set K\ :sub:`P` @ x\ :sub:`0`, CV_56 & CV_57 for K\ :sub:`P` @ x\ :sub:`1`, and CV_58 & CV_59 for K\ :sub:`P` @ x\ :sub:`2`. Additionally, CV_60 is used to shift x\ :sub:`1` from the leftmost point (0% = 0/255) to the rightmost point (100% = 255/255).


.. _feed_forward:

Feed-forward
^^^^^^^^^^^^^^^^^^

As previously mentioned, to achieve better control, `Feed-forward <https://en.wikipedia.org/wiki/Feed_forward_(control)>`_ is used.
In essence this means that the output of the PID controller is not only the sum of the proportional, integral, and derivative parts, but also the base PWM level.
The base PWM level is set by the startup controller and is the PWM level at which the motor starts moving.


Back-EMF voltage measurement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To provide a feedback signal proportional to the motor speed, the ADC is used to measure the `Back-EMF voltage <https://en.wikipedia.org/wiki/Counter-electromotive_force>`_. The measurement works by setting the PWM duty cycle to 0%, waiting for a certain delay time (CV_62), and then measuring x times (x = CV_63). While measuring, the array with measurement values is sorted using insertion sort. Afterwards, y elements (y = CV_63) from the left (lowest values) and z elements (z = CV_64) from the right (highest values) will be dismissed to mitigate the impact of potential outliers in measurement. The average value of the remaining values will be computed and fed back into the control algorithm. Considering default settings (100us delay, 100 samples, ~2µs sampling time), the complete measurement run, including averaging, takes about 0.3ms to 0.35ms, which effectively reduces the maximum possible duty cycle to about 93% to 94%.

DCC signal decoding
------------------------------

The detection of the DCC signal works by looking at every rising and falling edge and calculating the time between them. When the time between rising and falling edge is greater than 87μs, then this is equivalent to "0"; otherwise, "1". This value then gets shifted into a 64-Bit variable.

Decoding is done after every falling edge. It starts with an error detection, which, when not passed, dismisses the received command. Then the address will be decoded and compared to the address stored in the configuration. If the address matches, the command/instruction will be decoded.

Only a few instructions are currently implemented; only 128 speed step instructions are supported.

**Implemented instructions:**

- ``0011-1111`` - (128 Speed Step Control) - 2 Byte length
- ``10XX-XXXX`` - (Function Group Instruction) (F0 - F12)
- ``110X-XXXX`` - Expansion Instruction  (F13 - F31)

All DCC instructions can be found in Section 9.2, 9.2.1, and 9.2.1.1 of the `NMRA Communications Standard <https://www.nmra.org/index-nmra-standards-and-recommended-practices>`_.
