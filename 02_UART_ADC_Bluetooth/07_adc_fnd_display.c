/*
 * Lab 2: ADC value on four-digit FND
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
unsigned char digit[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4]={0x01,0x02,0x04,0x08};
unsigned char fnd[4];
void init_adc();
unsigned short read_adc();
int main()
{
    int i = 0;
    DDRC = 0xff;
    DDRG = 0x0f;
    init_adc();
    unsigned short value;
    unsigned short ADCtoSD;
    while(1)
    {
        value = read_adc();// AD 변환 시작 및 결과 읽어오기 (함수로 처리)
        ADCtoSD = value;// 기존에는 / 256 사용해 0~1023을 0~3으로 압축했음. UDR0 buffer -> 8 bit
        fnd[3] = (ADCtoSD / 1000) % 10;
        fnd[2] = (ADCtoSD / 100) % 10;
        fnd[1] = (ADCtoSD / 10) % 10;
        fnd[0] = ADCtoSD % 10;
        for(i=0; i<4; i++) //0.01초 증가
        {PORTC = digit[fnd[i]];
            PORTG = fnd_sel[i];
            _delay_ms(2);
            if(i%2)_delay_ms(1);
        }
    }
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
