/*
 * Lab 1: LED shift with inline NOP delay
 * Extracted from original course HWPX code notes.
 * Requires ATmega128 and course-specific wiring.
 */

#include <avr/io.h>
#define NOP_PER_SECOND 1200000
unsigned char pattern[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
int main() {
    int i = 0;
    DDRA = 0xFF; // PORTA를 출력으로 설정
    PORTA = pattern[i]; // 초기 LED 상태 설정
    while (1) {
        for (unsigned long j = 0; j < NOP_PER_SECOND; j++) {
            asm ("nop"::); asm ("nop"::); // 어셈블리 코드를 이용한 지연.
            //거의 1s delay로 동작한 NOP_PER_SECOND 1200000 * 2(어셈블리 명령어 수) = 2400000으로 16MHz와 약 6.67배 차이가 났다.
            //for 루프 속 어셈블리 명령어가 예상보다 많은 클럭을 필요로 할 수도 있다.
        }
        i++;
        if (i == 8) i = 0;
        PORTA = pattern[i];
    }
}
