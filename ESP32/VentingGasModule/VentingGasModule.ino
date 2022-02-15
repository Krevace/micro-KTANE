#include <LiquidCrystal_I2C.h>

char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int strikes = 0;
bool pressed[2];
long currentMillis;
long previousRedLedMillis = 0;
long previousMillis = 0;
long previousPrintMillis = 0;
long startOffset;
int timerNum = 40;
int displayQuestion; 
int displayState;
int cursorPos = 8; 
bool needToDisplay = true;
bool preventionState = true;
int preventionFlashAmount = 0;
bool disarmed = false; 
int disarmTime; 

//ESP32 Pins
LiquidCrystal_I2C lcd(0x27, 20, 4);  
int buttonPins[2] = {12, 14}; 
int redLedPin = 27;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 2; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(redLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);
  lcd.init();                 
  lcd.backlight();  

  //BOMB SETUP
  Serial.println("--BOMB CONFIGURATION--");
  String serialNum; 
  for (int i = 0; i < 5; i++) {
    serialNum += serialNumChars[random(0, 34)];
  }
  serialNum += serialNumChars[random(24, 34)];
  Serial.println(" Serial: " + serialNum);
  int x = serialNum.charAt(serialNum.length()-1);
  if (x % 2) serialOdd = true; 

  int batteryWidgetAmount = random(0, 6); 
  int indicatorWidgetAmount = random(0, 5-batteryWidgetAmount+1);
  int portWidgetAmount = 5-batteryWidgetAmount-indicatorWidgetAmount; 
  Serial.print("Widgets: "); 

  for (int i = 0; i < batteryWidgetAmount; i++) { 
    if (random(0, 2)) {
      Serial.print("|2 AA|");
      batteryAmount += 2; 
    } else {
      Serial.print("|1 D|");
      batteryAmount++;
    }
    Serial.print(" "); 
  }

  String chosenLabels[5]; 
  for (int i = 0; i < indicatorWidgetAmount; i++) {
    String indicator;
    bool alreadyExists;
    do {
      alreadyExists = false;
      indicator = indicatorLabels[random(0,11)];
      for (int i = 0; i < 5; i++) {
        if (indicator == chosenLabels[i]) alreadyExists = true;
      }
    } while (alreadyExists);
    chosenLabels[i] = indicator; 
    Serial.print("|");
    Serial.print(indicator); 
    if (random(0,2)) {
      Serial.print("(LIT)");
      if (indicator == "FRK" || indicator == "CAR") indicatorCheck = true; 
    } else Serial.print("(OFF)");
    Serial.print("| ");
  }

  for (int i = 0; i < portWidgetAmount; i++) {
    int lowerBound = 4;
    int upperBound = 2;
    int upperBound2 = 6;
    if (random(0, 2)) {
      lowerBound = 0;
      upperBound = 4;
      upperBound2 = 4;
    }
    Serial.print("|");
    int portAmount = random(0, upperBound+1); 
    String chosenPorts[upperBound];
    String port; 
    bool alreadyExists; 
    for (int i = 0; i < portAmount; i++) {
      do {
        alreadyExists = false;
        port = portTypes[random(lowerBound, upperBound2)];
        for (int i = 0; i < upperBound; i++) {
          if (port == chosenPorts[i]) alreadyExists = true;
        }
      } while (alreadyExists); 
      chosenPorts[i] = port; 
      if (port == "Parallel") parallelCheck = true; 
      Serial.print(port); 
      if (portAmount > 1 && i < portAmount-1) Serial.print(",");
    }
    Serial.print("| ");
  }

  //MODULE SETUP
  Serial.println();
  Serial.println();
  Serial.println("--MODULE INFORMATION--");
  Serial.print("Buttons: Y N");
  generateQuestion();
  
  //FIND SOLUTION
  Serial.println();
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("RESPOND TO PROMPTS WITH YES OR NO");

  //STATUS
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.println("Strikes: ");
  Serial.print("   Time: "); 
  Serial.print(timerNum);
  Serial.print(" ");

  startOffset = millis();
}

