/*
 * oled_gfx.c
 *
 *  Created on: 17.08.2018
 *      Author: Harry L.
 *  based on code from Michael Köhler
 *  See lcd.c fo details.
 *	simple graphic-functions
 *
 */

#ifdef __ARM_ARCH
#include "main.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "lcd.h"

#ifdef GRAPHICMODE


void
lcd_drawPixel (uint8_t x, uint8_t y, uint8_t color)
{
  if (x > OLED_WIDTH - 1 || y > (OLED_HEIGHT - 1))
    return; // out of Display
  if (color == WHITE)
    {
      displayBuffer.buf[y / 8][x] |=
	  (1 << (y % (OLED_HEIGHT / 8)));
    }
  else
    {
      displayBuffer.buf[y / 8][x] &=
	  ~(1 << (y % (OLED_HEIGHT / 8)));
    }
}

void
lcd_drawLine (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
  if (x1 > OLED_WIDTH - 1 || x2 > OLED_WIDTH - 1 || y1 > OLED_HEIGHT - 1
      || y2 > OLED_HEIGHT - 1)
    return;
  int dx = abs (x2 - x1), sx = x1 < x2 ? 1 : -1;
  int dy = -abs (y2 - y1), sy = y1 < y2 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  while (1)
    {
      lcd_drawPixel (x1, y1, color);
      if (x1 == x2 && y1 == y2)
	break;
      e2 = 2 * err;
      if (e2 > dy)
	{
	  err += dy;
	  x1 += sx;
	} /* e_xy+e_x > 0 */
      if (e2 < dx)
	{
	  err += dx;
	  y1 += sy;
	} /* e_xy+e_y < 0 */
    }
}

void
lcd_drawRect (uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color)
{
  if (px1 > OLED_WIDTH - 1 || px2 > OLED_WIDTH - 1 || py1 > OLED_HEIGHT - 1
      || py2 > OLED_HEIGHT - 1)
    return;
  lcd_drawLine (px1, py1, px2, py1, color);
  lcd_drawLine (px2, py1, px2, py2, color);
  lcd_drawLine (px2, py2, px1, py2, color);
  lcd_drawLine (px1, py2, px1, py1, color);
}

void
lcd_fillRect (uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color)
{
  if (px1 > px2)
    {
      uint8_t temp = px1;
      px1 = px2;
      px2 = temp;
      temp = py1;
      py1 = py2;
      py2 = temp;
    }
  for (uint8_t i = 0; i <= (py2 - py1); i++)
    {
      lcd_drawLine (px1, py1 + i, px2, py1 + i, color);
    }
}

void
lcd_drawCircle (uint8_t center_x, uint8_t center_y, uint8_t radius,
		uint8_t color)
{
  if (((center_x + radius) > OLED_WIDTH - 1)
      || ((center_y + radius) > OLED_HEIGHT - 1) || center_x < radius
      || center_y < radius)
    return;
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;

  lcd_drawPixel (center_x, center_y + radius, color);
  lcd_drawPixel (center_x, center_y - radius, color);
  lcd_drawPixel (center_x + radius, center_y, color);
  lcd_drawPixel (center_x - radius, center_y, color);

  while (x < y)
    {
      if (f >= 0)
	{
	  y--;
	  ddF_y += 2;
	  f += ddF_y;
	}
      x++;
      ddF_x += 2;
      f += ddF_x;

      lcd_drawPixel (center_x + x, center_y + y, color);
      lcd_drawPixel (center_x - x, center_y + y, color);
      lcd_drawPixel (center_x + x, center_y - y, color);
      lcd_drawPixel (center_x - x, center_y - y, color);
      lcd_drawPixel (center_x + y, center_y + x, color);
      lcd_drawPixel (center_x - y, center_y + x, color);
      lcd_drawPixel (center_x + y, center_y - x, color);
      lcd_drawPixel (center_x - y, center_y - x, color);
    }
}

void
lcd_fillCircle (uint8_t center_x, uint8_t center_y, uint8_t radius,
		uint8_t color)
{
  for (uint8_t i = 0; i <= radius; i++)
    {
      lcd_drawCircle (center_x, center_y, i, color);
    }
}

void
lcd_drawBitmap (uint8_t x, uint8_t y, const uint8_t *picture, uint8_t width,
		uint8_t height, uint8_t color)
{
  uint8_t i, j, byteWidth = (width + 7) / 8;
  for (j = 0; j < height; j++)
    {
      for (i = 0; i < width; i++)
	{
	  if (pgm_read_byte(picture + j * byteWidth + i / 8) & (128 >> (i & 7)))
	    {
	      lcd_drawPixel (x + i, y + j, color);
	    }
	}
    }
}

#endif

