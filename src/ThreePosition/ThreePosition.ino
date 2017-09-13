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

#define INITIAL_PERIOD 200
#define MIN_PERIOD 30
#define DEBUG_TIME 1000
#define DEBUG_TIME_QUIET 20000
#define CHANGE_PERIOD_TIME 1000
#define SENSOR_DRIVEN_LIMIT 100
#define SENSOR_TEST_NUMBER 2000 

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
int sensorPrevValue = 0;
int sensorPrevValue1 = 0;
int sensorPrevValue2 = 0;
int curveDirection = 0;
char buffer[50];
boolean coilDriven = false;

unsigned long timeToStartDebug = 0;
int logValues[100][4]; //0 sensorvalue, 1 currentcoil, 2 currentDegrees, 3 directions
boolean isDebug = false;
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


      digitalWrite(C1, HIGH);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      delay(2000);

      digitalWrite(C1, LOW);
      digitalWrite(C2, HIGH);
      digitalWrite(C3, LOW);
      delay(2000);
      
      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, HIGH);
      delay(2000);

      digitalWrite(C1, LOW);
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      

  Serial.println("Starting.");
}

// the loop function runs over and over again forever

void loop() {

    sensorPrevValue = sensorValue;
    sensorValue = analogRead(SENSOR_PORT);
  

     
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
    Serial.print("\tperiod");
    Serial.println(period);
    timeToStartDebug =  timeNew + DEBUG_TIME_QUIET;
  }  
  else if (timeNew >= timeToStartDebug && false == isDebug && false == coilDriven){ //  
    isDebug = true;
    if(false != coilDriven){
     Serial.print("logIdx");
     Serial.print("\ttimeNew");
     Serial.print("\tsensorValue");
     Serial.print("\tcurrentCoil");
     Serial.print("\tcurrentDegrees");
     Serial.print("\tcurveDirection");
     Serial.print("\tcoilDriven");       
     Serial.print("\tsensorMinValue");       
     Serial.print("\tsensorMaxValue");       
     Serial.print("\n");
    }

  }
  // end debug logic 

  // calculate sensorMax and sensorMin 
  if( abs(sensorValue - sensorPrevValue) < 20 ){ 
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
    else{ 
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
  
  currentDegrees = (((float)sensorValue - sensorMinValue) * 180/(sensorMaxValue - sensorMinValue));

  //calculate curve direction and keep prev sensor values
  if( abs(sensorValue - sensorPrevValue) < 20 ){   
    if ( sensorValue >= sensorPrevValue1 && sensorPrevValue1 >= sensorPrevValue2 )
          curveDirection = 1;
    else if( sensorValue <= sensorPrevValue1 && sensorPrevValue1 <= sensorPrevValue2  )
         curveDirection = -1;
    else curveDirection = 0;     
  
    sensorPrevValue2 = sensorPrevValue1;
    sensorPrevValue1 = sensorValue;
  }
  else curveDirection = 0;
  
  //calculate if the coild driven should start
  if( (sensorMaxValue - sensorMinValue) > SENSOR_DRIVEN_LIMIT && period < 30 ){
    if( coilDriven == false ){
      coilDriven = true;
      Serial.println("COIL DRIVEN");
    }
  }
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
    

  if( logIdx < 99  ) logIdx++; else logIdx=0;
  logTime[logIdx] = timeNew;
  logValues[logIdx][0] = sensorValue;
  logValues[logIdx][1] = currentCoil; 
  logValues[logIdx][2] = currentDegrees;  
  logValues[logIdx][3] = curveDirection;
  
  //print debug information
  if( true==isDebug ) { // && coilDriven == false
    /*
		Serial.print(logIdx);
		Serial.print("\t");
		Serial.print(timeNew);
		Serial.print("\t");
		Serial.print(sensorValue);
		Serial.print("\t");
		Serial.print(((currentCoil -9)*100)  );
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
                //sprintf(buffer, "%d\t%lu\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",logIdx,timeNew,sensorValue,((currentCoil -9)*100),currentDegrees,curveDirection,coilDriven,sensorMinValue,sensorMaxValue);
                sprintf(buffer, "%lu\t%d\t%d\n",timeNew,sensorValue,((currentCoil -9)*100));
                Serial.print(buffer);
  }    
  // end debug logic 
 
  //should I acelerate
  if( timeNew > timeToChangePeriod && period > MIN_PERIOD){
    period = period * 0.80;
    timeToChangePeriod =  timeNew + CHANGE_PERIOD_TIME;
    Serial.print("period:");
    Serial.print(period);
    Serial.print("\ttimeNew:");
    Serial.print(timeNew);
    Serial.print("\ttimeToChangePeriod:");
    Serial.println(timeToChangePeriod);
  }  
  
  
  if( coilDriven == true ){
    if(  1 == curveDirection  && currentDegrees > 0 && currentDegrees < 120 && currentCoil == C3){
      //go to 1
      digitalWrite(C2, LOW);
      digitalWrite(C3, LOW);
      digitalWrite(C1, HIGH);
      currentCoil = C1;
     
    }
    else if( ( (1==curveDirection && currentDegrees > 120) || (-1 == curveDirection && currentDegrees > 130)) && currentCoil == C1 ){
      //go to 2
      digitalWrite(C1, LOW);
      digitalWrite(C3, LOW);
      digitalWrite(C2, HIGH);
      currentCoil = C2;
    }
    else if( -1 == curveDirection && currentDegrees > 0 && currentDegrees < 130 && currentCoil == C2 ){
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
       //digitalWrite(LED_BUILTIN, HIGH);
  

        digitalWrite(C3, LOW);
        digitalWrite(C1, LOW);
        digitalWrite(C2, HIGH);
        
        currentCoil = C2;
      }  
      else if(C2 == currentCoil ){
        //go to position 3
        //digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C3, HIGH);
        currentCoil = C3; 
      }  
      else if(C3 == currentCoil ){
        //go to position 1
        
        //digitalWrite(LED_BUILTIN, LOW);

        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
  
        digitalWrite(C1, HIGH);
        currentCoil = C1;  
      }
      timeOld = timeNew;
    }
  }
}