void loop() {
  if (strikes < 3) {
    currentMillis = millis() - startOffset; 
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    if (currentMillis - previousMillis >= 1000 && !disarmed) {
      timerNum--;
      if (timerNum < 0) timerNum = 0;
      Serial.print(timerNum); 
      Serial.print(" ");
      previousMillis = currentMillis;
    } else if (currentMillis - previousMillis >= disarmTime*1000 && disarmed) {
      disarmed = false; 
      Serial.print(timerNum);
      Serial.print(" ");
      generateQuestion();
      needToDisplay = true;
      previousMillis = currentMillis; 
    }
    if (!timerNum) {
      lcd.clear();
      strikes++; 
      Serial.println();
      Serial.println();
      Serial.println("--STATUS--");
      Serial.print("Strikes: ");
      for (int i = 0; i < strikes; i++) {
        Serial.print("X ");
      }
      Serial.println();
      Serial.print("   Time: ");
      digitalWrite(redLedPin, HIGH); 
      previousRedLedMillis = currentMillis;
      disarm(); 
    }
    if (needToDisplay) {
      if (!displayState) {
        if (!displayQuestion) {
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("VENT GAS?");
          lcd.setCursor(8, 1);
          lcd.print("Y/N");
        } else {
          lcd.setCursor(5, 0);
          lcd.print("DETONATE?");
          lcd.setCursor(8, 1);
          lcd.print("Y/N");
        }
        needToDisplay = false;
      } else if (displayState == 1) {
        lcd.setCursor(cursorPos, 2); 
        if (currentMillis - previousPrintMillis >= 500) {
          if (cursorPos == 8) lcd.print('Y');
            else if (cursorPos == 9) lcd.print('E');
            else if (cursorPos == 10) lcd.print('S');
            else {
              cursorPos = 7; 
              if (!displayQuestion) displayState = 3;
                else strikes = 3;
            }
          cursorPos++; 
          previousPrintMillis = currentMillis; 
        }
      } else if (displayState == 2) {
        lcd.setCursor(cursorPos, 2); 
        if (currentMillis - previousPrintMillis >= 500) {
          if (cursorPos == 8) lcd.print('N');
            else if (cursorPos == 9) lcd.print('O');
            else {
              cursorPos = 7; 
              if (!displayQuestion) displayState = 4;
                else displayState = 5;
            }
          cursorPos++; 
          previousPrintMillis = currentMillis; 
          if (displayState == 4) previousPrintMillis -= 1000;
        }
      } else if (displayState == 3) {
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("VENTING");
        lcd.setCursor(6, 1); 
        lcd.print("COMPLETE");
        disarm(); 
      } else if (displayState == 4) {
        if (currentMillis - previousPrintMillis >= 400 && preventionState && preventionFlashAmount < 3) {
          lcd.clear();
          lcd.setCursor(6, 0);
          lcd.print("VENTING");
          lcd.setCursor(6, 1); 
          lcd.print("PREVENTS");
          lcd.setCursor(5, 2); 
          lcd.print("EXPLOSIONS");
          previousPrintMillis = currentMillis; 
          preventionFlashAmount++; 
          preventionState = false;
        } else if (currentMillis - previousPrintMillis >= 750 && !preventionState && preventionFlashAmount < 3) {
          lcd.clear(); 
          previousPrintMillis = currentMillis;
          preventionState = true;
        } else if (currentMillis - previousPrintMillis >= 1000) {
          preventionFlashAmount = 0;
          displayState = 0;
        }
      } else if (displayState == 5) {
        lcd.clear();
        disarm(); 
      }
    }
    for (int i = 0; i < 2; i++) {
      if (!digitalRead(buttonPins[i])) pressed[i] = true; 
      if (digitalRead(buttonPins[i]) && pressed[i]) {
        pressed[i] = false;
        if (!displayState) {
          if (!i) displayState = 1;
            else displayState = 2;
          needToDisplay = true;
          previousPrintMillis = millis() - startOffset;
        }
      }
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    lcd.clear(); 
  }
  delay(20); 
}

void generateQuestion() {
  if (!random(0, 10)) displayQuestion = 1;
    else displayQuestion = 0;
  displayState = 0;
}

void disarm() {
  disarmed = true; 
  disarmTime = random(10, 41); 
  timerNum = 40;
  needToDisplay = false;
}

//1 in 10 chance for the  question to be detonate
//2 seconds for yes to be submitted (.5s per key and .5s wait)
//1.5 seconds for no to be submitted (.5s per key, and .5s wait)
//3.3 seconds for "VENTING PREVENTS EXPLOSIONS" (.75s on, .4s off, flashes three times, 1s at end)
//40 seconds on the clock
//chooses between 10 and 40s for next activation
//activates 90s after start if no modules are solved or stikes given
//85% chance after first activation for when player changes bomb state
