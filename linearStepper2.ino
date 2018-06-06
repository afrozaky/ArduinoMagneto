/** LINEAR STEPPER ACCELERATION
 *  
 *  Authors        : Zaky Hussein, Priyanka Deo
 *  Lasst Modified : June 6th, 2018
 *  
 *  The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time, 
 *  then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
 */

// INITIALIZING
const double startFreq = 250;                                           // [Hz] min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 3000;                                            // [Hz] max value of 330 kHz by precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  // [us] final period corresponding to maximum frequency
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 // [us] base period or the period to ramp up from and ramp down to
double period = basePeriod;
unsigned long initialTime; 
const double peakTime;                                                  // [ms] time for which max frequency should be maintained
const double rampTime = 0.5 * ratio * totalTime;            //Time for one individual ramp up or down
const double df = 135;                       //Change in freq (Hz)
unsigned int dt = 2950;                     //Change in time (us)
double pulseAcceleration  = df / dt;      
const byte DIR = 9;
const byte STEP = 10;
double freq = startFreq;
unsigned long initialTime2;

void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, LOW);
  Serial.begin(9600);
}

void loop() {
  //Initial Direction
  digitalWrite(DIR, !digitalRead(DIR));
  initialTime2 = millis();

  //Ramp up to max frequency
  while (period > finalPeriod) {
    initialTime = micros();
    while(micros() - initialTime < dt){
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
    }
    freq+=df;
//    freq = 1/period * pow(10,6);
//    Serial.println(freq);
    period = 1/freq * pow(10,6);    
  }
//  Serial.print(millis()-initialTime2);
//  Serial.print(" ");

  //Remain at max frequency
  initialTime = millis();
  while (millis() - initialTime < peakTime * pow(10, 3)) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
//    freq = 1/finalPeriod* pow(10,6);
//    Serial.println(freq);
  }
//   Serial.print(millis()-initialTime);
//   Serial.print(" ");

  //Ramp down from max frequency
  initialTime2 = millis();
  while (period < basePeriod) {
    initialTime = micros();
    while(micros() - initialTime < dt){
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period / 2.0);
    }
    freq-=df;
//    freq = 1/period * pow(10,6);
//    Serial.println(freq);
    period = 1/freq * pow(10,6); 
  }
//    Serial.println(millis() - initialTime2);

}
