/*
 * Lab 3: Timer0 CTC LED sequence
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
static unsigned char pattern[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F}; // LED 패턴 테이블
ISR(TIMER0_COMP_vect) // 비교일치 인터럽트 서비스루틴
{
    static int index = 0;        // 패턴 인덱스
    static char n_enter = 0;     // 인터럽트 횟수
    n_enter++;                   // 인터럽트 횟수 증가 (10msec 간격)
    if(n_enter == 50)            // 500ms 경과 시
    {
        n_enter = 0;
        PORTA = pattern[index++]; // LED 패턴 이동
        if(index == 8) index = 0;
    }
}
int main()
{
    DDRA = 0xFF;                 // A 포트를 출력으로 설정
    DDRB |= (1 << DDB4);         // PB4(OC0)를 출력으로 설정
    PORTA = 0xFF;                // LED를 끈다 (모두 HIGH)
    TCCR0 = (1 << WGM01);        // CTC 모드, 타이머 정지 상태 (WGM01=1, WGM00=0)
    OCR0 = 156;                  // 10msec마다 비교일치 발생 (실제 값 계산 기준: 16MHz/1024 → 10ms = 156.25)
    TIMSK = (1 << OCIE0);        // 타이머 0 비교일치 인터럽트 허용
    sei();                       // 전역 인터럽트 허용
    TCCR0 |= 0x07;               // 분주비 1024로 타이머 시작 (CS02:CS00 = 111)
    while(1);
}
