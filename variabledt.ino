/** LINEAR STEPPER ACCELERATION

    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 12th, 2018

    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
*/

// DECLARING CONSTANTS
const double startFreq = 295;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 5250;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 // [us] base period or the period to ramp up from and ramp down to
const double peakTime = .3;                                             // [s] time for which max frequency should be maintained
double df = 40;                                                         // [Hz] frequency increments
unsigned int dt;                                                        // [us] time increments
const byte DIR = 9;                                                     // direction pin on Arduino board
const byte STEP = 10;                                                   // clock pin on Arduino board
//const double pulseAcceleration  = df / dt * pow(10,6);                // [s^-2] frequency acceleration rate


// INITIALIZING
double period = basePeriod;
unsigned long initialTime;
double freq = startFreq;
int nCycles = 1;
unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, LOW);
  (maxFreq > 4000) ? df = 40 : df = 45;
  Serial.begin(2000000);
}

void setnCycles() {
  if (freq < 3200)
    nCycles = 1;
  else if (freq < 4000)
    nCycles = 2;
  else if(freq < 6300)
    nCycles = 4;
}

void loop() {

  //Change DIRECTION
  digitalWrite(DIR, !digitalRead(DIR));
  initialTime2 = millis();

  // RAMP UP TO MAX FREQUENCY
  while (freq < maxFreq) {
    for (int i = 0; i < nCycles; i++) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
    }
    freq += df;
    period = 1 / freq * pow(10, 6);
    setnCycles();
  }
  Serial.print(millis() - initialTime2);
  Serial.print("\t");

  // REMAIN AT MAX FREQUENCY
  initialTime = millis();
  while (millis() - initialTime < peakTime * pow(10, 3)) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
    //   Serial.println(freq);
  }
  //Serial.print(millis()-initialTime);
  //Serial.print(" ");

  // RAMP DOWN FROM MAX FREQUENCY
  initialTime2 = millis();
  while (freq > startFreq) {
    for (int i = 0; i < nCycles; i++) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
    }
    freq -= df;
    period = 1 / freq * pow(10, 6);
    setnCycles();
  }
  Serial.println(millis() - initialTime2);
}
