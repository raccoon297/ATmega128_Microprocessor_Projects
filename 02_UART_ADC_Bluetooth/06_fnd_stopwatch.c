/*
 * Lab 2: FND software-timed stopwatch
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#include <util/delay.h>
unsigned char digit[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};
unsigned char fnd_sel[4]={0x01,0x02,0x04,0x08};
unsigned char fnd[4];
int main()
{
    int i=0, count=0;
    DDRC = 0xff;
    DDRG = 0x0f;
    while(1)
    {
        count++;
        if(count == 10000)
        count=0;
        fnd[3] = (count/1000)%10;
        fnd[2] = (count/100)%10;
        fnd[1] = (count/10)%10;
        fnd[0] = count%10;
        for(i = 0; i < 4; i++)
        {
            unsigned char seg = digit[fnd[i]];
            // fnd[2] 위치에 점을 찍는다
            if (i == 2) {
                seg |= 0x80;  // 소수점 on
            }
            PORTC = seg;
            PORTG = fnd_sel[i];
            _delay_ms(2);
            if (i % 2) _delay_ms(1);
        }
    }
}
