
#include "util/delay.h"
#define segNope 0b00000000
#define segZero 0b01111100
#define segOne 0b00001100
#define segTwo 0b11011010

#define __AVR_ATmega64__
#include "LCD_LM016L_v1.2.h"
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

int mode = -1;
int segNum = 0;

void setup() {

  DDRC = 0x0F; // Настройка портов C0-C3 на вывод (светодиоды)
  DDRD = 0x00; // Настройка портов D0-D3 на вход (кнопки)
  PORTD = 0x0F; // Настройка PullUp D0-D3

  DDRG = 0b00011011; // Настройка портов G0, G1, G3, G4 на вывод (7seg)
  PORTG = 0b00011011; // Настройка портов C0-C3 на вывод (светодиоды)

  DDRA = 0xFF;

  uint16_t CTC_Val = 625;
  TCCR1B |= (1 << WGM12); // Установка режима таймера
  TCCR1B |= (1 << CS10) | (1 << CS12); // делитель 1024
  OCR1AL = CTC_Val;
  OCR1AH = 0x00;
  TIMSK |= (1 << OCIE1A);

  EIMSK = 0x0F;          // Включаем прерывания int0 - int3
  EICRA = 0xAA;          // Прерывания по спаду
  SREG |= (1 << SREG_I); // Глобальное включение прерываний
}

void draw7seg() {
  if (segNum == 4)
    segNum = 0;
  const int letters[4] = {segNope, segOne, segOne, segTwo};
  PORTA = letters[segNum];
  PORTG &= ~(0b00011011); // сброс битов

  if (segNum == 0)
    PORTG |= (1 << 0);
  if (segNum == 1)
    PORTG |= (1 << 1);
  if (segNum == 2)
    PORTG |= (1 << 3);
  if (segNum == 3)
    PORTG |= (1 << 4);

  segNum++;
}

void setMode(int new_mode) {
  if (new_mode != mode) {
    PORTC &= ~(1 << mode);
    mode = new_mode;
    PORTC |= (1 << mode);
  }
}

ISR(INT0_vect) { setMode(0); }
ISR(INT1_vect) { setMode(1); }
ISR(INT2_vect) { setMode(2); }
ISR(INT3_vect) { setMode(3); }
ISR(TIMER1_COMPA_vect) { draw7seg(); }

void lcdMode() {
  DISPLAY_Inint(DisplayMode_4Bits);
  LCD_Write("Michailov A.I.  ");
  LCD_Cursor_Position(0, 1);
  LCD_Write("Brigada No.112  ");
}

int main() {
  setup();
  setMode(0);
  lcdMode();
  segNum = 0;

  while(1);
}
