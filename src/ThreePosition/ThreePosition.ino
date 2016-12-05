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
#define INPUT_PORT_1 A1
#define INPUT_PORT_2 A2
#define LED_BUILTIN 13
#define C1 12
#define C2 11
#define C3 10

unsigned long timeOld;
unsigned long timeNew;
int currentCoil = 3;
int minValue = 177;
int maxValue = 186;
int averageValue = 0;
int sensorValue = 0;
int sensorPrevValue = 0;

long logValues[100][3];
int idx = 0;

int period = 50;
int numturns = 0;
int numTurnsAcceleration = 10;
boolean isDebug = true;

unsigned long time10 = 0;

boolean coilDriven = false;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(INPUT_PORT, INPUT);
  pinMode(INPUT_PORT_1,INPUT);
  pinMode(INPUT_PORT_2,INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  //while (!Serial) ;
  Serial.println("*********************  Setup");
  /*
  for( int i=0; i<3;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  

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

  for( int i=0; i<3;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
*/
  delay(5000);
   Serial.println("***************************************");  
   Serial.println("Starting.");
}

// the loop function runs over and over again forever
void loop() {

  timeNew = millis();
  sensorPrevValue = sensorValue;
  sensorValue = analogRead(INPUT_PORT_1);

  /* this section controls de debug */ 
  if(period == 10 && time10 == 0){
    time10 = timeNew;
  }  
  
  if( (timeNew - time10) > 2000 && isDebug == true){
    //Serial.print("time since active");
    //Serial.println(timeNew - time10);
    isDebug = false;
    time10 =  timeNew + 10000;
  }  
  else if (timeNew > time10){
    isDebug = true;
  }
  /* end debug logic */
  
 if(  numturns > numTurnsAcceleration ) {
    numturns =0;
    if( period > 10){
      period = period - 1;
      Serial.print("period:");
      Serial.println(period);      
    }
  } 

  if( period == 10 && sensorValue> 15 && sensorValue >= sensorPrevValue  ){
  /*  
      if( isDebug ) {
        Serial.println("c2");
       Serial.print(sensorValue);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-20);       
      }  
    */
    if(coilDriven == false){
       //Serial.print("s1");
       coilDriven = true;  
     }  
     logValues[idx][0] = timeNew;
     logValues[idx][1] = sensorValue;
     logValues[idx][2] = 20;
     if( idx < 99) idx++; else idx=0;
     
      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      digitalWrite(C3, LOW);
      currentCoil = 2;
      
      timeOld = timeNew;
      
  }
  else if( period == 10 && sensorValue> 15 && sensorValue  <= sensorPrevValue  ){
   /* 
      if( isDebug ) {
        Serial.println("c3");
       Serial.print(sensorValue);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-30);       

      }  
  */    
    if(coilDriven == false){
       //Serial.println("s2");
       coilDriven = true;  
     }  
     logValues[idx][0] = timeNew;
     logValues[idx][1] = sensorValue;
     logValues[idx][2] = 30;
     if( idx < 99) idx++; else idx=0;

      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);
      currentCoil = 3;  
      timeOld = timeNew;
  }
  else if( period == 10 && coilDriven == true && sensorValue == 0  ){
    /*
     if( isDebug ) {
       Serial.println("c1");
       Serial.print(sensorValue);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-10);       
       
     }
     */
     logValues[idx][0] = timeNew;
     logValues[idx][1] = sensorValue;
     logValues[idx][2] = 10;
     if( idx < 99) idx++; else idx=0;
     
      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      currentCoil = 0;
  }
  else if( period == 10 && coilDriven == true ){
    /*
     if( isDebug ) {
       Serial.println("c1");
       Serial.print(sensorValue);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-10);       
       
     }
     */
     logValues[idx][0] = timeNew;
     logValues[idx][1] = sensorValue;
     logValues[idx][2] = 0;
     if( idx < 99) idx++; else idx=0;
  }  
  if( (coilDriven == false && (timeNew - timeOld) > period)  || ( coilDriven == true && (timeNew - timeOld) > 100) ){
    if( coilDriven == true ){
      Serial.print("Error while sensor driving elapsed:");
      Serial.println(timeNew - timeOld);
      Serial.print("Error while sensor driving idx:");
      Serial.println(idx);
      for(int i=idx; i<100; i++){
         Serial.print(i);
         Serial.print("\t");         
         Serial.print(logValues[i][0]);
         Serial.print("\t");
         Serial.print(logValues[i][1]);
         Serial.print("\t");         
         Serial.println(logValues[i][2]);
      }
      for(int i=0; i<idx; i++){
         Serial.print(i);
         Serial.print("\t");         
         Serial.print(logValues[i][0]);
         Serial.print("\t");
         Serial.print(logValues[i][1]);
         Serial.print("\t");         
         Serial.println(logValues[i][2]);
      }
      numTurnsAcceleration = 10;
      period = 50;      
    }
    coilDriven = false;
    if(3 == currentCoil ){
      //go to position 1
      /*
     if( isDebug ) {
       Serial.println("Position 1 vuelta:");
       Serial.print(numturns);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-10);       
       
     }
     digitalWrite(LED_BUILTIN, HIGH);
*/
      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      currentCoil = 1;
    }  
    else if(1 == currentCoil ){
      //go to position 1
      /*
      if( isDebug ) {
        Serial.println("Position 2 vuelta:");
       Serial.print(numturns);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-20);       
      }  
      digitalWrite(LED_BUILTIN, LOW);
*/
      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      digitalWrite(C3, LOW);
      currentCoil = 2; 
    }  
    else {
      //go to position 1
      /*
      if( isDebug ) {
        Serial.println("Position 3 vuelta:");
       Serial.print(numturns);                         
       Serial.print("\t");
       Serial.print(timeNew);
       Serial.print("\t");
       Serial.println(-30);       

      }  
      digitalWrite(LED_BUILTIN, LOW);
*/
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);
      currentCoil = 3;  
      numturns++;    
    } 
    timeOld = timeNew;
  }

  
}
