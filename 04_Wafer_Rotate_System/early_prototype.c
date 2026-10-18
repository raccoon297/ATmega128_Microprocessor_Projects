/*
 * Early spin-coater control draft, preserved for development history (not final firmware)
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
// === 상수 정의 ===
#define MAX_SPEED 255
#define MIN_SPEED 0
#define SPEED_STEP 25
#define MID_SPEED 130
#define BUZZER_PIN PC0
#define MOTOR_PWM OCR2
#define MOTOR_DIR1 PB0
#define MOTOR_DIR2 PB1
#define PWM_PIN    PB7
// === 전역 변수 ===
volatile char mode = 'P';          // 현재 상태
volatile unsigned char duty = 0;   // PWM 듀티비 (0~255)
// === 함수 선언 ===
void init_pwm_dc_motor();
void set_motor_speed(unsigned char speed);
void stop_motor();
void init_uart();
void putchar0(int n);
void init_interrupt();
void msec_delay(int ms);
void buzzer_beep_for_seconds(int sec);
void send_duty_value();
int main(void) {
    init_uart();
    init_pwm_dc_motor();
    init_interrupt();
    DDRC |= (1 << BUZZER_PIN);  // 부저 출력 설정
    while (1) {
        // 이제 메인 루프에서 특별히 할 일 없음
    }
}
// === PWM 모터 초기화 및 제어 ===
void init_pwm_dc_motor() {
    DDRB |= (1 << MOTOR_DIR1) | (1 << MOTOR_DIR2) | (1 << PWM_PIN);
    PORTB |= (1 << MOTOR_DIR1);  // 정방향
    PORTB &= ~(1 << MOTOR_DIR2);
    TCCR2 = (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS21);  // Fast PWM, 분주 8
    set_motor_speed(duty);
}
void set_motor_speed(unsigned char speed) {
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    MOTOR_PWM = speed;
    duty = speed;  // duty 변수 동기화
}
void stop_motor() {
    MOTOR_PWM = 0;
    duty = 0;
}
// === UART 설정 및 송신 ===
void init_uart() {
    UBRR0H = 0;
    UBRR0L = 103;  // 9600bps
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void putchar0(int n) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = n;
}
// === UART 수신 인터럽트 ===
ISR(USART0_RX_vect) {
    char received = UDR0;
    switch (received) {
    case 'A':  // Auto 모드
        mode = 'A';
        set_motor_speed(MID_SPEED);
        send_duty_value();
        break;
    case 'P':  // 정지 모드 + 부저
        mode = 'P';
        stop_motor();
        send_duty_value();
        buzzer_beep_for_seconds(7);
        break;
    case 'U':  // 속도 증가
        if (duty <= MAX_SPEED - SPEED_STEP)
        set_motor_speed(duty + SPEED_STEP);
        send_duty_value();
        break;
    case 'D':  // 속도 감소
        if (duty >= SPEED_STEP)
        set_motor_speed(duty - SPEED_STEP);
        else
        set_motor_speed(0);
        send_duty_value();
        break;
    default:
        break;
    }
}
// === 외부 인터럽트 (스위치 제어) ===
void init_interrupt() {
    DDRE &= ~((1 << PE4) | (1 << PE5));  // 스위치 입력
    EICRB = (1 << ISC41) | (1 << ISC40) | (1 << ISC51) | (1 << ISC50);  // 상승엣지
    EIMSK = (1 << INT4) | (1 << INT5);  // INT4, INT5 활성화
    sei();
}
ISR(INT4_vect) {  // 스위치1 → 속도 증가
    if (duty <= MAX_SPEED - SPEED_STEP)
    set_motor_speed(duty + SPEED_STEP);
    send_duty_value();
    EIFR = (1 << INTF4);
}
ISR(INT5_vect) {  // 스위치2 → 속도 감소
    if (duty >= SPEED_STEP)
    set_motor_speed(duty - SPEED_STEP);
    else
    set_motor_speed(0);
    send_duty_value();
    EIFR = (1 << INTF5);
}
// === 부저 제어 (초 단위 반복) ===
void buzzer_beep_for_seconds(int sec) {
    for (int i = 0; i < sec * 10; i++) {
        PORTC |= (1 << BUZZER_PIN);
        _delay_ms(50);
        PORTC &= ~(1 << BUZZER_PIN);
        _delay_ms(50);
    }
}
// === 듀티비 전송 (정수 3자리 문자열로: 000 ~ 255) ===
void send_duty_value() {
    putchar0((duty / 100) + '0');
    putchar0(((duty / 10) % 10) + '0');
    putchar0((duty % 10) + '0');
}
// === 밀리초 지연 ===
void msec_delay(int ms) {
    while (ms--) _delay_ms(1);
}
