/*
 * main.c
 *
 *  Created on: 16.08.2018
 *      Author: harry
 */

#include <avr/io.h>
#include <stdio.h>
#include "lcd.h"

int main()
{
char s[21];
	  // put your setup code here, to run once:
	lcd_init(OLED_DISPLAYON);    // init lcd and turn on

#if defined GRAPHICMODE
	sprintf(s, "%d-Line graphic-mode\r\n", OLED_LINES);
#else
	sprintf(s, "%d-Line text-mode\r\n", OLED_LINES);
#endif
	  lcd_puts(s);  // put string from RAM to display (TEXTMODE) or buffer (GRAPHICMODE)
	  lcd_puts("20 chars per line\r\n");
	  lcd_puts_p(PSTR("special chars:\r\n\x84\x94\x81\x8e\x99\x9a\xf8\xe1\r\n"));
	  for (uint8_t i=4;i < (OLED_LINES - 1);i++)
	  {
	  	  sprintf(s, "Line %2d\r\n", i+1);
	  	  lcd_puts(s);  // put string from RAM to display (TEXTMODE) or buffer (GRAPHICMODE)
	  } /**/
#if defined GRAPHICMODE
	  lcd_puts ("Isn't it nice?");
#else
	  lcd_puts ("Isn't it ugly?");
#endif

//	  lcd_gotoxy(0,2);          // set cursor to first column at line 3
//	  lcd_puts_p(PSTR("String from flash"));  // puts string form flash to display (TEXTMODE) or buffer (GRAPHICMODE)
	#if defined GRAPHICMODE
	  lcd_drawRect(0,0,127,63,WHITE);
	  lcd_drawCircle(96,42,7,WHITE); // draw circle to buffer white lines
	  lcd_display();                 // send buffer to display
	#endif

	  while(1)
	  {

	  }

}
