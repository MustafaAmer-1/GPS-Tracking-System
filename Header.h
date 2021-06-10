#include "tm4c123gh6pm.h"
#include "TM4c123.h"
#include "stdint.h"
#include "Math.h"

#define magicKey 0x4C4F434B
#define PI 3.14159265358979323846
#define OFF 0x00
#define RED 0x02
#define BLUE 0x04
#define GREEN 0x08
/* LCD Defines */
#define RS 0x20 /* PORTA BIT5 mask */
#define RW 0x40 /* PORTA BIT6 mask */
#define EN 0x80 /* PORTA BIT7 mask */
// Functions Prototypes
void PortA_Init();
void PortB_Init();
void PortC_Init();
void PortF_Init(void);
void PortE_Init();
double distance(double lat1, double lon1, double lat2, double lon2);
double deg2rad(double deg);
void led_control(int control);
void check_destination();
void delay(int n);
void display7segment(int num);
double stringToNum(char *str);
void GPS_process(void);
void delayUs(int n);
void LCD_command(unsigned char command);