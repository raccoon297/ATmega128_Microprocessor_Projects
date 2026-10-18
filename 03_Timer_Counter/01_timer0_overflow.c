/*
 * Lab 3: Timer0 overflow LED sequence
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
static unsigned char pattern[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F}; // LED 패턴 테이블
ISR(TIMER0_OVF_vect) // 타이머 0 오버플로 인터럽트 서비스루틴
{
    static int index = 0;       // 패턴 인덱스
    static char n_enter = 0;    // 인터럽트 횟수
    n_enter++;                  // 10msec 후에 인터럽트 발생, 인터럽트 횟수 증가
    if(n_enter == 20)           // 200msec
    {
        n_enter = 0;
        PORTA = pattern[index++]; // 패턴 이동
        if(index == 8) index = 0;
    }
}
int main()
{
    DDRA = 0xFF;        // A 포트를 출력으로 설정
    PORTA = 0xFF;       // LED를 끈다.
    TCCR0 = 0x00;       // 타이머/카운터 0 설정, 표준 모드, 타이머 정지
    TCNT0 = 0;          // 타이머 초기 값 설정
    TIMSK = 0x01;       // 인터럽트 설정, 타이머 오버플로 인터럽트 허용
    sei();              // 전역 인터럽트 허용
    TCCR0 |= (7<<CS00); // 분주비 1024로 타이머 시작
    while(1);
}
