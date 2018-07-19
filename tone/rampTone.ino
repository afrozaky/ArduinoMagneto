/** LINEAR STEPPER ACCELERATION

    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 6th, 2018

    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
*/

// DECLARING CONSTANTS
const double startFreq = 250;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 9000;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double peakTime = 1;                                              // [s] time for which max frequency should be maintained
const double df = 135;                                                  // [Hz] frequency increments
unsigned int dt;                                                        // [us] time increments
const byte DIR = 9;                                                     // direction pin on Arduino board
const byte STEP = 10;                                                   // clock pin on Arduino board
const double pulseAcceleration  = df / dt * pow(10, 6);                 // [s^-2] frequency acceleration rate

// INITIALIZING
unsigned long initialTime;
double freq = startFreq;
// unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, HIGH);
  Serial.begin(9600);

  // RAMP UP TO MAX FREQUENCY
  while (freq < maxFreq) {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      tone(STEP, freq);
    }
    freq += df;
    //  Serial.println(freq);
  }

  //REMAIN AT MAX FREQUENCY
  initialTime = millis();
  while (millis() - initialTime < peakTime * pow(10, 3)) {
    tone(STEP, freq);
    //   Serial.println(freq);
  }

  // RAMP DOWN FROM MAX FREQUENCY
  //initialTime2 = millis();
  while (freq > startFreq) {
    initialTime = micros();
    while (micros() - initialTime < dt) {
      tone(STEP, freq);
    }
    freq -= df;
    //   Serial.println(freq);
  }

  // Serial.println(millis() - initialTime2);
}


void loop() {

  // DIRECTION
  //digitalWrite(DIR, !digitalRead(DIR));
  // initialTime2 = millis();



  // Serial.print(millis() - initialTime2);
  // Serial.print(" ");



  // Serial.print(millis()-initialTime);
  // Serial.print(" ");


}
