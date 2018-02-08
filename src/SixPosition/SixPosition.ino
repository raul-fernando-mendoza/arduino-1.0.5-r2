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
#define C1 7
#define C2 8
#define C3 9
#define C4 10
#define C5 11
#define C6 2

#define INITIAL_PERIOD 2000
#define MIN_PERIOD 14
#define DEBUG_TIME_QUIET 20000
#define CHANGE_PERIOD_TIME 1000
#define SENSOR_DRIVEN_LIMIT 500
#define SENSOR_TEST_NUMBER 1000
#define DEBUG_SIZE 100

unsigned long timeOld = millis();
unsigned long timeNew = millis();
unsigned long logTime[DEBUG_SIZE];
int debugIdx = 0;
int debugIdxToPrint = 0;

int period = INITIAL_PERIOD;
int currentPosition = 3;
int sensorValue = 0;
int sensorMinValue = 0;
int sensorMaxValue = 0;
int sensorMinValueNext = 0;
int sensorMaxValueNext = 0;
int sensorAverageValue = 0;
int sensorIdx = 0;

int currentDegrees = 0;
int sensorPrevValue = 0;
int sensorPrevValue1 = 0;
int sensorPrevValue2 = 0;
int curveDirection = 0;
boolean coilDriven = false;

unsigned long debugStartTime = 0;
int logValues[100][4]; //0 sensorvalue, 1 currentPosition, 2 currentDegrees, 3 directions
boolean debugActivated = false;
int  logIdx = 0;
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
  pinMode(C4, OUTPUT);
  pinMode(C5, OUTPUT);
  pinMode(C6, OUTPUT);


  digitalWrite(C1, LOW);
  digitalWrite(C2, LOW);
  digitalWrite(C3, LOW);  
  digitalWrite(C4, LOW);
  digitalWrite(C5, LOW);
  digitalWrite(C6, LOW);  

  pinMode(SENSOR_PORT,INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
 // while (!Serial) ;
  Serial.println("Setup");

  //show the user that we are inititing
  for( int i=0; i<5;i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);  
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  for( int i=1; i<=7; i++){
        Serial.print("test position:");
        Serial.println(i);
        if( i==1 ) digitalWrite(C1, HIGH); else digitalWrite(C1,LOW);
        if( i==2 ) digitalWrite(C2, HIGH); else digitalWrite(C2,LOW);
        if( i==3 ) digitalWrite(C3, HIGH); else digitalWrite(C3,LOW);
        if( i==4 ) digitalWrite(C4, HIGH); else digitalWrite(C4,LOW);
        if( i==5 ) digitalWrite(C5, HIGH); else digitalWrite(C5,LOW);
        if( i==6 ) digitalWrite(C6, HIGH); else digitalWrite(C6,LOW);
        delay(INITIAL_PERIOD);
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
  debugActivated = false;
  debugStartTime = timeNew + DEBUG_TIME_QUIET;

  timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;

  //delay(10000);
  Serial.print("MIN_PERIOD:");
  Serial.print(MIN_PERIOD);

  Serial.println(" Starting 2 bridges.");
}

// the loop function runs over and over again forever

void loop() {

    sensorPrevValue = sensorValue;
    sensorValue = analogRead(SENSOR_PORT);
     
    timeNew = millis();
 

  // calculate sensorMax and sensorMin 
  if( abs(sensorValue - sensorPrevValue) < 20 && (timeNew - timeOld) > 5){
    if( sensorIdx >= SENSOR_TEST_NUMBER  ){
      sensorIdx = 0;
      sensorMinValue = sensorMinValueNext;
      sensorMaxValue = sensorMaxValueNext;
   //     Serial.print("MinValue:");
   //     Serial.print(sensorMinValue);
   //     Serial.print("\tMaxValue:");
   //     Serial.println(sensorMaxValue);      
      sensorMinValueNext = sensorValue;    
      sensorMaxValueNext = sensorValue;
    }
    else {   
      sensorIdx++;
      if( sensorValue > sensorMaxValueNext ){
         sensorMaxValueNext = sensorValue;
      }   
      if( sensorValue < sensorMinValueNext ){
         sensorMinValueNext = sensorValue;
      }   
    }
  }
  //calculate current degrees
  currentDegrees = ((double(sensorValue) - double(sensorMinValue)) * 180) / (sensorMaxValue - sensorMinValue);

  //calculate curve direction and keep prev sensor values
  if( abs(sensorValue - sensorPrevValue) < 20 && (timeNew - timeOld) > 5){   
    if ( sensorValue >= sensorPrevValue1 && sensorPrevValue1 >= sensorPrevValue2 )
          curveDirection = 1;
    else if( sensorValue <= sensorPrevValue1 && sensorPrevValue1 <= sensorPrevValue2  )
         curveDirection = -1;
  }
  else curveDirection = 0; //it is invalid

  sensorPrevValue2 = sensorPrevValue1;
  sensorPrevValue1 = sensorValue;

 
  //calculate if the coild driven should start
  if( (sensorMaxValue - sensorMinValue) > SENSOR_DRIVEN_LIMIT && period < 30 ){
    if( coilDriven == false ){
      coilDriven = true;
      Serial.println("COIL DRIVEN");
    }
  }
/*  else{
    if( true == coilDriven ){ // something when wrong
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      digitalWrite(C4, LOW);
      digitalWrite(C5, LOW);
      digitalWrite(C6, LOW);

      Serial.println("***********************   Error while sensor driving elapsed");
      Serial.print("idx");
      Serial.print("\ttime");    
      Serial.print("\tsensorValue:");
      Serial.print("\tcurrentPosition");
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

  // this section controls de debug  

  //first save the current values for debugging 
  if( logIdx + 1 < DEBUG_SIZE  ) logIdx++; else logIdx=0;
  logTime[logIdx] = timeNew;
  logValues[logIdx][0] = sensorValue;
  logValues[logIdx][1] = currentPosition; 
  logValues[logIdx][2] = currentDegrees;  
  logValues[logIdx][3] = curveDirection;
  

  if (timeNew >= debugStartTime && false == debugActivated && false == coilDriven ){ //&& false == coilDriven 
    debugActivated = true;
//    Serial.println("***************************************");
//    Serial.print("timeNew:");
//    Serial.println(timeNew);

    debugStartTime =  timeNew + DEBUG_TIME_QUIET;

//    Serial.print("debugStartTime:");
//    Serial.println(debugStartTime);

//    Serial.print("logIdx");
//    Serial.println(logIdx);

    debugIdx = 0; 
    if( logIdx + 1 < DEBUG_SIZE )
      debugIdxToPrint = logIdx + 1;
    else debugIdxToPrint = 0;

    Serial.print("debugIdxToPrint:\t");
    Serial.print(debugIdxToPrint);

    
    Serial.print("\tMinValue:\t");
    Serial.print(sensorMinValue);
    Serial.print("\tAverageValue:\t");
    Serial.print(sensorAverageValue);
    Serial.print("\tMaxValue:\t");
    Serial.print(sensorMaxValue);
    Serial.print("\tcoilDriven\t");
    Serial.print(coilDriven);
    Serial.print("\tperiod:\t");
    Serial.println(period);

     Serial.print("logIdx");
     Serial.print("\ttimeNew");
     Serial.print("\tsensorValue");
     Serial.print("\tcurrentPosition");
     Serial.print("\tcurrentDegrees");
     Serial.print("\tcurveDirection");
     Serial.print("\n");
  }
  else if( true==debugActivated){
    if(debugIdx < DEBUG_SIZE ){
      debugIdx++;
      //Serial.print("debugIdx:"); Serial.println(debugIdx);
      if( debugIdxToPrint + 1 < DEBUG_SIZE )
        debugIdxToPrint = logIdx + 1;
      else debugIdxToPrint = 0;
      //Serial.print("\tdebugIdxToPrint:"); Serial.println(debugIdxToPrint);
    }
    else{
      debugActivated = false;
    }
  }
  
  
  if( true==debugActivated) { //
    //calculate the index to print from the debugIndex

    Serial.print(debugIdxToPrint);
    Serial.print("\t");
    Serial.print(logTime[debugIdxToPrint]);
    Serial.print("\t");
    Serial.print(logValues[debugIdxToPrint][0]);
    Serial.print("\t");
    Serial.print(logValues[debugIdxToPrint][1]*100);
    Serial.print("\t");         
    Serial.print(logValues[debugIdxToPrint][2]);
    Serial.print("\t");         
    Serial.print(logValues[debugIdxToPrint][3]);
    Serial.print("\n");         

  }
  
  // end debug logic 
 
  //should I acelerate
  if( timeNew > timeToChangePeriod && period > MIN_PERIOD){
    if(period < 25)
      period = period - 1;
    else period = period * 0.90;
    timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;

    Serial.print("period:");
    Serial.print(period);
    Serial.print("\ttimeNew:");
    Serial.print(timeNew);
    Serial.print("\ttimeToChangePeriod:");
    Serial.println(timeToChangePeriod);
    
  }  
 
 
  if( coilDriven == true ){
    if(  1 == curveDirection && currentDegrees >= 0 && currentPosition == 3  ){
      //go to 1
        digitalWrite(LED_BUILTIN, HIGH);
      
        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
        digitalWrite(C4, LOW);        
        digitalWrite(C1, HIGH);
        digitalWrite(C5, LOW);
        digitalWrite(C6, HIGH);
        currentPosition = 1;
      timeOld = timeNew;
     
    }
    else if( 1==curveDirection && currentDegrees >= 165 && currentPosition == 1   ){
      //go to 2
        digitalWrite(LED_BUILTIN, LOW);
        
        digitalWrite(C1, LOW);
        digitalWrite(C3, LOW);        
        digitalWrite(C5, LOW);
        digitalWrite(C2, HIGH);
        digitalWrite(C4, HIGH);
        digitalWrite(C6, LOW);
        currentPosition = 2; 
      timeOld = timeNew;

    }
    else if( -1 == curveDirection && currentDegrees <= 138 && currentPosition == 2 ){
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
        digitalWrite(C6, LOW);
        digitalWrite(C3, HIGH);
        digitalWrite(C4, LOW);
        digitalWrite(C5, HIGH);
        currentPosition = 3;  
        
      timeOld = timeNew;

    }
  }
  else
    if( (timeNew - timeOld) > period ){

      if(3 == currentPosition ){
        //Serial.println("1");
       //digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
        digitalWrite(C4, LOW);        
        digitalWrite(C1, HIGH);
        digitalWrite(C5, LOW);
        digitalWrite(C6, HIGH);
        currentPosition = 1;
      }  
      else if(1 == currentPosition ){
        //Serial.println("2");
        //digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW);
        digitalWrite(C3, LOW);        
        digitalWrite(C5, LOW);
        digitalWrite(C2, HIGH);
        digitalWrite(C4, HIGH);
        digitalWrite(C6, LOW);
        currentPosition = 2; 
      }  
      else if(2 == currentPosition ){
        //Serial.println("3");
        
        //digitalWrite(LED_BUILTIN, LOW);

        digitalWrite(C1, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C6, LOW);
        digitalWrite(C3, HIGH);
        digitalWrite(C4, LOW);
        digitalWrite(C5, HIGH);
        currentPosition = 3;  
      }
      timeOld = timeNew;
    }
    if( (timeNew - timeOld) > 100 ){
        digitalWrite(C1, LOW);
        digitalWrite(C4, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C5, LOW);
        digitalWrite(C6, LOW);
        digitalWrite(C3, LOW);      
    }
  //
}
