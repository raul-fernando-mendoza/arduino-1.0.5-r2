#define LED_BUILTIN 13
#define C1 7
#define C2 8
#define C3 9
#define C4 10
#define C5 11
#define C6 0
#define PERIOD 15

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
}
void loop() 
{
       { //position 1
        //Serial.println("1");
       //digitalWrite(LED_BUILTIN, HIGH);

        digitalWrite(C2, LOW);
        digitalWrite(C3, LOW);
        digitalWrite(C4, LOW);        
        digitalWrite(C1, HIGH);
        digitalWrite(C5, LOW);
        digitalWrite(C6, HIGH);
      }
    
      delay(PERIOD);
      
      {
        //Serial.println("2");
        //digitalWrite(LED_BUILTIN, LOW);
  
        digitalWrite(C1, LOW);
        digitalWrite(C3, LOW);        
        digitalWrite(C5, LOW);
        digitalWrite(C2, HIGH);
        digitalWrite(C4, HIGH);
        digitalWrite(C6, LOW);
      }
    
      delay(PERIOD);  
      
      {
        //Serial.println("3");
        
        //digitalWrite(LED_BUILTIN, LOW);

        digitalWrite(C1, LOW);
        digitalWrite(C2, LOW);
        digitalWrite(C6, LOW);
        digitalWrite(C3, HIGH);
        digitalWrite(C4, LOW);
        digitalWrite(C5, HIGH);
      }
      
      delay(PERIOD);
    
}
