/*
 * oled_defines.h
 *
 *  Created on: 16.08.2018
 *      Author: Harry L.
 */

#ifndef OLED_CONFIG_H_
#define OLED_CONFIG_H_

/************************************************************************************
 *
 * Global Oled-Library configuration-defaults
 *
 ************************************************************************************/


/*
 * I²C-address
 */
#define OLED_I2C_ADR  0x78

/*
 * uncomment to increase I²C-speed from 100kHz to 400kHz
 * on STM32 this line has no effect
 */
// #define I2C_HIGHSPEED

/*
 * initial contrast
 */
#define OLED_CONTRAST	0x8f

/*
 * 128 pixel has enough room for 21 characters
 * for compatibility to the popular 20x4-displays the width is limited to 20 chars
 * the whole line ist centered by adding 4 empty pixel on both sides
 * uncomment, if you realy want to use 21 caracters
 */
//#define OLED_21COL

/*
 * number of text-lines on display
 *
 */
#define OLED_LINES	6	// 4, 6 or 8 Lines- 6 Lines uses GRAPHICMODE
				// any value other than 8 automaticaly enables graphic-Mode

/*
 * uncomment if you want to use graphics
 * on AVR this increases RAM-consumption by 1026 byte
 * on STM32 graphic-mode is the default - this line has no effect
 */
//#define GRAPHICMODE

/*
 * choose used Display-Controller
 * uncomment for SH1106 - default is SSD1306
 *
 */
// #define SH1106

#endif /* OLED_CONFIG_H_ */
