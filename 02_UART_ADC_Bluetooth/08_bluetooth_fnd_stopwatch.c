/*
 * Lab 2 extension: Bluetooth-controlled FND stopwatch
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
unsigned char digit[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4] = {0x01,0x02,0x04,0x08};
unsigned char fnd[4];
static void putchar0(int n);// 1 char 송신(Transmit)
unsigned char gettingnumber();
volatile unsigned short stopwatch = 0;     // 스톱워치 시간 값 (0~999.9까지 표시)
volatile unsigned char running = 0;        // 1이면 실행 중, 0이면 정지
volatile unsigned short delay_counter = 0; // FND 반복마다 증가 → 시간 흐름 구현
int main()
{
    int i = 0;
    DDRC = 0xFF;
    DDRG = 0x0F;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UBRR0H = 0;
    UBRR0L = 103;
    sei(); // 전역 인터럽트 활성화
    while(1)
    {
        // === 스톱워치 카운트 ===
        if (running) {
            delay_counter++;
            if (delay_counter >= 10) {  // 약 0.1초
                delay_counter = 0;
                stopwatch++;
                if (stopwatch >= 10000) stopwatch = 0;
                // === 블루투스로 시간 전송 ===
                putchar0((stopwatch / 1000) % 10 + '0');
                putchar0((stopwatch / 100) % 10 + '0');
                putchar0((stopwatch / 10) % 10 + '0');
                putchar0('.');
                putchar0((stopwatch % 10) + '0');
            }
        }
        // === 숫자를 FND 4자리로 변환 ===
        fnd[3] = (stopwatch / 1000) % 10;
        fnd[2] = (stopwatch / 100) % 10;
        fnd[1] = ((stopwatch / 10) % 10) | 0x00;
        fnd[0] = stopwatch % 10;
        // === FND 출력 ===
        for(i = 0; i < 4; i++)
        {
            unsigned char seg = digit[fnd[i]];
            // fnd[1] 위치에 점을 찍는다
            if (i == 1) {
                seg |= 0x80;  // 소수점 on
            }
            PORTC = seg;
            PORTG = fnd_sel[i];
            _delay_ms(2);
            if (i % 2) _delay_ms(1);
        }
    }
}
// === UART 수신 인터럽트 ===
ISR(USART0_RX_vect)
{
    switch(gettingnumber()) {
        // === 스톱워치 제어 ===
    case 'S':  // Start
        running = 1;
        break;
    case 'P':  // Pause (Stop)
        running = 0;
        break;
    case 'R':  // Reset
        stopwatch = 0;
        delay_counter = 0;
        // 실행 중이면 0에서 다시 시작, 정지 상태면 0 유지
        break;
    default:
        break;
    }
}
unsigned char gettingnumber()
{
    while(!(UCSR0A & (1<<RXC0)));
    return UDR0;
}
void putchar0(int n)
{
    while(!(UCSR0A & (1<<UDRE0))); // UCSR0A 5번 비트 = UDRE  0x20;
    UDR0 = n; // 1 character 전송
}
