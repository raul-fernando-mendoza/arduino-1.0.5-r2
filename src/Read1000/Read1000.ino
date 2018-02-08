//#define LED_BUILTIN 13
#define SENSOR_PORT A0
#define SENSOR_PORT_1 A1
#define PERIOD 10000
#define MAX_READS 100

int sensorValueNew=0;
int sensorValueNew1=0;
int sensorValueOld=0;
int sensorValueOld1=0;

unsigned long timeNew = millis();
unsigned long times[MAX_READS] ;
int sensorValues[MAX_READS];
int sensorValues1[MAX_READS];

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(SENSOR_PORT,INPUT);
  pinMode(SENSOR_PORT_1,INPUT);
  
  Serial.begin(9600);
   // while the serial stream is not open, do nothing:
  while (!Serial) ;
  Serial.println("Setup");
}
void loop() 
{
  Serial.println("*******************Starting***************");
  
  sensorValueOld = analogRead(SENSOR_PORT); 
  sensorValueNew = sensorValueOld;
  sensorValueOld1 = analogRead(SENSOR_PORT_1); 
  sensorValueNew1 = sensorValueOld1;
  times[0] = millis();
/*
  while( abs(sensorValueNew - sensorValueOld) < 150 && abs(sensorValueNew1 - sensorValueOld1) < 150 ) {
  //  Serial.println(sensorValueNew);
    sensorValueOld =sensorValueNew;
    sensorValueNew = analogRead(SENSOR_PORT);
    sensorValueOld1 =sensorValueNew1;
    sensorValueNew1 = analogRead(SENSOR_PORT_1);
    /*
    Serial.print(sensorValueOld);
    Serial.print("\t");
    Serial.print(sensorValueNew);
    Serial.print("\t");
    Serial.print(sensorValueOld1);
    Serial.print("\t");
    Serial.println(sensorValueNew1);
    
  };    
  
    Serial.print(sensorValueOld);
    Serial.print("\t");
    Serial.println(sensorValueNew);
 */     
  for(int i=0;i<MAX_READS;i++){
    //delay(1);
    times[i] = millis();
    sensorValues[i] = analogRead(SENSOR_PORT); 
    sensorValues1[i] = analogRead(SENSOR_PORT_1);
  }   

  for(int i=0;i<MAX_READS;i++){
    Serial.print(times[i]);
    Serial.print("\t");
    Serial.print(sensorValues[i]);
    Serial.print("\t");
    Serial.println(sensorValues1[i]);
  }   
   delay(PERIOD);
}
