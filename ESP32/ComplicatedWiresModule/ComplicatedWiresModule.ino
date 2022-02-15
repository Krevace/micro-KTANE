char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};
String wireColors[6] = {"WW", "RR", "WR", "BB", "WB", "RB"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int wireAmount; 
String wires[6];
bool leds[6]; 
bool stars[6];
bool cut[6]; 
int strikes = 0;
bool defused = false;
long currentMillis;
long previousRedLedMillis = 0;

//ESP32 Pins
int ledPin[6] = {32, 33, 25, 26, 27, 13};
int wirePin[6] = {15, 4, 5, 18, 19, 21}; 
int greenLedPin = 14; 
int redLedPin = 12;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200); 
  randomSeed(analogRead(0));
  for (int i = 0; i < 6; i++) {
    pinMode(wirePin[i], INPUT_PULLUP);
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], LOW);
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
  if (random(0, 2)) wireAmount = 6;
    else if (random(0, 2)) wireAmount = 5;
    else wireAmount = 4; 
  for (int i = 0; i < wireAmount; i++) {
    int randWire;
    do {
      randWire = random(0, 6);
    } while (!wires[randWire].isEmpty());
    if (i == 0) {
      wires[randWire] = wireColors[random(0, 3)];
      leds[randWire] = 0;
      if (wires[randWire] == "WW") stars[randWire] = random(0, 2);
        else stars[randWire] = 1;
    } else {
      wires[randWire] = wireColors[random(0, 6)];
      leds[randWire] = random(0, 2);
      stars[randWire] = random(0, 2); 
    }
  }
  Serial.print("   LEDs: ");
  for (int i = 0; i < 6; i++) {
    if (!leds[i]) Serial.print("--");
      else {
        Serial.print("OO");
        digitalWrite(ledPin[i], HIGH);
      }
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("  Wires: ");
  for (int i = 0; i < 6; i++) {
    if (wires[i].isEmpty()) Serial.print("--");
      else Serial.print(wires[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("  Stars: ");
  for (int i = 0; i < 6; i++) {
    if (!stars[i]) Serial.print("--");
      else Serial.print("**");
    Serial.print(" ");
  }

  //FIND SOLUTION
  Serial.println();
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("    Cut: ");
  for (int i = 0; i < 6; i++) {
    if (!wires[i].isEmpty()) {
      int cutCondition; 
      if (wires[i].indexOf('R') > -1 && wires[i].indexOf('B') > -1) {
        if (leds[i]) {
          if (stars[i]) cutCondition = 1; 
            else cutCondition = 4; 
        } else {
          if (stars[i]) cutCondition = 3; 
            else cutCondition = 4; 
        }
      } else if (wires[i].indexOf('R') > -1) {
        if (leds[i]) {
          cutCondition = 2; 
        } else {
          if (stars[i]) cutCondition = 0; 
            else cutCondition = 4; 
        }
      } else if (wires[i].indexOf('B') > -1) {
        if (leds[i]) {
          cutCondition = 3; 
        } else {
          if (stars[i]) cutCondition = 1; 
            else cutCondition = 4; 
        }
      } else {
        if (leds[i]) {
          if (stars[i]) cutCondition = 2; 
            else cutCondition = 1; 
        } else { 
          cutCondition = 0; 
        }
      }
      if ((cutCondition == 0) || (cutCondition == 2 && batteryAmount >= 2) || (cutCondition == 3 && parallelCheck) || (cutCondition == 4 && !serialOdd)) {
        cut[i] = true; 
        Serial.print("XX");
      } else Serial.print ("--");
    } else Serial.print("--");
    Serial.print(" ");
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
    defused = true;
    currentMillis = millis(); 
    if (currentMillis - previousRedLedMillis >= 500) {
      digitalWrite(redLedPin, LOW);
    }
    for (int i = 0; i < 6; i++) {
      if (!wires[i].isEmpty() && digitalRead(wirePin[i])) {
        if (cut[i]) cut[i] = false;
          else {
            strikes++;
            Serial.print("X ");
            previousRedLedMillis = currentMillis; 
            digitalWrite(redLedPin, HIGH);
          } 
        wires[i] = ""; 
      }
      if (cut[i]) defused = false;
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
  }
  delay(20);
}
