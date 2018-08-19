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
 *
 *
 *  14.08.2018 Harry L.
 *  added STM32/HAL Support
 *
 *  i2c.c and i2c.h renamed to i2c_master.c and i2c_master.h
 *  to avoid conflicts with foreign i2c-routines
 *
 *  replaced YES/NO with TRUE/FALSE
 *
 *  corrected NORMAL/INVERSE switching
 *
 *  corrected lcd_init()
 *  there is no way to write to PROGMEM
 *  LCD_DISP_ON moved to init_sequence
 *
 */

#ifdef __ARM_ARCH
#include "main.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#endif

#include "lcd.h"
#include <string.h>

#ifdef GRAPHICMODE
#include <stdlib.h>

FrameBuffer_t  displayBuffer;		// Framebuffer only needed or GRAPHICMODE
#endif

#if (OLED_LINES != 8)
const static struct PROGMEM
{
	uint8_t vline;
	uint8_t shift;
} shiftMap[OLED_LINES] =
#if (OLED_LINES == 4)
		{{0,4},
		{2,4},
		{4,4},
		{6,4}};
#elif (OLED_LINES == 6)
		{{0,3},
		{1,5},
		{2,7},
		{4,1},
		{5,3},
		{6,5}};
#endif
#endif

#ifdef __ARM_ARCH		// some variables only needed on ARM

/*
 * the I²C-handle used y the driver
 */
static I2C_HandleTypeDef *oled_hi2c;

/*
 * signal the DMA-callback that a custom command is waiting for insertion
 */
static volatile uint8_t cmdPending;

/*
 * temporary buffer for inserting custom OLED-commands
 * in DMA-transfer-sequence
 */
static struct
{
	uint8_t cnt;
	uint8_t prefix;
	uint8_t cmd[2];
} __attribute__((packed)) cmdBuf;
#endif

static struct
{
	uint8_t x;
	uint8_t y;
} cursor;

typedef struct
{
	char c;		// char-code
	uint8_t idx;		// index in font-table
} fnt_map_t;

#include "ascii6x8.fnt"

/*
 *
 * Initialization Sequence
 *
 */
const uint8_t initSequence[] PROGMEM =
{

// Address settings
		OLED_MEMORYMODE, 0x0,			// Set Memory Addressing Mode
										// 00=Horizontal Addressing Mode;
										// 01=Vertical Addressing Mode;
										// 10=Page Addressing Mode (RESET); 11=Invalid
		OLED_PAGESTART,					// Set Page Start Address for Page Addressing Mode, 0-7
		OLED_SETLOWCOLUMN,				// --set low column address
		OLED_SETHIGHCOLUMN,				// --set high column address
		OLED_COLUMNADDR, 0, (OLED_WIDTH - 1),

// Fundamental Commands
		OLED_SETCONTRAST, OLED_CONTRAST,// Set contrast control register
		OLED_NORMALDISPLAY,				// Set display mode. A6=Normal; A7=Inverse
		OLED_DISPLAY_RESUME,			// Output RAM to Display
		OLED_DISPLAYOFF,					// Display OFF (sleep mode)

// Hardware configuration
		OLED_SETSTARTLINE,				// set display startline (0x40)
		OLED_SEGREMAP | 1,				// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
		OLED_SETMULTIPLEX, 0x3f,		// Set Multiplex Ratio
		OLED_COMSCANDIR,				// Set COM Output Scan Direction
		OLED_SETDISPLAYOFFSET, 0x00,	// Set display offset. 00 = no offset
		OLED_SETCOMPINS, 0x12,			// Set com pins hardware configuration

// Timing and Driving
		OLED_SETDISPLAYCLOCKDIV, 0xF0,  // --set display clock divide ratio/oscillator frequency
		OLED_SETPRECHARGE, 0x22,		// Set pre-charge period
		OLED_SETVCOMDETECT,	0x20,		// --set vcomh

// Chargepump
		OLED_CHARGEPUMP, 0x14			// Set DC-DC enable

		};

