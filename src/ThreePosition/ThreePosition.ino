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
#define SENSOR_PORT A0
#define LED_BUILTIN 13
#define C1 10
#define C2 11
#define C3 12

#define INITIAL_PERIOD 100
#define MIN_PERIOD 30
#define DEBUG_TIME 1000
#define DEBUG_TIME_QUIET 10000
#define CHANGE_PERIOD_TIME 1000
#define SENSOR_DRIVEN_LIMIT 50
#define SENSOR_TEST_NUMBER 3000

unsigned long timeOld = millis();
unsigned long timeNew = millis();
unsigned long logTime[100];

int period = INITIAL_PERIOD;
int currentCoil = C2;
int sensorValue = 0;
int sensorMinValue = 0;
int sensorMaxValue = 0;
int sensorMinValueNext = 0;
int sensorMaxValueNext = 0;
int sensorAverageValue = 0;
int sensorIdx = 0;

int currentDegrees = 0;

int sensorPrevValue1 = 0;
int sensorPrevValue2 = 0;
int curveDirection = 0;
boolean coilDriven = false;

unsigned long timeToStartDebug = 0;
int logValues[100][4]; //0 sensorvalue, 1 currentcoil, 2 currentDegrees, 3 directions
boolean isDebug = false;
int  logIdx = 0;
unsigned timeToChangePeriod = millis() + CHANGE_PERIOD_TIME;
int previousSensorValue = 0;
unsigned long timeToReportSpeed = 0;
long timeLastTurn = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(SENSOR_PORT,INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  //while (!Serial) ;
  Serial.println("Setup");
  
  //show the user that we are inititing
  for( int i=0; i<5;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  //find the normal value for the sensor
  long sum =0 ;
  Serial.println("sensor value");
  for( int i=0; i<100;i++){
    
    sensorValue = analogRead(SENSOR_PORT);
    sum += sensorValue;
    Serial.println(sensorValue);
    delay(10);
  }
  sensorAverageValue = sum/100;
  Serial.print("SensorAverageValue:");
  Serial.println( sensorAverageValue ); 

  //initalize send min and max used to set the sensor driven flag
  sensorMinValue = analogRead(SENSOR_PORT);
  sensorMaxValue = analogRead(SENSOR_PORT);
  previousSensorValue = analogRead(SENSOR_PORT);
  coilDriven = false;


  //setup time variables
  timeNew = millis();
  timeOld = timeNew;

  //debug initialize
  timeToStartDebug = timeNew;
  isDebug = true;

  timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;

  //delay(10000);
  Serial.println("Starting.");
}

// the loop function runs over and over again forever

void loop() {
  do{
    previousSensorValue = sensorValue;
    sensorValue = analogRead(SENSOR_PORT);
  }while( abs(sensorValue - previousSensorValue) > 50 );
  

     
  timeNew = millis();
  
  // this section controls de debug  
  if( (timeNew - timeToStartDebug) > DEBUG_TIME && isDebug == true){
    //Serial.print("time since active");
    //Serial.println(timeNew - timeToStartDebug);
    isDebug = false;
    Serial.print("MinValue:");
    Serial.print(sensorMinValue);
    Serial.print("\tAverageValue");
    Serial.print(sensorAverageValue);
    Serial.print("\tMaxValue:");
    Serial.print(sensorMaxValue);
    Serial.print("\tcoilDriven");
    Serial.print(coilDriven);
    Serial.print("period");
    Serial.println(period);
    timeToStartDebug =  timeNew + DEBUG_TIME_QUIET;
  }  
  else if (timeNew >= timeToStartDebug && false == isDebug  ){
    isDebug = true;


         Serial.print("logIdx");
         Serial.print("\t");
         Serial.print("timeNew");
         Serial.print("\t");
         Serial.print("sensorValue");
         Serial.print("\t");
         Serial.print("currentCoil");
         Serial.print("\t");         
         Serial.print("currentDegrees");
         Serial.print("\t");         
         Serial.print("curveDirection");
         Serial.print("\t");         
         Serial.print("coilDriven");       
         Serial.print("\n");

  }
  // end debug logic 

  //calculate max and min sensor value
  if( sensorIdx >= SENSOR_TEST_NUMBER  ){
    sensorIdx = 0;
    sensorMinValue = sensorMinValueNext;
    sensorMaxValue = sensorMaxValueNext;
    sensorMinValueNext = sensorValue;    
    sensorMaxValueNext = sensorValue;
  }
  else{  
    sensorIdx++;
    if( sensorValue > sensorMaxValueNext ){
       sensorMaxValueNext = sensorValue;
    }   
    if( sensorValue < sensorMinValueNext ){
       sensorMinValueNext = sensorValue;
    }   
  }

  //calculate current degrees
  currentDegrees = ((sensorValue - sensorMinValue) * 180) / (sensorMaxValue - sensorMinValue);
  
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

  //calculate if the coild driven should start
  if( (sensorMaxValue - sensorMinValue) > SENSOR_DRIVEN_LIMIT && period < 30 ){
    coilDriven = true;
  }
  else{
    if( true == coilDriven ){ // something when wrong
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
      Serial.println(sensorMinValue );
      Serial.print("MaxSensor:");
      Serial.println(sensorMaxValue);
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
      delay(10000);
      period = INITIAL_PERIOD;
      timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
    }
    coilDriven = false;
    
  }
    
   if( logIdx < 99) logIdx++; else logIdx=0;
  
  logTime[logIdx] = timeNew;
  logValues[logIdx][0] = sensorValue;
  logValues[logIdx][1] = currentCoil * 10; 
  logValues[logIdx][2] = currentDegrees;  
  logValues[logIdx][3] = curveDirection;
  
  if( true==isDebug ) {
    
         Serial.print(logIdx);
         Serial.print("\t");
         Serial.print(timeNew);
         Serial.print("\t");
         Serial.print(sensorValue -550);
         Serial.print("\t");
         Serial.print(((currentCoil -9)*100)  );
         Serial.print("\t");         
         Serial.print(currentDegrees);
         Serial.print("\t");         
         Serial.print(curveDirection);
         Serial.print("\t");         
         Serial.print(coilDriven);       
         Serial.print("\n");
  }    
  // end debug logic 
 
  //should I acelerate
  if( timeNew > timeToChangePeriod ){
    if( period > MIN_PERIOD ){
      period = period * 0.8;
    }
    timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
  }  
  
  
  if( coilDriven == true ){
    if(  -1 == curveDirection  && currentDegrees < 120 && currentCoil == C3){
      //go to 1
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      digitalWrite(C1, HIGH);
      currentCoil = C1;
     
    }
    else if( 1==curveDirection && currentDegrees < 120 && currentCoil == C1 ){
      //go to 2
      digitalWrite(C1, LOW);
      digitalWrite(C3, LOW);
      digitalWrite(C2, HIGH);
      currentCoil = C2;
    }
    else if( (1 == curveDirection && currentDegrees > 120 && currentCoil == C2) || (-1==curveDirection && currentDegrees > 120 && currentCoil == C2 )){
          //go to 3  
        
      if( timeNew > timeToReportSpeed ){
        int currentRPM = (1000 / (timeNew - timeLastTurn)) * 60;
        Serial.print("RPM:");
        Serial.println(currentRPM);
        timeToReportSpeed = timeNew + 10000;
      } 
      timeLastTurn = timeNew; 

      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);

      currentCoil = C3;  
    }
  }
  else{
    if( (timeNew - timeOld) > period ){

      if(C1 == currentCoil ){
        //go to position 2
       digitalWrite(LED_BUILTIN, HIGH);
  

        digitalWrite(C3, LOW);
        digitalWrite(C1, LOW);
        digitalWrite(C2, HIGH);
        
        currentCoil = C2;
      }  
      else if(C2 == currentCoil ){
        //go to position 3
        digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C3, HIGH);
        currentCoil = C3; 
      }  
      else if(C3 == currentCoil ){
        //go to position 1
        
        digitalWrite(LED_BUILTIN, LOW);

        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
  
        digitalWrite(C1, HIGH);
        currentCoil = C1;  
      }
      timeOld = timeNew;
    }
  }
}
