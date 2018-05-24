const double stroke_length = 69.5841256;               //approx. stroke length in mm
const int max_freq = 3000;                             //Maximum frequency in Hertz
const int resolution = 200;                            //Motor resolution (steps/rev)
const double ang_speed = 1 / double(resolution)* max_freq;    //in revs/sec
const double linear_speed = 5 * ang_speed;             //in mm/sec
const int dir_pin = 9;                            //Used to change direction of motor
const int step_pin = 10;                          //Positive edge output from this pin advances motor one increment

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
  // put your setup code here, to run once:
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
}

double calcTime(){
  return stroke_length / linear_speed;
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
