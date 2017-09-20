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

#define INITIAL_PERIOD 100
#define MIN_PERIOD 15
#define DEBUG_TIME 1000
#define DEBUG_TIME_QUIET 20000
#define CHANGE_PERIOD_TIME 3000
#define SENSOR_DRIVEN_LIMIT 50
#define SENSOR_TEST_NUMBER 500 
#define LOG_SIZE 100

unsigned long timeOld = millis();
unsigned long timeNew = millis();
unsigned long timeLastChangeCoil = 0L;
unsigned long logTime[LOG_SIZE];

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
int sensorPrevValue = 0;
int sensorLogValue1 = 0;
int sensorLogValue2 = 0;
int curveDirection = 0;
boolean coilDriven = false;

unsigned long timeToStartDebug = 0;
int logValues[100][4]; //0 sensorvalue, 1 currentcoil, 2 currentDegrees, 3 directions
boolean isDebug = false;
int  logIdx = 0;
int l = 0; //index used to print log
int logCount = 0; //number of logs printed
unsigned long timeToChangePeriod = millis() + CHANGE_PERIOD_TIME;

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
  Serial.println("Setup 200");

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
  sensorPrevValue = analogRead(SENSOR_PORT);
  coilDriven = false;



  //setup time variables
  timeNew = millis();
  timeOld = timeNew;

  //debug initialize
  timeToStartDebug = timeNew;
  isDebug = true;

  timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;

  Serial.println("C1");
  digitalWrite(C1, HIGH);
  digitalWrite(C2, LOW);
  digitalWrite(C3, LOW);
  delay(2000);

  Serial.println("C2");
  digitalWrite(C1, LOW);
  digitalWrite(C2, HIGH);
  digitalWrite(C3, LOW);
  delay(2000);

  Serial.println("C3");      
  digitalWrite(C1, LOW);
  digitalWrite(C2, LOW);
  digitalWrite(C3, HIGH);
  delay(2000);


  Serial.println("Starting.");
}

// the loop function runs over and over again forever

