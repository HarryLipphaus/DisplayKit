
  #include "lcd.h"

void setup() {
  // put your setup code here, to run once:
  lcd_init(OLED_DISPLAYON);    // init lcd and turn on
  
  lcd_puts("Hello World Test 3");  // put string from RAM to display (TEXTMODE) or buffer (GRAPHICMODE)
  lcd_gotoxy(0,2);          // set cursor to first column at line 3
  lcd_puts_p(PSTR("String from flash"));  // puts string form flash to display (TEXTMODE) or buffer (GRAPHICMODE)
#if defined GRAPHICMODE
  lcd_drawCircle(64,32,7,WHITE); // draw circle to buffer white lines
  lcd_display();                 // send buffer to display
#endif
}

void loop() {
  // put your main code here, to run repeatedly:

}
