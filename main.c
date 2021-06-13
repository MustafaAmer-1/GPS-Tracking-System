#include "Header.h"

double totalDis = 0; //Global Variable holding the total distance taken
char lat_raw[12] = {'0'};            // latitude array before convertion
char lon_raw[12] = {'0'};             // longitude array before convertion
double lat_d, strt_lat, dis_lat;
double lon_d, strt_lon, dis_lon;
uint8_t first_coor = 1, distnationReached = 0;
volatile int target_distance = 100;

void SystemInit() {
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
}

int main(){
	PortB_Init();
	
	while(1){
			
	}
}


/* ----------------------- Ports Initializations --------------------*/

void PortA_Init(){
	// Initializing Clock and wait until get stabilized
	SYSCTL_RCGCGPIO_R |= 0x01;
	while((SYSCTL_PRGPIO_R & 0x01) == 0);
	
	// Initializing Port A pins
	GPIO_PORTA_LOCK_R = magicKey;
	GPIO_PORTA_CR_R |= 0xE0;
	GPIO_PORTA_AMSEL_R &= ~0xE0;
	GPIO_PORTA_PCTL_R &= ~0xFFF00000;
	GPIO_PORTA_DIR_R |= 0xE0;
	GPIO_PORTA_AFSEL_R &= ~0xE0;
	GPIO_PORTA_DEN_R |= 0xE0;
}

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

void PortC_Init() {
	// Initializing Clock and wait until get stablized
	SYSCTL_RCGCGPIO_R |= 0x04;
	while ((SYSCTL_PRGPIO_R & 0x04) == 0);
	// Initializing Port C pins
	GPIO_PORTC_LOCK_R = magicKey;
	GPIO_PORTC_CR_R |= 0x10;
	GPIO_PORTC_AMSEL_R &= ~0x10;
	GPIO_PORTC_PCTL_R &= ~0x0F0000;
	GPIO_PORTC_DIR_R |= 0x10;
	GPIO_PORTC_AFSEL_R &= ~0x10;
	GPIO_PORTC_DEN_R |= 0x10;
}

void UART7_Init(){
SYSCTL_RCGCUART_R |= 0x80;
while((SYSCTL_PRUART_R & 0x80) == 0);
UART7_CTL_R &= ~UART_CTL_UARTEN;
//set buad rate devider
UART7_IBRD_R = 104;
UART7_FBRD_R = 11;
UART7_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
UART7_CTL_R |= (UART_CTL_UARTEN | UART_CTL_RXE | UART_CTL_TXE);
}

// Initialization of Port E pins
void PortE_Init(){
	// Initializing Clock and wait until get stablized
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_PRGPIO_R & 0x10) == 0){};
	// Initializing Port E pins
	GPIO_PORTE_LOCK_R = magicKey;
	GPIO_PORTE_CR_R |= 0x13;
	GPIO_PORTE_AMSEL_R &= ~0x13;
	GPIO_PORTE_AFSEL_R |= 0x13;
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & ~0xF00FF) | (0x00010011);
	GPIO_PORTE_DEN_R |= 0x13;
	GPIO_PORTE_DIR_R &= ~0x11;
	GPIO_PORTE_DIR_R |= 0x02;
}


/* ----------------------- GPS --------------------*/

void UART5_Init() {
	SYSCTL_RCGCUART_R |= 0x20;
	while ((SYSCTL_PRUART_R & 0x20) == 0);

	UART5_CTL_R &= ~UART_CTL_UARTEN;
	//set buad rate devider
	UART5_IBRD_R = 104;
	UART5_FBRD_R = 11;
	UART5_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
	UART5_CTL_R |= (UART_CTL_UARTEN | UART_CTL_RXE);
}

double ConvertGpsRaw(char *str){ // done
char deg[4] = {0};
uint8_t i;
for(i = 0; i < 3 ; i++)
deg[i] = str[i];
return stringToNum(deg) + (stringToNum(&str[3]) / 60);
}





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

void LCD_command(unsigned char command){
GPIO_PORTA_DATA_R = 0; /* RS = 0, R/W = 0 */
GPIO_PORTB_DATA_R = command;
GPIO_PORTA_DATA_R = EN; /* pulse E */
delayUs(0);
GPIO_PORTA_DATA_R = 0;
if (command < 4)
delay(2); /* command 1 and 2 needs up to 1.64ms */
else
delayUs(40); /* all others 40 us */
}