/*
 * AVR specific I²C-functions
 */
#ifndef __ARM_ARCH
void lcd_command(uint8_t cmd[], uint8_t size, uint8_t fromFlash)
{
	i2c_start(OLED_I2C_ADR);
	i2c_byte(OLED_CMD_PREFIX);	// 0x80 for command
	for (uint8_t i = 0; i < size; i++)
	{
		if (fromFlash == TRUE)
			i2c_byte(pgm_read_byte(&cmd[i]));
		else
			i2c_byte((uint8_t)cmd[i]);
	}
	i2c_stop();
}

void lcd_data(uint8_t data[], uint16_t size, uint8_t fromFlash)
{
	i2c_start(OLED_I2C_ADR);
	i2c_byte(OLED_DTA_PREFIX);	// 0x40 for data
	for (uint16_t i = 0; i < size; i++)
	{
		if (fromFlash == TRUE)
			i2c_byte(pgm_read_byte(&data[i]));
		else
			i2c_byte(data[i]);
	}
	i2c_stop();
}
#endif

/*
 * initialize the display
 * on STM32 you have to put the I²C-handle in the parameter
 */
#ifdef __ARM_ARCH
void
lcd_init (uint8_t dispAttr, I2C_HandleTypeDef *hi2c)
{
	oled_hi2c = hi2c;
	displayBuffer.prefix = OLED_DTA_PREFIX;
	cmdBuf.prefix = OLED_CMD_PREFIX;
	cmdPending = FALSE;
	// enshure that I2C is ready
	while (HAL_I2C_GetState (oled_hi2c) != HAL_I2C_STATE_READY)
	;
	// send init-sequence
	HAL_I2C_Master_Transmit_DMA (oled_hi2c, OLED_I2C_ADR,
			(uint8_t *) &initSequence,
			sizeof(initSequence));

#else
void lcd_init(uint8_t dispAttr)
{

	i2c_init();
	lcd_command((void *)initSequence, sizeof(initSequence), TRUE);
#endif

	cursor.x = cursor.y = 0;
	if (dispAttr)			// turn display on
		lcd_on(TRUE);

	// clear FB
	lcd_clrscr();
}

#ifdef __ARM_ARCH
/*
 * Callback function is called on any successfull DMA-trasfer
 */
void
HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef *hi2c)
{
	if (hi2c == oled_hi2c)
	{
		if (cmdPending)	//inject command
		{
			HAL_I2C_Master_Transmit (oled_hi2c, OLED_I2C_ADR,
					(uint8_t *) &cmdBuf.prefix, cmdBuf.cnt, 5);
			cmdPending = FALSE;
		}
#ifndef SH1106
		HAL_I2C_Master_Transmit_DMA (oled_hi2c, OLED_I2C_ADR,
				(uint8_t *) &displayBuffer,
				sizeof(displayBuffer));
#else
// Todo: Code for SH1106 update
#endif
	}
}
#endif

/*
 * maps char to index of font-table
 * if char not found, 0xff is returned
 */
static uint8_t map_char2fnt(char c)
{
	uint8_t i, idx;
	if ((c >= 0x20) && (c <= 0x7f))
		idx = (uint8_t) c - 0x20;
	else
	{
		for (i = 0;
				(pgm_read_byte(&fnt_map[i].idx) != 0Xff)
						&& (pgm_read_byte(&fnt_map[i].c) != c); i++)
			;
		idx = pgm_read_byte(&fnt_map[i].idx);
	}
	return idx;
}

/*
 * print a single character on display
 *
 */
