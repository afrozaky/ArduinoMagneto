/** LINEAR STEPPER ACCELERATION
 *  
 *  Authors       : Zaky Hussein, Priyanka Deo
 *  Last Modified : June 11th, 2018
 *  
 *  The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time, 
 *  then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
 */

// DECLARING CONSTANTS
const double startFreq = 250;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 5000;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 // [us] base period or the period to ramp up from and ramp down to
const double peakTime = .3;                                             // [s] time for which max frequency should be maintained
double df = 40;                                                         // [Hz] frequency increments
unsigned int dt;                                                        // [us] time increments
const byte DIR = 9;                                                     // direction pin on Arduino board
const byte STEP = 10;                                                   // clock pin on Arduino board
//const double pulseAcceleration  = df / dt * pow(10,6);                // [s^-2] frequency acceleration rate
int nCycles = 1;

// INITIALIZING 
double period = basePeriod;
unsigned long initialTime; 
double freq = startFreq;
// unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, LOW);
//  Serial.begin(9600);
}

void loop() {
  
// DIRECTION
digitalWrite(DIR, !digitalRead(DIR));
//initialTime2 = millis();

// RAMP UP TO MAX FREQUENCY 
while (freq < 3200 && freq < maxFreq) {
  for(int i=0; i<nCycles;i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq+=df;
  period = 1 / freq * pow(10,6);    
}

nCycles = 2;
while (freq < 4000 && freq < maxFreq) {
  for(int i=0; i<nCycles;i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq+=df;
  period = 1 / freq * pow(10,6);    
}

nCycles = 4;
while (freq < 6300 && freq < maxFreq) {
  for(int i=0; i<nCycles;i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq+=df;
  period = 1 / freq * pow(10,6);    
}
  
// Serial.print(millis() - initialTime2);
// Serial.print(" ");

// REMAIN AT MAX FREQUENCY
initialTime = millis();
while (millis() - initialTime < peakTime * pow(10, 3)) {
  digitalWrite(STEP, HIGH);
  delayMicroseconds(period / 2.0);
  digitalWrite(STEP, LOW);
  delayMicroseconds(period / 2.0);
//   Serial.println(freq);
}
// Serial.print(millis()-initialTime);
// Serial.print(" ");

// RAMP DOWN FROM MAX FREQUENCY
//initialTime2 = millis();
while (freq > 4000) {  
  for(int i=0; i<nCycles; i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq-=df;
  // freq = 1 / period * pow(10,6);
  // Serial.println(freq);
  period = 1 / freq * pow(10,6); 
}

nCycles = 2;
while (freq > 3200) {  
  for(int i=0; i<nCycles; i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq-=df;
  // freq = 1 / period * pow(10,6);
  // Serial.println(freq);
  period = 1 / freq * pow(10,6); 
}

nCycles=1;
while (freq > df+startFreq) {
  for(int i=0; i<nCycles; i++){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq-=df;
  // freq = 1 / period * pow(10,6);
  // Serial.println(freq);
  period = 1 / freq * pow(10,6); 
}

// Serial.println(millis() - initialTime2);

}