void LCD_init(void){
delay(20); /* initialization sequence */
LCD_command(0x30);
delay(5);
LCD_command(0x30);
delayUs(100);
LCD_command(0x30);
LCD_command(0x38); /* set 8-bit data, 2-line, 5x7 font */
LCD_command(0x06); /* move cursor right */
LCD_command(0x01); /* clear screen, move cursor to home */
LCD_command(0x0F); /* turn on display, cursor blinking */
}

void LCD_data(unsigned char data){
GPIO_PORTA_DATA_R = RS; /* RS = 1, R/W = 0 */
GPIO_PORTB_DATA_R = data;
GPIO_PORTA_DATA_R = EN | RS; /* pulse E */
delayUs(0);
GPIO_PORTA_DATA_R = 0;
delayUs(40);
}

void LCD_write_line(char* str, uint8_t line_num){ // line_num -> 3 for first line without clear
uint8_t i;
if(line_num == 1) LCD_command(0x80);
else if (line_num == 2) LCD_command(0xC0);
else if (line_num == 3) LCD_command(0x8A);
for (i = 0; str[i] ; i++) LCD_data(str[i]);
}

/* ----------------------- Bluetooth --------------------*/
char UART7_read(){
while((UART7_FR_R & 0x10) == 0x10);
return UART7_DR_R & 0xFF;
}

void UART7_write(char c){
while((UART7_FR_R & UART_FR_TXFF) != 0);
UART7_DR_R = c;
}

void sendCoordinates(char *lat, char *lon){
uint8_t i = 0;
while(lat[i])
UART7_write(lat[i++]);
i = 0;
UART7_write('&');
while(lon[i])
UART7_write(lon[i++]);
UART7_write('\n');
}




/* ----------------------- utilities --------------------*/


//Updating Target Distance 
void updateTargetDistance(void){
	uint8_t end = 0;
	uint8_t pos = 0;
	char data;
	char dis[20];
	while((UART7_FR_R & 0x10) != 0x10 && !end){
		data = UART7_DR_R & 0xFF;
		if(data != '&'){
			dis[pos++] = data;
		}
		else{
			end = 1;
			dis[pos] = '\0';
		}
	}
	target_distance = stringToNum(dis);
	led_control(BLUE);
}

//Calculating Distance between two consecutive Longitudes and Latitudes and Accumulate total distance
double updateDistance(double lat1, double lon1, double lat2, double lon2, uint8_t *coord_cnt){
	const int R = 6371; //Radius of earth in (km)
	double phi1, phi2, delta1, delta2, a, c, d;
	if(*coord_cnt !=1){
		if(*coord_cnt == 3) *coord_cnt = 1;
		else *coord_cnt = *coord_cnt + 1;
		return 0;
	}
	*coord_cnt = *coord_cnt + 1;
	phi1 = deg2rad(lat1);
	phi2 = deg2rad(lat2);
	delta1 = deg2rad(lat2 - lat1);
	delta2 = deg2rad(lon2 - lon1);
	a = sin(delta1 / 2) * sin(delta1 / 2) + cos(phi1) * cos(phi2) * sin(delta2 / 2) * sin(delta2 / 2);
	c = 2 * asin(sqrt(a));
	d = R * c;
	totalDis += d;
	check_destination();
	return totalDis;
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
	if (totalDis < target_distance) {
		led_control(RED);
	}
	else {
		led_control(GREEN);
		distnationReached = 1;
		dis_lat = lat_d;
		dis_lon = lon_d;
	}
}


// makes n milliseconds delay
void delay(int n){
int i,j;
for(i=0;i<n;i++)
for(j=0;j<3180;j++)
{}
}

// makes n Us delay
void delayUs(int n){
 int i, j;
 for(i = 0 ; i < n; i++)
 for(j = 0; j < 3; j++)
 {} /* do nothing for 1 us */
}
// 

 
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


char *int_to_string(int num, char *end){ 
// call with end of char array buffer and will return pointer start of the string
*--end = '\0';
if(!num) *--end = '0';
while(num){
*--end = '0'+num%10;
num/=10;
}
return end;
}

void beeb(uint8_t st){ // output for PE2
	if(st)
		GPIO_PORTC_DATA_R |= 0x10;	
	else
		GPIO_PORTC_DATA_R &= ~0x10;
}

