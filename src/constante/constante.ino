

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care 
  of use the correct LED pin whatever is the board used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
*/
unsigned long timeOld;
unsigned long timeNew;
int currentStatus = HIGH;
int sensorValueOld = 0;
int sensorValueNew = 0;
int baseValue = 508;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
   //while (!Serial) ;
   // Serial.println("Starting");
   timeOld = millis();
}

// the loop function runs over and over again forever
void loop() {
  timeNew = millis();
  if( timeNew - timeOld > 500 ){
    //Serial.println("time elapsed");
    if(currentStatus == HIGH ){
      Serial.println("switch to low");
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      currentStatus = LOW;
    }
    else{
      Serial.println("switch to high");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      currentStatus = HIGH;
    }
    timeOld = timeNew;
  }
}
