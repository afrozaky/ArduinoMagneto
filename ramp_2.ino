const double stroke_length = 69.5841256;                        // [mm] approximate stroke length
const int resolution = 200;                                     // [steps/rev] Motor resolution
const double max_freq = 3000;                                   // [pulses/sec] Frequency of pulsing square waves
const double ang_speed = 1 / double(resolution)* max_freq;      // [revs/sec] Angular speed
const double linear_speed = 5 * ang_speed;                      // [mm/sec] Linear displacement
const int dir_pin = 9;                                          // Changes direction of motor
const int step_pin = 10;                                        // Positive edge output from this pin advances motor one increment
const double ratio = 0.2;                                       // ratio of time for ramping to time for constant frequency
const double tau = 1000*stroke_length / ((1-ratio)*linear_speed+.5*ratio*linear_speed);                // [ms] duration of stroke completion at maximum speed

// setPwmFrequency - sets the frequency of PWM of the input pin to maximum frequency divided by the divisor
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void setup() {
  // setting pins to output mode
  pinMode(dir_pin, OUTPUT);                                             
  pinMode(step_pin, OUTPUT);
  Serial.begin(9600);
  rampUp();
  maxFreq();
  rampDown();
  digitalWrite(dir_pin,0);
  maxFreq();
  rampUpAgain();
}

void maxFreq(){
  double dt = 0.5*(1-ratio)*tau/25;                     // [ms] time increments       
  setPwmFrequency(step_pin,1);
  double initial_time = millis();
  while(millis() - initial_time < (1-ratio)*tau){ 
   Serial.println(max_freq);
   //Serial.print(" ");
//   Serial.println(millis());
   analogWrite(step_pin, 128);
   delayMicroseconds(1000*dt*max_freq/31372.55);
   analogWrite(step_pin,0);
   delayMicroseconds(1000*dt*(1-max_freq/31372.55));
   
  }
}
void rampUp() {
  double freq = 0;
  double dt = 0.5*ratio*tau/25;                         // [ms] time increments
  double f_inc = max_freq/25;                           // [Hz] corresponding frequency increments
  int counter = 0;  
  digitalWrite(dir_pin, 255);
  double initial_time = millis();
  while(counter<5){
    counter++;
     Serial.println(freq);
     freq = counter*f_inc;
//   Serial.print(" ");
//   Serial.println(millis());
    analogWrite(step_pin, 128);
    delayMicroseconds(1000*(15*dt*freq/490.2)/(1+14*freq/490.2));
    analogWrite(step_pin,0);
    delayMicroseconds(1000*dt*(1-freq/490.2)/(1+14*freq/490.2));
  }
  setPwmFrequency(step_pin, 1);
  while(millis() - initial_time <= 0.5*ratio*tau && freq < max_freq){    
    counter++;
    Serial.println(freq);
    freq = counter*f_inc;              // incrementing desired freq by f_inc every iteration
//   Serial.print(" ");
//   Serial.println(millis());
    analogWrite(step_pin, 128);
    delayMicroseconds(1000*(dt*freq/31372.55));
    analogWrite(step_pin,0);
    delayMicroseconds(1000*dt*(1-freq/31372.55));   
  }
}

void rampDown() {
    setPwmFrequency(step_pin, 1);

  double freq = max_freq;
  double dt = 0.5*ratio*tau/25;                         // [ms] time increments
  double f_inc = max_freq/25;                           // [Hz] corresponding frequency increments
  int counter = 0;
  digitalWrite(dir_pin, 255);
  double initial_time = millis();  
  while(millis() - initial_time <= 0.5*ratio*tau && freq > 0){    
    counter++;
    Serial.println(freq);
//   Serial.print(" ");
//   Serial.println(millis());
    freq = max_freq - counter*f_inc;
    analogWrite(step_pin, 128);
    delayMicroseconds(1000*(dt*freq/31372.55));
    analogWrite(step_pin,0);
    delayMicroseconds(1000*dt*(1-freq/31372.55));   
  }
 digitalWrite(dir_pin, 0);
 freq = 0;
 counter = 0;
  initial_time = millis();
  while(millis() - initial_time <= 0.5*ratio*tau && freq < max_freq){    
    counter++;
    Serial.println(freq);
    freq = counter*f_inc;              // incrementing desired freq by f_inc every iteration
//   Serial.print(" ");
//   Serial.println(millis());
    analogWrite(step_pin, 128);
    delayMicroseconds(1000*(dt*freq/31372.55));
    analogWrite(step_pin,0);
    delayMicroseconds(1000*dt*(1-freq/31372.55));  
 }
}

void rampUpAgain() {
    setPwmFrequency(step_pin, 1);

  double freq = max_freq;
  double dt = 0.5*ratio*tau/25;                         // [ms] time increments
  double f_inc = max_freq/25;                           // [Hz] corresponding frequency increments 
  int counter = 0;
  digitalWrite(dir_pin, 0);
  double initial_time = millis();  
  while(millis() - initial_time <= 0.5*ratio*tau && freq > 0){    
    counter++;
    Serial.println(freq);
//   Serial.print(" ");
//   Serial.println(millis());
    freq = max_freq - counter*f_inc;
      setPwmFrequency(step_pin, 1);
    analogWrite(step_pin, 128);
    delayMicroseconds(1000*(dt*freq/31372.55));
    analogWrite(step_pin,0);
    delayMicroseconds(1000*dt*(1-freq/31372.55));     
}
}
void loop(){}

