# Getting started
In order to make use of this project, you will need the following:
 - The RP2040-Decoder board itself
 - Raspberry Pi Pico or another multi-drop SWD debug interfaces might work as well
 - Micro-USB cable

## 1. Where to get the board?
You can of course choose any preferred PCB manufacturer, the PCB is designed to be compliant with JLCPCB.com 2 layer board capabilities (hole diameter/clearence/etc.). Regarding assembly, you can of course assemble the board yourself but I would not recommend it (especially the RP2040 QFN Package is not easy to solder). Alternatively you can order the board fully or partly assembled. All parts are normally available at LCSC.com who supply parts to JLCPCB.com.

## 2. Production files?
In order to generate export the production files (Gerber/BOM/Pick and Place) you need [KiCad](https://www.kicad.org/download/) 6 which is open source.
[How to do this?](https://jlcpcb.com/help/article/362-how-to-generate-gerber-and-drill-files-in-kicad-7)

## 3. Building and flashing

To build the software and run it on the Controller we need to interface the RP2040 using Serial Wire Debug. This can be done using a Raspberry Pi Pico board.

### Windows


1. Connect Raspberry Pi Pico via USB while holding the reset button. Your Pi Pico should now be visible in Windows explorer as a mass storage device.
2. Download the latest prebuilt "picoprobe.uf2" release binary from [here](https://github.com/raspberrypi/picoprobe/releases) or download it from the [Raspberry Pi Website](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html#debugging-using-another-raspberry-pi-pico). 
3. Copy "picoprobe.uf2" to your Pi Pico. The Pi Pico onboard LED should now light up.
4. Connect GND to GND, GPIO2 to SWCLK, GPIO3 to SWDIO, VBUS to +V, as shown in the picture below .
<img src="https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/Debug_Connections.svg" width="700"/>

5. Download and install [Pico setup for Windows](https://github.com/raspberrypi/pico-setup-windows)
6. Download and install [MSYS2](https://www.msys2.org/)
7. Open a MSYS2 MINGW64 shell window
8. Update all pacman packages
```sh
pacman -Syu
``` 
9. Install mingw-w64-x86_64-toolchain (choose to install all)
```sh
pacman -S mingw-w64-x86_64-toolchain
``` 
10. Add "C:\msys64\mingw64\bin" to your users PATH environment variable. If you have choosen to not use the default directory for MSYS2 add the one you have choosen. [How to do this?](https://www.computerhope.com/issues/ch000549.htm)
11. Open the Pico - Developer Powershell
12. Navigate to a folder you like and clone the project repository 
```sh
git clone https://github.com/gab-k/RP2040-Decoder
``` 
13. Enter cloned repository, create and enter build directory
```sh
cd RP2040-Decoder
mkdir build
cd build
``` 
14. Run CMake and the mingw32-make to build
```sh
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
mingw32
``` 
15. Flash the decoders software using openocd
```sh
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c"program rp2040-decoder.elf verify reset exit"
``` 

### Other OS

The first four steps are identical to the steps described above for windows users. Regarding building and flashing, please refer to [Getting started - Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf). Chapter 3 & 5 and Appendix A are the most relevant but you might also refer to chapter 9.1 if you are using macOS.


## 4. Wiring and initial startup
<img src="https://github.com/gab-k/RP2040-Decoder/blob/main/docs/svg/Wiring_diagram.svg" width="700"/>

The illustration above shows an example wiring configuration, you can connect up to 4 outputs(up to 400mA) and 6 GPIOs for lighter loads or custom applications. A Free-wheeling diode should be used when switching inductive loads!

Another important note is that when the decoder is put into operation for the first time, it automatically performs a motor calibration, which is required for feedforward control. The calibration takes about one to two minutes.
 
## 5. If it's not working as expected

1. Follow the [Troubleshooting instructions](https://github.com/gab-k/RP2040-Decoder/wiki/Troubleshooting)
2. If you still have trouble create a new [discussion](https://github.com/gab-k/RP2040-Decoder/discussions) or [issue](https://github.com/gab-k/RP2040-Decoder/issues) and describe your question/issue.