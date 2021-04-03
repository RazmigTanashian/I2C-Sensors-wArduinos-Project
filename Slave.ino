#include <Wire.h> //i2c library
#include <NewPing.h> //HC-SR04 library
#include <dht.h> //Thermostat library

#define SLAVE_ADDRESS 9

//HC-SR04 sensor in 1-pin mode
#define TRIGGER_PIN 10
#define ECHO_PIN 10
#define MAX_DISTANCE 260
int distance_val;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//photo resistor, dht11, and tiltswitch
#define PHOTO_PIN A0
int photo_val;
#define DHT_PIN 6
#define TILT_PIN 2
dht DHT;

unsigned long curr_time;
const unsigned long event_interval_dht = 2000;
unsigned long prev_time_dht = 0;
const unsigned long event_interval_main = 500;
unsigned long prev_time_main = 0;

//data storage for the 4 sensors
int sensors_data[4]; //[0] -> sonar, [1] -> photo-resistor, [2] -> temperature, [3] -> tilt switch

int b_count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(PHOTO_PIN, INPUT);
  pinMode(DHT_PIN, INPUT);
  pinMode(TILT_PIN, INPUT);
  
  PORTD |= (1<<2); //internal pullup res, aka digitalWrite(TILIT_PIN, HIGH);
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  curr_time = millis();
  
  if(curr_time - prev_time_main >= event_interval_main) {
    readSensors(curr_time);

    prev_time_main = curr_time;
  }
}

void requestEvent() {

  uint8_t b_val;

  switch(b_count) {
    case 0:
      b_val = 255;
      break;
    case 1:
      b_val = sensors_data[0];
      break;
    case 2:
      b_val = sensors_data[1];
      break;
    case 3:
      b_val = sensors_data[2];
      break;
    case 4:
      b_val = sensors_data[3];
      break;
  }

  Wire.write(b_val); //write data to master
  
  b_count += 1; //move to next sensor data
  if(b_count > 4) { //reset
    b_count = 0;
  }
}

void readSensors(unsigned long current_time) {
  //************
  sensors_data[0] = sonar.ping_cm();
  if(sensors_data[0] > 254) {
    sensors_data[0] > 254;
  }
  //************

  //************
  photo_val = analogRead(PHOTO_PIN);
  sensors_data[1] = photo_val * (255. / 1023.); //adjust the scale from 0-1023 to 0-255 to be able to fit in a byte
  //************

  //************
  //Read only every two seconds.
  if(current_time - prev_time_dht >= event_interval_dht) {
    DHT.read11(DHT_PIN);
    sensors_data[2] = DHT.temperature;

    prev_time_dht = current_time;
  }
  //************

  //************
  sensors_data[3] = digitalRead(TILT_PIN); //PIND & (1 << 2);
  //************
}
