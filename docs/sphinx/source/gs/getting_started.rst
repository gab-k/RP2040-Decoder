Getting started
==================================

1. Where to get the board?
----------------------------------

You can choose any PCB manufacturer. The PCB will be designed to fit the capabilities and constraints of JLCPCB.com.
You have 3 options for board assembly from the most difficult to the easiest:

1. You can assemble the board yourself, but it's best not to do so if you're not familiar with soldering small components. You order the board with the necessary parts and solder the parts. The parts are available at LCSC.com, they supply parts to JLCPCB.com.
2. Order the PCB from any PCB manufacturer and have only the top side assembled. This way You have to separately order the parts for the bottom part from LCSC.com (or any other supplier) and solder just the bottom part yourself. Soldering the back of the PCB by hand is not too difficult.
3. Alternatively, you can order the board fully assembled both on the top and bottom side. This is the easiest, but the most expensive solution.

2. Production files?
----------------------------------
The latest release includes the production files for JLCPCB, which includes the Gerber files, BOM, and Pick and Place files. You can find the latest release `here <https://github.com/gab-k/RP2040-Decoder/releases>`_.

In order to manually export the production files (Gerber/BOM/Pick and Place), you need `KiCad <https://www.kicad.org/download/>`_ which is open source.
In case you are ordering from JLCPCB it is recommended to use the `Fabrication-Toolkit KiCad plugin by bennymeg <https://github.com/bennymeg/Fabrication-Toolkit>`_

.. _building_the_software:

3. Building the software
----------------------------------

If you got a RP2040-Decoder with USB the easiest way to get the RP2040-Decoder up and running is to just use the precompiled .uf2 file in the latest `release <https://github.com/gab-k/RP2040-Decoder/releases>`_ and skip to :ref:`flashing_the_software`.

However sometimes you might want to build by yourself, be it for debugging or to make your own modifications to the source code.
The easiest way to do this is to just use `Visual Studio Code <https://code.visualstudio.com/>`_ and the `Raspberry Pi Pico extension <https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico>`_. 
The `official documentation <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf#vscode-extension>`_ of the Pi Pico does a great job of explaining this in detail.

.. note::
   - Make sure to choose the correct board definition file in your CMakeLists.txt file for example ``RP2040-Decoder-board-Rev-1_0`` for Hardware Rev 1.0.
   - In VSCode open the "Software" subdirectory for the extension to detect you project automatically.

.. _flashing_the_software:

4. Flashing the software
----------------------------------

For RP2040-Decoders with USB there is two ways of flashing the software:

1. Flashing via USB using the .uf2 file by copying it with your preferred file explorer. 
2. Using Serial Wire Debug (SWD)

   2.1. Using another RP2040/RP2350-based board with `debugprobe software <https://github.com/raspberrypi/debugprobe>`_ on it. (Could be the RP2040-Decoder itself)

   2.2. Using any other supported SWD debug interface

For RP2040-Decoders without USB the only way of flashing the software is via SWD

4.1. Flashing the software via .uf2 file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
When first plugged in the RP2040 boots into Mass Storage Device mode. To flash the software, you need to drag the '.uf2' file onto the disk using any file explorer.
This will write the file to the Flash and restart the RP2040.
In case you want to repeat the flashing process, hold the button SW1 while re-plugging the decoder into your computer. The decoder should now show as a Mass Storage Device again.
This procedure is also described in the `Raspberry Pi Pico Datasheet Chapter 4.1 <https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf#_programming_the_flash>`_

.. _flashing_the_software_swd:

4.2. Flashing the software via SWD
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Flashing via SWD is a little more complicated but also comes with great advantages in terms of debugging.
The book on `Getting started with Raspberry Pi Pico-series <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf>`_ does a great job of explaining how debugging works.
As already suggested in :ref:`building_the_software` it is recommended to use the `Visual Studio Code Extension <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf#vscode-extension>`_.
The most relevant sections in the book are `Chapter 4.3. Debug <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf#_debug>`_ and `Appendix A: Debugprobe <https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf#debugprobe_section>`_


5. Wiring
----------------------------------

| Please refer to the hardware section for your board:
| :ref:`Wiring Rev 1.0 <wiring_rev_1_0>`
| :ref:`Wiring Rev 0.3 <wiring_rev_0_3>`

6. Resolving issues
----------------------------------

If the decoder is not working as expected:

1. Follow the `Troubleshooting instructions <https://github.com/gab-k/RP2040-Decoder/wiki/Troubleshooting>`_
2. If you still have trouble, create a new `discussion <https://github.com/gab-k/RP2040-Decoder/discussions>`_ or `issue <https://github.com/gab-k/RP2040-Decoder/issues>`_ and describe your question/issue.
