/** LINEAR STEPPER ACCELERATION

    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 6th, 2018

    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
*/

// DECLARING CONSTANTS
const double startFreq = 250;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 3500;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 // [us] base period or the period to ramp up from and ramp down to
const double peakTime = 0.3;                                     // [s] time for which max frequency should be maintained
const double df = 135;                                                  // [Hz] frequency increments
unsigned int dt = 2950;                                                 // [us] time increments
const byte DIR = 9;                                                     // direction pin on Arduino board
const byte STEP = 10;                                                   // clock pin on Arduino board
const double pulseAcceleration  = df / dt * pow(10, 6);                 // [s^-2] frequency acceleration rate

// NON CONSTANT VARIABLES
double period = basePeriod;
unsigned long initialTime;
double freq = startFreq;
unsigned long stepCount = 0;
// unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, LOW);
  Serial.begin(2000000);
}


void loop() {
  stepCount = 0;
  //Add delay between changing directions to match Gauss curve
  // DIRECTION
  digitalWrite(DIR, !digitalRead(DIR));
  // initialTime2 = millis();

  // RAMP UP TO MAX FREQUENCY
  while (period > finalPeriod) {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
    }
    freq += df;
    period = 1 / freq * pow(10, 6);
  }
  Serial.print(stepCount);
  Serial.print(" ");

  // REMAIN AT MAX FREQUENCY
  stepCount = 0;

  initialTime = millis();
  while (millis() - initialTime <= (unsigned long)(peakTime * pow(10, 3))) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
    // freq = 1 / finalPeriod * pow(10,6);
    // Serial.println(freq);
    stepCount++;
  }

  Serial.print(stepCount);
  Serial.print(" ");

  // RAMP DOWN FROM MAX FREQUENCY
  //initialTime2 = millis();
  stepCount = 0;
  while (period < basePeriod) {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
    }
    freq -= df;
    // freq = 1 / period * pow(10,6);
    // Serial.println(freq);
    period = 1 / freq * pow(10, 6);
  }

  Serial.println(stepCount);

}
