char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int strikes = 0;
long currentMillis;
long previousRedLedMillis = 0;
long previousMillis = 0;
long startOffset;
int timerNum = 45;
bool hold = false;

//ESP32 Pins
int buttonPin = 27;
int ledPins[9] = {13, 22, 21, 19, 18, 5, 4, 2, 15};
int redLedPin = 12;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < 9; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); 
  }
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
  
  //FIND SOLUTION
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("HOLD BEFORE TIMER RUNS OUT");

  //STATUS
  Serial.println();
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("   Time: "); 
  Serial.print(timerNum);
  Serial.print(" ");

  startOffset = millis();
}

void loop() {
  if (strikes < 3) {
    currentMillis = millis() - startOffset; 
    if (currentMillis - previousRedLedMillis >= 500) digitalWrite(redLedPin, LOW);
    if (strikes < 1) {
      if (currentMillis - previousMillis >= 1000) {
        if (hold) timerNum += 2;
          else timerNum--;
        if (timerNum > 45) timerNum = 45;
          else if (timerNum < 0) timerNum = 0;
        Serial.print(timerNum); 
        Serial.print(" ");
        previousMillis = currentMillis;
      }
      int numToOff = timerNum / 5;
      if (timerNum % 5) numToOff++;
      for (int i = 0; i < 9; i++) {
        if (i < numToOff) digitalWrite(ledPins[i], LOW); 
          else digitalWrite(ledPins[i], HIGH);
      }
      if (!digitalRead(buttonPin)) {
        hold = true;
      } else hold = false;
      if (!timerNum) {
        strikes++; 
        Serial.println();
        Serial.print("Strikes: X");
        digitalWrite(redLedPin, HIGH); 
        previousRedLedMillis = currentMillis;
      }
    }
  } 
  delay(20); 
}