void loop() {

  sensorPrevValue = sensorValue;
  sensorValue = analogRead(SENSOR_PORT);
  timeOld = timeNew;
  timeNew = millis();
  if(   logTime[logIdx] < timeNew && abs(sensorPrevValue - sensorValue) < 10 ){    

    //calculate previous values
    switch (logIdx) {
    case 0:
      sensorLogValue1 = logValues[logIdx][0];
      sensorLogValue2 = logValues[LOG_SIZE-1][0];
      break;
    default:
      sensorLogValue1 = logValues[logIdx][0];
      sensorLogValue2 = logValues[logIdx-1][0];
      break;
    }    


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
      if( sensorValue < sensorMinValueNext){
        sensorMinValueNext = sensorValue;
      }   
    }

    //calculate current degrees
    currentDegrees = ((sensorValue - sensorMinValue) * 180) / (sensorMaxValue - sensorMinValue);

    //Calculate curve direction
    if( sensorValue > sensorLogValue1 && sensorLogValue1 > sensorLogValue2  )
      curveDirection = 1;
    else if ( sensorValue < sensorLogValue1 && sensorLogValue1 < sensorLogValue2 )
      curveDirection = -1;
    //else if there is no change keep the same curve as before     

    
  //calculate if the coild driven should start
     if(  coilDriven == false && (sensorMaxValue - sensorMinValue) > SENSOR_DRIVEN_LIMIT && period <= MIN_PERIOD ){
       coilDriven = true;
       Serial.println("c");
     }
     
    /*
  else{
     if( true == coilDriven || period < 30){ // something when wrong
     digitalWrite(C1, LOW);
     digitalWrite(C2, LOW);
     digitalWrite(C3, LOW);
     Serial.println("***********************   Error while sensor driving elapsed");
     Serial.print("idx");
     Serial.print("\ttime");    
     Serial.print("\tsensorValue:");
     Serial.print("\tcurrentCoil");
     Serial.print("\tcurrentDegrees");
     Serial.println("\tcurveDirection");
     
     for(int i=logIdx+1; i<100; i++){
     Serial.print(i);
     Serial.print("\t");         
     Serial.print(logTime[i]);           
     Serial.print("\t");         
     Serial.print(logValues[i][0]);
     Serial.print("\t");
     Serial.print(((logValues[i][1] -9)*100));
     Serial.print("\t");         
     Serial.print(logValues[i][2]);
     Serial.print("\t");         
     Serial.print(logValues[i][3]);
     Serial.print("\t");           
     Serial.print(sensorMinValue);
     Serial.print("\t");
     Serial.print(sensorMaxValue);
     Serial.print("\n");         
     }
     for(int i=0; i<logIdx+1; i++){
     Serial.print(i);
     Serial.print("\t");         
     Serial.print(logTime[i]);           
     Serial.print("\t");         
     Serial.print(logValues[i][0]);
     Serial.print("\t");
     Serial.print(((logValues[i][1] -9)*100));
     Serial.print("\t");         
     Serial.print(logValues[i][2]);
     Serial.print("\t");         
     Serial.print(logValues[i][3]);  
     Serial.print("\t");           
     Serial.print(sensorMinValue);
     Serial.print("\t");
     Serial.print(sensorMaxValue);
     Serial.print("\n");          
     }
     delay(5000);
     period = INITIAL_PERIOD;
     timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;  
     sensorMinValue = sensorValue;
     sensorMaxValue = sensorValue;      
     coilDriven = false;
     }
     
     
     }
     */

    if( coilDriven == true ){
        if(   C3 == currentCoil  && 1 == curveDirection  && currentDegrees > 15    ){
          //go to 3  
          digitalWrite(LED_BUILTIN, LOW);  
          if( timeNew > timeToReportSpeed ){
            int currentRPM = (1000 / (timeNew - timeLastTurn)) * 60;
            Serial.print("RPM:");
            Serial.println(currentRPM);
            timeToReportSpeed = timeNew + 10000;
          } 
          timeLastTurn = timeNew; 

          digitalWrite(C3, LOW);
          digitalWrite(C2, LOW);
          digitalWrite(C1, HIGH);

          currentCoil = C1;  
        }
        else if( C1 == currentCoil && 1 == curveDirection && currentDegrees > 105   ){
          //go to 2
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(C1, LOW);
          digitalWrite(C3, LOW);
          digitalWrite(C2, HIGH);
          currentCoil = C2;
        }
        else if(  C2 == currentCoil  && -1 == curveDirection && currentDegrees < 135 ){
          //go to 1
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(C2, LOW);
          digitalWrite(C1, LOW);
          digitalWrite(C3, HIGH);
          currentCoil = C3;
        }
    }
    else{
      if( (timeNew - timeLastChangeCoil) > period ){

        if(C1 == currentCoil ){
          //go to position 2
          //digitalWrite(LED_BUILTIN, HIGH);

          //Serial.println("C2");
          digitalWrite(C1, LOW);
          digitalWrite(C3, LOW);

          digitalWrite(C2, HIGH);

          currentCoil = C2;
        }  
        else if(C2 == currentCoil ){
          //go to position 3
          //digitalWrite(LED_BUILTIN, LOW);

          //Serial.println("c3");
          digitalWrite(C2, LOW);
          digitalWrite(C1, LOW);

          digitalWrite(C3, HIGH);

          currentCoil = C3; 
        }  
        else if(C3 == currentCoil ){
          //go to position 1

          //digitalWrite(LED_BUILTIN, LOW);
          //Serial.println("C1");

          digitalWrite(C3, LOW);
          digitalWrite(C2, LOW);

          digitalWrite(C1, HIGH);

          currentCoil = C1;  
        }
        timeLastChangeCoil = timeNew;
      }
    }

    //should I acelerate
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

    if( logIdx < LOG_SIZE-2  ) logIdx++; 
    else logIdx=0;
    logTime[logIdx] = timeNew;
    logValues[logIdx][0] = sensorValue;
    logValues[logIdx][1] = currentCoil; 
    logValues[logIdx][2] = currentDegrees;  
    logValues[logIdx][3] = curveDirection;

    // this section controls de debug  
    if( isDebug == true && ((timeNew - timeToStartDebug) > DEBUG_TIME  || logCount >= LOG_SIZE ) ){
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
      Serial.print("\tperiod");
      Serial.println(period);


      timeToStartDebug =  timeNew + DEBUG_TIME_QUIET;

    }  
    else if (timeNew >= timeToStartDebug && false == isDebug ){ // && false == coilDriven  
      isDebug = true;
      logCount=0;
      Serial.print("idx");
      Serial.print("\ttime");    
      Serial.print("\tsensorValue:");
      Serial.print("\tcurrentCoil");
      Serial.print("\tcurrentDegrees");
      Serial.print("\tcurveDirection");
      Serial.print("\tMin");
      Serial.print("\tAverage");
      Serial.println("\tMax");

    }
    // end debug logic 

    //print debug information

      if( true==isDebug  ) { // && coilDriven == false //|| sensorValue < 200
      /*
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
       case C2:
       Serial.print("200");
       break;
       case C3:
       Serial.print("300");
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
       Serial.print("\t");
       Serial.print(sensorMinValue);
       Serial.print("\t");
       Serial.print(sensorMaxValue);
       Serial.print("\n");
       */


      if( logIdx < (LOG_SIZE-2)){ 
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
      case C2:
        Serial.print("200");
        break;
      case C3:
        Serial.print("300");
        break;
      }
      Serial.print("\t");
      Serial.print(logValues[l][2]);
      Serial.print("\t");         
      Serial.print(logValues[l][3]);
      Serial.print("\t");
      Serial.print(sensorMinValue);
      Serial.print("\t");
      Serial.print(sensorAverageValue);
      Serial.print("\t");
      Serial.print(sensorMaxValue);
      Serial.print("\t");
      Serial.print(coilDriven);
      Serial.print("\n");

      logCount++;
    }  
    // end debug logic 
  } 

}


