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
#define C1 8
#define C2 9
#define C3 10
#define C4 11

#define INITIAL_PERIOD 3000
#define MIN_PERIOD 15
#define DEBUG_TIME 1000
#define DEBUG_TIME_QUIET 10000
#define CHANGE_PERIOD_TIME 1000
#define SENSOR_DRIVEN_LIMIT 60
#define SENSOR_TEST_NUMBER 1000

unsigned long timeOld = millis();
unsigned long timeNew = millis();


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
#define LOG_SIZE 100
int logValues[LOG_SIZE][5]; //0 sensorvalue, 1 currentcoil, 2 currentDegrees, 3 directions, 4 coildriven
unsigned long logTime[LOG_SIZE];
int  logIdx = 0;
int l=0;
boolean isDebug = false;
int debugCount = 0;
unsigned long timeToChangePeriod = millis() + CHANGE_PERIOD_TIME;
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

  
        Serial.println("Current coild 1");
        digitalWrite(C1, HIGH); 
        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
        digitalWrite(C4, LOW);
        delay(3000);  

        Serial.println("Current coild 2");
        digitalWrite(C1, LOW); 
        digitalWrite(C2, HIGH);
        digitalWrite(C3, LOW);
        digitalWrite(C4, LOW);
        delay(3000);  

        Serial.println("Current coild 3");
        digitalWrite(C1, LOW); 
        digitalWrite(C2, LOW);
        digitalWrite(C3, HIGH);
        digitalWrite(C4, LOW);
        delay(3000);  

        Serial.println("Current coild 4");
        digitalWrite(C1, LOW); 
        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
        digitalWrite(C4, HIGH);
        delay(3000);  


  //find the normal value for the sensor
  long sum =0 ;
  for( int i=0; i<100;i++){
    sum += analogRead(SENSOR_PORT);
    delay(10);
  }
  sensorAverageValue = sum/10;
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
/*if( true!=isDebug) {
  Serial.print(sensorValue);
  Serial.print("\t");
  Serial.print(currentCoil);
  Serial.print("\t");
  Serial.println(coilDriven);
}*/
  //calculate curve direction and keep prev sensor values
  if (logIdx >= 0)
    sensorPrevValue1 = logValues[logIdx][0];
  else sensorPrevValue1 = logValues[LOG_SIZE-1][0];
  if (logIdx > 1 )
     sensorPrevValue2 = logValues[logIdx-1][0];
  else sensorPrevValue2 = logValues[LOG_SIZE-1][0]; 

//Calculate curve direction
    if( sensorValue >= sensorPrevValue1 && sensorPrevValue1 >= sensorPrevValue2  )
         curveDirection = 1;
    else if ( sensorValue <= sensorPrevValue1 && sensorPrevValue1 <= sensorPrevValue2 )
         curveDirection = -1;
    //else if there is no change keep the same curve as before     

  //calculate max and min sensor value
  if(sensorIdx > SENSOR_TEST_NUMBER ){  
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
  /*
  //something when wrong restart the period and stop the coil driven
  if( (period  <= MIN_PERIOD || true == coilDriven) && (sensorMaxValue - sensorMinValue) < SENSOR_DRIVEN_LIMIT ){
      Serial.print("ERROR");
      Serial.print(" sensorMaxValue:");
      Serial.print(sensorMaxValue);
      Serial.print(" sensorMinValue:");
      Serial.print(sensorMinValue);
      Serial.print(" period:");
      Serial.print(period);
      Serial.print(" coilDriven:");
      Serial.println(coilDriven);
       delay(5000);
       period = INITIAL_PERIOD;
       coilDriven = false;
  }
  */
    
  //calculate current degrees
  currentDegrees = ((sensorValue - sensorMinValue) * 180) / (sensorMaxValue - sensorMinValue);
  
  //calculate if the coild driven should start
  if( false == coilDriven && period  <= MIN_PERIOD && (sensorMaxValue - sensorMinValue) > SENSOR_DRIVEN_LIMIT){
    coilDriven = true;
   // Serial.println("c");
  }

  
  if( true == coilDriven  ){
      if(  C3 == currentCoil  && 1 == curveDirection && currentDegrees > 120 ){
        //go to position 1
        digitalWrite(LED_BUILTIN, HIGH);
  
        digitalWrite(C2, LOW); 
        digitalWrite(C3, LOW);
        

        //Serial.println("Current coild 1-4");
        digitalWrite(C4, HIGH);
        digitalWrite(C1, HIGH);

        currentCoil = C1;
        timeOld = timeNew;
      }  
      else if( C1 == currentCoil  && -1 == curveDirection &&  currentDegrees < 110 ){
        //go to position 4
       digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C3, LOW);
        digitalWrite(C1, LOW);
        
        //Serial.println("Current coild 4-2");
        digitalWrite(C2, HIGH);
        digitalWrite(C4, HIGH);
        currentCoil = C4;
        timeOld = timeNew;
      }  
      else if( C4 == currentCoil  && -1 == curveDirection &&  currentDegrees < 60 ){
        //go to position 2
        digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW); 
        digitalWrite(C4, LOW);

        //Serial.println("Current coild 2-3");
        digitalWrite(C3, HIGH);
        digitalWrite(C2, HIGH);
        
        currentCoil = C2;
        timeOld = timeNew;
      }  
      else if( C2 == currentCoil  && 1 == curveDirection &&  currentDegrees > 30 ){
        //go to position 3
        digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C4, LOW); 
        digitalWrite(C2, LOW);

        //Serial.println("Current coild 3-1");
        digitalWrite(C1, HIGH);
        digitalWrite(C3, HIGH);

        currentCoil = C3;
        timeOld = timeNew;
      }  
  }
  else{
    if( (timeNew - timeOld) > period ){
     
      if(C3 == currentCoil || 0 == currentCoil){
        //go to position 1
//       digitalWrite(LED_BUILTIN, HIGH);
  
        digitalWrite(C2, LOW); 
        digitalWrite(C3, LOW);
        

        //Serial.println("Current coild 1-4");
        digitalWrite(C4, HIGH);
        digitalWrite(C1, HIGH);

        currentCoil = C1;
        timeOld = timeNew;
      }  
      else if(C1 == currentCoil ){
        //go to position 4
  //     digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C3, LOW);
        digitalWrite(C1, LOW);
        
        //Serial.println("Current coild 4-2");
        digitalWrite(C2, HIGH);
        digitalWrite(C4, HIGH);
        currentCoil = C4;
        timeOld = timeNew;
      }  
      else if(C4 == currentCoil ){
        //go to position 2
   //    digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW); 
        digitalWrite(C4, LOW);

        //Serial.println("Current coild 2-3");
        digitalWrite(C3, HIGH);
        digitalWrite(C2, HIGH);
        
        currentCoil = C2;
        timeOld = timeNew;
      }  
      else if(C2 == currentCoil ){
        //go to position 3
   //    digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C4, LOW); 
        digitalWrite(C2, LOW);

        //Serial.println("Current coild 3-1");
        digitalWrite(C1, HIGH);
        digitalWrite(C3, HIGH);

        currentCoil = C3;
        timeOld = timeNew;
      }
    }
    if( false == coilDriven && period > MIN_PERIOD && timeNew > timeToChangePeriod ){
      if( period > 20 ){
        period = period * 0.9;
      }
      else period = period -1;
      Serial.print("p:");
      Serial.print(period);
      Serial.print("\n");
      timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
    }  
  }
  
