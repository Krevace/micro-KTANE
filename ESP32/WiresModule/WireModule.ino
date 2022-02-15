char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int strikes = 0;
bool defused = false;
long currentMillis;
long previousRedLedMillis = 0;
char wires[6];
int wireAmount;
int correctPosition; 

//ESP32 PINS
int wirePin[6] = {15, 4, 5, 18, 19, 21}; 
int greenLedPin = 14; 
int redLedPin = 12;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 6; i++) {
    pinMode(wirePin[i], INPUT_PULLUP);
  }
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW); 
  digitalWrite(redLedPin, LOW); 

  //BOMB SETUP
  Serial.println();
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
  wireAmount = random(3, 7); 
  for (int i = 0; i < wireAmount; i++) {
    int randWire;
    int randColor;
    do {
      randWire = random(0, 6);
    } while (wires[randWire]);
    randColor = random(0, 5);
    if (randColor == 0) wires[randWire] = 'R';
    else if (randColor == 1) wires[randWire] = 'Y';
    else if (randColor == 2) wires[randWire] = 'B';
    else if (randColor == 3) wires[randWire] = 'b';
    else if (randColor == 4) wires[randWire] = 'W';
  }
  Serial.print("  Wires: ");
  for (int i = 0; i < 6; i++) {
    if (!wires[i]) Serial.print("-");
    else Serial.print(wires[i]);
  }
  
  //FIND SOLUTION
  Serial.println();
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  if (wireAmount == 3) {
    if (noWires('R')) wirePosition(2);  
    else if (lastWireIs('W')) lastWirePosition(0);
    else if (moreThanOne('b')) lastWirePosition('b');
    else lastWirePosition(0);
  } else if (wireAmount == 4) {
    if (moreThanOne('R') && serialOdd) lastWirePosition('R');
    else if (lastWireIs('Y') && noWires('R')) wirePosition(1);
    else if (exactlyOne('b')) wirePosition(1);
    else if (moreThanOne('Y')) lastWirePosition(0);
    else wirePosition(2);
  } else if (wireAmount == 5) {
    if (lastWireIs('B') && serialOdd) wirePosition(4);
    else if (exactlyOne('R') && moreThanOne('Y')) wirePosition(1);
    else if (noWires('B')) wirePosition(2);
    else wirePosition(1);
  } else if (wireAmount == 6) {
    if (noWires('Y') && serialOdd) wirePosition(3);
    else if (exactlyOne('Y') && moreThanOne('W')) wirePosition(4);
    else if (noWires('R')) lastWirePosition(0);
    else wirePosition(4);
  }
  Serial.print("    Cut: ");
  for (int i = 0; i < 6; i++) {
    if (i == correctPosition) Serial.print("X");
    else Serial.print("-");
  }

  //STRIKES
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("Strikes: ");
}

void loop() {
  if (strikes < 3) {
    if (defused) digitalWrite(greenLedPin, HIGH);
    currentMillis = millis();
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    for (int i = 0; i < 6; i++) {
      if (digitalRead(wirePin[i]) && wires[i]) {
        if (i == correctPosition) defused = true;
          else {
            wires[i] = 0x00;
            strikes++; 
            Serial.print("X ");
            digitalWrite(redLedPin, HIGH); 
            previousRedLedMillis = currentMillis; 
          }
        }
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
  }
  delay(20);
}

void wirePosition(int x) {
  int u = 0;
  for (int i = 0; i < 6; i++) {
    if (wires[i]) u++; 
    if (u == x) {
      correctPosition = i;
      return;
    }
  }
}

bool noWires(char x) {
  for (int i = 0; i < 6; i++) {
    if (wires[i] == x) return false;
  }
  return true;
}

bool lastWireIs(char x) {
  for (int i = 5; i > -1; i--) {
    if (wires[i]) {
      if (wires[i] == x) return true; 
      return false;
    }
  }
  return false;
}

void lastWirePosition(char x) {
  for (int i = 5; i > -1; i--) {
    if (wires[i]) {
      if (wires[i] == x || !x) {
        correctPosition = i;
        return;
      }
    }
  }
}

bool moreThanOne(char x) {
  int u = 0;
  for (int i = 0; i < 6; i++) {
    if (wires[i] == x) u++;
    if (u > 1) return true; 
  }
  return false;
}

bool exactlyOne(char x) {
  int u = 0;
  for (int i = 0; i < 6; i++) {
    if (wires[i] == x) u++;
    if (u > 1) return false;
  }
  if (u == 0) return false;
  return true;
}
