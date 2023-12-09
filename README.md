LGT8FX8P ISP programmer using FT232R in bitbanging mode.

**ATTENTION:**  this is not a working version; try it at your own risk.


When the test program (HEX format) flashed with the bootloader or the LGTISP here's the result:

![Screenshot1](LGTISP.JPG)

When converted to BIN and flashed with the FT232R here's the result:

![Screenshot1](FT232R.JPG)

The PB5 LED blinks as expected in the two cases.

Can some one, please, geuss what's going wrong in the 2nd case (using FT232R)? 

Usage:

  1- compile this program using mingw, dont' forget ftd2xx.lib and ftd2xx.h
  
  you get : lgtdude.exe
  
  2- build then convert your LGT program to flat binary format using avr-objcopy:
  
  avr-objcopy -I ihex -O binary myprog.hex myprog.bin
  
  3-run lgtdude
  
  lgtdude.exe myprog.bin

NOTE:

 TX  ==> clk
 
 RX ==>  reset 
 
 DTR ==>  data

PLEASE: if you can help drop a word to malaouar67@gmail.com
=========================================================

Resources:

https://github.com/LGTMCU/LarduinoISP

https://github.com/nicechao/ISP4LGT8F328P

https://oshwhub.com/brother_yan/LGTISP

https://github.com/SuperUserNameMan/LGTISP

