/*
 * This file is part of lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any later version.
 *
 * lcd library for ssd1306/sh1106 oled-display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * lcd library for ssd1306/sh1106 oled-display wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 *  lcd.h
 *
 *  Created by Michael Köhler on 22.12.16.
 *  Copyright 2016 Skie-Systems. All rights reserved.
 *
 *  lib for OLED-Display with ssd1306/sh1106-Controller
 *  first dev-version only for I2C-Connection
 *  at ATMega328P like Arduino Uno
 *
 *  at GRAPHICMODE lib needs SRAM for display
 *  DISPLAY-WIDTH * DISPLAY-HEIGHT + 2 bytes
 */

#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif


#if (__GNUC__ * 100 + __GNUC_MINOR__) < 303
#error "This library requires AVR-GCC 3.3 or later, update to newer AVR-GCC compiler !"
#endif

#ifndef __ARM_ARCH
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "i2c_master.h"                            // library for I2C-communication
                                            // if you want to use other lib for I2C
                                            // edit i2c_xxx commands in this library
                                            // i2c_start(), i2c_byte(), i2c_stop()
#endif

#include <oled_config.h>

/************************************************************************************
 *
 *
 ************************************************************************************/

#ifdef SH1106
#define OLED_WIDTH	132				// SH1106
#define OLED_RAM_WIDTH	160				// bytes per line in OLED-RAM
#else
#define OLED_WIDTH	128				// SSD1306 is 128 x 64
#define OLED_RAM_WIDTH	128				// bytes per line in OLED-RAM
#endif

#define OLED_HEIGHT	64				// height in pixel

#define OLED_VLINES	(OLED_HEIGHT / 8)		// height in memory lines
#define DISPLAYSIZE	(OLED_WIDTH * OLED_VLINES)	// overall size in bytes

#define OLED_FONT_WIDTH	6

/*
 * Commands
 * See datasheet
 *
 */
#define OLED_DTA_PREFIX			0x40
#define OLED_CMD_PREFIX			0x0

#define OLED_SETCONTRAST 		0x81
#define OLED_DISPLAY_RESUME 	0xA4
#define OLED_DISPLAYALLON 		0xA5
#define OLED_NORMALDISPLAY 		0xA6
#define OLED_INVERTDISPLAY 		0xA7
#define OLED_DISPLAYOFF 		0xAE
#define OLED_DISPLAYON 			0xAF

#define OLED_SETDISPLAYOFFSET 		0xD3
#define OLED_SETCOMPINS 		0xDA

#define OLED_SETVCOMDETECT 		0xDB

#define OLED_SETDISPLAYCLOCKDIV 	0xD5
#define OLED_SETPRECHARGE 		0xD9

#define OLED_SETMULTIPLEX 		0xA8

#define OLED_SETLOWCOLUMN 		0x00
#define OLED_SETHIGHCOLUMN 		0x10

#define OLED_SETSTARTLINE 		0x40

#define OLED_MEMORYMODE 		0x20
#define OLED_COLUMNADDR 		0x21
#define OLED_PAGEADDR   		0x22
#define OLED_PAGESTART			0xB0

#define OLED_COMSCANINC 		0xC0
#define OLED_COMSCANDIR 		0xC8

#define OLED_SEGREMAP 			0xA0

#define OLED_CHARGEPUMP 		0x8D

#define OLED_EXTERNALVCC 		0x01
#define OLED_SWITCHCAPVCC 		0x02

#define	OLED_NOP				0xe3

// Scrolling #defines
#define OLED_ACTIVATE_SCROLL 				0x2F
#define OLED_DEACTIVATE_SCROLL 				0x2E
#define OLED_SET_VERTICAL_SCROLL_AREA 			0xA3
#define OLED_RIGHT_HORIZONTAL_SCROLL 			0x26
#define OLED_LEFT_HORIZONTAL_SCROLL 			0x27
#define OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x29
#define OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	0x2A

#ifdef __ARM_ARCH			// if running on ARM always use GRAPHICMODE
#define GRAPHICMODE			// STM32 always uses graphic-mode
#endif

#ifdef OLED_21COL
#define OLED_HSHIFT	1
#define OLED_COLUMS	21
#else
#define OLED_HSHIFT	4
#define OLED_COLUMS	20
#endif

#if (OLED_LINES == 4) || (OLED_LINES == 6)
#define GRAPHICMODE
#elif (OLED_LINES != 8)
#error "Ivalid line-count in OLED_LINES - valid values are: 4, 6 and 8"
#endif


#define WHITE		0x01
#define BLACK		0x00

#ifdef GRAPHICMODE

typedef struct {
#ifdef __ARM_ARCH
  uint8_t prefix;
#endif
  uint8_t buf[OLED_VLINES][OLED_WIDTH];
} __attribute__((packed)) FrameBuffer_t;

extern FrameBuffer_t	displayBuffer;		// Framebuffer only needed or GRAPHICMODE

#endif

#ifdef __ARM_ARCH
/*
 * ARM-speciffic macros and functions
 */
#define PROGMEM
#define pgm_read_byte(a)	(*(a))
#define PSTR(a)				(a)
#define lcd_puts_p(a)		lcd_puts(a)
#define memcpy_P(a,b,c)		memcpy(a,b,c)

void lcd_init (uint8_t dispAttr, I2C_HandleTypeDef *hi2c);

#else
/*
 *  AVR specific stuff
 */
void lcd_init(uint8_t dispAttr);
/*
 * I²C lowlevel functions
 */
void lcd_command(uint8_t cmd[], uint8_t size, uint8_t fromflash);	// transmit command to display
void lcd_data(uint8_t data[], uint16_t size, uint8_t fromflash);	// transmit data to display
#endif

/*
 * character writing
 */
void lcd_putc(char c);							// print character on screen at TEXTMODE
												// at GRAPHICMODE print character to buffer
void lcd_puts(const char* s);					// print string, \n-terminated, from ram on screen (TEXTMODE)
// or buffer (GRAPHICMODE)
void lcd_puts_p(const char* progmem_s);			// print string from flash on screen (TEXTMODE)
// or buffer (GRAPHICMODE)

/*
 * display control
 */
void lcd_home(void);                        	// set cursor to 0,0
void lcd_clrscr(void);							// clear screen (and buffer at GRFAICMODE)
void lcd_gotoxy(uint8_t x, uint8_t y);			// set curser at pos x, y. x means character,
void lcd_invert(uint8_t invert);				// invert display
void lcd_set_contrast(uint8_t contrast);		// set contrast for display
void lcd_on(uint8_t onoff);							// turn ddisplay on and off

#if defined GRAPHICMODE
void lcd_display(void);							// copy buffer to display RAM

/*
 * simple graphic-functions
 */
void lcd_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void lcd_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void lcd_drawRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color);
void lcd_fillRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color);
void lcd_drawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color);
void lcd_fillCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color);
void lcd_drawBitmap(uint8_t x, uint8_t y, const uint8_t picture[], uint8_t width, uint8_t height, uint8_t color);
#endif

#ifdef __cplusplus
}
#endif

#endif /*  OLED_H  */
