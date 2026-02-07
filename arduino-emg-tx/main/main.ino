#include <math.h>

const int analogPin = A0;
const int outPin = 8; // Kommunikajson


const int windowSize = 50;      // RMS-vindu (50 ms)
const float dcAlpha = 0.005;    // DC-fjerner

const int printEvery = 40;      // 1000 Hz / 40 = 25 Hz printing

int acBuf[windowSize];
int idx = 0;
long sumSquares = 0;
int filled = 0;

float dcEst = 0;

unsigned int sampleCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outPin, OUTPUT);
}

void loop() {
  int raw = analogRead(analogPin);

  // --- DC-fjerning ---
  dcEst += dcAlpha * (raw - dcEst);
  int ac = (int)lround(raw - dcEst);

  // --- RMS ---
  long old = acBuf[idx];
  sumSquares -= old * old;

  acBuf[idx] = ac;
  sumSquares += (long)ac * ac;

  idx = (idx + 1) % windowSize;
  if (filled < windowSize) filled++;

  float rms = 0;
  if (filled == windowSize) {
    rms = sqrt((float)sumSquares / windowSize);
  }

  // --- Print kun 20–50 Hz ---
  sampleCounter++;
  if (sampleCounter >= printEvery) {
    sampleCounter = 0;

    if (rms > 9){
      digitalWrite(outPin, HIGH);
    } else {
      digitalWrite(outPin, LOW);
    }
    
    Serial.print(rms);

    // For grenser av plot
    Serial.print(" ");
    Serial.print(0);
    Serial.print(" ");
    Serial.println(100);
  }

  delayMicroseconds(1000); // behold ~1 kHz sampling
}