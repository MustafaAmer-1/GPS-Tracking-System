#include "Header.h"

double totalDis = 0; //Global Variable holding the total distance taken
char lat_raw[12] = {'0'};            // latitude array before convertion
char lon_raw[12] = {'0'};             // longitude array before convertion
double lat_d, strt_lat, dis_lat;
double lon_d, strt_lon, dis_lon;
uint8_t first_coor = 1, distnationReached = 0;

void SystemInit() {
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
}

int main(){
	PortB_Init();
	
	while(1){
			
	}
}


/* ----------------------- Ports Initializations --------------------*/

// Initialization of Port B pins
void PortB_Init(){
	// Initializing Clock and wait until get stablized
	SYSCTL_RCGCGPIO_R |= 0x02;
	while((SYSCTL_PRGPIO_R & 0x02) == 0);
	
	// Initializing Port B pins
	GPIO_PORTB_LOCK_R = magicKey;
	GPIO_PORTB_CR_R |= 0xFF;
	GPIO_PORTB_AMSEL_R &= ~0xFF;
	GPIO_PORTB_AFSEL_R &= 0xFF;
	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTB_DIR_R |= 0xFF;
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




/* ----------------------- GPS --------------------*/

void GPS_process(void){
    char data; // for incoming serial data
    uint8_t end =0;  // indicate end of message
    uint8_t cnt = 0;  // position counter
    uint8_t lat_cnt = 1;  // latitude data counter // starts from one to set first char 0 as lon_raw
    uint8_t log_cnt = 0;  // longitude data counter
    uint8_t flag;  // GPS flag
    uint8_t valid = 1;  // GPS flag
    uint8_t com_cnt = 0;  // comma counter
    uint8_t i;
		char header[] = {'$', 'G', 'P', 'R', 'M', 'C'};
		
		if(distnationReached) return;
    
		strt_lat = lat_d;
		strt_lon = lon_d;
        while(!end){
        while((UART5_FR_R & 0x10) != 0x10){         // Check GPS data
            data = UART5_DR_R & 0xFF;
            flag = 1;
						
            for (i= 0; i < 6; i++)
            {
                if(data == header[i] && cnt == i)
                    cnt++;
            }
            
            if(data ==',' && cnt == 6){   // count commas in message
                com_cnt++;
                flag=0;
            }

            if(com_cnt == 2 && data == 'V'){
                end = 1;
                valid = 0;
            }
            
            if(com_cnt == 3 && flag == 1 && valid)
                lat_raw[lat_cnt++] =  data;         // latitude

            if(com_cnt == 5 && flag == 1 && valid)
                lon_raw[log_cnt++] =  data;         // Longitude

            if(data == '*' && com_cnt >= 5){
                end  = 1;
            }
        }
    }
		lat_d = stringToNum(lat_raw);
		lon_d = stringToNum(lon_raw);
		if(first_coor){
			strt_lat = lat_d;
			strt_lon = lon_d;
			first_coor = 0;
		}
}

/* ----------------------- LCD --------------------*/


/* ----------------------- Bluetooth --------------------*/


/* ----------------------- utilities --------------------*/

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

// makes n milliseconds delay
void delay(int n){
int i,j;
for(i=0;i<n;i++)
for(j=0;j<3180;j++)
{}
}

// 
 void display7segment(int num){
  int org = num, tmp;
  int enables[3] = {0x10, 0x08,0x04};
  int i;
  for(i = 0; i < 3; i++){
    tmp = org % 10;
    org /= 10;
    GPIO_PORTB_DATA_R |= 0x1c;
    GPIO_PORTB_DATA_R &= ~enables[i];
    GPIO_PORTD_DATA_R = tmp;
    delay(10);
  }
}
 
double stringToNum(char *str){
    double res = 0;
    uint8_t i = 0;
    uint8_t decimal = 1;
    float div = 10;
    while(str[i]){
        if(str[i] == '.')
            decimal = 0;
        else if(decimal) 
            res = (str[i] - '0') + res*10;
        else{
            res += (str[i] - '0')/div;
            div *= 10;
        }
        i++;
    }
    return res;
}
