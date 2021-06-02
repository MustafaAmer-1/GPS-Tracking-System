#include "Header.h"

double totalDis = 0; //Global Variable holding the total distance taken

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

void PortF_Init(void){ 
  // Initializing Clock and wait until get stablized
  SYSCTL_RCGCGPIO_R |= 0x20;
  while((SYSCTL_PRGPIO_R & 0x20) == 0);
	
	// Initializing Port F pins
  GPIO_PORTF_LOCK_R = magicKey;     
  GPIO_PORTF_CR_R |= 0x0E;               
  GPIO_PORTF_AMSEL_R &= ~0x0E;        
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0;
  GPIO_PORTF_DIR_R |= 0x0E;            
  GPIO_PORTF_AFSEL_R &= ~0x0E;               
  GPIO_PORTF_DEN_R |= 0x0E;
  GPIO_PORTF_DATA_R &= ~0x0E;	
}
void PortD_Init(){
 // Initializing Clock and wait until get stablized
 SYSCTL_RCGCGPIO_R |= 0x08;
 while((SYSCTL_PRGPIO_R & 0x08) == 0);
 
 // Initializing Port D pins
 GPIO_PORTD_LOCK_R = magicKey;
 GPIO_PORTD_CR_R |= 0x0F;
 GPIO_PORTD_AMSEL_R &= ~0x0F;
 GPIO_PORTD_PCTL_R &= ~0x0F;
 GPIO_PORTD_DIR_R |= 0x0F;
 GPIO_PORTD_AFSEL_R &= ~0x0F;
 GPIO_PORTD_DEN_R |= 0x0F;
}

//Calculating Distance between two consecutive Longitudes and Latitudes and Accumulate total distance
double distance(double lat1, double lon1, double lat2, double lon2){
  const int R = 6371; //Radius of earth in (km)
  double phi1 = deg2rad(lat1);
  double phi2 = deg2rad(lat2);
  double delta1 = deg2rad(lat2 - lat1);
  double delta2 = deg2rad(lon2 - lon1);

  double a = sin(delta1 / 2) * sin(delta1 / 2) + cos(phi1) * cos(phi2) * sin(delta2 / 2) * sin(delta2 / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double d = R * c;

  return totalDis += d;
}

// Converting angle unit from degree to radian
double deg2rad(double deg){
  return (deg * PI / 180);
}
//Lighting LED
void led_control(int control) {

	GPIO_PORTF_DATA_R &= ~0xE;
	GPIO_PORTF_DATA_R |= control;
}

//Checking the reached destination
void check_destination() {

	if (totalDis <= 100) {
		led_control(RED);
	}

	else if (totalDis > 100) {
		led_control(GREEN);
	}
}