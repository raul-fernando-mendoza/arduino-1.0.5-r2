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

#define INITIAL_PERIOD 3000
#define MIN_PERIOD 300
#define DEBUG_TIME 1000
#define DEBUG_TIME_QUIET 10000
#define CHANGE_PERIOD_TIME 1000
#define SENSOR_DRIVEN_LIMIT 20
#define SENSOR_TEST_NUMBER 1000

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


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);

  pinMode(SENSOR_PORT,INPUT);

  for( int i=0; i<5 ;i++){
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  //while (!Serial) ;
  Serial.println("Starting.");

  //find the normal value for the sensor
  long sum =0 ;
  for( int i=0; i<100;i++){
    sum += analogRead(SENSOR_PORT);
    delay(10);
  }
  sensorAverageValue = sum/100;
  Serial.print("SensorAverageValue:");
  Serial.println( sensorAverageValue );  
      
  
  
  sensorMinValue = analogRead(SENSOR_PORT);
  sensorMaxValue = analogRead(SENSOR_PORT);
  
  
  
  timeNew = millis();
  timeToStartDebug = timeNew;
  isDebug = true;
  timeOld = timeNew;
  timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
   
}

// the loop function runs over and over again forever
void loop() {

  sensorValue = analogRead(SENSOR_PORT);
  timeNew = millis();
  
  // this section controls de debug  
  if( (timeNew - timeToStartDebug) > DEBUG_TIME && isDebug == true){
    //Serial.print("time since active");
    //Serial.println(timeNew - timeToStartDebug);
    isDebug = false;
    timeToStartDebug =  timeNew + DEBUG_TIME_QUIET;
  }  
  else if (timeNew >= timeToStartDebug && false == isDebug ){
    isDebug = true;
    Serial.print("MinValue:");
    Serial.print(sensorMinValue);
    Serial.print(" AverageValue");
    Serial.print(sensorAverageValue);
    Serial.print(" MaxValue:");
    Serial.print(sensorMaxValue);
    Serial.print(" coilDriven");
    Serial.println(coilDriven);
  }



  //calculate max and min sensor value
  if(SENSOR_TEST_NUMBER == sensorIdx){  
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
  if( sensorMinValue < (sensorAverageValue - SENSOR_DRIVEN_LIMIT) && sensorMaxValue > (sensorAverageValue + SENSOR_DRIVEN_LIMIT) ){
    //coilDriven = true;
  }
  else{
    coilDriven = false;
    period = INITIAL_PERIOD;
  }

  if( logIdx < 99) logIdx++; else logIdx=0;
  
  logTime[logIdx] = timeNew;
  logValues[logIdx][0] = sensorValue;
  logValues[logIdx][1] = currentCoil * 100; 
  logValues[logIdx][2] = currentDegrees;  
  logValues[logIdx][3] = curveDirection;
  
  if( true==isDebug) {
    
         Serial.print(logIdx);
         Serial.print("\t");
         Serial.print(timeNew);
         Serial.print("\t");
         Serial.print(sensorValue);
         Serial.print("\t");
         Serial.print(currentCoil * 100);
         Serial.print("\t");         
         Serial.print(currentDegrees);
         Serial.print("\t");         
         Serial.print(curveDirection);
         Serial.print("\t");         
         Serial.print(coilDriven);       
         Serial.print("\n");
  }    
  // end debug logic 
  if( true == coilDriven ){
    if(  -1 == curveDirection  && currentDegrees < 90  && (currentCoil == C1 && currentCoil == 0) ){
      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      currentCoil = C2;
    }
    else if( 1==curveDirection && currentDegrees > 90 && (currentCoil == C2 && currentCoil == 0) ){
      digitalWrite(C2, LOW);
      digitalWrite(C1, HIGH);
      currentCoil = C1;
    }
    else if( currentDegrees > 90 && currentDegrees < 90){
      digitalWrite(C1, LOW );
      digitalWrite(C2, LOW );
      currentCoil = 0;
    }
  }
  else{
    if( (timeNew - timeOld) > period ){
     
      if(C2 == currentCoil || 0 == currentCoil){
        //go to position 1
       digitalWrite(LED_BUILTIN, HIGH);
  
        digitalWrite(C2, LOW);
        digitalWrite(C1, HIGH);
        currentCoil = C1;
        timeOld = timeNew;
      }  
      else if(C1 == currentCoil ){
        //go to position 1
        digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW);
        digitalWrite(C2, HIGH);
        currentCoil = C2;
        timeOld = timeNew; 
      }  
      
    }
    if( timeNew > timeToChangePeriod ){
      if( period > MIN_PERIOD ){
        //period = period * 0.99;
      }
      timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
    }  
  }

}
