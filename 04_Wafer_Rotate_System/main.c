/*
 * Final coursework team project: Wafer Rotate System / firmware implementation
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// === 상수 정의 ===
#define MAX_SPEED 255
#define SPEED_STEP 25
#define MID_SPEED 125
#define MOTOR_PWM OCR2
#define PWM_PIN PB7     // OC2
#define DIR_PIN PB6     // 방향 핀
// === 전역 변수 ===
volatile unsigned char duty = 0;
volatile unsigned char pwm_mode = 0;    // 0: Fast, 1: Phase Correct
volatile char direction = 1;             // +1: 정방향, -1: 역방향
unsigned char digit[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67, 0xf1, 0xf3};
unsigned char fnd_sel[4]={0x01,0x02,0x04,0x08};
unsigned char fnd[4];
// === 함수 선언 ===
void init_motor();
void set_pwm_mode();
void set_motor_speed(unsigned char speed);
void stop_motor();
void init_uart();
void putchar0(int n);
void init_interrupt();
void send_duty_value();
void apply_direction();
int main(void) {
    init_uart();
    init_motor();
    init_interrupt();
    sei();
    int i = 0;
    DDRC = 0xff;
    DDRG = 0x0f;
    while (1) {
        if (pwm_mode == 0) {
            fnd[3] = 10;  // 'F'
        } else {
            fnd[3] = 11;  // 'P'
        }
        fnd[2] = (duty / 100) % 10;
        fnd[1] = (duty / 10) % 10;
        fnd[0] = duty % 10;
        for(i=0; i<4; i++) //0.01초 증가
        {PORTC = digit[fnd[i]];
            PORTG = fnd_sel[i];
            _delay_ms(2);
            if(i%2)_delay_ms(1);
        }
    }
}
// === 모터 초기화 ===
void init_motor() {
    DDRB |= (1 << PWM_PIN) | (1 << DIR_PIN);  // PWM & DIR 출력
    direction = 1;  // 기본 정방향
    set_pwm_mode();
    apply_direction();
    set_motor_speed(duty);
}
void set_pwm_mode() {
    if (pwm_mode == 0) {
        // Fast PWM
        TCCR2 = (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS21);
    } else {
        // Phase Correct PWM
        TCCR2 = (1 << WGM20) | (1 << COM21) | (1 << CS21);
    }
}
void set_motor_speed(unsigned char speed) {
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    MOTOR_PWM = speed;
    duty = speed;
}
void stop_motor() {
    MOTOR_PWM = 0;
    duty = 0;
}
void apply_direction() {
    if (direction == 1) {
        // 정방향: PB7 = PWM, PB6 = LOW
        PORTB &= ~(1 << DIR_PIN);
        // PWM 모드 유지
        set_pwm_mode();
    } else {
        // 역방향: PB7 = LOW (PWM 끄기), PB6 = HIGH
        PORTB &= ~(1 << PWM_PIN);  // 강제로 PB7 LOW
        TCCR2 = 0;                  // PWM OFF
        PORTB |= (1 << DIR_PIN);    // PB6 HIGH
    }
}
// === UART ===
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
void send_duty_value() {
    putchar0((duty / 100) + '0');
    putchar0(((duty / 10) % 10) + '0');
    putchar0((duty % 10) + '0');
}
// === UART 수신 ===
ISR(USART0_RX_vect) {
    char received = UDR0;
    switch (received) {
    case 'A':  // 정방향 + Fast PWM + MID_SPEED
        direction = 1;
        pwm_mode = 0;
        set_pwm_mode();
        apply_direction();
        set_motor_speed(MID_SPEED);
        send_duty_value();
        break;
    case 'R':  // 역방향
        direction *= -1;
        apply_direction();
        break;
    case 'C':  // PWM 모드 토글
        pwm_mode ^= 1;
        if (direction == 1) {
            set_pwm_mode();
        }
        break;
    case 'P':  // 정지
        stop_motor();
        send_duty_value();
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
// === 외부 인터럽트 ===
void init_interrupt() {
    DDRE &= ~((1 << PE4) | (1 << PE5));  // 스위치 입력
    EICRB = (1 << ISC41) | (1 << ISC40) | (1 << ISC51) | (1 << ISC50);  // 상승엣지
    EIMSK = (1 << INT4) | (1 << INT5);  // INT4, INT5 활성화
}
ISR(INT4_vect) {
    if (duty <= MAX_SPEED - SPEED_STEP)
    set_motor_speed(duty + SPEED_STEP);
    send_duty_value();
    EIFR = (1 << INTF4);
}
ISR(INT5_vect) {
    if (duty >= SPEED_STEP)
    set_motor_speed(duty - SPEED_STEP);
    else
    set_motor_speed(0);
    send_duty_value();
    EIFR = (1 << INTF5);
}
