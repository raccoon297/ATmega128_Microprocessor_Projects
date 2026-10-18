/*
 * Lab 1 extension: select LED bit shift
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7
void msec_delay(int msec);
int main()
{
    int i=0; // 패턴 인덱스
    DDRA = 0xFF; // 포트A를 모두 출력포트로 설정
    DDRE &= ~((1<<LED4) | (1<<LED5)); // 포트E를 모두 입력포트로 설정
    PORTA = (1<<LED0); // 처음 패턴으로 LED를 켠다.
    while (1)
    {
        // 스위치1 (PINE4) 눌림 확인
        if (!(PINE & (1<<LED4)))  // PINE4가 0이면 (눌림)
        {
            PORTA = (1<<i);
            while (~PINE & (1<<LED4)); // 스위치 뗄 때까지 대기
            msec_delay(50);
        }
        // 스위치2 (PINE5) 눌림 확인
        if (!(PINE & (1<<LED5))) // PINE5가 0이면 (눌림)
        {
            if (++i == 8) i = 0; // 왼쪽으로 이동, 끝이면 처음으로
            while (~PINE & (1<<LED5)); // 스위치 뗄 때까지 대기
            msec_delay(50);
        }
    }
}
void msec_delay(int n) // msec 단위 지연
{
    for(; n>0; n--)
    {
        _delay_ms(1);
    }
}
