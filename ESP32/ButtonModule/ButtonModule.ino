char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};
String buttonLabels[4] = {"ABORT", "DETONATE", "HOLD", "PRESS"};
char buttonColors[5] = {'b', 'R', 'W', 'Y', 'B'};
char releaseColor[4] = {'b', 'W', 'Y', 'R'};
int releaseNum[4] = {4, 1, 5, 1};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck[2];
bool parallelCheck = false;
int strikes = 0;
bool defused = false; 
String buttonLabel;
char buttonColor;
bool hold;
bool pressCheck;
int timerNum = 0;
long currentMillis;
long previousMillis = 0;
long previousRedLedMillis = 0;
long holdStart;
int chosenReleaseNum;

//ESP32 PINS
int buttonPin = 5; 
int ledPins[3] = {21, 22, 23};
int greenLedPin = 4; 
int redLedPin = 15;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
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
      if (indicator == "CAR") indicatorCheck[0] = true;
        else if (indicator == "FRK") indicatorCheck[1] = true; 
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
  buttonLabel = buttonLabels[random(0, 4)];
  buttonColor = buttonColors[random(0, 5)];
  Serial.print(" Button: ");
  Serial.print(buttonColor);
  Serial.print(" " + buttonLabel);
  Serial.println();
  
  //FIND SOLUTION
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  if (buttonColor == 'b' && buttonLabel == "ABORT") hold = true;
    else if (batteryAmount > 1 && buttonLabel == "DETONATE") hold = false;
    else if (buttonColor == 'W' && indicatorCheck[0]) hold = true;
    else if (batteryAmount > 2 && indicatorCheck[1]) hold = false;
    else if (buttonColor == 'Y') hold = true;
    else if (buttonColor == 'R' && buttonLabel == "HOLD") hold = false;
    else hold = true;
  if (hold) Serial.print("HOLD AND RELEASE(BLUE = 4, RED/WHITE = 1, YELLOW = 5)");
    else Serial.print("PRESS");

  //STRIKES
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.println("Strikes: ");
  Serial.print("   Time: ");
  Serial.print(timerNum);
  Serial.print(" ");
}

void loop() {
  if (strikes < 3) {
    if (defused) digitalWrite(greenLedPin, HIGH);
    currentMillis = millis();
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    if (currentMillis - previousMillis >= 1000) {
      timerNum = timerNum == 9 ? 0 : timerNum + 1;
      Serial.print(timerNum);
      Serial.print(" ");
      previousMillis = currentMillis;
    }
    if (!digitalRead(buttonPin) && !pressCheck) {
      pressCheck = true;
      int rand = random(0,4);
      char stripColor = releaseColor[rand];
      chosenReleaseNum = releaseNum[rand];
      if (stripColor == 'b') ledColor(0, 0, 255);
        else if (stripColor == 'W') ledColor(255, 255, 255);
        else if (stripColor == 'Y') ledColor(255, 255, 0);
        else if (stripColor == 'R') ledColor(255, 0, 0);
      holdStart = currentMillis; 
    } else if (digitalRead(buttonPin) && pressCheck) {
      if (!hold) {
        if (currentMillis - holdStart >= 500) {
          strike();
        } else defused = true;
      } else {
        if (currentMillis - holdStart >= 500 && timerNum == chosenReleaseNum) defused = true;
          else {
            strike();
          }
      }
      pressCheck = false;
      ledColor(0, 0, 0); 
    } else if (digitalRead(buttonPin)) pressCheck = false;
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW); 
    ledColor(0, 0, 0);
  }
  delay(20);
}

void ledColor(int r, int g, int b) {
  analogWrite(ledPins[0], r);
  analogWrite(ledPins[1], g);
  analogWrite(ledPins[2], b);
}

void strike() {
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
}
