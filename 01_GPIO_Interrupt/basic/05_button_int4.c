/*
 * Lab 1: external interrupt INT4
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
void msec_delay(int msec);
unsigned char pattern[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
volatile int i = 0;  // ISR과 main 양쪽에서 사용할 변수는 volatile로
int main()
{
    DDRA = 0xFF; // 포트A 출력 설정
    DDRE = 0xEF; // 포트E 입력 설정 (PE4)
    PORTA = pattern[i]; // 초기 LED 패턴 출력
    EICRB = 0x20; // INT4: 하강엣지(0x20)
    EIMSK = 1 << INT4; // INT4 인터럽트 허용
    //EIMSK = 0x10;으로도 작성 가능
    sei(); // 전체 인터럽트 허용
    while (1)
    {
        // 메인 루프는 아무 동작도 하지 않고, 인터럽트를 기다림. 이게 없을 경우 메인함수 불안정하게 종료됨.
    }
}
// INT4 인터럽트 (스위치1)
ISR(INT4_vect)
{
    if (++i == 8) i = 0; // 왼쪽 이동
    PORTA = pattern[i];
    while (~PINE & (1 << INT4)); // 스위치 뗄 때까지 대기
    msec_delay(50);
}
void msec_delay(int n)
{
    while (n--) _delay_ms(1);
}
