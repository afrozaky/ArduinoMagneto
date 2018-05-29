const double stroke_length = 69.5841256;                        // [mm] approximate stroke length
const int resolution = 200;                                     // [steps/rev] Motor resolution
const double max_freq = 31372.55 / 8;                           // [pulses/sec] Frequency of pulsing square waves
const double ang_speed = 1 / double(resolution)* max_freq;      // [revs/sec] Angular speed
const double linear_speed = 5 * ang_speed;                      // [mm/sec] Linear displacement
const int dir_pin = 9;                                          // Changes direction of motor
const int step_pin = 10;                                        // Positive edge output from this pin advances motor one increment
const double tau = stroke_length/linear_speed;

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
}

void loop() {
// part 1 - low frequency positive waveform to start motor
  digitalWrite(dir_pin, HIGH);                                          
  analogWrite(step_pin, 128);
  delay(10)
  
// part 2 - high frequency positive waveform to cover stroke range
  setPwmFrequency(step_pin, 8);
  analogWrite(step_pin, 128);
  delay(tau-1.25);
  analogWrite(step_pin, 0);     // ending the first half of cycle
  
// part 3 - low frequency negative waveform to start motor in reverse direction
  digitalWrite(dir_pin, LOW);
  setPwmFrequency(step_pin, 64);
  analogWrite(step_pin, 128);
  delay(10);
  
// part 4 - high frequency negative waveform to bring the pistons back to initial position
  setPwmFrequency(step_pin, 8);
  analogWrite(step_pin, 128);
  delay(tau-1.25);
  analogWrite(step_pin, 0);   // ending the cycle
}

