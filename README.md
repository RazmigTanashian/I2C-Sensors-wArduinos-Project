# I2C-Sensors-wArduinos-Project
Using slave Arduino as an I2c device to gather data for a master Arduino


I was in the need of many sensors that supported I2C for the purpose of not using so many GPIO pins on the Arduino Uno. None of the
sensors I needed didn't support I2C. I decided to use a second Arduino Uno to play the role of a slave device. 
It acted as an I2C device that would collect data from all the sensors and relay that data through the I2C bus back to the master Arduino. 
After the master Arduino receives the data, it then will activate the active buzzer and red LED if some values from the sensors were too high/low. I took advantage of
the I2C LCD as a way to display the data to a user. 

   Master Arduino
   /            \
Slave Arduino    LCD
    |
Sensors
