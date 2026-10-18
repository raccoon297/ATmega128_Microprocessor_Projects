/*
 * Lab 2: UART multiplication table
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>         // AVR 기본 include
#include <stdio.h>          // printf 함수를 위한 include
static int putchar0(char c,  FILE *stream); //1 char 송신(Transmit)
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);
int putchar0(char c, FILE *stream)
{
    if (c == '\n')
    putchar0('\r', stream);
    while(!(UCSR0A & 0x20));         // UCSR0A 5번 비트 = UDRE
    UDR0 = c;                       // 1 character 전송
    return 0;
}
char getchar0() // 1 character를 수신(receive)하는 함수
{
    while (!(UCSR0A & (1 << RXC0)));  // UCSRA 7번 비트 = RXC
    return UDR0;  // 1 character 수신
}
void init_uart()
{
    UBRR0H = 0;  // 12비트의 의미를 가짐.
    UBRR0L = 8;  // ATmega128 datasheet 참조 요망 (16,000,000 / (16 * 115200)) - 1 = 8.68 → 8 설정
    // 16Mhz, 115200 baud의 경우
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);  // Receive(RX) 및 Transmit(TX) Enable
    UCSR0C = (3<<UCSZ00);  // UART Mode, 8 Bit Data, No Parity, 1 Stop Bit
}
int main()
{
    char c; int i;
    init_uart();
    stdout = &mystdout;
    while (1)
    {printf("Input Number for GuGuDan : ");
        c = getchar0( );// 1 character를 받아서
        printf("%c\n", c);// 원래 character를 돌려보냄
        c = c-'0';// ASCII -> 숫자로 변환
        for(i=1; i<=9; i++)// 구구단 디스플레이
        printf("\t%d x %d = %d\n", c, i, c*i);
    }
}
