/*
 * Lab 2: ADC light value sent by UART
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
#define DELAY 50
static void putchar0(int n);// 1 char 송신(Transmit)
void init_adc();
unsigned short read_adc();
void msec_delay(int n); // 시간지연함수
int main()
{
    init_adc();
    unsigned short value;
    unsigned short ADCtoSD;
    // UCSRnA 설정
    UCSR0B = (1<<TXEN0);// UCSRnB 설정
    UCSR0C = (3<<UCSZ00);// UCSRnC 설정
    UBRR0H = 0;// UBRRnH 설정
    UBRR0L = 8;// UBRRnL 설정
    while(1)
    {
        value = read_adc();// AD 변환 시작 및 결과 읽어오기 (함수로 처리)
        ADCtoSD = value;// 기존에는 / 256 사용해 0~1023을 0~3으로 압축했음. UDR0 buffer -> 8 bit
        if (ADCtoSD >= 1000)putchar0((ADCtoSD / 1000) % 10 + '0');
        if (ADCtoSD >= 100)putchar0((ADCtoSD / 100) % 10 + '0');
        if (ADCtoSD >= 10)putchar0((ADCtoSD / 10) % 10 + '0');
        putchar0(ADCtoSD % 10 + '0');
        putchar0('\r');  // 캐리지 리턴
        putchar0('\n');  // 라인 피드
        msec_delay(DELAY);// 시간지연
    }
}
void putchar0(int n)
{
    while(!(UCSR0A & (1<<UDRE0))); // UCSR0A 5번 비트 = UDRE  0x20;
    UDR0 = n; // 1 character 전송
}
void init_adc()
{
    ADMUX = 0x00;
    // REFS(1:0) = “00” AREF(+5V) 기준전압 사용
    // ADLAR = ‘0’ 디폴트 오른쪽 정렬
    // MUX(4:0) = “00000” ADC0 사용, 단극 입력
    ADCSRA = 0x87;
    // ADEN = ‘1’ ADC를 Enable
    // ADFR = ‘0’ single conversion 모드
    // ADPS(2:0) = “111” 프리스케일러 128분주
}
unsigned short read_adc()
{
    unsigned char adc_low, adc_high;
    unsigned short value;
    ADCSRA |= (1<<ADSC);// ADC start conversion, ADSC = ‘1’  0x40;
    while ((ADCSRA & 0x10) != 0x10);// ADC 변환 완료 검사.
    adc_low = ADCL;// 변환된 Low 값 읽어오기
    adc_high = ADCH;// 변환된 High 값 읽어오기
    value = (adc_high << 8) | adc_low;
    // value 는 High 및 Low 연결 16비트값
    return value;
}
void msec_delay(int n)
{
    for(; n>0; n--)
    {
        _delay_ms(1);
    }
}
