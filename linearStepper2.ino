const double startFreq = 300;      //in Hz, min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 6000;      //in Hz, max value value of 330 kHz precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10, 6);                  //Final period in microseconds
const double basePeriod = 1 / (startFreq) * pow(10, 6);                 //base period or the period to ramp up from and ramp down to
double period = basePeriod;
unsigned long initialTime;
const double ratio = 0.1;                                          //ratio of rampTime / total time
const double totalTime = 1;                              //total time to ramp up, maintain max frequency, and ramp down
const double peakTime = (1 - ratio) * totalTime;                                 //number of seconds for the time max frequency should be maintained
const double rampTime = 0.5 * ratio * totalTime;            //Time for one individual ramp up or down
unsigned int df = 50;                       //Change in freq (Hz)
unsigned int dt = 3000;                     //Change in time (us)
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
  //digitalWrite(DIR, !digitalRead(DIR));
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
    Serial.println(freq);
    period = 1/freq * pow(10,6);    
  }

  //Remain at max frequency
  initialTime = millis();
  while (millis() - initialTime < peakTime * pow(10, 3)) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(period / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(period / 2.0);
//    freq = 1/finalPeriod* pow(10,6);
    Serial.println(freq);
  }

  //Ramp down from max frequency
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
    Serial.println(freq);
    period = 1/freq * pow(10,6); 
  }
//    Serial.println(millis() - initialTime2);

}

