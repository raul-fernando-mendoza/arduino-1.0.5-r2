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
int minValue = 177;
int maxValue = 186;
int averageValue = 0;
int sensorValueNew = 0;
int period = 1000;
int numVueltas = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  while (!Serial) ;
  Serial.println("Setup");
  timeOld = millis();

  long sum = 0;
  minValue = analogRead(A0);
  maxValue = analogRead(A0);
  
  
  
  for(int i=0; i<100; i++){
    int v = analogRead(A0);
    if( v < minValue ){ minValue = v-1; };
    if( v > maxValue ){ maxValue = v+1; };
    sum += v;
    delay(100);
  }

 
  Serial.print("  Average: ");
  averageValue = sum / 100;
  Serial.print(averageValue);

  Serial.print("  Min: ");

  Serial.print(minValue);

  Serial.print("  Max: ");
  Serial.print(maxValue);
  
  Serial.println();
  delay(5000);
   Serial.println("Starting.");
}

// the loop function runs over and over again forever
void loop() {

  timeNew = millis();
  
  sensorValueNew = analogRead(A0);
  //if result is negative the north is facing the coil so 0 should be send
  
  if( sensorValueNew < minValue ){
  //   digitalWrite(LED_BUILTIN, HIGH);
  //   currentStatus = HIGH;
     Serial.print("L\t");
     Serial.print(timeNew);
     Serial.print("\t");
     Serial.println(sensorValueNew);
     
  }
  else if( sensorValueNew  > maxValue ){
  //   digitalWrite(LED_BUILTIN, LOW);
  //   currentStatus = LOW;
     Serial.print("H\t");
     Serial.print(timeNew);
     Serial.print("\t");
     Serial.println(sensorValueNew);
  }
  else 
  if( (timeNew - timeOld) > period ){
    //numVueltas += 1;
    //Serial.print("vuelta:");
    //Serial.println(numVueltas);
    //if (numVueltas == 10) {delay(10000);}
    /*if( period > 50 ){
      period = period * 0.98;
      Serial.print(period);
      Serial.println(" time elapsed");
    } 
    */   
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
