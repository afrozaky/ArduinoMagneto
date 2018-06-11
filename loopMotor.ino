/** LINEAR STEPPER ACCELERATION
 *  
 *  Authors       : Zaky Hussein, Priyanka Deo
 *  Last Modified : June 6th, 2018
 *  
 *  The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time, 
 *  then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
 */

// DECLARING CONSTANTS
const double startFreq = 250;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 3000;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 // [us] base period or the period to ramp up from and ramp down to
const double peakTime = .84;                                              // [s] time for which max frequency should be maintained
const double df = 120;                                                  // [Hz] frequency increments
unsigned int dt = 2950;                                                 // [us] time increments
const byte DIR = 9;                                                     // direction pin on Arduino board
const byte STEP = 10;                                                   // clock pin on Arduino board
const double pulseAcceleration  = df / dt * pow(10,6);                  // [s^-2] frequency acceleration rate
const byte FB = A0;
int pinVal;

// INITIALIZING 
double period = basePeriod;
unsigned long initialTime; 
double freq = startFreq;
// unsigned long initialTime2;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(FB, INPUT);
  digitalWrite(DIR, HIGH);
  digitalWrite(STEP, HIGH);
  Serial.begin(9600);
  pinVal = analogRead(FB);
  Serial.println(pinVal);  
  
}


void loop() {
  
// initialTime2 = millis();

// RAMP UP TO MAX FREQUENCY 
while (period > finalPeriod) {
  initialTime = micros();
  while(micros() - initialTime < dt){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq+=df;
//  Serial.println(freq);
  period = 1 / freq * pow(10,6);    
}

 //REMAIN AT MAX FREQUENCY
initialTime = millis();
while (millis() - initialTime < peakTime * pow(10, 3)) {
  digitalWrite(STEP, HIGH);
  delayMicroseconds(period / 2.0);
  digitalWrite(STEP, LOW);
  delayMicroseconds(period / 2.0);
//   freq = 1 / finalPeriod * pow(10,6);
//   Serial.println(freq);
}
//
  // RAMP DOWN FROM MAX FREQUENCY
//initialTime2 = millis();
while (period < basePeriod) {
  initialTime = micros();
  while(micros() - initialTime < dt){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
  }
  freq-=df;
  period = 1 / freq * pow(10,6); 
  // freq = 1 / period * pow(10,6);
//   Serial.println(freq);
}
  
//// Serial.println(millis() - initialTime2);
digitalWrite(STEP,LOW);
pinVal = analogRead(FB);
  Serial.println(pinVal);

  // CHANGE DIRECTION
digitalWrite(DIR, !digitalRead(DIR));
// Serial.print(millis() - initialTime2);
// Serial.print(" ");


  
// Serial.print(millis()-initialTime);
// Serial.print(" ");


}

