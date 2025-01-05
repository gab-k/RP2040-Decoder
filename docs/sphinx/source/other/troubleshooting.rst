Troubleshooting
================

To narrow down the issue when your decoder is not working as expected, you can do the following (in no particular order):

Enable logging and or debug via SWD
------------------------------------
Log messages can be enabled and output via either UART or USB please see :ref:`logging` for details.
For more advanced users it might also be beneficial to :ref:`debug with SWD <debugging_via_swd>`

Checking the power supply:
--------------------------------------

* **+3V3 to GND** should be 3.3V ± 1%
* **VDC/+V to GND** should be somewhere in the range of at least 6V to 23V depending on your track voltage.

If you don't measure the correct voltages, try to `measure your track voltage <https://dccwiki.com/Measuring_Track_Voltage>`_

Check your motor:
-----------------

Desolder your motor and try the motor with an appropriate power supply and DC voltage, to check if the motor is working or not.

CV Reset:
------------

Try to reset the decoder by writing a value of ``8`` to ``CV8`` (beware, this will reset the complete CV memory!)
