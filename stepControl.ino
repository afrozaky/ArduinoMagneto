/** LINEAR STEPPER ACCELERATION

    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 6th, 2018

    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
*/

// INPUTS
const double strokeLength = 20;                                                     // [mm] stroke length to cover in a half cycle
const double totalTime = .3;                                                         // [s] time for one half cycle/full stroke length

// DECLARING CONSTANTS
const double peakTime = sqrt(pow(totalTime, 2) - 3840 * strokeLength / pow(10, 6)); // [s] time for which max frequency should be maintained
const double startFreq = 250;                                                       // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = pow(10, 6) * (totalTime - peakTime) / 48;
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                              // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                             // [us] base period or the period to ramp up from and ramp down to
const double df = 135;                                                              // [Hz] frequency increments
unsigned int dt = 2950;                                                             // [us] time increments
const byte DIR = 9;                                                                 // direction pin on Arduino board
const byte STEP = 10;                                                               // clock pin on Arduino board
const double pulseAcceleration  = df / dt * pow(10, 6);                             // [s^-2] frequency acceleration rate
const double nPulses = ((maxFreq - startFreq) / 2.0) * .024 * maxFreq * .001;
const double constPulses = maxFreq * peakTime;
int sensor;

// NON CONSTANT VARIABLES
double period = basePeriod;
unsigned long initialTime;
double freq = startFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(DIR, HIGH);   //LOW -> TOWARDS MOTOR
  digitalWrite(STEP, LOW);
    Serial.begin(2000000);
  //  Serial.print(peakTime);
  //  Serial.print(" ");
  //  Serial.print(nPulses);
  //  Serial.print(" ");
  //  Serial.print(maxFreq);
    sensor = analogRead(A0);
  //  Serial.print(" ");
    Serial.println(sensor);


  stepCount = 0;
  //Add delay between changing directions to match Gauss curve
  // DIRECTION
  //digitalWrite(DIR, !digitalRead(DIR));
  // initialTime2 = millis();
  initialTime2 = millis();
  // RAMP UP TO MAX FREQUENCY
  while (stepCount < nPulses && freq < maxFreq) {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
      if (stepCount >= nPulses)
        break;
    }
    freq += df;
    period = 1 / freq * pow(10, 6);
  }
  //  Serial.print(millis() - initialTime2);
  //  Serial.print(" ");
  //  Serial.print((double)stepCount / 40.0);
  //  Serial.print(" ");
  //  Serial.println(freq);

  // REMAIN AT MAX FREQUENCY
  stepCount = 0;
  initialTime = millis();
  for (int i = 0; i < constPulses; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
    // freq = 1 / finalPeriod * pow(10,6);
    // Serial.println(freq);
    stepCount++;
  }

  //  Serial.print(millis() - initialTime);
  //  Serial.print(" ");
  //  Serial.print((double)stepCount / 40.0);
  //  Serial.print(" ");
  //  Serial.println(freq);

  // RAMP DOWN FROM MAX FREQUENCY
//  initialTime2 = millis();
  stepCount = 0;
  while (freq > startFreq)  {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
      //       if(stepCount>=nPulses)
      //        break;
    }
    freq -= df;
    // freq = 1 / period * pow(10,6);
    // Serial.println(freq);
    period = 1 / freq * pow(10, 6);
  }
    sensor = analogRead(A0);
  //  Serial.print(millis() - initialTime2);
  //  Serial.print(" ");
  //  Serial.print((double)stepCount / 40.0);
  //  Serial.print(" ");
  //  Serial.println(freq);
  //Serial.print(" ");
    Serial.println(millis()-initialTime2);
  Serial.println(sensor);


}

void loop() {
}
