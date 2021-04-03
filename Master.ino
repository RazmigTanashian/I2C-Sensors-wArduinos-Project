#include <Wire.h> //i2c
#include <LiquidCrystal_I2C.h> //i2c lcd

#define SLAVE_ADDRESS 9
#define BUZZ_PIN 2
#define RED_LED 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long curr_time;
const unsigned long event_update_LCD = 1000;
unsigned long prev_time = 0;

int b_count;

uint8_t sensors_data[4]; //{sonar_data, photo-resistor_data, temperature_data, tiltswtich_data}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); //passing in no address, so it knows that it's a master
  lcd.begin();

  //set pins 2 and 5 as digital output
  DDRD |= 0X24; //0b0010.0100

  //initialize lcd text
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  
  lcd.setCursor(9,0);
  lcd.print("PhR:");
  
  lcd.setCursor(0,1);
  lcd.print("Sonar:");
  
  lcd.setCursor(10,1);
  lcd.print("Tilt:");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  curr_time = millis();
  
  while(readI2C(SLAVE_ADDRESS) < 255) { //while the ACK bit isn't sent as the correct 255
    Serial.print("Waiting...");
  }
  for(b_count = 0; b_count < 4; b_count++) { //after the ack bit is properly received. READ in the data of the sensors.
    sensors_data[b_count] = readI2C(SLAVE_ADDRESS);
  }
  for(int i = 0; i < 4; i++) {
    Serial.print(sensors_data[i]);
    Serial.print("\t");
  }
  Serial.println();
  
  updateLCD();

  //WARNING BUZZER AND RED LED
  if(sensors_data[3] == 1 || sensors_data[2] > 30 || (sensors_data[0] <= 15 && sensors_data[0] != 0)) {
    PORTD |= 0X24; //0b0010.0100
  }
  else { //Shut warning components off
    PORTD &= ~(0b00100100);
  }
  
}

uint8_t readI2C(int address) {
  uint8_t b_val;
  long entry = millis();

  //request 1 byte from the given address
  Wire.requestFrom(address, 1);

  //while there is not data available, we wait..
  while(Wire.available() == 0 && (millis() - entry) < 100) {
    Serial.print("Waiting...");
  }

  //when data becomes available, we read into the b_val
  if(millis() - entry < 100) {
    b_val = Wire.read();
  }
  
  return b_val;
}

void updateLCD() {
  if(curr_time - prev_time >= event_update_LCD) {
     lcd.setCursor(5,0);
     lcd.print(sensors_data[2]); //TEMPERATURE
     lcd.setCursor(13,0);
     lcd.print(sensors_data[1]); //PHOTO-RESISTOR
     lcd.setCursor(6,1);
     lcd.print(sensors_data[0]); //SONAR
     lcd.setCursor(15,1);
     lcd.print(sensors_data[3]); //TILT SWITCH
     prev_time = curr_time;
  }
}
