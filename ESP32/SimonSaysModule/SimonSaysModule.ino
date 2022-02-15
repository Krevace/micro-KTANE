char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
bool serialVowel = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int strikes = 0;
int sequenceLength;
int sequence[5];
int solutionSequence[5];
int vowel0[4] = {3, 2, 1, 0};
int vowel1[4] = {2, 3, 0, 1};
int vowel2[4] = {3, 0, 1, 2};
int noVowel0[4] = {1, 3, 2, 0};
int noVowel1[4] = {0, 2, 1, 3};
int noVowel2[4] = {2, 3, 0, 1};
long playerInputTime = -5000; 
long currentMillis; 
long previousMillis = -500;
int currentSequencePosition = 0; 
int playerSequencePosition = 0; 
int playerInputPosition = 0;
long previousRedLedMillis = 0;
int pressedLed = -1;
bool pressed[4];
int finalSequenceException = 0;

//ESP32 PINS
int ledPins[4] = {26, 18, 32, 25};
int buttonPins[4] = {34, 23, 22, 27};
int greenLedPin = 15;
int redLedPin = 2;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);

  //BOMB SETUP
  Serial.println("--BOMB CONFIGURATION--");
  String serialNum; 
  for (int i = 0; i < 5; i++) {
    serialNum += serialNumChars[random(0, 34)];
  }
  serialNum += serialNumChars[random(24, 34)];
  Serial.println("  Serial: " + serialNum);
  int x = serialNum.charAt(serialNum.length()-1);
  if (x % 2) serialOdd = true; 
  if (serialNum.indexOf('A') > -1 || serialNum.indexOf('E') > -1 || serialNum.indexOf('I') > -1 || serialNum.indexOf('O') > -1 || serialNum.indexOf('U') > -1) serialVowel = true;

  int batteryWidgetAmount = random(0, 6); 
  int indicatorWidgetAmount = random(0, 5-batteryWidgetAmount+1);
  int portWidgetAmount = 5-batteryWidgetAmount-indicatorWidgetAmount; 
  Serial.print(" Widgets: "); 

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
  sequenceLength = random(3, 6); 
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4); 
  }
  
  //FIND SOLUTION
  findSolutionSequence();
  printDefusalSolution();

  //STRIKES
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("Strikes: ");
}

void loop() {
  if (playerSequencePosition < sequenceLength && strikes < 3) {
    currentMillis = millis();
    if (currentMillis - playerInputTime >= 250) {
      if (finalSequenceException) finalSequenceException++;
      if (pressedLed != -1) digitalWrite(ledPins[pressedLed], LOW); 
      pressedLed = -1; 
    }
    if (currentMillis - previousRedLedMillis >= 500) {
      digitalWrite(redLedPin, LOW); 
    }
    if (currentMillis - previousMillis >= 250) {
      for (int i = 0; i < 4; i++) {
        if ((currentSequencePosition && sequence[currentSequencePosition-1] == i) || (currentSequencePosition-1 == -1 && sequence[playerSequencePosition] == i)) digitalWrite(ledPins[i], LOW);
      }
    }
    if ((currentMillis - playerInputTime >= 5000 && currentMillis - previousMillis >= 500) || finalSequenceException == 2) {
      if (finalSequenceException == 2) playerInputTime -= 4500; 
      finalSequenceException = 0;
      playerInputPosition = 0;
      for (int i = 0; i < 4; i++) {
        if (sequence[currentSequencePosition] == i) digitalWrite(ledPins[i], HIGH);
      }
      currentSequencePosition++; 
      if (currentSequencePosition > playerSequencePosition) {
        playerInputTime = currentMillis; 
        currentSequencePosition = 0; 
      }
      previousMillis = currentMillis;
    }
    for (int i = 0; i < 4; i++) {  
      if (!digitalRead(buttonPins[i])) pressed[i] = true; 
      if (digitalRead(buttonPins[i]) && pressed[i]) {
        pressed[i] = false;
        digitalWrite(ledPins[i], HIGH);
        pressedLed = i; 
        playerInputTime = millis();
        if (solutionSequence[playerInputPosition] == i) {
          playerInputPosition++;
          if (playerInputPosition > playerSequencePosition) {
            playerSequencePosition++; 
            finalSequenceException = 1;
          }
        } else {
          digitalWrite(redLedPin, HIGH);
          previousRedLedMillis = millis(); 
          strikes++;    
          findSolutionSequence();
          Serial.println();
          printDefusalSolution();
          playerInputTime -= 4500; 
          Serial.println();
          Serial.println();
          Serial.println("--STATUS--");
          Serial.print("Strikes: ");
          for (int i = 0; i < strikes; i++) {
            Serial.print("X ");
          }
        }
      }
    }
  } else if (strikes < 3) {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW); 
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW); 
    }
  }
  delay(20);
}

void findSolutionSequence() {
  if (serialVowel) {
    if (strikes == 0) convertEntry(vowel0);
      else if (strikes == 1) convertEntry(vowel1);
      else convertEntry(vowel2);
  } else {
    if (strikes == 0) convertEntry(noVowel0);
      else if (strikes == 1) convertEntry(noVowel1);
      else convertEntry(noVowel2);
  }
}

void convertEntry(int dictionary[]) {
  for (int i = 0; i < sequenceLength; i++) {
    solutionSequence[i] = dictionary[sequence[i]];
  }
}
void printDefusalSolution() {
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("Sequence: ");
  for (int i = 0; i < sequenceLength; i++) {
    if (!sequence[i]) Serial.print('B');
      else if (sequence[i] == 1) Serial.print('Y');
      else if (sequence[i] == 2) Serial.print('G');
      else if (sequence[i] == 3) Serial.print('R');
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Solution: ");
  for (int i = 0; i < sequenceLength; i++) {
    if (!solutionSequence[i]) Serial.print('B');
      else if (solutionSequence[i] == 1) Serial.print('Y');
      else if (solutionSequence[i] == 2) Serial.print('G');
      else if (solutionSequence[i] == 3) Serial.print('R');
    Serial.print(" ");
  }
}
