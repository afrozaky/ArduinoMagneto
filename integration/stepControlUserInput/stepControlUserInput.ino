/** LINEAR STEPPER MOTOR ACCELERATION
    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 18th, 2018
    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence a given number of times.
    Please follow the folllowing instructions for running the code:
    1) Hit the upload button
    2) Turn the motor driver ON
    3) Select Tools -> Serial Monitor
    4) Follow instructions on the serial monitor
    Please DO NOT touch the following script, thank you.    
**/

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

// NON CONSTANT VARIABLES
unsigned long initialTime;
double freq = startStepFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;
double period;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(CLOCK, LOW);
  Serial.begin(2000000);
  //  Serial.print(peakTime);
  //  Serial.print(" ");
  //  Serial.print(nPulses);
  //  Serial.print(" ");
  //  Serial.print(finalStepFreq);
  //  Serial.print(" ");
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

void loop() {

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
      Serial.print("Total time entered: ");
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

      Serial.print("Max RPM: ");
      Serial.println(finalStepFreq / 200.0 * 60);
      Serial.println("Beginning motor control sequence in 3 seconds");
      delay(3000);                                                           // Give user time to read out
    }

    // MAIN MOTOR CONTROL SECTION OF LOOP
  } else if (counter < 2 * cycles) {
    stepCount = 0;
    initialTime2 = millis();
    limitSwitch = analogRead(A0);
    //    Serial.println(limitSwitch);

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

    limitSwitch = analogRead(A0);
    //    Serial.println(millis() - initialTime2);
    //    Serial.println(stepCount);
    //    Serial.println(limitSwitch);
    counter++;                                        // End of one half cycle, increment counter
    delay(0.1 * totalTime * pow(10, 3));              // Add delay between changing directions to match Gauss curve

    // DIRECTION SWITCH
    digitalWrite(DIR, !digitalRead(DIR));
  }
}