//  if ( abs(sensorValue - logValues[logIdx][0]) > 0 ){
    if( logIdx < (LOG_SIZE-1) ) logIdx++; else logIdx=0;
    
    logTime[logIdx] = timeNew;
    logValues[logIdx][0] = sensorValue;
    logValues[logIdx][1] = currentCoil; 
    logValues[logIdx][2] = currentDegrees;  
    logValues[logIdx][3] = curveDirection;
    logValues[logIdx][4] = coilDriven;
    
  //}
//    Serial.print(timeNew);
//    Serial.print("\t");
//    Serial.print(sensorValue);
//    Serial.print("\n");

  // this section controls de debug  
  if(  isDebug == true && timeNew > (timeToStartDebug + DEBUG_TIME)){ //debugCount > LOG_SIZE
    //Serial.print("time since active");
    //Serial.println(timeNew - timeToStartDebug);
    isDebug = false;
    if( logIdx < (LOG_SIZE-1) ) l = logIdx + 1; else l = 0;
    timeToStartDebug =  timeNew + DEBUG_TIME_QUIET;
  }  
  else if (timeNew >= timeToStartDebug  && false == isDebug && false == coilDriven ){
         debugCount = 0;
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
  
  if( true==isDebug ) {
 //       debugCount++;
/*
         if( logIdx < (LOG_SIZE-1)){ 
           l = logIdx+1; 
         }
         else l=0;
    
         Serial.print(logIdx);
         Serial.print("\t");
         Serial.print(timeNew);
         Serial.print("\t");
         Serial.print(logValues[l][0]);
         Serial.print("\t");
          switch (logValues[l][1]) {
            case C1:
              Serial.print("100");
              break;
            case C3:
              Serial.print("300");
              break;
            case C2:
              Serial.print("200");
              break;
            case C4:
              Serial.print("400");
              break;
            default:
              Serial.print("-1");
          }
         Serial.print("\t");
         Serial.print(logValues[l][2]);
         Serial.print("\t");         
         Serial.print(logValues[l][3]);
         Serial.print("\t");
         Serial.print(logValues[l][4]);
         Serial.print("\n");
*/
         Serial.print(logIdx);
         Serial.print("\t");
         Serial.print(timeNew);
         Serial.print("\t");
         Serial.print(sensorValue);
         Serial.print("\t");
          switch (currentCoil) {
            case C1:
              Serial.print("100");
              break;
            case C3:
              Serial.print("300");
              break;
            case C2:
              Serial.print("200");
              break;
            case C4:
              Serial.print("400");
              break;
            default:
              Serial.print("-1");
          }
         Serial.print("\t");
         Serial.print(currentDegrees);
         Serial.print("\t");         
         Serial.print(curveDirection);
         Serial.print("\t");
         Serial.print(coilDriven);
         Serial.print("\n");
  }

}
