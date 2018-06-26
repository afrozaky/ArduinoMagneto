// External interrupt pin for IR sensor
#define interruptPin 3

unsigned long lastUpdate = 0;  // for timing display updates
volatile long accumulator = 0;  // sum of last 8 revolution times
volatile unsigned long startTime = 0; // start of revolution in microseconds
volatile unsigned int revCount = 0; // number of revolutions since last display update
double fluxRPM = 0;
double fluxFreq = 0;
double oldfluxRPM;
double error = 1;
double eps=0.01;

// INPUTS
double strokeLength = 0;                                                            // [mm] stroke length to cover in a half cycle
double cycles = 0;

// DECLARING CONSTANTS
double peakTime;
const double startStepFreq = 250;                                                       // [Hz] min value of 62 Hz by precision of delayMicroseconds
double finalStepFreq;
double finalStepPeriod;
double startStepPeriod;
const double df = 135;                                                              // [Hz] frequency increments
unsigned int dt = 2950;                                                             // [us] time increments
const byte DIR = 9;                                                                 // direction pin on Arduino board
const byte CLOCK = 10;                                                              // clock pin on Arduino board
const byte LIMIT = 7;                                                               // limit switch
double nPulses;
double constPulses;
int limitSwitch;
int counter = 0;
double totalTime;                                                                   // [s] time for one half cycle/full stroke length

//-------------------------------------
// Non-constant variables - parameters changing with time
//-------------------------------------
unsigned long initialTime;
double freq = startStepFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;
double period;

//-------------------------------------
// Setup - runs once at startup
//-------------------------------------
void setup() {
  // Initialize the serial port and display
  Serial.begin(2000000);

  // Enable the pullup resistor and attach the interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tach_interrupt, FALLING);

  // Driver and limtSwitch settings
  pinMode(DIR, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(CLOCK, LOW);
  limitSwitch = analogRead(A0);
  homeleft();
  digitalWrite(DIR, HIGH);   //LOW -> TOWARDS MOTOR; HIGH -> AWAY FROM MOTOR
}

// HOME TO THE LEFT LIMIT SWITCH
void homeleft() {
  digitalWrite(DIR, LOW);    //LOW  -> TOWARDS MOTOR (LEFT)
  double homingFreq = 750;
  double homingPeriod = 1 / homingFreq * pow(10, 6);
  while (digitalRead(LIMIT) == HIGH) {   //Run at homing frequency until limit hit
    digitalWrite(CLOCK, HIGH);
    delayMicroseconds(homingPeriod / 2.0);
    digitalWrite(CLOCK, LOW);
    delayMicroseconds(homingPeriod / 2.0);
  }
}

//-------------------------------------
// Interrupt Handler
// IR reflective sensor - target passed
// Calculate revolution time
//-------------------------------------
void tach_interrupt()
{
  // calculate the microseconds since the last interrupt
  long usNow = micros();
  long elapsed = usNow - startTime;
  startTime = usNow;  // reset the clock

  //  // Accumulate the last 8 interrupt intervals
  accumulator -= (accumulator >> 1);
  accumulator += elapsed;
  revCount++;
}
void loop() {
  while (error > eps) {
    if (millis() - lastUpdate > 1000) // update every second
    {
      // divide number of microseconds in a minute, by the average interval.
      if (revCount > 0)
      {
        oldfluxRPM = fluxRPM;
        fluxRPM = 60000000.0 / (accumulator >> 1);
        fluxFreq = fluxRPM / 60.0;
        error = abs(oldfluxRPM - fluxRPM) / fluxRPM;
      }          
      lastUpdate = millis();
      revCount = 0;
    }

    if (error <= eps) {
      Serial.print("Calculated fluxFreq: ");
      Serial.println(fluxFreq);
      Serial.print("Calculated fluxRPM: ");
      Serial.println(fluxRPM);
      detachInterrupt(interruptPin);
    }
  }
  totalTime = 1 / 2 / fluxFreq;
  
  // HANDLING USER INPUT SECTION
  if (strokeLength == 0 || cycles == 0) {
    counter = 0;                                                             // Reset  
    Serial.println("Enter stroke length in mm ");                            // Prompt User for input
    while (Serial.available() == 0) {}
    strokeLength = Serial.parseFloat();                                      // Read user input into height
    Serial.println("Enter number of cycles ");                               // Prompt User for input
    while (Serial.available() == 0) {}
    cycles = Serial.parseFloat();                                            // Read user input into height

    if (cycles != 0 && strokeLength != 0) {
      Serial.print("Total time calculated: ");
      Serial.println(totalTime);
      Serial.print("Stroke length entered: ");
      Serial.println(strokeLength);
      Serial.print("Number of cycles entered: ");
      Serial.println(cycles);
      Serial.print("Stroke Frequency: ");
      Serial.println(1.0 / (2.0 * totalTime));

      //Initialize variables from user input
      peakTime = sqrt(pow(totalTime, 2) - 3840 * strokeLength / pow(10, 6)); // [s] time for which max frequency should be maintained
      finalStepFreq = pow(10, 6) * (totalTime - peakTime) / 48;
      finalStepPeriod = 1 / (finalStepFreq) * pow(10, 6);                              // [us] final period corresponding to maximum frequency
      startStepPeriod = 1 / (startStepFreq) * pow(10, 6);                             // [us] base period or the period to ramp up from and ramp down to
      nPulses = ((finalStepFreq - startStepFreq) / 2.0) * .024 * finalStepFreq * .001;
      constPulses = finalStepFreq * peakTime;
      period = startStepPeriod;

      Serial.print("Max Stepper RPM: ");
      Serial.println(finalStepFreq / 200.0 * 60);
      }
}
