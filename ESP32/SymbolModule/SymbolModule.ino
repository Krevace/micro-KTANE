char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int strikes = 0;
bool defused = false;
char symbolRows[6][7] = { {'A', 'B', 'C', 'D', 'E', 'F', 'G'},
                          {'H', 'A', 'G', 'I', 'J', 'F', 'K'},
                          {'L', 'M', 'I', 'N', 'O', 'C', 'J'},
                          {'P', 'Q', 'R', 'E', 'N', 'K', 'S'},
                          {'T', 'S', 'R', 'U', 'Q', 'V', 'W'},
                          {'P', 'H', 'X', 'Y', 'T', 'Z', '0'} };
int selectedSymbolRow; 
char selectedSymbols[4];
char sortedSymbols[4];
int rightInARow = 0;
long currentMillis;
long previousRedLedMillis = 0;
int pressed[4];

//ESP32 PINS
int symbolPins[4] = {14, 5, 13, 15};
int greenLedPins[4] = {27, 18, 12, 4};
int redLedPin = 19;
int greenLedPin = 21;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 4; i++) {
    pinMode(symbolPins[i], INPUT_PULLUP);
    pinMode(greenLedPins[i], OUTPUT);
    digitalWrite(greenLedPins[i], LOW); 
  }
  pinMode(redLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW); 
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW); 

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
  selectedSymbolRow = random(0, 6); 
  for (int i = 0; i < 4; i++) {
    char symbol;
    bool alreadyExists;
    do {
      alreadyExists = false;
      symbol = symbolRows[selectedSymbolRow][random(0, 7)];
      for (int i = 0; i < 4; i++) {
        if (selectedSymbols[i] == symbol) alreadyExists = true;
      }
    } while (alreadyExists);
    selectedSymbols[i] = symbol;
  }
  Serial.print("Symbols: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(selectedSymbols[i]);
  }
  Serial.println();
  
  //FIND SOLUTION
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  int u = 0;
  for (int i = 0; i < 7; i++) {
    for (int o = 0; o < 4; o++) {
      if (symbolRows[selectedSymbolRow][i] == selectedSymbols[o]) {
        sortedSymbols[u] = selectedSymbols[o];
        u++;
      }
    }
  }
  Serial.print(" Sorted: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(sortedSymbols[i]);
  }

  //STRIKES
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("Strikes: ");
}

void loop() {
  if (strikes < 3) {
    currentMillis = millis();
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    if (!defused) {
      for (int i = 0; i < 4; i++) {
        if (!digitalRead(symbolPins[i])) pressed[i] = true; 
        if (digitalRead(symbolPins[i]) && pressed[i]) {
          pressed[i] = false;
          bool equals = false;
          for (int o = rightInARow; o > -1; o--) {
            if (selectedSymbols[i] == sortedSymbols[o]) {
              if (o == rightInARow) rightInARow++;
              equals = true; 
              o = -1;
            }
          }
          if (equals) {
            digitalWrite(greenLedPins[i], HIGH);
            if (rightInARow == 4) defused = true;
          } else {
              strikes++; 
              Serial.print("X ");
              digitalWrite(redLedPin, HIGH); 
              previousRedLedMillis = currentMillis; 
            }
        }
      } 
    } else digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(greenLedPins[i], LOW);
    }
  }
  delay(20);
}
