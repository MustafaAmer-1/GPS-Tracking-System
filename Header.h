#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "Math.h"

#define magicKey 0x4C4F434B
#define PI 3.14159265358979323846

// Functions Prototypes
void PortB_Init();
double distance(double lat1, double lon1, double lat2, double lon2);
double deg2rad(double deg);
