#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MIN_PULSE_WIDTH       650
#define MAX_PULSE_WIDTH       2350
#define DEFAULT_PULSE_WIDTH   1500
#define FREQUENCY             50

const int OPEN_ALL = 100;
const int CLOSE_ALL = 99;

const int NUMBER_OF_TOOLS = 2;
const int NUMBER_OF_SECTIONS = 2;

//Set the throw of each gate separately, if needed
int toolPrefs[NUMBER_OF_TOOLS][6] = {
  /*pwm pin, button pin, led pin, open, close, section*/
  {5, 0, 13, 0, 180, 0},//Workbench
  {7, 1, 2, 0, 180, 1}//Bandsaw
};

const int TOOLPREF_PWM = 0;
const int TOOLPREF_BUTTON = 1;
const int TOOLPREF_LED = 2;
const int TOOLPREF_CLOSE = 3;
const int TOOLPREF_OPEN = 4;
const int TOOLPREF_SECTION = 5;

int sectionPrefs[NUMBER_OF_SECTIONS][3] = {
  {0, 0, 180},
  {1, 0, 180}
};

const int SECTIONPREF_PWM = 0;
const int SECTIONPREF_CLOSE = 1;
const int SECTIONPREF_OPEN = 2;

//button debouncing
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 500;   // the debounce time, increase if the output flickers

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);  // Default is 1000mS

  for(int i=0;i<NUMBER_OF_SECTIONS;i++){
    //close all
    pwm.setPWM(sectionPrefs[i][SECTIONPREF_PWM], 0, pulseWidth(sectionPrefs[i][SECTIONPREF_CLOSE]));
  }
  
  for(int i=0;i<NUMBER_OF_TOOLS;i++){
    Serial.print("Setup Tool: ");
    Serial.println(i);

    Serial.print("Button: ");
    Serial.println(toolPrefs[i][TOOLPREF_BUTTON]);

    Serial.print("LED: ");
    Serial.println(toolPrefs[i][TOOLPREF_LED]);

    Serial.print("Servo: ");
    Serial.println(toolPrefs[i][TOOLPREF_PWM]);
    
    pinMode(toolPrefs[i][TOOLPREF_BUTTON],INPUT); //button
    pinMode(toolPrefs[i][TOOLPREF_LED], OUTPUT); //led

    //close all
    pwm.setPWM(toolPrefs[i][TOOLPREF_PWM], 0, pulseWidth(toolPrefs[i][TOOLPREF_CLOSE]));

    Serial.println(" ");
  }

  for (int i = 0; i <= 10; i++) {
    for(int m=0;m<NUMBER_OF_TOOLS;m++){
      digitalWrite(toolPrefs[m][TOOLPREF_LED], HIGH);
      delay(100);
      digitalWrite(toolPrefs[m][TOOLPREF_LED], LOW);
    }
  }

  //open single gate
  openGate(0);
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
      Serial.print("Set tool: ");
      Serial.println(activeTool);
      //manage all gate positions
      for(int s=0;s<NUMBER_OF_TOOLS;s++){
        if(s == activeTool){
          openGate(s);    
        } else {
          closeGate(s);
        }
      }
    }
   

  
}

boolean checkForButtonPush(int tool){
  reading = digitalRead(toolPrefs[tool][TOOLPREF_BUTTON]);  
  if(reading == HIGH && millis() - time > debounce) {
    Serial.print("BUTTON PRESSED: ");
    Serial.println(tool);
    time = millis();
    previous == reading;
    return true;
  }

  return false;
}

void closeGate(uint8_t num){
  Serial.print("closeGate ");
  Serial.println(num);
  pwm.setPWM(toolPrefs[num][TOOLPREF_PWM], 0, pulseWidth(toolPrefs[num][TOOLPREF_CLOSE])); 
  digitalWrite(toolPrefs[num][TOOLPREF_LED], LOW);
}
void openGate(uint8_t num){
  Serial.print("openGate ");
  Serial.println(num);
  pwm.setPWM(toolPrefs[num][TOOLPREF_PWM], 0, pulseWidth(toolPrefs[num][TOOLPREF_OPEN])); 
  digitalWrite(toolPrefs[num][TOOLPREF_LED], HIGH);

  int section = toolPrefs[num][TOOLPREF_SECTION];
  
  Serial.print("openSection ");
  Serial.println(section);
  
  for(int s=0;s<NUMBER_OF_SECTIONS;s++){
      if(s == section){
        //open section
        pwm.setPWM(sectionPrefs[s][SECTIONPREF_PWM], 0, pulseWidth(sectionPrefs[s][SECTIONPREF_OPEN])); 
      } else {
        //close section
        pwm.setPWM(sectionPrefs[s][SECTIONPREF_PWM], 0, pulseWidth(sectionPrefs[s][SECTIONPREF_CLOSE])); 
      }
    }
}

int pulseWidth(int angle)
{
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  Serial.println(analog_value);
  return analog_value;
}
