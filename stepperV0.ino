const double stroke_length = 69.5841256;                        // [mm] approximate stroke length
const int max_freq = 3000;                                      // [steps/s] Maximum pulsing rate
const int resolution = 200;                                     // [steps/rev] Motor resolution
const double ang_speed = 1 / double(resolution)* max_freq;      // [revs/sec] Angular speed
const double linear_speed = 5 * ang_speed;                      // [mm/sec] Linear displacement
const int dir_pin = 9;                                          // Changes direction of motor
const int step_pin = 10;                                        // Positive edge output from this pin advances motor one increment
const double percent = .2;                                      // Percentage time for ramp up

// setPwmFrequency 
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
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
}

void rampUp() {
  double val = max_freq / pwmFreq * 255;
  double rval = 0;
  double increment = 300*255/3921.16;
  double initial_time = millis();
  while(millis() - initial_time < percent*calcTime() && rval < val){    
    rval+=increment;    
    analogWrite(dir_pin, 255);    
    analogWrite(step_pin, rval);
    delay(increment*percent*calcTime()/max_freq);
  }
}
 
void rampDown() {
  double val = max_freq / pwmFreq * 255;
  double rval = val;
  double increment = 300*255/3921.16;
  double initial_time = millis();
  analogWrite(dir_pin, 255);
  while(millis() - initial_time < percent*calcTime() && rval > 0){    
    rval-=increment;            
    analogWrite(step_pin, rval);
    delay(increment*percent*calcTime()/max_freq);
  }
  rval = 0;
  analogWrite(dir_pin, 0);      
  while(millis() - initial_time < percent*calcTime() && rval < val){    
    rval+=increment;    
    analogWrite(step_pin, rval);
    delay(increment*percent*calcTime()/max_freq);
  }
}

double calcTime(){
  return stroke_length / ((1-2*percent)*linear_speed+percent*linear_speed);
}

void loop() {
  // put your main code here, to run repeatedly:
  double pwmFreq = 3921.16;
  int val = max_freq / pwmFreq * 255;
  setPwmFrequency(step_pin, 8);
  analogWrite(dir_pin, 255);
  analogWrite(step_pin, val);
  delay(calcTime());
  analogWrite(step_pin, 0);
  analogWrite(dir_pin, 0);
  analogWrite(step_pin, val);
  delay(calcTime());
  analogWrite(step_pin, 0);
}


