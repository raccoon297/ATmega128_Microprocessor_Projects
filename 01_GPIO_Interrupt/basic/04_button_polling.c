/*
 * Lab 1: polling with PE4
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
    DDRE = 0xEF; // 포트E 입력 설정 (PE4, PE5)
    PORTA = pattern[i]; // 처음 패턴으로 LED를 켠다.
    while (1) // 계속적으로 사건 발생을 점검하는 폴링 방식으로, 비효율적일 수 있다.
    {
        // 스위치1 (PINE4) 눌림 확인
        if (!(PINE & 0x10))  // PINE4가 0이면 (눌림)
        {
            if (++i == 8) i = 0; // 왼쪽으로 이동, 끝이면 처음으로
            PORTA = pattern[i];
            while (~PINE & 0x10); // 스위치 뗄 때까지 대기
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
