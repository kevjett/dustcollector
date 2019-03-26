#include <Servo.h>

// our servo # counter
uint8_t servoCount = 6;
uint8_t servonum = 0;

const int OPEN_ALL = 100;
const int CLOSE_ALL = 99;

boolean buttonTriggered = 0;

const int NUMBER_OF_TOOLS = 6;

//Set the throw of each gate separately, if needed
int toolPrefs[NUMBER_OF_TOOLS][5] = {
  /*pwm pin, button pin, led pin, open, close*/
  {0, 6, A0, 0,180},//Workbench
  {1, 7, A1, 0,180},//Bandsaw
  {2, 8, A2, 0,180},//Jointer
  {3, 9, A3, 0,180},//Planer
  {4, 10, A4, 0,180},//Table Saw
  {5, 11, A5, 0,180},//Miter Saw
  //{0, 0, 0, 0,180},//Drill Press
  //{0, 0, 0, 0,180},//Etc
};

//button debouncing
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 500;   // the debounce time, increase if the output flickers


int stateLED = LOW;
int stateButton;
int previous = LOW;

Servo myservos[NUMBER_OF_TOOLS];  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);
  
  for(int i=0;i<NUMBER_OF_TOOLS;i++){
    pinMode(toolPrefs[i][1],INPUT); //button
    pinMode(toolPrefs[i][2], OUTPUT); //led
    myservos[i].attach(toolPrefs[i][0]); //servo
  }
  
}

void loop() {
  int activeTool = 50;// a number that will never happen
  boolean changeTool = false;
   for(int i=0;i<NUMBER_OF_TOOLS;i++){
      if( checkForButtonPush(i)){
        activeTool = i;
        changeTool = true;
        exit;
      }
   }

    if (changeTool && activeTool != 50) {
      //manage all gate positions
      for(int s=0;s<NUMBER_OF_TOOLS;s++){
        if(s == activeTool){
          openGate(s);    
        } else {
          closeGate(s);
        }
      }
    }
   

  
 /* stateButton = digitalRead(pinButton);  
  if(stateButton == HIGH && previous == LOW && millis() - time > debounce) {
    if(stateLED == HIGH){
      stateLED = LOW; 
    } else {
       stateLED = HIGH; 
    }
    time = millis();
  previous == stateButton;
  }
  digitalWrite(LED, stateLED);

  if (stateLED == HIGH) {
    myservo.write(180); 
  } else {
    myservo.write(0); 
  }*/

  /*for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }*/
}

boolean checkForButtonPush(int tool){
  if (activeTool == tool) return false;
  
  stateButton = digitalRead(toolPrefs[tool][1]);  
  if(stateButton == HIGH && millis() - time > debounce) {
    time = millis();
    previous == stateButton;
    return true;
  }

  return false;
}

void closeGate(uint8_t num){
  Serial.print("closeGate ");
  Serial.println(num);
  myservos[num].write(toolPrefs[num][3]); 
  analogWrite(toolPrefs[num][1], HIGH);
}
void openGate(uint8_t num){
  Serial.print("openGate ");
  Serial.println(num);
  myservos[num].write(toolPrefs[num][4]); 
  analogWrite(toolPrefs[num][1], LOW);
}
