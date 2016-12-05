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
#define LED_BUILTIN 13
#define C1 12
#define C2 11
#define C3 10
#define R1 A1

unsigned long timeOld;
unsigned long timeNew;
int currentCoil = 3;
int minValue = 177;
int maxValue = 186;
int averageValue = 0;
int sensorValueNew = 0;
int sensorValueOld = 0;
int period = 400;
int active = period * 0.95;
int numVueltas = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(R1, INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  while (!Serial) ;

  
  Serial.println("Starting.");
}

// the loop function runs over and over again forever
void loop() {
/*
  Serial.println("Setup");    
  for( int i=0; i<3;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  

  timeOld = millis();

  long sum = 0;
  minValue = analogRead(R1);
  maxValue = analogRead(R1);
  
  for(int i=0; i<100; i++){
    int v = analogRead(R1);
    if( v < minValue ){ minValue = v-8; };
    if( v > maxValue ){ maxValue = v+8; };
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

  for( int i=0; i<3;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
*/
     sensorValueNew = analogRead(R1);
    if( abs(sensorValueOld - sensorValueNew) > 5 ){
    //   digitalWrite(LED_BUILTIN, HIGH);
    //   currentStatus = HIGH;
       timeOld = millis();
       Serial.print(timeOld);
       Serial.print("\t");
       Serial.print(sensorValueOld - sensorValueNew);
       Serial.print("\t");
       Serial.println(sensorValueNew);
       sensorValueOld = sensorValueNew ;
    }
    
   
/*
   timeNew = millis();
   timeOld = millis();
  
  sensorValueNew = analogRead(R1);
  sensorValueOld = sensorValueNew ;
  
  while((timeNew - timeOld) < 10000 ){ 
    timeNew = millis();
  
 
    sensorValueNew = analogRead(R1);
  //if result is negative the north is facing the coil so 0 should be send
    if( sensorValueNew < minValue and sensorValueOld != sensorValueNew ){
    //   digitalWrite(LED_BUILTIN, HIGH);
    //   currentStatus = HIGH;
       Serial.print("L\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(sensorValueNew);
    }
    else if( sensorValueNew  > maxValue and sensorValueOld != sensorValueNew ){
    //   digitalWrite(LED_BUILTIN, LOW);
    //   currentStatus = LOW;
       Serial.print("H\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(sensorValueNew);
    }
    sensorValueOld = sensorValueNew ;
 }
 Serial.println("End.");
*/
}
