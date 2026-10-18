/*
 * Lab 1 extension: variable LED step
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
void msec_delay(int msec);
unsigned char pattern[8]
= {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
int main()
{
    int i=0; // 패턴 인덱스
    DDRA = 0xFF; // 포트A를 모두 출력포트로 설정
    DDRE = 0xCF; // 포트E를 모두 입력포트로 설정
    PORTA = pattern[i]; // 처음 패턴으로 LED를 켠다.
    while (1)
    {
        // 스위치1 (PINE4) 눌림 확인
        if (!(PINE & 0x10))  // PINE4가 0이면 (눌림)
        {
            if (++i == 8) i = 0; // 왼쪽으로 이동, 끝이면 처음으로
            if(i>7) i = i-8;
            PORTA = pattern[i];
            while (~PINE & 0x10); // 스위치 뗄 때까지 대기
            msec_delay(50);
        }
        // 스위치2 (PINE5) 눌림 확인
        if (!(PINE & 0x20)) // PINE5가 0이면 (눌림)
        {
            i++;
            while (~PINE & 0x20); // 스위치 뗄 때까지 대기
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
