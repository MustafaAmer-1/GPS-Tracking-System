#include "Header.h"

int main(){
	while(1){
			
	}
}


// Initialization of Port B pins
void PortB_Init(){
	// Initializing Clock and wait until get stablized
	SYSCTL_RCGCGPIO_R |= 0x02;
	while((SYSCTL_PRGPIO_R & 0x02) == 0);
	
	// Initializing Port B pins
	GPIO_PORTB_LOCK_R = magicKey;
	GPIO_PORTB_CR_R |= 0x1F;
	GPIO_PORTB_AMSEL_R &= ~0x1F;
	GPIO_PORTB_PCTL_R |= 0x11;
	GPIO_PORTB_DIR_R |= 0x1E;
	GPIO_PORTB_DIR_R &= ~0x01;
	GPIO_PORTB_AFSEL_R |= 0x03;
	GPIO_PORTB_DEN_R |= 0x1F;
}
