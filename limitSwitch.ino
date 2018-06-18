/** LINEAR STEPPER ACCELERATION
    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 18th, 2018
    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence.
*/

// INPUTS
double strokeLength = 0;                                                            // [mm] stroke length to cover in a half cycle
double totalTime = 0;                                                               // [s] time for one half cycle/full stroke length
double cycles = 0;

// DECLARING CONSTANTS
double peakTime;
const double startFreq = 250;                                                       // [Hz] min value of 62 Hz by precision of delayMicroseconds
double maxFreq;
double finalPeriod;
double basePeriod;
const double df = 135;                                                              // [Hz] frequency increments
unsigned int dt = 2950;                                                             // [us] time increments
const byte DIR = 9;                                                                 // direction pin on Arduino board
const byte STEP = 10;                                                               // clock pin on Arduino board
const byte LIMIT = 7;
double nPulses;
double constPulses;
int sensor;
int counter = 0;

// NON CONSTANT VARIABLES
unsigned long initialTime;
double freq = startFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;
double period;



// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(DIR, HIGH);   //LOW -> TOWARDS MOTOR; HIGH -> AWAY FROM MOTOR
  digitalWrite(STEP, LOW);
  Serial.begin(2000000);
  homeleft();
}


//Home to the left limit switch
void homeleft() {
  digitalWrite(DIR, LOW);                 //LOW == towards motor (left)            
  double homingFreq = 750;
  double homingPeriod = 1 / homingFreq * pow(10, 6);
  while (digitalRead(LIMIT) == HIGH) {   //Run at homing frequency until limit hit
    digitalWrite(STEP, HIGH);
    delayMicroseconds(homingPeriod / 2.0);
    digitalWrite(STEP, LOW);
    delayMicroseconds(homingPeriod / 2.0);
  }
}

void loop() {
}
