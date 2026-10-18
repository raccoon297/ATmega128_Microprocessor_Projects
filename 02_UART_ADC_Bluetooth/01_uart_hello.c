/*
 * Lab 2: UART periodic greeting
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>// AVR 기본 include
#define NULL 0
void putchar0(char c)// 1 char를 송신(Transmit)하는 함수
{
    while(!(UCSR0A & 0x20)) ; // UDRE : UCSR0A 5번 비트
    UDR0 = c;// 1 character 전송
}
int main()
{
    int i;
    char data[]="Hi~\n\r";// 보내고 싶은 문자열
    // \n(New Line), \r(Carrage Return)
    UBRR0H = 0;// 12비트가 의미를 가짐,
    UBRR0L = 8;// ATmega128 datasheet 참조 요망, 0000 1000
    UCSR0B = (1<<RXEN0) | (1<<TXEN0); // RX,TX 허용
    UCSR0C = (3<<UCSZ00);// 비동기, 1 정지비트, 8데이터비트
    while(1)// 연속적으로 보내기
    {
        i=0;
        while (data[i]!= NULL)// check String End
        putchar0(data[i++]);// 순서대로 보내기
    }
}
