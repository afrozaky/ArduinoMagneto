const double stroke_length = 69.5841256;                        // [mm] approximate stroke length
const int resolution = 200;                                     // [steps/rev] Motor resolution
const double max_freq = 3000;                                   // [pulses/sec] Frequency of pulsing square waves
const double ang_speed = 1 / double(resolution)* max_freq;      // [revs/sec] Angular speed
const double linear_speed = 5 * ang_speed;                      // [mm/sec] Linear displacement
const int dir_pin = 9;                                          // Changes direction of motor
const int step_pin = 10;                                        // Positive edge output from this pin advances motor one increment
const double ratio = 0.1;                                       // ratio of time for ramping to time for constant frequency
const double tau = 1000*stroke_length / ((1-ratio)*linear_speed+.5*ratio*linear_speed);                // [ms] duration of stroke completion at maximum speed
double acceleration;
double RPM;

//Generates a square wave at the given frquency in Hz for runtime seconds
void generateWave(double freq, unsigned long runTime){
  unsigned long startTime = millis();
  RPM = freq / resolution * 60;
  double delayTime = 1/freq * pow(10, 6);
  
  if(delayTime < 10){
    Serial.println("Delay time less than 10 microseconds");
    return;
  }
  
  while((millis() - startTime) < (runTime * pow(10,3))){
    Serial.println(freq);
    digitalWrite(step_pin, HIGH);
    //Serial.print(millis()-startTime);
    //Serial.print(" ");
    //Serial.println(digitalRead(step_pin));
    delayMicroseconds(.5*delayTime);
    digitalWrite(step_pin, LOW);
    //Serial.print(millis()-startTime);
    //Serial.print(" ");
    //Serial.println(digitalRead(step_pin));
    delayMicroseconds(.5*delayTime);
  }
}

//Accelerates from freq1 to freq2 in runTime seconds
void accelerate(double freq1, double freq2, unsigned long runTime){
  double delta = freq2 - freq1;
  acceleration = abs(delta)/ runTime;
  double tsteps = 25;
  double dt = runTime / tsteps;
  while(freq1 != freq2){
      freq1 += delta/tsteps; 
      generateWave(freq1, dt);
  }
}

void setup() {
  // setting pins to output mode
  Serial.begin(9600);
  pinMode(dir_pin, OUTPUT);                                             
  pinMode(step_pin, OUTPUT);
  digitalWrite(step_pin, LOW);
  digitalWrite(dir_pin, LOW);
  accelerate(0, 100, 5);
  //generateWave(100, 3);
}


void loop(){

}
