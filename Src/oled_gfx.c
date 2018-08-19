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

#define SetPixel(x,y) (displayBuffer.buf[(y) / 8][x] |= (1 << ((y) % 8)))
#define ClearPixel(x,y) (displayBuffer.buf[(y) / 8][x] &= ~(1 << ((y) % 8)))
#define InvertPixel(x,y) (displayBuffer.buf[(y) / 8][x] ^= (1 << ((y) % 8)))



void
lcd_drawPixel (uint8_t x, uint8_t y, uint8_t color)
{
  if (x > OLED_WIDTH - 1 || y > (OLED_HEIGHT - 1))
    return; // out of Display
  switch (color)
  {
    case BLACK:
      ClearPixel(x,y);
      break;

    case WHITE:
      SetPixel(x,y);
      break;

    case INVERT:
      InvertPixel(x,y);
      break;
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
lcd_drawEllipse (int8_t xm, int8_t ym, int8_t a, int8_t b, uint8_t c)
{
  int dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
  long a2 = a * a, b2 = b * b;
  long err = b2 - (2 * b - 1) * a2, e2; /* Fehler im 1. Schritt */

  do
    {
      lcd_drawPixel (xm + dx, ym + dy, c); /* I. Quadrant */
      lcd_drawPixel (xm - dx, ym + dy, c); /* II. Quadrant */
      lcd_drawPixel (xm - dx, ym - dy, c); /* III. Quadrant */
      lcd_drawPixel (xm + dx, ym - dy, c); /* IV. Quadrant */

      e2 = 2 * err;
      if (e2 < (2 * dx + 1) * b2)
	{
	  dx++;
	  err += (2 * dx + 1) * b2;
	}
      if (e2 > -(2 * dy - 1) * a2)
	{
	  dy--;
	  err -= (2 * dy - 1) * a2;
	}
    }
  while (dy >= 0);

  while (dx++ < a)
    { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
      lcd_drawPixel (xm + dx, ym, c); /* -> Spitze der Ellipse vollenden */
      lcd_drawPixel (xm - dx, ym, c);
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

