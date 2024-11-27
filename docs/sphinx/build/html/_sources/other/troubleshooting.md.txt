# Troubleshooting

To narrow down the issue when your decoder is not working as expected, you can do the following:

Check the power supply of the decoder:
* +3V3 to GND should be 3.3V
* +V to GND should be somewhere in the range of around +10V to +20V
* If you dont measure the correct voltages, try to [measure Track Voltage](https://dccwiki.com/Measuring_Track_Voltage)

Check your motor:
* Desolder your motor and try the motor with a analog power supply 0...+-16V to check if the motor is working or not (desolder the wires because the decoder doesn't like voltage on these pins!)

Reset:
* Try to reset the decoder by writing a value of 8 to CV8 (beware, this will reset the complete CV memory!)