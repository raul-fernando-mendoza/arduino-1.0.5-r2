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

#define INPUT_PORT A0
#define LED_BUILTIN 13
#define C1 10
#define C2 11
#define C3 12

int sensorValueNew = 0;
int sensorValueOld = 1024;
unsigned long timeNew = 0L;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(INPUT_PORT,INPUT);  
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  //while (!Serial) ;

   Serial.println("Starting.");
    digitalWrite(C1, HIGH);
    delay(3000);     
    digitalWrite(C1, LOW);
    
    digitalWrite(C2, HIGH);
    delay(3000);     
    digitalWrite(C2, LOW);

    digitalWrite(C3, HIGH);
    delay(3000);     
    digitalWrite(C3, LOW);
    
    int sensorValue = 858;
    int sensorMinValue = 655;
    int sensorMaxValue = 979;
    float currentDegrees = 0;
      float f = (sensorValue - sensorMinValue) ;
      Serial.print("f:");
      Serial.println(f);
       f = f * 180;
      Serial.print("f:");      
      Serial.println(f);
      currentDegrees = (f/(sensorMaxValue - sensorMinValue));
      Serial.print("currentDegrees:");
      Serial.println(currentDegrees); 
      currentDegrees = (((float)sensorValue - sensorMinValue) * 180/(sensorMaxValue - sensorMinValue));  
      Serial.print("currentDegrees:");
      Serial.println(currentDegrees);      
}

// the loop function runs over and over again forever
void loop() {

  timeNew = millis();
 
  sensorValueNew = analogRead(INPUT_PORT);
  
    if( abs( sensorValueNew  - sensorValueOld) > 6 ){
     /*  Serial.print(sensorValueNew  - sensorValueOld);
       Serial.print("\t");
       Serial.print(timeNew);
       */

       Serial.println(sensorValueNew);
       sensorValueOld = sensorValueNew;
    }

    
}
