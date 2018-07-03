/** LINEAR STEPPER MOTOR ACCELERATION
    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 26th, 2018
    This program does the following:
    -> Measures the frequency of EMF cycles
    -> Synchronizes the working fluid cycle with the EMF cycle
    -> Linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence a given number of times.

    Please follow the folllowing instructions for running the code:
    1) Hit the upload button
    2) Turn the motor driver ON
    3) Select Tools -> Serial Monitor
    4) Follow instructions on the serial monitor
    Please DO NOT touch the following script, thank you.
**/

//-------------------------------------
// INITIALIZING - Declaring inputs and constants
//-------------------------------------

// Part I - IR SENSOR PARAMETERS
#define interruptPin 3                                                              // tachometer limit switch pin on Arduino board
unsigned long lastUpdate = 0;                                                       // for timing display updates
volatile long accumulator = 0;                                                      // sum of last 8 revolution times
volatile unsigned long startTime = 0;                                               // start of revolution in microseconds
volatile unsigned int revCount = 0;                                                 // number of revolutions since last display update
double fluxRPM = 0;
double fluxFreq = 0;
double oldfluxRPM;
double error = 1;                                                                   // initializing to a high value
double eps = 0.01;                                                                  // desired precision in measurement
int IRread = 0;
int lastIRread = 0;

// Part II - USER INPUTS
double strokeLength = 0;                                                            // [mm] stroke length to cover in a half cycle
double cycles = 0;

// Part III - DECLARING CONSTANT VARIABLES
double peakTime;
const double startStepFreq = 250;                                                   // [Hz] min value of 62 Hz by precision of delayMicroseconds
double finalStepFreq;
double finalStepPeriod;
double startStepPeriod;
const double df = 135;                                                              // [Hz] frequency increments
unsigned int dt = 2950;                                                             // [us] time increments
const byte DIR = 9;                                                                 // direction pin on Arduino board
const byte CLOCK = 10;                                                              // clock pin on Arduino board
const byte LIMIT = 12;                                                              // limit switch pin on Arduino board
const byte ONOFF = 2;                                                               //On off pin on Arduino board, HIGH turns off the driver
double nPulses;
double constPulses;
int limitVal;
int counter = 0;
double totalTime;                                                                   // [s] time for one half cycle/full stroke length
double travelTime;

// Part IV - NON-CONSTANT VARIABLES THAT UPDATE WITH TIME
unsigned long initialTime;
double freq = startStepFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;
double period;

//-------------------------------------
// setup() - runs once at startup
//-------------------------------------

void setup() {
  Serial.begin(2000000);                                                            // initialize the serial port and display
  pinMode(interruptPin, INPUT_PULLUP);                                              // enable the pullup resistor and attach the interrupt
  attachInterrupt(digitalPinToInterrupt(interruptPin), tach_interrupt, FALLING);

  pinMode(ONOFF, OUTPUT);
  pinMode(DIR, OUTPUT);                                                             // driver and limit switch settings
  pinMode(CLOCK, OUTPUT);
  pinMode(interruptPin, INPUT);
  pinMode(LIMIT, INPUT);
  digitalWrite(ONOFF, LOW);
  digitalWrite(CLOCK, LOW);
  limitVal = analogRead(A0);
  homeleft();                                                                       // bring pistons to home position before starting cycles
  digitalWrite(DIR, HIGH);   //LOW -> TOWARDS MOTOR; HIGH -> AWAY FROM MOTOR
}

//-------------------------------------
// Helper methods - called throughout the program
//-------------------------------------

//-------------------------------------
// homeleft() - HOME TO THE LEFT LIMIT SWITCH
// home position determined by limt switch
// Brings pistons back to the home position
//-------------------------------------

void homeleft() {
  digitalWrite(DIR, LOW);                                                          // LOW  -> TOWARDS MOTOR (LEFT)
  double homingFreq = 750;
  double homingPeriod = 1 / homingFreq * pow(10, 6);
  while (digitalRead(LIMIT) == HIGH) {                                             // run at homing frequency until limit hit
    digitalWrite(CLOCK, HIGH);
    delayMicroseconds(homingPeriod / 2.0);
    digitalWrite(CLOCK, LOW);
    delayMicroseconds(homingPeriod / 2.0);
  }
}

//-------------------------------------
// tach_interrupt() - INTERRUPT HANDLER
// IR reflective sensor - target passed
// Calculate revolution time
//-------------------------------------
void tach_interrupt() {
  long usNow = micros();                                                          // calculate the microseconds since the last interrupt
  long elapsed = usNow - startTime;
  if (elapsed > 70000) {
    startTime = usNow;                                                              // reset the clock
    accumulator -= (accumulator >> 1);                                              // accumulate the last 2 interrupt intervals
    accumulator += elapsed;
    revCount++;
  }
}

