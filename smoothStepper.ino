int dt;
const double startFreq = 1000;      //in Hz, min value of 62 Hz by precision of delayMicroseconds
const double maxFreq = 3500;      //in Hz, max value value of 330 kHz precision of delayMicroseconds
const double finalPeriod = 1 / (maxFreq) * pow(10,6);                   //Final period in microseconds
const double basePeriod = 1 /(startFreq) * pow(10, 6);                  //base period or the period to ramp up from and ramp down to
double period = basePeriod;
unsigned long initialTime;
const double ratio = 0.1;                                          //ratio of rampTime / total time
const double totalTime = 10;                              //total time to ramp up, maintain max frequency, and ramp down
const double peakTime = (1 - ratio) * totalTime;                                 //number of seconds for the time max frequency should be maintained
const double rampTime = 0.5*ratio*totalTime;                //Time for one individual ramp up or down
const int ncycles = 5;                      //the number of cycles the same frequency will be maintained at on the ramp up and down part of the curve, could be a function of rampTime
const byte DIR = 9;
const byte STEP = 10;

void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(STEP, LOW);
  Serial.begin(9600);
}

void loop() {
  //Ramp up to max frequency
  initialTime = millis();
  while(period > finalPeriod){
    for(int i = 0; i < ncycles; i++){
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period/2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period/2.0);
    }
    period--;
  }
  Serial.println(millis() - initialTime);

  initialTime = millis();
  while(millis() - initialTime < peakTime * pow(10, 3)){
    digitalWrite(STEP, HIGH);
    delayMicroseconds(finalPeriod/2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(finalPeriod/2.0);  
  }

  initialTime = millis();
  //Ramp down from max frequency to period
  while(period < basePeriod){
    for(int i = 0; i < ncycles; i++){
      digitalWrite(STEP, HIGH);
      delayMicroseconds(period/2.0);
      digitalWrite(STEP, LOW);
      delayMicroseconds(period/2.0);
    }
    period++;
  }
  Serial.println(millis() - initialTime);
}
