#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "Math.h"

#define magicKey 0x4C4F434B
#define PI 3.14159265358979323846
#define OFF 0x00
#define RED 0x02
#define BLUE 0x04
#define GREEN 0x08

// Functions Prototypes
void PortB_Init();
void PortF_Init(void);
void PortD_Init();
double distance(double lat1, double lon1, double lat2, double lon2);
double deg2rad(double deg);
void led_control(int control);
void check_destination();