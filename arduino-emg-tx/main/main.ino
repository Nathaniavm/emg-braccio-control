#include <math.h>
#include <SoftwareSerial.h>

// RX dummy on A3, TX on D2
SoftwareSerial outSerial(A5, 2); // A5 er bare en dummy pin som ikke er i bruk, men må defineres

const int Button1 = A2;
const int Button2 = 3;
const int Button3 = 4;
const int Button4 = 5;
const int Button5 = 6;
const int Button6 = 7;

const int LED1 = 8;
const int LED2 = 9;
const int LED3 = 10;
const int LED4 = 11;
const int LED5 = 12;
const int LED6 = 13;

int ButtonState1 = 0;
int ButtonState2 = 0; 
int ButtonState3 = 0; 
int ButtonState4 = 0; 
int ButtonState5 = 0; 
int ButtonState6 = 0; 

int currentButton = 6;   


const int analogPin1 = A0;
const int analogPin2 = A1;


const int windowSize = 100;      // RMS-vindu
const float dcAlpha1 = 0.01;
const float dcAlpha2 = 0.01;     // DC-fjerner

const int printEvery = 40;       // 1000 Hz / 40 = 25 Hz printing

int acBuf1[windowSize];
int acBuf2[windowSize];

int idx = 0;
int idx2 = 0;
long sumSquares = 0;
long sumSquares2 = 0;
int filled1 = 0;
int filled2 = 0;

float dcEst1 = 0;
float dcEst2 = 0;

unsigned int sampleCounter = 0;

// -------- Baseline (NYTT) --------
float base1 = 0.0f, base2 = 0.0f;
bool baseInit = false;

// hvor raskt baseline får endre seg (lav = treg, stabil) 0.2
const float baseAlpha = 0.08f;

// oppdater baseline bare når signalet er "nær ro"
const float baseGate  = 4.0f;    // prøv 3–10 ved behov

static inline float clamp0(float x) { return (x < 0) ? 0 : x; }

void setup() {
  Serial.begin(9600);
  outSerial.begin(9600);

  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
  pinMode(Button4, INPUT);
  pinMode(Button5, INPUT);
  pinMode(Button6, INPUT);

  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  pinMode(LED4, OUTPUT); 
  pinMode(LED5, OUTPUT); 
  pinMode(LED6, OUTPUT); 

  // Init buffere (anbefalt)
  for (int i = 0; i < windowSize; i++) {
    acBuf1[i] = 0;
    acBuf2[i] = 0;
  }
  sumSquares = 0;
  sumSquares2 = 0;
}

unsigned long previousMicros = 0; 
unsigned long previousMillisJointChange = 0;

void loop() {
  const long intervalMainLoop = 1000; 
  unsigned long currentMicros = micros();
  if (currentMicros - previousMicros >= intervalMainLoop) {
    previousMicros = currentMicros;
  
    ButtonState1 = digitalRead(Button1);
    if (digitalRead(A2) == LOW) {currentButton = 1;}

    ButtonState2 = digitalRead(Button2);
    if (ButtonState2 == HIGH) {currentButton = 2;}

    ButtonState3 = digitalRead(Button3);
    if (ButtonState3 == HIGH) {currentButton = 3;}

    ButtonState4 = digitalRead(Button4);
    if (ButtonState4 == HIGH) {currentButton = 4;}

    ButtonState5 = digitalRead(Button5);
    if (ButtonState5 == HIGH) {currentButton = 5;}

    ButtonState6 = digitalRead(Button6);
    if (ButtonState6 == HIGH) {currentButton = 6;}

    // Control LEDs based on stored button
    digitalWrite(LED1, currentButton == 1);
    digitalWrite(LED2, currentButton == 2);
    digitalWrite(LED3, currentButton == 3);
    digitalWrite(LED4, currentButton == 4);
    digitalWrite(LED5, currentButton == 5);
    digitalWrite(LED6, currentButton == 6);

    
    int raw1 = analogRead(analogPin1);
    int raw2 = analogRead(analogPin2);

    // --- DC-fjerning ---
    dcEst1 += dcAlpha1 * (raw1 - dcEst1);
    int ac1 = (int)lround(raw1 - dcEst1);

    dcEst2 += dcAlpha2 * (raw2 - dcEst2);
    int ac2 = (int)lround(raw2 - dcEst2);

    // --- RMS kanal 1 ---
    long old1 = acBuf1[idx];
    sumSquares -= old1 * old1;

    acBuf1[idx] = ac1;
    sumSquares += (long)ac1 * ac1;

    idx = (idx + 1) % windowSize;
    if (filled1 < windowSize) filled1++;

    float rms1 = 0;
    if (filled1 == windowSize) {
      rms1 = sqrt((float)sumSquares / windowSize);
    }

    // --- RMS kanal 2 ---
    long old2 = acBuf2[idx2];
    sumSquares2 -= old2 * old2;

    acBuf2[idx2] = ac2;
    sumSquares2 += (long)ac2 * ac2;

    idx2 = (idx2 + 1) % windowSize;
    if (filled2 < windowSize) filled2++;

    float rms2 = 0;
    if (filled2 == windowSize) {
      rms2 = sqrt((float)sumSquares2 / windowSize);
    }

    // --- Print / logikk 25 Hz ---
    sampleCounter++;
    if (sampleCounter >= printEvery) {
      sampleCounter = 0;

      // Baseline init når RMS er gyldig
      if (!baseInit && filled1 == windowSize && filled2 == windowSize) {
        base1 = rms1;
        base2 = rms2;
        baseInit = true;
      }

      // Adaptiv baseline: oppdater bare når signalet er lavt (i ro)
      if (baseInit) {
        if (rms1 <= base1 + baseGate) base1 += baseAlpha * (rms1 - base1);
        if (rms2 <= base2 + baseGate) base2 += baseAlpha * (rms2 - base2);
      }

      // Baseline-korrigert RMS (det du vil ha ~0 i ro)
      float rms1z = baseInit ? clamp0(rms1 - base1) : 0.0f;
      float rms2z = baseInit ? clamp0(rms2 - base2) : 0.0f;

      // Utgang: HIGH hvis en av dem er over terskel
      const float thr = 9.0f;
      //digitalWrite(outPin, (rms1z > thr || rms2z > thr) ? HIGH : LOW);


      // --- Klassifisering basert på forskjell (bruk baseline-korrigerte) ---
      int midVal;
      unsigned long currentMillisJointChange = millis();
      if (rms1z > 20 && rms2z > 20) {
        if (currentMillisJointChange - previousMillisJointChange >= 750) {
        previousMillisJointChange = currentMillisJointChange;
        currentButton += 1;
        if (currentButton > 6) {
          currentButton = 1;
          }
        }

      } else if ((rms1z - rms2z) > 15) {     
        midVal = 60;
        String message = String(2) + ";" + String(currentButton);
        outSerial.println(message);

      } else if ((rms2z - rms1z) > 15) { 
        midVal = 40;
        String message = String(1) + ";" + String(currentButton);
        outSerial.println(message);

      } else {
        midVal = 50;
        String message = String(0) + ";" + String(currentButton);
        outSerial.println(message);

      }

      // Serial Plotter: alltid samme antall verdier per linje!
      Serial.print(rms1z); Serial.print(" ");
      Serial.print(rms2z); Serial.print(" ");
      //Serial.print(midVal); Serial.print(" ");
      Serial.print(currentButton * 10); Serial.print(" ");
      Serial.print(0); Serial.print(" ");
      Serial.println(100);
    }
  }
  //delayMicroseconds(1000); // ~1 kHz sampling
}
