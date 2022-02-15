#include <MD_MAX72xx.h>
#include <SPI.h>

char serialNumChars[34] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
String indicatorLabels[11] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
String portTypes[6] = {"DVI-D", "PS/2", "RJ-45", "Stereo RCA", "Parallel", "Serial"};

bool serialOdd = false;
int batteryAmount = 0;
bool indicatorCheck = false;
bool parallelCheck = false;
int mazes[9][11][11] = { {
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1},
                           {2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2},
                           {0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
                           {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                         },

                         {
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
                           {0, 0, 0, 1, 0, 0, 0, 1, 2, 0, 0},
                           {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0},
                           {0, 0, 2, 1, 0, 0, 0, 1, 0, 1, 0},
                           {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                         },
                         
                         {
                           {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                           {0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                           {1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0},
                           {0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 2},
                           {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                         },
                         
                         {
                           {2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
                           {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                         },
                         
                         {
                           {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
                           {0, 0, 0, 1, 0, 0, 0, 1, 2, 0, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0},
                         },
                         
                         {
                           {0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0},
                           {0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
                           {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1},
                           {0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0},
                           {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                           {0, 0, 0, 1, 2, 1, 0, 1, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                         },
                         
                         {
                           {0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                           {1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
                           {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
                         },
                         
                         {
                           {0, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                           {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 0, 2, 1, 0, 0, 0, 0, 0},
                           {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
                           {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
                           {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         },
                         
                         {
                           {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 1, 2, 0, 0, 1, 0, 1, 0},
                           {0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
                           {0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                           {0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0},
                           {2, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                           {0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1},
                           {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                         },
                       };
int mazeNum;
int playerPos[2]; 
int targetPos[2];
int trackPos[2][2];
bool pressed[4];
long currentMillis;
long previousRedLedMillis = 0;
int strikes = 0;
bool defused = false;
bool written = false;
long previousPlayerMillis = -500;
long previousTargetMillis = -500;
bool playerState = false;
bool targetState = false;

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define CLK_PIN   15  
#define DATA_PIN  4  
#define CS_PIN    5

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, 1); 
int buttonPins[4] = {33, 32, 18, 14};
int greenLedPin = 25;
int redLedPin = 27;

void setup() {
  //GENERAL SETUP
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); 
  }
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW); 
  digitalWrite(redLedPin, LOW);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/2);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear(); 

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
  mazeNum = random(0, 9); 
  do {
    for (int i = 0; i < 2; i++) {
      do {
        playerPos[i] = random(0, 11);
        targetPos[i] = random(0, 11);
      } while (playerPos[i] % 2 || targetPos[i] % 2);
    }
  } while (playerPos[0] == targetPos[0] && playerPos[1] == targetPos[1]);
  for (int i = 0; i < 11; i++) {
    for (int ii = 0; ii < 11; ii++) {
      if (mazes[mazeNum][i][ii] == 2) {
        if (written) {
          trackPos[1][0] = i;
          trackPos[1][1] = ii;
        } else {
          trackPos[0][0] = i;
          trackPos[0][1] = ii;
          written = true;
        }
      }
    }
  }
  mazes[mazeNum][playerPos[0]][playerPos[1]] = 3;
  mazes[mazeNum][targetPos[0]][targetPos[1]] = 4;
  
  //FIND SOLUTION
  Serial.println();
  Serial.println("--DEFUSAL SOLUTION--");
  Serial.print("   Maze: ");
  Serial.println(mazeNum+1);
  Serial.print(" Layout: ");
  for (int i = 0; i < 11; i++) {
    for (int ii = 0; ii < 11; ii++) {
      if (!(i % 2) && !(ii % 2)) {
        if (!mazes[mazeNum][i][ii]) Serial.print("O");
          else if (mazes[mazeNum][i][ii] == 2) Serial.print("X");
          else if (mazes[mazeNum][i][ii] == 3) Serial.print("P");
          else if (mazes[mazeNum][i][ii] == 4) Serial.print("T");
      } else if (i % 2 && !(ii % 2)) {
        if (mazes[mazeNum][i][ii]) Serial.print("-");
          else Serial.print(" ");
      } else if (!(i % 2) && ii % 2) {
        if (mazes[mazeNum][i][ii]) Serial.print("|");
          else Serial.print(" ");
      } else Serial.print(" ");
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("         ");
  }

  //STRIKES
  Serial.println();
  Serial.println("--STATUS--");
  Serial.print("Strikes: ");
}

void loop() {
  if (strikes < 3) {
    if (defused) digitalWrite(greenLedPin, HIGH);
    currentMillis = millis();
    if (currentMillis - previousRedLedMillis >= 500) {
      digitalWrite(redLedPin, LOW);
    }
    mx.setPoint(trackPos[0][0]/2, convertToDisplay(trackPos[0][1]/2), true);
    mx.setPoint(trackPos[1][0]/2, convertToDisplay(trackPos[1][1]/2), true);
    if (currentMillis - previousPlayerMillis >= 250) {
      if (!playerState) mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), true);
        else mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), false);
      playerState = !playerState;
      previousPlayerMillis = currentMillis;
    }
    if (currentMillis - previousTargetMillis >= 500) {
      if (!targetState) mx.setPoint(targetPos[0]/2, convertToDisplay(targetPos[1]/2), true);
        else mx.setPoint(targetPos[0]/2, convertToDisplay(targetPos[1]/2), false);
      targetState = !targetState; 
      previousTargetMillis = currentMillis;
    }
    if (!defused) {
      for (int i = 0; i < 4; i++) {
        if (!digitalRead(buttonPins[i])) pressed[i] = true;
        if (digitalRead(buttonPins[i]) && pressed[i]) {
          pressed[i] = false;
          if (i == 0 && playerPos[0] > 0) {
            if (mazes[mazeNum][playerPos[0]-1][playerPos[1]] == 1) {
              strike();
            } else {
              mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), false);
              playerPos[0] -= 2; 
            }
          } else if (i == 1 && playerPos[1] < 10) {
            if (mazes[mazeNum][playerPos[0]][playerPos[1]+1] == 1) {
              strike();
            } else {
              mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), false);
              playerPos[1] += 2; 
            }
          } else if (i == 2 && playerPos[0] < 10) {
            if (mazes[mazeNum][playerPos[0]+1][playerPos[1]] == 1) {
              strike();
            } else {
              mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), false);
              playerPos[0] += 2; 
            }
          } else if (i == 3 && playerPos[1] > 0) {
            if (mazes[mazeNum][playerPos[0]][playerPos[1]-1] == 1) {
              strike();
            } else {
              mx.setPoint(playerPos[0]/2, convertToDisplay(playerPos[1]/2), false);
              playerPos[1] -= 2; 
            }
          }
        }
      }
      if (playerPos[0] == targetPos[0] && playerPos[1] == targetPos[1]) defused = true;
    }
  } else {
    digitalWrite(redLedPin, HIGH);
    mx.clear();
  }
  delay(20);
}

void strike() {
  strikes++;
  Serial.print("X ");
  previousRedLedMillis = currentMillis; 
  digitalWrite(redLedPin, HIGH);
}

int convertToDisplay(int x) {
  if (x == 0) return 7;
    else if (x == 1) return 6;
    else if (x == 2) return 5;
    else if (x == 3) return 4;
    else if (x == 4) return 3;
    else if (x == 5) return 2;
  return 0;
}