void lcd_putc(char c)
{
	uint8_t fnt_idx;

	switch (c)
	{
	case '\r':			// cariagereturn
		cursor.x = 0;
		lcd_gotoxy(cursor.x, cursor.y);
		break;

	case '\n':			// linefeed
		if (++cursor.y == OLED_VLINES)
			cursor.y = 0;
		lcd_gotoxy(cursor.x, cursor.y);
		break;

	case '\b':			// backspace
		if (cursor.x)
		{
			cursor.x--;
			lcd_gotoxy(cursor.x, cursor.y);
		}
		break;

	default:			// regular char
		fnt_idx = map_char2fnt(c);
		if (fnt_idx != 0xff)
		{
#ifdef GRAPHICMODE
#if (OLED_LINES == 8)
			memcpy_P(&displayBuffer.buf[cursor.y][(cursor.x * OLED_FONT_WIDTH) + OLED_HSHIFT],
					&oled_font6x8[fnt_idx], OLED_FONT_WIDTH);
#else
		for (uint8_t i=0;i < OLED_FONT_WIDTH; i++)
		{
			displayBuffer.buf[pgm_read_byte(&shiftMap[cursor.y].vline)][(cursor.x * OLED_FONT_WIDTH) + OLED_HSHIFT + i] &=
					(0xff >> (8 - pgm_read_byte(&shiftMap[cursor.y].shift)));
			displayBuffer.buf[pgm_read_byte(&shiftMap[cursor.y].vline)][(cursor.x * OLED_FONT_WIDTH) + OLED_HSHIFT + i] |=
					 (pgm_read_byte(&(oled_font6x8[fnt_idx][i])) << pgm_read_byte(&shiftMap[cursor.y].shift)) ;
			displayBuffer.buf[pgm_read_byte(&shiftMap[cursor.y].vline) + 1][(cursor.x * OLED_FONT_WIDTH) + OLED_HSHIFT + i] &=
					(0xff << (pgm_read_byte(&shiftMap[cursor.y].shift)));
			displayBuffer.buf[pgm_read_byte(&shiftMap[cursor.y].vline) + 1][(cursor.x * OLED_FONT_WIDTH) + OLED_HSHIFT + i] |=
					(pgm_read_byte(&(oled_font6x8[fnt_idx][i])) >> (8 - pgm_read_byte(&shiftMap[cursor.y].shift)));
		}

#endif
#else
			lcd_data((void *) &oled_font6x8[fnt_idx], OLED_FONT_WIDTH, TRUE);	// print font to ram, print 6 columns
#endif

			/*
			 * advance cursor to next char-position
			 * wraps to next line, if EOL
			 * wraps to start (0,0) on end of last line
			 */
			if (++cursor.x >= OLED_COLUMS)
			{
				cursor.x = 0;
				if (++cursor.y >= OLED_LINES)
				{
					cursor.y = 0;
				}
				lcd_gotoxy(cursor.x, cursor.y);
			}
		}
		break;
	}
}

void lcd_puts(const char* s)
{
	while (*s)
	{
		lcd_putc(*s++);
	}
}

#ifndef __ARM_ARCH
void lcd_puts_p(const char* progmem_s)
{
	register uint8_t c;
	while ((c = pgm_read_byte(progmem_s++)))
	{
		lcd_putc(c);
	}
}
#endif

/*
 * set txt-cursor to left upper corner
 */
void lcd_home(void)
{
	lcd_gotoxy(0, 0);
}

