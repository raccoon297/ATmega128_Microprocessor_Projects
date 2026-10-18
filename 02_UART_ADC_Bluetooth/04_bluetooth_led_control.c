/*
 * Lab 2: HC-06 Bluetooth UART LED control
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
unsigned char gettingnumber();
int main()
{
    DDRA = 0xFF;
    PORTA = 0x00;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UBRR0H = 0;
    UBRR0L = 103;
    sei(); // 전역 인터럽트 활성화
    while(1); // 메인 루프는 아무것도 하지 않음, 인터럽트로 처리
}
ISR(USART0_RX_vect)
{
    switch(gettingnumber()) {
    case '0':
        PORTA = 0x01;
        break;
    case '1':
        PORTA = 0x02;
        break;
    case '2':
        PORTA = 0x04;
        break;
    case '3':
        PORTA = 0x08;
        break;
    case '4':
        PORTA = 0x10;
        break;
    case '5':
        PORTA = 0x20;
        break;
    case '6':
        PORTA = 0x40;
        break;
    case '7':
        PORTA = 0x80;
        break;
    default:
        PORTA = 0x00;
        break;
    }
}
unsigned char gettingnumber()
{
    while(!(UCSR0A & (1<<RXC0)));
    return UDR0;
}
