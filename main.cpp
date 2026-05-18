#include <Arduino.h>

const int numLights = 5;

const int buttonPins[numLights] = {25, 26, 27, 14, 12};
const int ledPins[numLights] = {17, 16, 4, 2, 15};

const int modeButtonPin = 33;

int currentMode = 1;
bool lastModeButtonState = HIGH;
int activeLight = -1;

int pressCount = 0;
bool lastButtonStates[numLights] = {HIGH, HIGH, HIGH, HIGH, HIGH};

unsigned long lastFlashTime = 0;
unsigned long lastModePressTime = 0;
int modePressCount = 0;

void showModeIndicator() {
  for (int i = 0; i < numLights; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  if (currentMode == 1) {
    for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], HIGH);
    delay(2000);
    for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], LOW);
  } 
  else if (currentMode == 2) {
    for (int b = 0; b < 2; b++) {
      for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], HIGH);
      delay(1000);
      for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], LOW);
      delay(1000);
    }
  } 
  else if (currentMode == 3) {
    for (int b = 0; b < 3; b++) {
      for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], HIGH);
      delay(1000);
      for (int i = 0; i < numLights; i++) digitalWrite(ledPins[i], LOW);
      delay(1000);
    }
  }

  if (currentMode == 1) {
    digitalWrite(ledPins[activeLight], HIGH);
  } else if (currentMode == 3) {
    lastFlashTime = millis() - 3000;
  }
}

void triggerReward() {
  unsigned long startMillis = millis();
  while (millis() - startMillis < 3000) {
    for (int i = 0; i < numLights; i++) {
      digitalWrite(ledPins[i], random(2));
    }
    delay(100);
  }

  for (int i = 0; i < numLights; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  pressCount = 0;

  if (currentMode == 1) {
    digitalWrite(ledPins[activeLight], HIGH);
  } else if (currentMode == 3) {
    lastFlashTime = millis() - 3000;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(modeButtonPin, INPUT_PULLUP);

  for (int i = 0; i < numLights; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  for (int j = 0; j < 1; j++) {
    for (int i = 0; i < numLights; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(100);
      digitalWrite(ledPins[i], LOW);
    }
  }
  delay(500);

  randomSeed(analogRead(0));

  activeLight = random(0, numLights);
  digitalWrite(ledPins[activeLight], HIGH);
}

void loop() {
  bool currentModeButtonState = digitalRead(modeButtonPin);
  
  if (currentModeButtonState == LOW && lastModeButtonState == HIGH) {
    delay(50);
    if (digitalRead(modeButtonPin) == LOW) {
      modePressCount++;
      lastModePressTime = millis();
    }
  }
  lastModeButtonState = currentModeButtonState;

  if (modePressCount > 0 && (millis() - lastModePressTime > 400)) {
    if (modePressCount == 1) {
      currentMode++;
      if (currentMode > 3) {
        currentMode = 1;
      }
      
      for (int i = 0; i < numLights; i++) {
        digitalWrite(ledPins[i], LOW);
        lastButtonStates[i] = digitalRead(buttonPins[i]);
      }

      if (currentMode == 1 || currentMode == 3) {
        activeLight = random(0, numLights);
        if (currentMode == 1) {
          digitalWrite(ledPins[activeLight], HIGH);
        } else if (currentMode == 3) {
          lastFlashTime = millis() - 3000;
        }
      }
    } else if (modePressCount >= 2) {
      showModeIndicator();
    }
    modePressCount = 0;
  }

  if (currentMode == 1) {
    if (digitalRead(buttonPins[activeLight]) == LOW) {
      digitalWrite(ledPins[activeLight], LOW);
      
      delay(50);
      while(digitalRead(buttonPins[activeLight]) == LOW) {
        delay(10);
      }
      delay(50);

      activeLight = random(0, numLights);
      digitalWrite(ledPins[activeLight], HIGH);

      pressCount++;
      if (pressCount >= 5) {
        triggerReward();
      }
    }
  } 
  else if (currentMode == 2) {
    for (int i = 0; i < numLights; i++) {
      bool currentState = digitalRead(buttonPins[i]);
      
      if (currentState != lastButtonStates[i]) {
        delay(50);
        currentState = digitalRead(buttonPins[i]);
        
        if (currentState == LOW) {
          pressCount++;
          if (pressCount >= 5) {
            triggerReward();
          }
        }
        lastButtonStates[i] = currentState;
      }

      if (currentState == LOW) {
        digitalWrite(ledPins[i], HIGH);
      } else {
        digitalWrite(ledPins[i], LOW);
      }
    }
  } 
  else if (currentMode == 3) {
    if (millis() - lastFlashTime >= 3000) {
      digitalWrite(ledPins[activeLight], HIGH);
      delay(300);
      digitalWrite(ledPins[activeLight], LOW);
      lastFlashTime = millis();
    }
    
    if (digitalRead(buttonPins[activeLight]) == LOW) {
      delay(50);
      while(digitalRead(buttonPins[activeLight]) == LOW) {
        delay(10);
      }
      delay(50);

      activeLight = random(0, numLights);
      lastFlashTime = millis() - 3000; 

      pressCount++;
      if (pressCount >= 5) {
        triggerReward();
      }
    }
  }
}