void lcd_clrscr(void)
{
#ifdef GRAPHICMODE
	memset(&displayBuffer.buf, 0x00, sizeof(displayBuffer.buf));
#else

	uint8_t tmpLine[OLED_WIDTH];
	memset(tmpLine, 0, OLED_WIDTH);

#ifdef SH1106
	/*      for (uint8_t j=0; j< OLED_WIDTH; j++)
	 {
	 actualLine[j]=displayBuffer.buf[i*OLED_WIDTH+j];

	 }*/

#else

	lcd_gotoxy(0, 0);
	lcd_data(tmpLine, OLED_WIDTH, FALSE);
	for (uint8_t i=0; i < OLED_VLINES; i++)
	{
		lcd_data(tmpLine, OLED_WIDTH, FALSE);
	}
#endif
#endif
	lcd_home();
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{
	if ((x >= OLED_COLUMS) || (y >= OLED_LINES))
		return;	// out of display
	cursor.x = x;
	cursor.y = y;
#ifndef __ARM_ARCH
#if defined SH1106
	uint8_t commandSequence[] =
	{	OLED_PAGESTART + y,
		OLED_COLUMNADDR, 0x00+((2+x) & (0x0f)), 0x10+( ((2+x) & (0xf0)) >> 4 ), 0x7f};
};
#else
#ifndef GRAPHICMODE
	uint8_t commandSequence[] =
	{
	OLED_PAGESTART + y,
	OLED_COLUMNADDR, ((x * OLED_FONT_WIDTH) + OLED_HSHIFT), (OLED_WIDTH - 1),
	OLED_SETLOWCOLUMN + (((x * OLED_FONT_WIDTH) + OLED_HSHIFT) & 0x0f),			// lower nibble
	OLED_SETHIGHCOLUMN + ((((x * OLED_FONT_WIDTH) + OLED_HSHIFT) & 0xf0) >> 4),	// upper nibble
	};
	lcd_command(commandSequence, sizeof(commandSequence), FALSE);
#endif
#endif
#endif
}

#ifdef GRAPHICMODE
void lcd_display()
{
#ifndef __ARM_ARCH
#ifdef GRAPHICMODE
#ifdef SH1106
	for (uint8_t i=0; i < OLED_HEIGHT/8; i++)
	{
		lcd_gotoxy(0, i);
		uint8_t actualLine[OLED_WIDTH];
		memcpy_P(actualLine, dsplayBuffer.buf[i], OLED_WIDTH):
		lcd_data(actualLine, sizeof(actualLine));
	}
#else
	lcd_data((uint8_t *)displayBuffer.buf , sizeof(displayBuffer.buf), FALSE);

#endif
#endif
#endif
}

#endif

/*
 * invert the whole display
 * swaps black and white
 */
void lcd_invert(uint8_t invert)
{
#ifdef __ARM_ARCH
	while (cmdPending)
	;
	cmdBuf.cnt = 2;
	if (invert == TRUE)
	  cmdBuf.cmd[0] = OLED_INVERTDISPLAY;
	else
	  cmdBuf.cmd[0] = OLED_NORMALDISPLAY;
	cmdPending = TRUE;
#else
	uint8_t commandSequence[1];
	if (invert == TRUE)
	{
		commandSequence[0] = OLED_INVERTDISPLAY;
	}
	else
	{
		commandSequence[0] = OLED_NORMALDISPLAY;
	}
	lcd_command(commandSequence, sizeof(commandSequence), FALSE);
#endif
}

/*
 * control the display-contrast
 * te current-consumption of the OLED increases with conrast
 * valid range: 0-255
 */
void lcd_set_contrast(uint8_t contrast)
{
#ifdef __ARM_ARCH
	while (cmdPending)
	;
	cmdBuf.cnt = 3;
	cmdBuf.cmd[0] = OLED_SETCONTRAST;
	cmdBuf.cmd[1] = contrast;
	cmdPending = TRUE;

#else
	uint8_t commandSequence[] =
	{ OLED_SETCONTRAST, contrast };
	lcd_command((void *)commandSequence, sizeof(commandSequence),FALSE);
#endif
}

void lcd_on(uint8_t onoff)
{
#ifdef __ARM_ARCH
	while (cmdPending)
	;
	cmdBuf.cnt = 2;
	if (onoff == TRUE)
	  cmdBuf.cmd[0] = OLED_DISPLAYON;
	else
	  cmdBuf.cmd[0] = OLED_DISPLAYOFF;
	cmdPending = TRUE;
#else
	uint8_t commandSequence[1];
	if (onoff == TRUE)
	{
		commandSequence[0] = OLED_DISPLAYON;
	}
	else
	{
		commandSequence[0] = OLED_DISPLAYOFF;
	}
	lcd_command(commandSequence, sizeof(commandSequence), FALSE);
#endif

}
