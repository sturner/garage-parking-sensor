#include "SR04.h"
#define TRIG_PIN 26
#define ECHO_PIN 30

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    3

#define COL_SIZE 8
#define RED_ZONE_INCHES 6
#define YELLOW_ZONE_INCHES 12
#define GREEN_ZONE_INCHES 18
#define ALERT_THRESHOLD_MILLIS 5000  // 5 seconds

// Hardware SPI connection
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long d;
long inches = GREEN_ZONE_INCHES + 1;
long lastMeasure = 0;
long measureChange = 0;
unsigned long lastTimestamp = 0;
unsigned long timeDelta = 0;
unsigned long timestamp = 0;
char buffer[100];

uint8_t RED_ZONE_GRID[COL_SIZE] =
  {
    0b11000011,
    0b11100111,
    0b01111110,
    0b00111100,
    0b00111100,
    0b01111110,
    0b11100111,
    0b11000011
  };

uint8_t YELLOW_ZONE_GRID[COL_SIZE] =
  {
    0b00111100,
    0b01111110,
    0b11000011,
    0b11000011,
    0b11000011,
    0b11000011,
    0b01111110,
    0b00111100
  };
uint8_t GREEN_ZONE_GRID[COL_SIZE] =
  {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  };


void renderGrid(long measure) {
    uint8_t* grid = GREEN_ZONE_GRID;
    if(measure < RED_ZONE_INCHES) {
        grid = RED_ZONE_GRID;
    } 
    else if(measure < YELLOW_ZONE_INCHES) {
        grid = YELLOW_ZONE_GRID;
    } 
    mx.setBuffer(COL_SIZE-1, COL_SIZE, grid);
}

void setup() {
   Serial.begin(9600);
   delay(1000);
   mx.begin();
}

void loop() {
   d = sr04.Distance();
   inches = ceil(d/2.54);
   delay(100);
   
   measureChange = abs(lastMeasure - inches);
   if(measureChange  > 0) {
       renderGrid(inches);
       lastMeasure = inches;
       lastTimestamp = millis();
   } else {
       timestamp = millis();
       timeDelta = abs(timestamp - lastTimestamp);
       if(timeDelta > ALERT_THRESHOLD_MILLIS)  {
           renderGrid(GREEN_ZONE_INCHES);
       }
   }
}


/*
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}
 
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
*/

