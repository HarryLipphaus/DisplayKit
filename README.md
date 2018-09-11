# OledLib

<img src="https://github.com/HarryLipphaus/OledLib/blob/master/images/6line-graphic.jpg" width="240" height="160">
This library was originaly inspired by Michael Köhlers SSD1306-Library, which is discussed her:

https://www.mikrocontroller.net/topic/415651

Now this is a nearly complete rewrite of the above library and it compiles on AVR (8bit) AND on STM32/HAL without any changes.

On AVR you can choose between pure text-mode with less than 2kB of Flash and 21 byte of RAM consumption and graphics-mode, which increases RAM consumption by 1026 byte.

The API ist widely compatibel with Peter Fleury's HD44780-Library here:

http://homepage.hispeed.ch/peterfleury/avr-software.html

This makes porting from the popular 20x4-LCDs very easy.

The Library may be configured for 4-, 6- and 8-line mode.
The 4- and 6-line mode requires graphic-mode.

All configuration is done in oled_config.h

Datasheet for SSD1306:

https://cloud.it-livetalk.de/index.php/s/AFcVnulocRBnGUV
