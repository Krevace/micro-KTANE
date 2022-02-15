//CANT TEST ON WOKWI, 7-SEGMENT DISPLAY IS BROKEN :/

char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};
byte table[10] {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int labelMemory[4];
int positionMemory[2]; 
int topLabel;
int buttonLabels[4]; 
int stage = 0;
int correctButton; 
int strikes = 0;
bool defused = false;
int pressed[4]; 
int count = 0;
long currentMillis;
long previousRedLedMillis = 0;

//ESP32 PINS
int stageLedPins[5] = {32, 33, 26, 21, 2};
int seg7DigitPins[5] = {25, 22, 15, 19, 4};
int latchPin = 5;
int clockPin = 18;
int dataPin = 23;
int buttonPins[4] = {13, 12, 14, 27};
int greenLedPin = 34;
int redLedPin = 35;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 5; i++) {
    pinMode(seg7DigitPins[i], OUTPUT);
    pinMode(stageLedPins[i], OUTPUT);
    digitalWrite(seg7DigitPins[i], HIGH);
    digitalWrite(stageLedPins[i], LOW); 
  }
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);

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
  generateSetup();
  
  //FIND SOLUTION
  findCorrectButton();
  printDefusalSolution();
}

void loop() {
  if (strikes < 3) {
    if (defused) digitalWrite(greenLedPin, HIGH);
    currentMillis = millis();
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    clearDisplay();
    digitalWrite(latchPin, LOW); 
    if (!count) shiftOut(dataPin, clockPin,LSBFIRST, table[topLabel]);
      else shiftOut(dataPin, clockPin,LSBFIRST, table[buttonLabels[count-1]]); 
    digitalWrite(seg7DigitPins[count], LOW); 
    digitalWrite(latchPin, HIGH); 
    count++; 
    if (count == 5) { 
      count = 0;
    }
    for (int i = 0; i < stage; i++) {
      digitalWrite(stageLedPins[i], HIGH);
    }
    for (int i = 0; i < 4; i++) {
      if (!digitalRead(buttonPins[i])) pressed[i] = true; 
      if (digitalRead(buttonPins[i]) && pressed[i]) {
        pressed[i] = false;
        if (i == correctButton) {
          stage++;
          if (stage == 5) defused = true; 
          generateSetup();
          findCorrectButton();
          Serial.println();
          printDefusalSolution();
        } else {
          strikes++;
          stage = 0;
          for (int i = 0; i < 5; i++) {
            digitalWrite(stageLedPins[i], LOW); 
          }
          generateSetup();
          findCorrectButton();
          Serial.println();
          printDefusalSolution();
          digitalWrite(redLedPin, HIGH); 
          previousRedLedMillis = currentMillis; 
        }
      }
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    clearDisplay();
  }
}

void generateSetup() {
  topLabel = random(1,5); 
  int label;
  bool alreadyExists;
  for (int i = 0; i < 4; i++) {
    buttonLabels[i] = 0x00;
  }
  for (int i = 0; i < 4; i++) {
    do {
      alreadyExists = false;
      label = random(1,5);
      for (int i = 0; i < 4; i++) {
        if (buttonLabels[i] == label) alreadyExists = true;
      }
    } while (alreadyExists);
    buttonLabels[i] = label; 
  }
}

void findCorrectButton() {
  if (stage == 0) {
    if (topLabel == 1) correctButton = 1; 
      else if (topLabel == 2) correctButton = 1;
      else if (topLabel == 3) correctButton = 2;
      else if (topLabel == 4) correctButton = 3;
  } else if (stage == 1) {
    if (topLabel == 1) buttonLabeledFour();
      else if (topLabel == 2) correctButton = positionMemory[0];
      else if (topLabel == 3) correctButton = 0;
      else if (topLabel == 4) correctButton = positionMemory[0];
  } else if (stage == 2) {
    if (topLabel == 1) correctButton = labelMemory[1];
      else if (topLabel == 2) correctButton = labelMemory[0];
      else if (topLabel == 3) correctButton = 2;
      else if (topLabel == 4) buttonLabeledFour();
  } else if (stage == 3) {
    if (topLabel == 1) correctButton = positionMemory[0];
      else if (topLabel == 2) correctButton = 0;
      else if (topLabel == 3) correctButton = positionMemory[1];
      else if (topLabel == 4) correctButton = positionMemory[1];
  } else if (stage == 4) {
    if (topLabel == 1) correctButton = labelMemory[0];
      else if (topLabel == 2) correctButton = labelMemory[1];
      else if (topLabel == 3) correctButton = labelMemory[3];
      else if (topLabel == 4) correctButton = labelMemory[2];
  }
  labelMemory[stage] = buttonLabels[correctButton]; 
  if (stage < 2) positionMemory[stage] = correctButton; 
}

void buttonLabeledFour() {
  for (int i = 0; i < 4; i++) {
    if (buttonLabels[i] == 4) correctButton = i; 
  }
}

void clearDisplay() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(seg7DigitPins[i], HIGH);
  }
}

void printDefusalSolution() {
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("Press: "); 
  Serial.print(buttonLabels[correctButton]);
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("Strikes: ");
  for (int i = 0; i < strikes; i++) {
    Serial.print("X ");
  }
}
