/*
 * Lab 1: LED left-to-right shift using delay
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
void msec_delay(int msec); //msec_delay(int n)함수를 아직 정의하지 않았다. 이를 먼저 호출하려고 하면, 컴파일러에서 그 함수가 어떤 건지 몰라 에러가 발생하므로 이를 막기 위해서 작성한 것이다.
unsigned char pattern[8]
= {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
int main()
{
    int i=0;
    DDRA = 0xFF; // PORTA를 출력으로 설정
    PORTA  = pattern[i];
    while(1)
    {
        msec_delay(1000);
        if(++i==8) i = 0;
        PORTA = pattern[i];
    }
}
void msec_delay(int n) //입력받은 n만큼 1ms 지연시키도록 정의.
{
    for(; n>0; n--)
    {
        _delay_ms(1);
    }
}