//-------------------------------------
// waitTime() - SYNCHRONIZE MOTIONS
// holds the pistons at home until
// new EMF cycle is started
//-------------------------------------

void waitTime() {
  while (digitalRead(interruptPin) == HIGH) {
    //  IRread = digitalRead(interruptPin);
    //  if (IRread == LOW) {
    ////    if (IRread != lastIRread) {
    ////      lastIRread = IRread;
    //      break;
    //    }
    ////  }
    ////  lastIRread = IRread;
  }
}

//-------------------------------------
// loop() - runs until input number of cycles
//-------------------------------------

void loop() {

  // PART I - CALCULATE EMF CYCLE FREQUENCY
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

  // PART II - USE CALCULATED FREQUENCY TO EVAVLUATE TIME FOR COVERING STROKE LENGTH
  totalTime = 1.0 / 2.0 / fluxFreq;
  travelTime = 0.9 * totalTime - 0.1;

  // PART III - HANDLE USER INPUT SECTION
  if (strokeLength == 0 || cycles == 0) {
    counter = 0;                                                                    // Reset
    Serial.println("Enter stroke length in mm ");                                   // Prompt User for input
    while (Serial.available() == 0) {}
    strokeLength = Serial.parseFloat();                                             // Read user input into height
    Serial.println("Enter number of cycles ");                                      // Prompt User for input
    while (Serial.available() == 0) {}
    cycles = Serial.parseFloat();                                                   // Read user input into height

    if (cycles != 0 && strokeLength != 0) {
      Serial.print("Total time calculated: ");
      Serial.println(totalTime);
      Serial.print("Stroke length entered: ");
      Serial.println(strokeLength); 
      Serial.print("Number of cycles entered: ");
      Serial.println(cycles);
      Serial.print("Average Stroke Frequency: ");
      Serial.println(1.0 / (2.0 * totalTime));

      //Initialize variables from user input
      peakTime = sqrt(pow(travelTime, 2) - 3840 * strokeLength / pow(10, 6));        // [s] time for which max frequency should be maintained
      finalStepFreq = pow(10, 6) * (travelTime - peakTime) / 48;
      finalStepPeriod = 1 / (finalStepFreq) * pow(10, 6);                           // [us] final period corresponding to maximum frequency
      startStepPeriod = 1 / (startStepFreq) * pow(10, 6);                           // [us] base period or the period to ramp up from and ramp down to
      nPulses = ((finalStepFreq - startStepFreq) / 2.0) * .024 * finalStepFreq * .001;
      constPulses = finalStepFreq * peakTime;
      period = startStepPeriod;

      Serial.print("Max Stepper RPM: ");
      Serial.println(finalStepFreq / 200.0 * 60);
      Serial.println("Beginning motor control sequence in 3 seconds");
      delay(3000);
      waitTime();
    }
  }

  // PART IV - SYNCHRONIZE AND RUN THE STEPPER MOTOR
  delay(0.05 * totalTime * pow(10, 3));              // Add delay between changing directions to match Gauss curve
  if (counter < 2 * cycles) {
    stepCount = 0;
    initialTime2 = millis();
    limitVal = analogRead(A0);

    // RAMP UP TO MAX FREQUENCY
    while (stepCount < nPulses && freq < finalStepFreq) {
      initialTime = micros();
      while (micros() - initialTime < dt) {
        digitalWrite(CLOCK, HIGH);
        delayMicroseconds(period / 2.0);
        digitalWrite(CLOCK, LOW);
        delayMicroseconds(period / 2.0);
        stepCount++;
        if (stepCount >= nPulses)
          break;
      }
      freq += df;
      period = 1 / freq * pow(10, 6);
    }

    // REMAIN AT MAX FREQUENCY
    stepCount = 0;
    for (int i = 0; i < constPulses; i++) {
      digitalWrite(CLOCK, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(CLOCK, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
    }

    // RAMP DOWN FROM MAX FREQUENCY
    stepCount = 0;
    while (freq > startStepFreq)  {
      initialTime = micros();
      while (micros() - initialTime < dt) {
        digitalWrite(CLOCK, HIGH);
        delayMicroseconds(period / 2.0);
        digitalWrite(CLOCK, LOW);
        delayMicroseconds(period / 2.0);
        stepCount++;
      }
      freq -= df;
      period = 1 / freq * pow(10, 6);
    }

    limitVal = analogRead(A0);
    //    Serial.println(millis() - initialTime2);
    //    Serial.println(stepCount);
    //    Serial.println(limitVal);
    counter++;                                        // End of one half cycle, increment counter
    delay(0.05 * totalTime * pow(10, 3));              // Add delay between changing directions to match Gauss curve

    // DIRECTION SWITCH
    digitalWrite(DIR, !digitalRead(DIR));

    if (counter % 2 == 0) {
      waitTime();
    }
  }

}
