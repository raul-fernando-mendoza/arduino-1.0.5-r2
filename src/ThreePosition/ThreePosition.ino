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
#define INITIAL_PERIOD 40

unsigned long timeOld;
unsigned long timeNew;
int currentCoil = 3;
int minSensorValue = 1024/2;
int maxSensorValue = 1024/2;
int sensorValue = 0;
int sensorPrevValue1 = 0;
int sensorPrevValue2 = 0;
int curveDirection = 0;

unsigned long logTime[100]; 
int logValues[100][4]; //0 sensorvalue, 1 currentcoil, 2 currentDegrees, 3 directions
int  logIdx = 0;

int period = INITIAL_PERIOD;
int numturns = 0;
int numTurnsAcceleration = 10;
boolean isDebug = false;
int currentDegrees;
unsigned long timeLastTurn = 0;
int previousRPM = 1;
int currentRPM = 1;
int maxSensorValueNext = 1024/2;
int minSensorValueNext = 1024/2;
unsigned long lastSensorValueMaxTime = 0;

unsigned long timeToStartDebug = 0;

boolean coilDriven = false;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(INPUT_PORT_1,INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  while (!Serial) ;
  Serial.println("*Setup");
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
   Serial.println("Starting.");
}

// the loop function runs over and over again forever
void loop() {

  timeNew = millis();
  
  // this section controls de debug  
  if(timeToStartDebug == 0){
    timeToStartDebug = timeNew;
  }  
  
  if( (timeNew - timeToStartDebug) > 2000 && isDebug == true){
    //Serial.print("time since active");
    //Serial.println(timeNew - timeToStartDebug);
    isDebug = false;
    timeToStartDebug =  timeNew + 10000;
  }  
  else if (timeNew >= timeToStartDebug){
    isDebug = true;
  }
  
  // end debug logic 


  
  sensorValue = analogRead(INPUT_PORT_1);


  
  //calculate max and min sensor value
  
  if(logIdx == 0){  
    minSensorValue = minSensorValueNext;
    maxSensorValue = maxSensorValueNext;
    minSensorValueNext = sensorValue;    
    maxSensorValueNext = sensorValue;
  }
  else{  
    if( sensorValue > maxSensorValueNext ){
       maxSensorValueNext = sensorValue;
    }   
        
    if( sensorValue < minSensorValueNext ){
       minSensorValueNext = sensorValue;
    }   
    
  }
    

  currentDegrees = ((sensorValue - minSensorValue) * 180) / (maxSensorValue - minSensorValue);
  

  
  //calculate curve direction and keep prev sensor values
  if ( sensorValue == sensorPrevValue1 && sensorPrevValue1 == sensorPrevValue2 )
        curveDirection = 0;
  else if( sensorValue >= sensorPrevValue1 && sensorPrevValue1 >= sensorPrevValue2  )
       curveDirection = 1;
  else if ( sensorValue <= sensorPrevValue1 && sensorPrevValue1 <= sensorPrevValue2 )
       curveDirection = -1;
  else  curveDirection = 0;     
  
  sensorPrevValue2 = sensorPrevValue1;
  sensorPrevValue1 = sensorValue;

  /*
  if( isDebug ){
    Serial.print("sensorValue:");
    Serial.print(sensorValue);

    Serial.print(" sensorPrevValue1:");
    Serial.print(sensorPrevValue1);
    
    Serial.print(" sensorPrevValue2:");
    Serial.print(sensorPrevValue2);
    
    Serial.print(" curveDirection:");
    Serial.println(curveDirection);
  }
  */
    
  if(  coilDriven == false && numturns > numTurnsAcceleration ) {
    numturns =0;
    if( period > 10){
      period = period - 1;
      Serial.print("period:");
      Serial.println(period);  
      Serial.print("Max sensor value:");
      Serial.println(maxSensorValue);
      Serial.print("Min sensor value:");
      Serial.println(minSensorValue);
      if( period < 11 )
       timeToStartDebug = timeNew;
    }
    else{
      Serial.print("coildriven");
      Serial.println(timeNew);
      coilDriven = true;
      timeToStartDebug =  timeNew;
    }  
  }  
  

  
  if( coilDriven == true ){
    if(  1 == curveDirection  && currentDegrees > 20 && currentCoil == 3){
      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      currentCoil = 1;
      timeOld = timeNew;
    }
    else if( 1==curveDirection && currentDegrees > 120 && currentCoil == 1 ){

      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      digitalWrite(C3, LOW);
      currentCoil = 2;
      
      timeOld = timeNew;
      
      
    }
    else if(  -1 == curveDirection && currentDegrees <= 90 && currentCoil == 2 ){
      /*      
      if( 1 == currentCoil ){
        
        currentRPM = (1000 / (timeNew - timeLastTurn)) * 60;
        if( abs( currentRPM - previousRPM ) > 300 and isDebug == true ){
          Serial.println("RPM:");
          Serial.println(currentRPM);
        }  
        previousRPM = currentRPM;
        timeLastTurn = timeNew;
        
      } 
      */  
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);
      currentCoil = 3;  
      timeOld = timeNew;
    }
    else {
      /*
      if( 2 == currentCoil){
        //do nothing is crossing up
        currentCoil = 2;
      }
      else if( 1 == currentCoil){
        digitalWrite(C1, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C3, HIGH);
        currentCoil = 3;
      }
      else if( 3 == currentCoil){
        digitalWrite(C1, LOW);
        digitalWrite(C2, HIGH);
        digitalWrite(C3, LOW);
        currentCoil = 2;
      }
      
        if(1 == currentCoil && currentDegrees > 100){
          digitalWrite(C1, LOW);
          digitalWrite(C2, HIGH);
          digitalWrite(C3, LOW);
          currentCoil = 2;
        }  
       */
    }  
  }
  if( (coilDriven == false && (timeNew - timeOld) > period)  || ( coilDriven == true && (timeNew - timeOld) > 100) ){
   
    if( coilDriven == true ){
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      Serial.print("Error while sensor driving elapsed:");
      Serial.println(timeNew - timeOld);
      Serial.print("timeOld:");
      Serial.println(timeOld);
      Serial.print("timeNew:");
      Serial.println(timeNew);
      Serial.print("MinSensor:");
      Serial.println(minSensorValue);
      Serial.print("MaxSensor:");
      Serial.println(maxSensorValue);
      Serial.print("currentDegrees:");
      Serial.print(currentDegrees);
      Serial.print("idx:");
      Serial.println(logIdx);
      for(int i=logIdx+1; i<100; i++){
         Serial.print(i);
         Serial.print("\t");         
         Serial.print(logTime[i]);           
         Serial.print("\t");         
         Serial.print(logValues[i][0]);
         Serial.print("\t");
         Serial.print(logValues[i][1]);
         Serial.print("\t");         
         Serial.print(logValues[i][2]);
         Serial.print("\t");         
         Serial.println(logValues[i][3]);          
      }
      for(int i=0; i<logIdx+1; i++){
         Serial.print(i);
         Serial.print("\t");         
         Serial.print(logTime[i]);           
         Serial.print("\t");         
         Serial.print(logValues[i][0]);
         Serial.print("\t");
         Serial.print(logValues[i][1]);
         Serial.print("\t");         
         Serial.print(logValues[i][2]);
         Serial.print("\t");         
         Serial.println(logValues[i][3]);         
      }
      numTurnsAcceleration = 10;
      period = INITIAL_PERIOD;      
      delay(10000);
    }
    coilDriven = false;
    if(3 == currentCoil ){
      //go to position 1
     digitalWrite(LED_BUILTIN, HIGH);

      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      currentCoil = 1;
    }  
    else if(1 == currentCoil ){
      //go to position 1
      digitalWrite(LED_BUILTIN, LOW);

      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      digitalWrite(C3, LOW);
      currentCoil = 2; 
    }  
    else {
      //go to position 1
      
      digitalWrite(LED_BUILTIN, LOW);

      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);
      currentCoil = 3;  
      numturns++;  
    } 
    timeOld = timeNew;
  }
  
  if( logIdx < 99) logIdx++; else logIdx=0;
  
  logTime[logIdx] = timeNew;
  logValues[logIdx][0] = sensorValue;
  logValues[logIdx][1] = currentCoil * 100; 
  logValues[logIdx][2] = currentDegrees;  
  logValues[logIdx][3] = curveDirection;
  
  if( true==isDebug && coilDriven == false) {
        if(logIdx == 0){
          Serial.print("MinSensor:");
          Serial.println(minSensorValue);
          Serial.print("MaxSensor:");
          Serial.println(maxSensorValue);            
        }         
         Serial.print(logIdx);
         Serial.print("\t");         
         Serial.print(logTime[logIdx]);
         Serial.print("\t");
         Serial.print(logValues[logIdx][0]);
         Serial.print("\t");         
         Serial.print(logValues[logIdx][1]);
         Serial.print("\t");         
         Serial.print(logValues[logIdx][2]);         
         Serial.print("\t");         
         Serial.println(logValues[logIdx][3]);
  }  
}
